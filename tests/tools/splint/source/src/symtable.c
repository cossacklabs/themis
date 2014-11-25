/*
** Splint - annotation-assisted static program checker
** Copyright (C) 1994-2003 University of Virginia,
**         Massachusetts Institute of Technology
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or (at your
** option) any later version.
** 
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** General Public License for more details.
** 
** The GNU General Public License is available from http://www.gnu.org/ or
** the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
** MA 02111-1307, USA.
**
** For information on splint: info@splint.org
** To report a bug: splint-bug@splint.org
** For more information: http://www.splint.org
*/
/*
** symtable.c
**
** Symbol table abstraction
**
**  AUTHORS:
**
**	Gary Feldman, Technical Languages and Environments, DECspec project
**	Steve Garland,
**         Massachusetts Institute of Technology
**	Joe Wild, Technical Languages and Environments, DECspec project
**	Yang Meng Tan,
**         Massachusetts Institute of Technology
**
**  CREATION DATE:
**
**	20 January 1991
*/

# include "splintMacros.nf"
# include "basic.h"
# include "gram.h"
# include "lclscan.h"
# include "lclsyntable.h"
# include "lslparse.h"

/*@+ignorequals@*/

static bool isBlankLine (char *p_line);
static bool inImport = FALSE;

/*@constant static int MAXBUFFLEN;@*/
# define MAXBUFFLEN 512
/*@constant static int DELTA;@*/
# define DELTA 100

static void symHashTable_dump (symHashTable * p_t, FILE * p_f, bool p_lco);

static void tagInfo_free (/*@only@*/ tagInfo p_tag);
static /*@observer@*/ scopeInfo symtable_scopeInfo (symtable p_stable);

static void symtable_dumpId (symtable p_stable, FILE *p_f, bool p_lco);
static lsymbol nameNode2key (nameNode p_n);

typedef enum
{
  SYMK_FCN, SYMK_SCOPE, SYMK_TYPE, SYMK_VAR
} symKind;

typedef struct
{
  symKind kind;
  union
  {
    /*@only@*/ fctInfo fct;
    /*@only@*/ scopeInfo scope;
    /*@only@*/ typeInfo type;
    /*@only@*/ varInfo var;
  } info;
} idTableEntry;

typedef struct
{
  unsigned int size;
  unsigned int allocated;
  /*@relnull@*/ idTableEntry *entries;
  bool exporting;
} idTable;

struct s_symtableStruct
{
  idTable *idTable;		/* data is idTableEntry */
  symHashTable *hTable;		/* data is htData */
  mapping type2sort;		/* maps LCL type symbol to LSL sort */
} ;

static /*@observer@*/ ltoken idTableEntry_getId (idTableEntry *p_x);
static /*@out@*/ /*@exposed@*/ idTableEntry *nextFree (idTable * p_st);
static /*@dependent@*/ /*@null@*/ idTableEntry *symtable_lookup (idTable * p_st, lsymbol p_id);
static /*@dependent@*/ /*@null@*/ idTableEntry *symtable_lookupInScope (idTable * p_st, lsymbol p_id);

static /*@only@*/ idTable *symtable_newIdTable (void);
static void idTableEntry_free (idTableEntry p_x);

/* Local implementatio of hash table */

static bool allowed_redeclaration = FALSE;
static symbolKey htData_key (htData *p_x);

static void symHashTable_free (/*@only@*/ symHashTable *p_h);
static /*@only@*/ symHashTable *symHashTable_create (unsigned int p_size);
static /*@null@*/ /*@exposed@*/ htData *
  symHashTable_get (symHashTable * p_t, symbolKey p_key, infoKind p_kind, 
		 /*@null@*/ nameNode p_n);
static bool symHashTable_put (symHashTable *p_t, /*@only@*/ htData *p_data);
static /*@only@*/ /*@exposed@*/ /*@null@*/ htData *
  symHashTable_forcePut (symHashTable * p_t, /*@only@*/ htData *p_data);
/* static unsigned int symHashTable_count (symHashTable * t); */

static void idTable_free (/*@only@*/ idTable *p_st);

void varInfo_free (/*@only@*/ varInfo v)
{
  ltoken_free (v->id);
  sfree (v);
}

static /*@only@*/ varInfo varInfo_copy (varInfo v)
{
  varInfo ret = (varInfo) dmalloc (sizeof (*ret));

  ret->id = ltoken_copy (v->id);
  ret->sort = v->sort;
  ret->kind = v->kind;
  ret->export = v->export;

  return ret;
}

void symtable_free (symtable stable)
{
  /* symtable_printStats (stable); */

  idTable_free (stable->idTable);
  symHashTable_free (stable->hTable);
  mapping_free (stable->type2sort);
  sfree (stable);
}

static void idTable_free (idTable *st)
{
  unsigned int i;

  for (i = 0; i < st->size; i++)
    {
      idTableEntry_free (st->entries[i]);
    }

  sfree (st->entries);
  sfree (st);
}

static void fctInfo_free (/*@only@*/ fctInfo f)
{
  signNode_free (f->signature);
  pairNodeList_free (f->globals);
  ltoken_free (f->id);
  sfree (f);
}

static void typeInfo_free (/*@only@*/ typeInfo t)
{
  ltoken_free (t->id);
  sfree (t);
}

static void scopeInfo_free (/*@only@*/ scopeInfo s)
{
  sfree (s);
}

static void idTableEntry_free (idTableEntry x)
{
  switch (x.kind)
    {
    case SYMK_FCN:
      fctInfo_free (x.info.fct);
      break;
    case SYMK_SCOPE:
      scopeInfo_free (x.info.scope);
      break;
    case SYMK_TYPE:
      typeInfo_free (x.info.type);
      break;
    case SYMK_VAR:
      varInfo_free (x.info.var);
      break;
    }
}

static /*@observer@*/ ltoken idTableEntry_getId (idTableEntry *x)
{
  switch (x->kind)
    {
    case SYMK_FCN:
      return (x->info.fct->id);
    case SYMK_SCOPE:
      return ltoken_undefined;
    case SYMK_TYPE:
      return (x->info.type->id);
    case SYMK_VAR:
      return (x->info.var->id);
    }

  BADBRANCHRET (ltoken_undefined);
}

/*@only@*/ symtable
symtable_new (void)
{
  symtable stable = (symtable) dmalloc (sizeof (*stable));
  idTableEntry *e;
  
  stable->idTable = symtable_newIdTable ();
  stable->hTable = symHashTable_create (HT_MAXINDEX);
  stable->type2sort = mapping_create ();
  
  /* add builtin synonym:  Bool -> bool */
  
  mapping_bind (stable->type2sort, lsymbol_getBool (), lsymbol_getbool ());

  /*
  ** done by symtable_newIdTable
  ** st->allocated = 0;
  ** st->entries = (idTableEntry *) 0;
  ** st->exporting = TRUE;
  */

  /* this is global scope */
  e = nextFree (stable->idTable);
  e->kind = SYMK_SCOPE;
  (e->info).scope = (scopeInfo) dmalloc (sizeof (*((e->info).scope)));
  (e->info).scope->kind = SPE_GLOBAL;
  
  return stable;
}

static /*@only@*/ idTable *symtable_newIdTable (void)
{
  idTable *st = (idTable *) dmalloc (sizeof (*st));

  st->size = 0;
  st->allocated = 0;
  st->entries = (idTableEntry *) 0;
  st->exporting = TRUE;
  
  /* this was being done twice!
     e = nextFree (st);
     e->kind = SYMK_SCOPE;
     (e->info).scope.kind = globScope;
     */

  return st;
}

static lsymbol
nameNode2key (nameNode n)
{
  unsigned int ret;

  if (n->isOpId)
    {
      ret =  ltoken_getText (n->content.opid);
    }
  else
    {
      /* use opForm's key as its Identifier */
      llassert (n->content.opform != NULL);
      ret = (n->content.opform)->key;
    }

  return ret;
}

/*
** requires: nameNode n is already in st.
*/

static bool
htData_insertSignature (htData *d, /*@owned@*/ sigNode oi)
{
  sigNodeSet set = d->content.op->signatures;

  
  if (oi != (sigNode) 0)
    {
      return (sigNodeSet_insert (set, oi));
    }
  return FALSE;
}

void
symtable_enterOp (symtable st, /*@only@*/ /*@notnull@*/ nameNode n, 
		  /*@owned@*/ sigNode oi)
{
  /*
  ** Operators are overloaded, we allow entering opInfo more than once,
  ** even if it's the same signature. 
  **
  ** Assumes all sorts are already entered into the symbol table 
  */

  symHashTable *ht = st->hTable;
  htData *d;
  lsymbol id;

  
  
  id = nameNode2key (n);

  d = symHashTable_get (ht, id, IK_OP, n);
  
  if (d == (htData *) 0)
    {				/* first signature of this operator */
      opInfo op = (opInfo) dmalloc (sizeof (*op));
      htData *nd = (htData *) dmalloc (sizeof (*nd));

      op->name = n;

      if (oi != (sigNode) 0)
	{
	  op->signatures = sigNodeSet_singleton (oi);
	  ht->count++;
	}
      else
	{
	  op->signatures = sigNodeSet_new ();
	  sigNode_markOwned (oi);
	}

      nd->kind = IK_OP;
      nd->content.op = op;
      (void) symHashTable_put (ht, nd);
    }
  else
    {
      
      nameNode_free (n);  /*<<<??? */

      if (htData_insertSignature (d, oi))
	{
	  ht->count++;
	}
    }
}

bool
  symtable_enterTag (symtable st, tagInfo ti)
{
  /* put ti only if it is not already in symtable */
  symHashTable *ht = st->hTable;
  htData *d;
  symbolKey key = ltoken_getText (ti->id);

  d = symHashTable_get (ht, key, IK_TAG, (nameNode) 0);
  if (d == (htData *) 0)
    {
      d = (htData *) dmalloc (sizeof (*d));
      d->kind = IK_TAG;
      d->content.tag = ti;
      d->content.tag->imported = context_inImport ();
      (void) symHashTable_put (ht, d);
      return TRUE;
    }
  else
    {
      if (d->content.tag->imported)
	{
	  d->content.tag = ti;
	  d->content.tag->imported = context_inImport ();
	  return TRUE;
	}
      else
	{
	  tagInfo_free (ti);
	  return FALSE;
	}
    }
}

bool
symtable_enterTagForce (symtable st, tagInfo ti)
{
 /* put ti, force-put if necessary */
  symHashTable *ht = st->hTable;
  htData *d;
  symbolKey key = ltoken_getText (ti->id);

  d = symHashTable_get (ht, key, IK_TAG, (nameNode) 0);

  if (d == (htData *) 0)
    {
      d = (htData *) dmalloc (sizeof (*d));
      
      d->kind = IK_TAG;
      d->content.tag = ti;
      d->content.tag->imported = context_inImport ();
      (void) symHashTable_put (ht, d);
      return TRUE;
    }
  else
    {
            
      d->kind = IK_TAG;
      d->content.tag = ti;
      d->content.tag->imported = context_inImport ();
      /* interpret return data later, htData * */
      /*@i@*/ (void) symHashTable_forcePut (ht, d);
      return FALSE;
    }
}

/*@null@*/ opInfo
symtable_opInfo (symtable st, /*@notnull@*/ nameNode n)
{
  symHashTable *ht = st->hTable;
  lsymbol i = nameNode2key (n);

  htData *d;
  d = symHashTable_get (ht, i, IK_OP, n);
  if (d == (htData *) 0)
    {
      return (opInfo)NULL;
    }

  return (d->content.op);
}

/*@null@*/ tagInfo
symtable_tagInfo (symtable st, lsymbol i)
{
  symHashTable *ht = st->hTable;
  htData *d;
  d = symHashTable_get (ht, i, IK_TAG, 0);

  if (d == (htData *) 0)
    {
      return (tagInfo) NULL;
    }

  return (d->content.tag);
}

void
  symtable_enterScope (symtable stable, scopeInfo si)
{
  idTable *st = stable->idTable;
  idTableEntry *e = nextFree (st);
  if (si->kind == SPE_GLOBAL)
    llbuglit ("symtable_enterScope: SPE_GLOBAL");
  e->kind = SYMK_SCOPE;
  (e->info).scope = si;
}

void
symtable_exitScope (symtable stable)
{
  idTable *st = stable->idTable;
  int n;

  if (st->entries != NULL)
    {
      for (n = st->size - 1; (st->entries[n]).kind != SYMK_SCOPE; n--)
	{
	  ;
	}
    }
  else
    {
      llcontbuglit ("symtable_exitScope: no scope to exit");
      n = 0;
    }

  st->size = n;
}

bool
symtable_enterFct (symtable stable, fctInfo fi)
{
  idTable *st = stable->idTable;
  idTableEntry *e;
  bool redecl = FALSE;

  if (!allowed_redeclaration &&
      symtable_lookup (st, ltoken_getText (fi->id)) != (idTableEntry *) 0)
    {
      lclRedeclarationError (fi->id);
      redecl = TRUE;
    }
  
  e = nextFree (st);
  e->kind = SYMK_FCN;
  fi->export = st->exporting;	/* && !fi->private; */
  (e->info).fct = fi;

  return redecl;
}

void
symtable_enterType (symtable stable, /*@only@*/ typeInfo ti)
{
  idTable *st = stable->idTable;
  idTableEntry *e;
  bool insertp = TRUE;
  scopeKind k = (symtable_scopeInfo (stable))->kind;

  /* symtable_disp (stable); */

  if (k != SPE_GLOBAL && k != SPE_INVALID)	/* fixed for Splint */
    {
      llbug (message ("%q: symtable_enterType: expect global scope. (type: %s)",
		      ltoken_unparseLoc (ti->id),
		      ltoken_getRawString (ti->id)));
    }

  if (!allowed_redeclaration &&
      symtable_lookup (st, ltoken_getText (ti->id)) != (idTableEntry *) 0)
    {
     /* ignore if Bool is re-entered */
      if (ltoken_getText (ti->id) == lsymbol_getBool () ||
	  ltoken_getText (ti->id) == lsymbol_getbool ())
	{
	  insertp = FALSE;
	}
      else
	{
	  lclRedeclarationError (ti->id);
	}
    }
  if (insertp)
    {
      /* make sure it is a type TYPEDEF_NAME; */
      
      if (ltoken_getCode (ti->id) != LLT_TYPEDEF_NAME)
	{
	  lclbug (message ("symtable_enterType: gets a simpleId, expect a type: %s",
			   ltoken_getRawString (ti->id)));
	}
      
      e = nextFree (st);
      e->kind = SYMK_TYPE;
      ti->export = st->exporting;/* && !ti->private; */
      (e->info).type = ti;
      mapping_bind (stable->type2sort, ltoken_getText (ti->id),
		    sort_getLsymbol (sort_makeVal (sort_getUnderlying (ti->basedOn))));
    }
  else
    {
      typeInfo_free (ti);
    }
}

lsymbol
lsymbol_sortFromType (symtable s, lsymbol typename)
{
  lsymbol inter;
  lsymbol out;
  ltoken tok;
 /* check the synonym table first */
  if (LCLIsSyn (typename))
    {
      tok = LCLGetTokenForSyn (typename);
      inter = ltoken_getText (tok);
     /*    printf ("In lsymbol_sortFromType: %s -> %s\n",
                lsymbol_toChars (typename), lsymbol_toChars (inter)); */
    }
  else
    {
      inter = typename;
    }

  /* now map LCL type to sort */
  out = mapping_find (s->type2sort, inter);
  
  if (out == lsymbol_undefined)
    {
      return inter;
    }

  return out;
}

/* really temp! */

/*
** returns true is vi is a redeclaration
*/

bool
symtable_enterVar (symtable stable, /*@temp@*/ varInfo vi)
{
  idTable *st = stable->idTable;
  bool insertp = TRUE;
  bool redecl = FALSE;

  
  /* symtable_disp (symtab); */
  
  if (!allowed_redeclaration &&
      (symtable_lookupInScope (st, ltoken_getText (vi->id)) != (idTableEntry *) 0))
    {
      if (ltoken_getText (vi->id) == lsymbol_getTRUE () ||
	  ltoken_getText (vi->id) == lsymbol_getFALSE ())
	{
	  insertp = FALSE;
	}
      else
	{
	  if (usymtab_existsEither (ltoken_getRawString (vi->id)))
	    {
	      	      lclRedeclarationError (vi->id);
	      redecl = TRUE;
	    }
	  else
	    {
	      llbuglit ("redeclared somethingerother?!");
	    }
	}
    }

  if (insertp)
    {
      idTableEntry *e = nextFree (st);

      e->kind = SYMK_VAR;
      vi->export = st->exporting &&	/* !vi.private && */
	(vi->kind == VRK_VAR || vi->kind == VRK_CONST || vi->kind == VRK_ENUM);
      (e->info).var = varInfo_copy (vi);
    }
  
    return (redecl);
}

bool
symtable_exists (symtable stable, lsymbol i)
{
  idTable *st = stable->idTable;
  return symtable_lookup (st, i) != (idTableEntry *) 0;
}

/*@null@*/ typeInfo
symtable_typeInfo (symtable stable, lsymbol i)
{
  idTable *st;
  idTableEntry *e;

  st = stable->idTable;
  e = symtable_lookup (st, i);

  if (e == (idTableEntry *) 0 || e->kind != SYMK_TYPE)
    {
      return (typeInfo) NULL;
    }

  return (e->info).type;
}

/*@null@*/ varInfo
symtable_varInfo (symtable stable, lsymbol i)
{
  idTable *st = stable->idTable;
  idTableEntry *e;

  e = symtable_lookup (st, i);

  if (e == (idTableEntry *) 0 || e->kind != SYMK_VAR)
    {
      return (varInfo) NULL;
    }

  return (e->info).var;
}

/*@null@*/ varInfo
symtable_varInfoInScope (symtable stable, lsymbol id)
{
  /* if current scope is a SPE_QUANT, can go beyond current scope */
  idTable *st = stable->idTable;
  idTableEntry *e2 = (idTableEntry *) 0;
  int n;
  
  for (n = st->size - 1; n >= 0; n--)
    {
      ltoken tok;

      e2 = &(st->entries[n]);
      
      if (e2->kind == SYMK_SCOPE && e2->info.scope->kind != SPE_QUANT)
	{
	  return (varInfo) NULL;
	}

      tok = idTableEntry_getId (e2);

      if (e2->kind == SYMK_VAR && ltoken_getText (tok) == id)
	{
	  return (e2->info).var;
	}
    }

  return (varInfo) NULL;
}

scopeInfo
symtable_scopeInfo (symtable stable)
{
  idTable *st = stable->idTable;
  int n;
  idTableEntry *e;

  for (n = st->size - 1; n >= 0; n--)
    {
      e = &(st->entries[n]);
      if (e->kind == SYMK_SCOPE)
	return (e->info).scope;
    }

  lclfatalbug ("symtable_scopeInfo: not found");
  BADEXIT;
}

void
symtable_export (symtable stable, bool yesNo)
{
  idTable *st = stable->idTable;
  st->exporting = yesNo;
  (void) sort_setExporting (yesNo);
}

static void
symHashTable_dump (symHashTable * t, FILE * f, bool lco)
{
  /* like symHashTable_dump2 but for output to .lcs file */
  int i, size;
  bucket *b;
  htEntry *entry;
  htData *d;
  ltoken tok;
  sigNodeSet sigs;
  
  for (i = 0; i <= HT_MAXINDEX; i++)
    {
            b = t->buckets[i];

      for (entry = b; entry != NULL; entry = entry->next)
	{
	  d = entry->data;

	  switch (d->kind)
	    {
	    case IK_SORT:
	      /*@switchbreak@*/ break;
	    case IK_OP:
	      {
		char *name = cstring_toCharsSafe (nameNode_unparse (d->content.op->name));
		sigs = d->content.op->signatures;
		size = sigNodeSet_size (sigs);

		
		sigNodeSet_elements (sigs, x)
		  {
		    cstring s = sigNode_unparse (x);

		    if (lco)
		      {
			fprintf (f, "%%LCL");
		      }

		    fprintf (f, "op %s %s\n", name, cstring_toCharsSafe (s));
		    cstring_free (s);
		  } end_sigNodeSet_elements;

		sfree (name);
		/*@switchbreak@*/ break;
	      }
	    case IK_TAG:
	      tok = d->content.tag->id;
	      
	      if (!ltoken_isUndefined (tok))
		{
		  cstring s = tagKind_unparse (d->content.tag->kind);

		  if (lco)
		    {
		      fprintf (f, "%%LCL");
		    }

		  fprintf (f, "tag %s %s\n", ltoken_getTextChars (tok), 
			   cstring_toCharsSafe (s));
		  cstring_free (s);
		}
	      /*@switchbreak@*/ break;
	    }
	}
    }
}

void
symtable_dump (symtable stable, FILE * f, bool lco)
{
  symHashTable *ht = stable->hTable;


  fprintf (f, "%s\n", BEGINSYMTABLE);
   
  symHashTable_dump (ht, f, lco);
   
  symtable_dumpId (stable, f, lco);
  
  fprintf (f, "%s\n", SYMTABLEEND);
  }

lsymbol
lsymbol_translateSort (mapping m, lsymbol s)
{
  lsymbol res = mapping_find (m, s);
  if (res == lsymbol_undefined)
    return s;
  return res;
}

static /*@null@*/ lslOp
  lslOp_renameSorts (mapping map,/*@returned@*/ /*@null@*/ lslOp op)
{
  sigNode sign;

  if (op != (lslOp) 0)
    {
      ltokenList domain;
      ltoken range;

      sign = op->signature;
      range = sign->range;
      domain = sign->domain;

      ltokenList_elements (domain, dt)
	{
	  ltoken_setText (dt, 
			  lsymbol_translateSort (map, ltoken_getText (dt)));
	} end_ltokenList_elements;

      /*@-onlytrans@*/ /* A memory leak... */
      op->signature = makesigNode (sign->tok, domain, range);
      /*@=onlytrans@*/
    }

  return op;
}

static /*@null@*/ signNode
  signNode_fromsigNode (sigNode s)
{
  signNode sign;
  sortList slist;
  
  if (s == (sigNode) 0)
    {
      return (signNode) 0;
    }
  
  sign = (signNode) dmalloc (sizeof (*sign));
  slist = sortList_new ();
  sign->tok = ltoken_copy (s->tok);
  sign->key = s->key;
  sign->range = sort_makeSort (ltoken_undefined, ltoken_getText (s->range));

  ltokenList_elements (s->domain, dt)
    {
      sortList_addh (slist, sort_makeSort (ltoken_undefined, ltoken_getText (dt)));
    } end_ltokenList_elements;

  sign->domain = slist;
  return sign;
}


/**  2.4.3 ymtan  93.09.23  -- fixed bug in parseGlobals: removed ";" at the
 **                            end of pairNode (gstr).
 */

static /*@only@*/ pairNodeList
parseGlobals (char *line, inputStream srce)
{
  pairNodeList plist = pairNodeList_new ();
  pairNode p;
  int semi_index;
  char *lineptr, sostr[MAXBUFFLEN], gstr[MAXBUFFLEN];

  /* line is not all blank */
  /* expected format: "sort1 g1; sort2 g2; sort3 g3;" */
  lineptr = line;
  
  while (!isBlankLine (lineptr))
    {
      if (sscanf (lineptr, "%s %s", &(sostr[0]), gstr) != 2)
	{
	  lclplainerror 
	    (message 
	     ("%q: Imported file contains illegal function global declaration.\n"
	      "Skipping rest of the line: %s (%s)",
	      fileloc_unparseRaw (inputStream_fileName (srce), 
				  inputStream_thisLineNumber (srce)), 
	      cstring_fromChars (line), 
	      cstring_fromChars (lineptr)));
	  return plist;
	}
      
      p = (pairNode) dmalloc (sizeof (*p));
      
      /* Note: remove the ";" separator at the end of gstr */
      semi_index = size_toInt (strlen (gstr));
      gstr[semi_index - 1] = '\0';

      p->tok = ltoken_create (NOTTOKEN, lsymbol_fromChars (gstr));
      p->sort = sort_makeSort (ltoken_undefined, lsymbol_fromChars (sostr));

      pairNodeList_addh (plist, p);
      lineptr = strchr (lineptr, ';');	/* go pass the next; */

      llassert (lineptr != NULL);
      lineptr = lineptr + 1;
    }

  return plist;
}

static bool
isBlankLine (char *line)
{
  int i;

  if (line == NULL) return TRUE;

  for (i = 0; line[i] != '\0'; i++)
    {
      if (line[i] == ' ')
	continue;
      if (line[i] == '\t')
	continue;
      if (line[i] == '\n')
	return TRUE;
      return FALSE;
    }
  return TRUE;
}

typedef /*@only@*/ fctInfo o_fctInfo;

static void
parseLine (char *line, inputStream srce, mapping map)
{
  static /*@owned@*/ o_fctInfo *savedFcn = NULL;
  char *lineptr, *lineptr2;
  cstring importfile = inputStream_fileName (srce);
  char namestr[MAXBUFFLEN], kstr[20], sostr[MAXBUFFLEN];
  sort bsort, nullSort = sort_makeNoSort ();
  int col = 0;
  fileloc imploc = fileloc_undefined;

  
  if (inImport)
    {
      imploc = fileloc_createImport (importfile, inputStream_thisLineNumber (srce));
    }
  
  if (firstWord (line, "op"))
    {
      lslOp op;

      lineptr = strchr (line, 'o');	/* remove any leading blanks */
      llassert (lineptr != NULL);
      lineptr = strchr (lineptr, ' ');	/* go past "op" */
      llassert (lineptr != NULL);

      /* add a newline to the end of the line since parseOpLine expects it */
      lineptr2 = strchr (lineptr, '\0');

      if (lineptr2 != 0)
	{
	  *lineptr2 = '\n';
	  *(lineptr2 + 1) = '\0';
	}

      llassert (cstring_isDefined (importfile));
      op = parseOpLine (importfile, cstring_fromChars (lineptr + 1));
      
      if (op == (lslOp) 0)
	{
	  lclplainerror
	    (message
	     ("%q: Imported file contains illegal operator declaration:\n "
	      "skipping this line: %s",
	      fileloc_unparseRaw (importfile, inputStream_thisLineNumber (srce)), 
	      cstring_fromChars (line)));
	  fileloc_free (imploc);
	  return;
	}

            op = lslOp_renameSorts (map, op);

      llassert (op != NULL);
      llassert (op->name != NULL);

      symtable_enterOp (g_symtab, op->name, 
			sigNode_copy (op->signature));
          /*@-mustfree@*/ } /*@=mustfree@*/
  else if (firstWord (line, "type"))
    {
      typeInfo ti;

      if (sscanf (line, "type %s %s %s", namestr, sostr, kstr) != 3)
	{
	  lclplainerror 
	    (message ("%q: illegal type declaration:\n skipping this line: %s",
		      fileloc_unparseRaw (importfile, inputStream_thisLineNumber (srce)), 
		      cstring_fromChars (line)));
	  fileloc_free (imploc);
	  return;
	}
      
      ti = (typeInfo) dmalloc (sizeof (*ti));
      ti->id = ltoken_createFull (LLT_TYPEDEF_NAME, lsymbol_fromChars (namestr),
				    importfile, inputStream_thisLineNumber (srce), col);

      bsort = sort_lookupName (lsymbol_translateSort (map, lsymbol_fromChars (sostr)));

      if (sort_isNoSort (bsort))
	{
	  lineptr = strchr (line, ' ');	/* go past "type" */
	  llassert (lineptr != NULL);
	  lineptr = strchr (lineptr + 1, ' ');	/* go past namestr */
	  llassert (lineptr != NULL);
	  col = 5 + ((int) (lineptr - line));	/* 5 for initial "%LCL "*/

	  lclbug (message ("%q: Imported files contains unknown base sort",
			   fileloc_unparseRawCol (importfile, inputStream_thisLineNumber (srce), col)));

	  bsort = nullSort;
	}
      ti->basedOn = bsort;

      if (strcmp (kstr, "exposed") == 0)
	{
	  ti->abstract = FALSE;
	  ti->modifiable = TRUE;
	}
      else
	{
	  ti->abstract = TRUE;
	  if (strcmp (kstr, "mutable") == 0)
	    ti->modifiable = TRUE;
	  else
	    ti->modifiable = FALSE;
	}
      ti->export = TRUE;
      
      /* 
      ** sort of a hack to get imports to work...
      */
      
      if (inImport)
	{
	  cstring cnamestr = cstring_fromChars (namestr);

	  if (!usymtab_existsGlobEither (cnamestr))
	    {
	      (void) usymtab_addEntry 
		(uentry_makeDatatype
		 (cnamestr, ctype_unknown,
		  ti->abstract ? ynm_fromBool (ti->modifiable) : MAYBE,
		  ti->abstract ? qual_createAbstract () : qual_createConcrete (),
		  fileloc_copy (imploc)));
	    }
	}

      symtable_enterType (g_symtab, ti);
    }
  else if (firstWord (line, "var"))
    {
      varInfo vi;

      if (sscanf (line, "var %s %s", namestr, sostr) != 2)
	{
	  lclplainerror
	    (message ("%q: Imported file contains illegal variable declaration.  "
		      "Skipping this line.", 
		      fileloc_unparseRaw (importfile, inputStream_thisLineNumber (srce))));
	  fileloc_free (imploc);
	  return;
	}

      vi = (varInfo) dmalloc (sizeof (*vi));
      bsort = sort_lookupName (lsymbol_translateSort (map, lsymbol_fromChars (sostr)));
      lineptr = strchr (line, ' ');	/* go past "var" */
      llassert (lineptr != NULL);
      lineptr = strchr (lineptr + 1, ' ');	/* go past namestr */
      llassert (lineptr != NULL);
      col = 5 + ((int) (lineptr - line));	/* 5 for initial "%LCL "*/

      if (sort_isNoSort (bsort))
	{
	  lclplainerror (message ("%q: Imported file contains unknown base sort",
				  fileloc_unparseRawCol (importfile, inputStream_thisLineNumber (srce), col)));
	  bsort = nullSort;
	}

      vi->id = ltoken_createFull (simpleId, lsymbol_fromChars (namestr),
				  importfile, inputStream_thisLineNumber (srce), col);
      vi->sort = bsort;
      vi->kind = VRK_VAR;
      vi->export = TRUE;
      (void) symtable_enterVar (g_symtab, vi);
      varInfo_free (vi);

      if (inImport)
	{
	  cstring cnamestr = cstring_fromChars (namestr);

	  if (!usymtab_existsGlobEither (cnamestr))
	    {
	      
	      (void) usymtab_supEntrySref 
		(uentry_makeVariable (cnamestr, ctype_unknown, 
				      fileloc_copy (imploc), 
				      FALSE));
	    }
	}
    }
  else if (firstWord (line, "const"))
    {
      varInfo vi;

      if (sscanf (line, "const %s %s", namestr, sostr) != 2)
	{
	  lclbug (message ("%q: Imported file contains illegal constant declaration: %s",
			   fileloc_unparseRaw (importfile, inputStream_thisLineNumber (srce)), 
			   cstring_fromChars (line)));
	  fileloc_free (imploc);
	  return;
	}

      vi = (varInfo) dmalloc (sizeof (*vi));
      bsort = sort_lookupName (lsymbol_translateSort (map, lsymbol_fromChars (sostr)));
      lineptr = strchr (line, ' ');	/* go past "var" */
      llassert (lineptr != NULL);
      lineptr = strchr (lineptr + 1, ' ');	/* go past namestr */
      llassert (lineptr != NULL);
      
      col = 5 + ((int) (lineptr - line));	/* 5 for initial "%LCL "*/

      if (sort_isNoSort (bsort))
	{
	  lclplainerror (message ("%q: Imported file contains unknown base sort",
				  fileloc_unparseRawCol (importfile, inputStream_thisLineNumber (srce), col)));
	  bsort = nullSort;
	}

      vi->id = ltoken_createFull (simpleId, lsymbol_fromChars (namestr),
				    importfile, inputStream_thisLineNumber (srce), col);
      vi->sort = bsort;
      vi->kind = VRK_CONST;
      vi->export = TRUE;
      (void) symtable_enterVar (g_symtab, vi);
      varInfo_free (vi);
      
      if (inImport)
	{
	  cstring cnamestr = cstring_fromChars (namestr);
	  
	  if (!usymtab_existsGlobEither (cnamestr))
	    {
	      	      
	      (void) usymtab_addEntry (uentry_makeConstant (cnamestr,
							    ctype_unknown,
							    fileloc_copy (imploc)));
	    }
	}
      /* must check for "fcnGlobals" before "fcn" */
    }
  else if (firstWord (line, "fcnGlobals"))
    {
      pairNodeList globals;
      lineptr = strchr (line, 'f');	/* remove any leading blanks */
      llassert (lineptr != NULL);
      lineptr = strchr (lineptr, ' ');	/* go past "fcnGlobals" */
      llassert (lineptr != NULL);

     /* a quick check for empty fcnGlobals */
      if (!isBlankLine (lineptr))
	{
	  globals = parseGlobals (lineptr, srce);
	  /* should ensure that each global in an imported function
	     corresponds to some existing global.  Since only
	     "correctly processed" .lcs files are imported, this is
	     true as an invariant. */
	}
      else
	{
	  globals = pairNodeList_new ();
	}
      
      /* check that they exist, store them on fctInfo */

      if (savedFcn != NULL)
	{
	  pairNodeList_free ((*savedFcn)->globals);
	  (*savedFcn)->globals = globals;  /* evs, moved inside if predicate */

	  (void) symtable_enterFct (g_symtab, *savedFcn);
	  savedFcn = NULL;
	}
      else
	{
	  lclplainerror 
	    (message ("%q: Unexpected function globals.  "
		      "Skipping this line: \n%s",
		      fileloc_unparseRaw (importfile, inputStream_thisLineNumber (srce)), 
		      cstring_fromChars (line)));
	  savedFcn = NULL;
	  pairNodeList_free (globals);
	}
    }
  else if (firstWord (line, "fcn"))
    {
      lslOp op;
      lslOp op2; 

      if (savedFcn != (fctInfo *) 0)
	{
	  lclplainerror 
	    (message ("%q: illegal function declaration.  Skipping this line:\n%s",
		      fileloc_unparseRaw (importfile, inputStream_thisLineNumber (srce)), 
		      cstring_fromChars (line)));
	  fileloc_free (imploc);
	  return;
	}

      savedFcn = (fctInfo *) dmalloc (sizeof (*savedFcn));

      lineptr = strchr (line, 'f');	/* remove any leading blanks */
      llassert (lineptr != NULL);
      lineptr = strchr (lineptr, ' ');	/* go past "fcn" */
      llassert (lineptr != NULL);

      /* add a newline to the end of the line since parseOpLine expects it */

      lineptr2 = strchr (lineptr, '\0');

      if (lineptr2 != 0)
	{
	  *lineptr2 = '\n';
	  *(lineptr2 + 1) = '\0';
	}

      op = parseOpLine (importfile, cstring_fromChars (lineptr + 1));

      if (op == (lslOp) 0)
	{
	  lclplainerror
	    (message ("%q: illegal function declaration: %s",
		      fileloc_unparseRaw (importfile, inputStream_thisLineNumber (srce)), 
		      cstring_fromChars (line)));
	  fileloc_free (imploc);
	  return;
	}
	
      op2 = lslOp_renameSorts (map, op);

      llassert (op2 != NULL);

      if ((op->name != NULL) && op->name->isOpId)
	{
	  (*savedFcn) = (fctInfo) dmalloc (sizeof (**savedFcn));
	  (*savedFcn)->id = op->name->content.opid;
	  (*savedFcn)->signature = signNode_fromsigNode (op2->signature);
	  (*savedFcn)->globals = pairNodeList_new ();
	  (*savedFcn)->export = TRUE;
	  
	  if (inImport)
	    {
	      /* 27 Jan 1995 --- added this to be undefined namestr bug */
	      cstring fname = ltoken_unparse ((*savedFcn)->id);
	      
	      if (!usymtab_existsGlobEither (fname))
		{
		  (void) usymtab_addEntry (uentry_makeFunction
					   (fname, ctype_unknown, 
					    typeId_invalid, globSet_new (),
					    sRefSet_undefined, 
					    warnClause_undefined,
					    fileloc_copy (imploc)));
		}
	    }
	}
      else
	{
	  /* evans 2001-05-27: detected by splint after fixing external alias bug. */
	  if (op->name != NULL) 
	    {
	      ltoken_free (op->name->content.opid); 
	    }

	  lclplainerror 
	    (message ("%q: unexpected function name: %s",
		      fileloc_unparseRaw (importfile, inputStream_thisLineNumber (srce)), 
		      cstring_fromChars (line)));
	}
    }
  else if (firstWord (line, "enumConst"))
    {
      varInfo vi;

      if (sscanf (line, "enumConst %s %s", namestr, sostr) != 2)
	{
	  lclplainerror 
	    (message ("%q: Illegal enum constant declaration.  "
		      "Skipping this line:\n%s",
		      fileloc_unparseRaw (importfile, inputStream_thisLineNumber (srce)), 
		      cstring_fromChars (line)));
	  fileloc_free (imploc);
	  return;
	}
      
      vi = (varInfo) dmalloc (sizeof (*vi));
      bsort = sort_lookupName (lsymbol_translateSort (map, lsymbol_fromChars (sostr)));
      lineptr = strchr (line, ' ');	/* go past "var" */
      llassert (lineptr != NULL);
      lineptr = strchr (lineptr + 1, ' ');	/* go past namestr */
      llassert (lineptr != NULL);

      col = 5 + ((int) (lineptr - line));	/* 5 for initial "%LCL "*/
      if (sort_isNoSort (bsort))
	{
	  lclplainerror (message ("%q: unknown base sort\n",
				  fileloc_unparseRawCol (importfile, inputStream_thisLineNumber (srce), col)));
	  bsort = nullSort;
	}

      vi->id = ltoken_createFull (simpleId, lsymbol_fromChars (namestr),
				    importfile, inputStream_thisLineNumber (srce), col);
			
      vi->sort = bsort;
      vi->kind = VRK_ENUM;
      vi->export = TRUE;
      (void) symtable_enterVar (g_symtab, vi);
      varInfo_free (vi);

      if (inImport)
	{
	  cstring cnamestr = cstring_fromChars (namestr);
	  if (!usymtab_existsEither (cnamestr))
	    {
	      	      (void) usymtab_addEntry (uentry_makeConstant (cnamestr, ctype_unknown,
							    fileloc_copy (imploc)));
	    }
	}
    }
  else if (firstWord (line, "tag"))
    {
     /* do nothing, sort processing already handles this */
    }
  else
    {
      lclplainerror 
	(message ("%q: Unknown symbol declaration.  Skipping this line:\n%s",
		  fileloc_unparseRaw (importfile, inputStream_thisLineNumber (srce)), 
		  cstring_fromChars (line)));
    }

    fileloc_free (imploc);
}

void
symtable_import (inputStream imported, ltoken tok, mapping map)
{
  char *buf;
  cstring importfile;
  inputStream lclsource;
  int old_lsldebug;
  bool old_inImport = inImport;

  buf = inputStream_nextLine (imported);
  importfile = inputStream_fileName (imported);

  llassert (buf != NULL);

  if (!firstWord (buf, "%LCLSymbolTable"))
    {
      lclsource = LCLScanSource ();
      lclfatalerror (tok, 
		     message ("Expecting '%%LCLSymbolTable' line in import file %s:\n%s\n",
			      importfile, 
			      cstring_fromChars (buf)));
    }

  old_lsldebug = lsldebug;
  lsldebug = 0;
  allowed_redeclaration = TRUE;
  inImport = TRUE;

  for (;;)
    {
      buf = inputStream_nextLine (imported);
      llassert (buf != NULL);

      
      if (firstWord (buf, "%LCLSymbolTableEnd"))
	{
	  break;
	}
      else
	{			/* a good line, remove %LCL from line first */
	  if (firstWord (buf, "%LCL"))
	    {
	      parseLine (buf + 4, imported, map);
	    }
	  else
	    {
	      lclsource = LCLScanSource ();
	      lclfatalerror 
		(tok,
		 message ("Expecting '%%LCL' prefix in import file %s:\n%s\n",
			  importfile, 
			  cstring_fromChars (buf)));
	    }
	}
    }

  /* restore old value */
  inImport = old_inImport;
  lsldebug = old_lsldebug;
  allowed_redeclaration = FALSE;
  }

static void
symtable_dumpId (symtable stable, FILE *f, bool lco)
{
  idTable *st = stable->idTable;
  unsigned int i;
  idTableEntry *se;
  fctInfo fi;
  typeInfo ti;
  varInfo vi;

  for (i = 1; i < st->size; i++)
    {
      /* 2/22/93 I think there is a off-by-1 error, 0 entry is never used */
      se = st->entries + i;
      llassert (se != NULL);
      
      
      /*@-loopswitchbreak@*/
      switch (se->kind)
	{
	case SYMK_FCN:
	  {
	    cstring tmp;
	    
	    fi = (se->info).fct;
	    
	    if (lco)
	      {
		fprintf (f, "%%LCL");
	      }

	    if (!lco && !fi->export)
	      {
		fprintf (f, "spec ");
	      }
	    
	    tmp = signNode_unparse (fi->signature);
	    fprintf (f, "fcn %s %s \n", ltoken_getTextChars (fi->id), 
		     cstring_toCharsSafe (tmp));
	    cstring_free (tmp);
	    
	    tmp = pairNodeList_unparse (fi->globals);
	    fprintf (f, "%%LCLfcnGlobals %s\n", cstring_toCharsSafe (tmp));
	    cstring_free (tmp);
	    break;
	  }
	case SYMK_SCOPE:
	  if (lco)
	    {
	      break;
	    }

	  /*@-switchswitchbreak@*/
	  switch ((se->info).scope->kind)
	    {
	    case SPE_GLOBAL:
	      fprintf (f, "Global scope\n");
	      break;
	    case SPE_ABSTRACT:
	      fprintf (f, "Abstract type scope\n");
	      break;
	    case SPE_FCN:
	      fprintf (f, "Function scope\n");
	      break;
	     /* a let scope can only occur in a function scope, should not push
                a new scope, so symtable_lookupInScope works properly
                   case letScope:
	             fprintf (f, "Let scope\n");
	             break; */
	    case SPE_QUANT:
	      fprintf (f, "Quantifier scope\n");
	      break;
	    case SPE_CLAIM:
	      fprintf (f, "Claim scope\n");
	      break;
	    case SPE_INVALID:
	      break;
	    }
	  break;
	case SYMK_TYPE:
	  ti = (se->info).type;
	  if (lco)
	    fprintf (f, "%%LCL");
	  if (!lco && !ti->export)
	    fprintf (f, "spec ");
	  fprintf (f, "type %s %s",
		   ltoken_getTextChars (ti->id), sort_getName (ti->basedOn));
	  if (ti->abstract)
	    {
	      if (ti->modifiable)
		fprintf (f, " mutable\n");
	      else
		fprintf (f, " immutable\n");
	    }
	  else
	    fprintf (f, " exposed\n");
	  break;
	case SYMK_VAR:

	  vi = (se->info).var;
	  if (lco)
	    {
	      fprintf (f, "%%LCL");
	    }

	  if (!lco && !vi->export)
	    {
	      fprintf (f, "spec ");
	    }
	  switch (vi->kind)
	    {
	    case VRK_CONST:
	      fprintf (f, "const %s %s\n",
		       ltoken_getTextChars (vi->id), sort_getName (vi->sort));
	      break;
	    case VRK_VAR:
	      fprintf (f, "var %s %s\n",
		       ltoken_getTextChars (vi->id), sort_getName (vi->sort));
	      break;
	    case VRK_ENUM:
	      fprintf (f, "enumConst %s %s\n",
		       ltoken_getTextChars (vi->id), sort_getName (vi->sort));
	      break;
	    default:
	      if (lco)
		{
		  switch (vi->kind)
		    {
		    case VRK_GLOBAL:
		      fprintf (f, "global %s %s\n", ltoken_getTextChars (vi->id), sort_getName (vi->sort));
		      break;
		    case VRK_PRIVATE:	/* for private vars within function */
		      fprintf (f, "local %s %s\n",
			       ltoken_getTextChars (vi->id), sort_getName (vi->sort));
		      break;
		    case VRK_LET:
		      fprintf (f, "let %s %s\n",
			       ltoken_getTextChars (vi->id), sort_getName (vi->sort));
		      break;
		    case VRK_PARAM:
		      fprintf (f, "param %s %s\n",
			       ltoken_getTextChars (vi->id), sort_getName (vi->sort));
		      break;
		    case VRK_QUANT:
		      fprintf (f, "quant %s %s\n",
			       ltoken_getTextChars (vi->id), sort_getName (vi->sort));
		      break;
		    BADDEFAULT;
		    }
		  /*@=loopswitchbreak@*/
		  /*@=switchswitchbreak@*/
		}
	    }
	}
    }
}

static /*@exposed@*/ /*@out@*/ idTableEntry *
nextFree (idTable * st)
{
  idTableEntry *ret;
  unsigned int n = st->size;

  if (n >= st->allocated)
    {
      /*
      ** this loses with the garbage collector
      ** (and realloc is notoriously dangerous)
      **
      ** st->entries = (idTableEntry *) LSLMoreMem (st->entries, (n + DELTA) 
      **                                * sizeof (idTableEntry));
      **
      ** instead, we copy the symtable...
      */
      
      idTableEntry *oldentries = st->entries;
      unsigned int i;
      
      st->entries = dmalloc ((n+DELTA) * sizeof (*st->entries));
      
      for (i = 0; i < n; i++)
	{
	  st->entries[i] = oldentries[i];
	}
      
      sfree (oldentries);
      
      st->allocated = n + DELTA;
    }
  
  ret = &(st->entries[st->size]);
  st->size++;
  return ret;
}


static /*@dependent@*/ /*@null@*/ idTableEntry *
  symtable_lookup (idTable *st, lsymbol id)
{
  int n;
  idTableEntry *e;

  for (n = st->size - 1; n >= 0; n--)
    {
      e = &(st->entries[n]);

      /*@-loopswitchbreak@*/
      switch (e->kind)
	{
	case SYMK_SCOPE: 
	  break;
	case SYMK_FCN:
	  if (ltoken_getText (e->info.fct->id) == id) return e;
	  break;
	case SYMK_TYPE:
	  if (ltoken_getText (e->info.type->id) == id) return e;
	  break;
	case SYMK_VAR:
	  if (ltoken_getText (e->info.var->id) == id) return e;
	  break;
	BADDEFAULT;
	}
      /*@=loopswitchbreak@*/
    }

  return (idTableEntry *) 0;
}


static /*@dependent@*/ /*@null@*/ idTableEntry *
  symtable_lookupInScope (idTable *st, lsymbol id)
{
  int n;
  idTableEntry *e;
  for (n = st->size - 1; n >= 0; n--)
    {
      e = &(st->entries[n]);
      if (e->kind == SYMK_SCOPE)
	break;
      if (ltoken_getText (e->info.fct->id) == id)
	{
	  return e;
	}
    }
  return (idTableEntry *) 0;
}

/* hash table implementation */

static symbolKey
htData_key (htData * x)
{
  /* assume x points to a valid htData struct */
  switch (x->kind)
    {
    case IK_SORT:
      return x->content.sort;
    case IK_OP:
      {				/* get the textSym of the token */
	nameNode n = (x->content.op)->name;

	if (n->isOpId)
	  {
	    return ltoken_getText (n->content.opid);
	  }
	else
	  {
	    llassert (n->content.opform != NULL);
	    return (n->content.opform)->key;
	  }
      }
    case IK_TAG:
      return ltoken_getText ((x->content).tag->id);
    }
  BADEXIT;
}

static void htData_free (/*@null@*/ /*@only@*/ htData *d)
{
  if (d != NULL)
    {
      switch (d->kind)
	{
	case IK_SORT:
	  break;
	case IK_OP:
	  /* nameNode_free (d->content.op->name);*/
	  sigNodeSet_free (d->content.op->signatures);
	  break;
	case IK_TAG:
	  {
	    switch (d->content.tag->kind)
	      {
	      case TAG_STRUCT:
	      case TAG_UNION:
	      case TAG_FWDSTRUCT:
	      case TAG_FWDUNION:
		/*
		 ** no: stDeclNodeList_free (d->content.tag->content.decls);
		 **     it is dependent!
		 */
		/*@switchbreak@*/ break;
	      case TAG_ENUM:
		
		/* no: ltokenList_free (d->content.tag->content.enums);
		 **     it is dependent!
		 */
		
		/*@switchbreak@*/ break;
	      }
	  }
	}
      
      sfree (d);
    }
}

static void bucket_free (/*@null@*/ /*@only@*/ bucket *b)
{
  if (b != NULL)
    {
      bucket_free (b->next);
      htData_free (b->data);
      sfree (b);
    }
}

static void symHashTable_free (/*@only@*/ symHashTable *h)
{
  unsigned int i;

  for (i = 0; i < h->size; i++)
    {
      bucket_free (h->buckets[i]);
    }

  sfree (h->buckets);
  sfree (h);
}      
      
static /*@only@*/ symHashTable *
symHashTable_create (unsigned int size)
{
  unsigned int i;
  symHashTable *t = (symHashTable *) dmalloc (sizeof (*t));
  
  t->buckets = (bucket **) dmalloc ((size + 1) * sizeof (*t->buckets));
  t->count = 0;
  t->size = size;

  for (i = 0; i <= size; i++)
    {
      t->buckets[i] = (bucket *) NULL;
    }

  return t;
}

static /*@null@*/ /*@exposed@*/ htData *
symHashTable_get (symHashTable *t, symbolKey key, infoKind kind, /*@null@*/ nameNode n)
{
  bucket *b;
  htEntry *entry;
  htData *d;

  b = t->buckets[MASH (key, kind)];
  if (b == (bucket *) 0)
    {
      return ((htData *) 0);
    }
  
  for (entry = (htEntry *) b; entry != NULL; entry = entry->next)
    {
      d = entry->data;

      if (d->kind == kind && htData_key (d) == key)
	if (kind != IK_OP || sameNameNode (n, d->content.op->name))
	  {
	    return d;
	  }
    }
  return ((htData *) 0);
}

static bool
symHashTable_put (symHashTable *t, /*@only@*/ htData *data)
{
  /* if key is already taken, don't insert, return FALSE
     else insert it and return TRUE. */
  symbolKey key;
  htData *oldd;
  infoKind kind;
  nameNode name;

  key = htData_key (data);
  kind = data->kind;

  if (kind == IK_OP && (!data->content.op->name->isOpId))
    {
      name = data->content.op->name;
    }
  else
    {
      name = (nameNode) 0;
    }
  
  oldd = symHashTable_get (t, key, kind, name);

  if (oldd == (htData *) 0)
    {
      /*@-deparrays@*/
      bucket *new_entry = (bucket *) dmalloc (sizeof (*new_entry));
      bucket *b = (t->buckets[MASH (key, kind)]);
      htEntry *entry = (htEntry *) b;
      /*@=deparrays@*/

      new_entry->data = data;
      new_entry->next = entry;
      t->buckets[MASH (key, kind)] = new_entry;
      t->count++;

      return TRUE;
    }
  else
    {
      htData_free (data);
    }

  return FALSE;
}

static /*@only@*/ /*@exposed@*/ /*@null@*/ htData *
  symHashTable_forcePut (symHashTable *t, /*@only@*/ htData *data)
{
  /* Put data in, return old value */
  symbolKey key;
  bucket *b;
  htData *oldd;
  htEntry *entry, *new_entry;
  infoKind kind;
  nameNode name;

  key = htData_key (data);
  kind = data->kind;

  if (kind == IK_OP && (!data->content.op->name->isOpId))
    {
      name = data->content.op->name;
    }
  else
    {
      name = (nameNode) 0;
    }

  oldd = symHashTable_get (t, key, kind, name);

  if (oldd == (htData *) 0)
    {
      new_entry = (htEntry *) dmalloc (sizeof (*new_entry));

      /*@-deparrays@*/
      b = t->buckets[MASH (key, kind)];
      /*@=deparrays@*/

      entry = b;
      new_entry->data = data;
      new_entry->next = entry;
      t->buckets[MASH (key, kind)] = new_entry;
      t->count++;

      return NULL;
    }
  else
    {				/* modify in place */
      *oldd = *data;		/* copy new info over to old info */

      /* dangerous: if the data is the same, don't free it */
      if (data != oldd)   
	{
	  sfree (data); 
        /*@-branchstate@*/ 
	} 
      /*@=branchstate@*/

      return oldd;
    }
}

#if 0
static unsigned int
symHashTable_count (symHashTable * t)
{
  return (t->count);
}

#endif

static void
symHashTable_printStats (symHashTable * t)
{
  int i, bucketCount, setsize, sortCount, opCount, tagCount;
  int sortTotal, opTotal, tagTotal;
  bucket *b;
  htEntry *entry;
  htData *d;

  sortTotal = 0;
  opTotal = 0;
  tagTotal = 0;
  sortCount = 0;
  opCount = 0;
  tagCount = 0;

  /* for debugging only */
  printf ("\n Printing symHashTable stats ... \n");
  for (i = 0; i <= HT_MAXINDEX; i++)
    {
      b = t->buckets[i];
      bucketCount = 0;
      for (entry = b; entry != NULL; entry = entry->next)
	{
	  d = entry->data;
	  bucketCount++;
	  switch (d->kind)
	    {
	    case IK_SORT:
	      sortCount++;
	      /*@switchbreak@*/ break;
	    case IK_OP:
	      {
		cstring name = nameNode_unparse (d->content.op->name);
		cstring sigs = sigNodeSet_unparse (d->content.op->signatures);
		opCount++;
		/* had a tt? */
		setsize = sigNodeSet_size (d->content.op->signatures);
		printf ("       Op (%d): %s %s\n", setsize, 
			cstring_toCharsSafe (name), 
			cstring_toCharsSafe (sigs));
		cstring_free (name);
		cstring_free (sigs);
		/*@switchbreak@*/ break;
	      }
	    case IK_TAG:
	      tagCount++;
	      /*@switchbreak@*/ break;
	    }
	}
      if (bucketCount > 0)
	{
	  printf ("   Bucket %d has count = %d; opCount = %d; sortCount = %d; tagCount = %d\n", i, bucketCount, opCount, sortCount, tagCount);
	  sortTotal += sortCount;
	  tagTotal += tagCount;
	  opTotal += opCount;
	}
    }
  printf ("SymHashTable has total count = %d, opTotal = %d, sortTotal = %d, tagTotal = %d :\n", t->count, opTotal, sortTotal, tagTotal);

}

void
symtable_printStats (symtable s)
{
  symHashTable_printStats (s->hTable);
 /* for debugging only */
  printf ("idTable size = %d; allocated = %d\n",
	  s->idTable->size, s->idTable->allocated);
}

/*@only@*/ cstring
tagKind_unparse (tagKind k)
{
  switch (k)
    {
    case TAG_STRUCT:
    case TAG_FWDSTRUCT:
      return cstring_makeLiteral ("struct");
    case TAG_UNION:
    case TAG_FWDUNION:
      return cstring_makeLiteral ("union");
    case TAG_ENUM:
      return cstring_makeLiteral ("enum");
    }
  BADEXIT;
}

static void tagInfo_free (/*@only@*/ tagInfo tag)
{
  ltoken_free (tag->id);
  sfree (tag);
}

/*@observer@*/ sigNodeSet 
  symtable_possibleOps (symtable tab, nameNode n)
{
  opInfo oi = symtable_opInfo (tab, n);
  
  if (opInfo_exists (oi))
    {
      return (oi->signatures);
    }

  return sigNodeSet_undefined;
}

bool
symtable_opExistsWithArity (symtable tab, nameNode n, int arity)
{
  opInfo oi = symtable_opInfo (tab, n);

  if (opInfo_exists (oi))
    {
      sigNodeSet sigs = oi->signatures;
      sigNodeSet_elements (sigs, sig)
      {
	if (ltokenList_size (sig->domain) == arity)
	  return TRUE;
      } end_sigNodeSet_elements;
    }
  return FALSE;
}

static bool
domainMatches (ltokenList domain, sortSetList argSorts)
{
  /* expect their length to match */
  /* each domain sort in op must be an element of
     the corresponding set in argSorts. */
  bool matched = TRUE;
  sort s;

  sortSetList_reset (argSorts);
  ltokenList_elements (domain, dom)
    {
      s = sort_fromLsymbol (ltoken_getText (dom));
      if (!(sortSet_member (sortSetList_current (argSorts), s)))
	{
	  /*      printf ("   mismatched element is: %s in %s\n", ltoken_getTextChars (*dom),
		  sortSet_unparse (sortSetList_current (argSorts))); */
	  matched = FALSE;
	  break;
	}
      sortSetList_advance (argSorts);
  } end_ltokenList_elements;

  return matched;
}

/*@only@*/ lslOpSet
  symtable_opsWithLegalDomain (symtable tab, /*@temp@*/ /*@null@*/ nameNode n,
			       sortSetList argSorts, sort q)
{
 /* handles nil qual */
  lslOpSet ops = lslOpSet_new ();
  lslOp op;
  sort rangeSort;
  opInfo oi;

  llassert (n != NULL);
  oi = symtable_opInfo (tab, n);

  if (opInfo_exists (oi))
    {
      sigNodeSet sigs = oi->signatures;

      sigNodeSet_elements (sigs, sig)
	{
	  if (ltokenList_size (sig->domain) == sortSetList_size (argSorts))
	    {
	      rangeSort = sigNode_rangeSort (sig);
	      
	      if ((q == NULL) || (sort_equal (rangeSort, q)))
		{
		  if (domainMatches (sig->domain, argSorts))
		    {
		      op = (lslOp) dmalloc (sizeof (*op));
		      
		      /* each domain sort in op must be an element of
			 the corresponding set in argSorts. */
		      op->signature = sig;
		      op->name = nameNode_copy (n);
		      (void) lslOpSet_insert (ops, op);
		    }
		}
	    }
	} end_sigNodeSet_elements;
    }
  return ops;
}
