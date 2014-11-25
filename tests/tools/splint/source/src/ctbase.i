/* ;-*-C-*-; 
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
** For information on splint: splint@cs.virginia.edu
** To report a bug: splint-bug@cs.virginia.edu
** For more information: http://www.splint.org
*/
/*
** ctbase.i
**
** NOTE: This is not a stand-alone source file, but is included in ctype.c.
**       (This is necessary because there is no other way in C to have a
**       hidden scope, besides at the file level.)
*/

/*@access cprim*/

abst_typedef /*@null@*/ struct s_ctbase *ctbase;

/*@function static bool ctuid_isAnyUserType (sef ctuid p_cid) @*/

/*@-macrofcndecl@*/ /*@-macroparams@*/
# define ctuid_isAnyUserType(cid) \
   ((cid) == CT_ABST || (cid) == CT_USER || (cid) == CT_NUMABST)
/*@=macrofcndecl@*/ /*@=macroparams@*/

/*:private:*/ typedef struct {
  ctkind kind;
  ctbase ctbase; 
  ctype base;     /* type I point to (or element of array) */
  ctype ptr;     /* type of pointer to me */
  ctype array;    /* type of array of me */
  cstring unparse; /* unparse me, if memoized */
} *ctentry ;

typedef /*@only@*/ ctentry o_ctentry;
          
typedef struct {
  int size;
  int nspace;
  /*@relnull@*/ /*@only@*/ o_ctentry *entries;
  /* memoize matches...maybe in context? */
} cttable ;

extern bool ctentry_isBogus (/*@sef@*/ ctentry p_c) /*@*/;
# define ctentry_isBogus(c) \
   ((c)->kind == CTK_INVALID || (c)->kind == CTK_DNE)

static cttable cttab = { 0, 0, NULL };

static /*@notnull@*/ /*@only@*/ ctbase ctbase_createAbstract (typeId p_u);
static /*@notnull@*/ /*@only@*/ ctbase ctbase_createNumAbstract (typeId p_u);
static /*@observer@*/ cstring ctentry_doUnparse (ctentry p_c) /*@modifies p_c@*/;
static /*@only@*/ ctentry
  ctentry_make (ctkind p_ctk, /*@keep@*/ ctbase p_c, ctype p_base, 
		ctype p_ptr, ctype p_array, /*@keep@*/ cstring p_unparse);
static /*@only@*/ ctentry ctentry_makeNew (ctkind p_ctk, /*@only@*/ ctbase p_c);
static /*@only@*/ cstring ctentry_unparse (ctentry p_c) /*@*/ ;

static void cttable_grow (void);
static ctype cttable_addDerived (ctkind p_ctk, /*@keep@*/ ctbase p_cnew, ctype p_base);
static ctype cttable_addFull (/*@keep@*/ ctentry p_cnew);
static bool ctentry_isInteresting (ctentry p_c) /*@*/;
static /*@notnull@*/ /*@only@*/ ctbase ctbase_makeFixedArray (ctype p_b, size_t p_size) /*@*/ ;
static bool ctbase_isAnytype (/*@notnull@*/ ctbase p_b) /*@*/ ;

/* 
** These are file-static macros (used in ctype.c).  No way to
** declare them as static in C.
*/

/*@-allmacros@*/ /*@-macrospec@*/ /*@-namechecks@*/
# define ctentry_getBase(c)       ((c)->base)
# define ctentry_getKind(c)       ((c)->kind)
# define ctentry_getArray(c)      ((c)->array)
# define ctentry_getPtr(c)        ((c)->ptr)
# define ctentry_isArray(c)       ((c)->kind == CTK_ARRAY)
# define ctentry_isComplex(c)     ((c)->kind == CTK_COMPLEX)
# define ctentry_isPlain(c)       ((c)->kind == CTK_PLAIN)
# define ctentry_isPointer(c)     ((c)->kind == CTK_PTR)
# define ctentry_setArray(c,b)    ((c)->array = (b))
# define ctentry_setPtr(c,b)      ((c)->ptr = (b))

# define ctbase_fixUser(c)        (c = ctbase_realType(c))
/*@=allmacros@*/ /*@=macrospec@*/ /*@=namechecks@*/

static ctype cttable_addComplex (/*@notnull@*/ /*@only@*/ ctbase p_cnew);
static /*@observer@*/ ctbase ctype_getCtbase (ctype p_c) /*@*/ ;
static ctype ctype_makeConjAux (ctype p_c1, ctype p_c2, bool p_isExplicit) /*@*/ ;
static /*@notnull@*/ /*@observer@*/ ctbase ctype_getCtbaseSafe (ctype p_c) /*@*/ ;
static /*@observer@*/ ctentry ctype_getCtentry (ctype p_c)  /*@*/ ;
static /*@observer@*/ /*@notnull@*/ ctbase 
  ctbase_realType (/*@notnull@*/ ctbase p_c) /*@*/ ;
static bool ctbase_isPointer (/*@notnull@*/ /*@dependent@*/ ctbase p_c) /*@*/ ;
static bool ctbase_isEitherArray (/*@notnull@*/ /*@dependent@*/ ctbase p_c) /*@*/ ;
static /*@observer@*/ enumNameList ctbase_elist (ctbase p_c) /*@*/ ;
static /*@only@*/ cstring ctbase_unparse (ctbase p_c) /*@*/ ;
static /*@only@*/ cstring ctbase_unparseDeep (ctbase p_c) /*@*/ ;
static /*@only@*/ /*@notnull@*/ ctbase ctbase_copy (/*@notnull@*/ ctbase p_c) /*@*/ ;
static void ctbase_free (/*@only@*/ ctbase p_c);
static /*@notnull@*/ /*@only@*/ ctbase ctbase_createPrim (cprim p_p) /*@*/ ;
static /*@notnull@*/ /*@only@*/ ctbase ctbase_createBool (void) /*@*/ ;
static /*@notnull@*/ /*@observer@*/ ctbase ctbase_getBool (void) /*@*/ ;
static /*@notnull@*/ /*@only@*/ ctbase ctbase_createUser (typeId p_u) /*@*/ ;

static /*@notnull@*/ /*@only@*/ ctbase
  ctbase_createStruct (/*@only@*/ cstring p_n, /*@only@*/ uentryList p_f);

static /*@notnull@*/ /*@only@*/ ctbase 
  ctbase_createUnion  (/*@keep@*/ cstring p_n, /*@only@*/ uentryList p_f);
static /*@notnull@*/ /*@only@*/ ctbase ctbase_createEnum (/*@keep@*/ cstring p_etag, /*@keep@*/ enumNameList p_emembers);
static /*@notnull@*/ /*@only@*/ ctbase ctbase_createUnknown (void);
static bool ctbase_match (ctbase p_c1, ctbase p_c2) /*@modifies nothing@*/;
static bool ctbase_matchDef (ctbase p_c1, ctbase p_c2) /*@modifies nothing@*/;
static bool ctbase_genMatch (ctbase p_c1, ctbase p_c2, bool p_force, bool p_arg, bool p_def, bool p_deep);
static bool ctbase_isAbstract (/*@notnull@*/ ctbase p_c) /*@*/ ;
static /*@notnull@*/ /*@only@*/ ctbase ctbase_makePointer (ctype p_b) /*@*/ ;
static /*@notnull@*/ /*@only@*/ ctbase ctbase_makeArray (ctype p_b) /*@*/ ;
static /*@notnull@*/ ctype 
  ctbase_makeFunction (ctype p_b, /*@only@*/ uentryList p_p) /*@*/ ;
static /*@notnull@*/ /*@observer@*/ ctbase 
  ctbase_realFunction (/*@notnull@*/ /*@dependent@*/ ctbase p_c) /*@*/ ;
static ctype ctbase_baseArrayPtr (/*@notnull@*/ ctbase p_c) /*@*/ ;
static ctype ctbase_baseFunction (/*@notnull@*/ ctbase p_c) /*@*/ ;
static /*@observer@*/ uentryList ctbase_argsFunction (/*@notnull@*/ ctbase p_c) /*@*/ ;
static /*@observer@*/ uentryList ctbase_getuentryList (/*@notnull@*/ ctbase p_c) /*@*/ ;
static ctype ctbase_newBase (ctype p_c, ctype p_p) /*@*/ ;
static ctype ctbase_newBaseExpFcn (ctype p_c, ctype p_p) /*@*/ ;
static bool ctbase_isFixedArray (/*@notnull@*/ ctbase p_c) /*@*/ ;

/*@-macroundef@*/
extern int cttable_lastIndex();
# define cttable_lastIndex()  (cttab.size - 1)
/*@=macroundef@*/

typedef struct
{
  ctype rval;
  /*@only@*/ uentryList params; 
} *cfcn; 

typedef struct
{
  cstring   name;
  uentryList fields;
} *tsu;

typedef struct
{
  ctype a;
  ctype b;
  bool  isExplicit;
} *tconj;

typedef struct
{
  cstring      tag;
  enumNameList members;
} *tenum;

typedef struct
{
  ctype base;
  size_t size;
} *tfixed;
 
typedef union 
{
  cprim         prim;      /* primitive */
  typeId        tid;       /* abstract, user */
  ctype         base;      /* ptr, array */
  cfcn          fcn;       /* function     */
  tsu           su;        /* struct union */
  tenum         cenum;     /* enum */
  tconj         conj;      /* conj */
  tfixed        farray;    /* fixed array */
} uconts;

struct s_ctbase
{
  ctuid    type;
  uconts   contents;
} ;

static /*@falsenull@*/ bool ctbase_isUA (ctbase p_c) /*@*/ ;
static bool ctbase_isBaseUA(ctbase p_c) /*@*/ ;
static typeId  ctbase_typeBaseUid(ctbase p_c) /*@*/ ;
static bool ctbase_isKind (/*@notnull@*/ ctbase p_c, ctuid p_kind) /*@*/ ;
static bool ctbase_isKind2 (/*@notnull@*/ ctbase p_c, ctuid p_kind1, ctuid p_kind2) /*@*/ ;
static /*@only@*/ /*@notnull@*/ ctbase 
  ctbase_getBaseType (/*@notnull@*/ ctbase p_c) /*@*/ ;

static /*@falsenull@*/ bool ctbase_isFunction(ctbase p_c) /*@*/ ;

/*@constant null ctbase ctbase_undefined; @*/
# define ctbase_undefined        ((ctbase)0)

static /*@owned@*/ ctbase ctbase_bool = ctbase_undefined;
static /*@owned@*/ ctbase ctbase_unknown = ctbase_undefined;

static /*@falsenull@*/ bool ctbase_isDefined (ctbase c) /*@*/
{
  return ((c) != ctbase_undefined);
}

static /*@truenull@*/ bool ctbase_isUndefined (ctbase c)
{
  return ((c) == ctbase_undefined);
}

static ctkind ctype_getCtKind (ctype c)
{
  ctentry ce = ctype_getCtentry (c);

  return ctentry_getKind (ce);
}

static bool ctbase_isUser (ctbase c)
{
  if (ctbase_isDefined (c))
    {
      return (ctbase_isKind (c, CT_USER));
    }
  else
    {
      return FALSE;
    }
}

static bool ctbase_isEnum (ctbase c)
{
  if (ctbase_isDefined (c))
    {
      return (ctbase_isKind (c, CT_ENUM));
    }
  else
    {
      return FALSE;
    }
}

static bool ctbase_isExpFcn (ctbase c)
{
  if (ctbase_isDefined (c))
    {
      return (c->type == CT_EXPFCN);
    }
  else
    {
      return FALSE;
    }
}

static /*@falsenull@*/ bool ctbase_isConj (ctbase c)
{
  if (ctbase_isDefined (c))
    {
      return (c->type == CT_CONJ);
    }
  else
    {
      return FALSE;
    }
}

static bool ctuid_isAP (ctuid c) /*@*/ 
{
  return (c == CT_ARRAY || c == CT_PTR);
}

static typeId ctbase_typeId (ctbase p_c);
static /*@only@*/ cstring ctbase_dump (ctbase p_c);
static /*@only@*/ ctbase ctbase_undump (char **p_c) /*@requires maxRead(*p_c) >= 2 @*/;
static int ctbase_compare (ctbase p_c1, ctbase p_c2, bool p_strict);
static bool ctbase_matchArg (ctbase p_c1, ctbase p_c2);
static /*@notnull@*/ /*@only@*/ ctbase 
  ctbase_makeConj (ctype p_c1, ctype p_c2, bool p_isExplicit) /*@*/ ;
static ctype ctbase_getConjA (/*@notnull@*/ ctbase p_c) /*@*/ ;
static ctype ctbase_getConjB (/*@notnull@*/ ctbase p_c) /*@*/ ;
static bool ctbase_isExplicitConj (/*@notnull@*/ ctbase p_c) /*@*/ ;
static bool ctbase_forceMatch (ctbase p_c1, ctbase p_c2) /*@modifies p_c1, p_c2@*/ ;
static /*@notnull@*/ /*@only@*/ ctbase ctbase_expectFunction (ctype p_c);
static bool ctbase_isVoidPointer(/*@notnull@*/ /*@dependent@*/ ctbase p_c) /*@*/ ;
static bool ctbase_isUnion (/*@notnull@*/ /*@temp@*/ ctbase p_c) /*@*/ ;
static bool ctbase_isStruct (/*@notnull@*/ /*@temp@*/ ctbase p_c) /*@*/ ;
static /*@observer@*/ cstring ctbase_enumTag (/*@notnull@*/ ctbase p_ct) /*@*/ ;
static /*@only@*/ cstring ctbase_unparseNotypes (ctbase p_c) /*@*/ ;

static /*@out@*/ /*@notnull@*/ /*@only@*/ ctbase ctbase_new (void) /*@*/ ;
static int nctbases = 0;

static /*@notnull@*/ /*@only@*/ 
  ctbase ctbase_makeLiveFunction (ctype p_b, /*@only@*/ uentryList p_p);

static bool ctbase_isUnnamedSU (ctbase c)
{
  return (ctbase_isDefined (c)
	  && (ctbase_isStruct (c) || ctbase_isUnion (c))
	  && isFakeTag (c->contents.su->name));
}

static /*@observer@*/ ctbase ctbase_realType (ctbase c)
{
  if (ctbase_isUA (c))
    {
      typeId uid = ctbase_typeId (c);

      if (usymtab_isBoolType (uid))
	{
	  return ctbase_getBool ();
	}
      else
	{
	  ctbase ret = ctype_getCtbase 
	    (uentry_getRealType (usymtab_getTypeEntry (ctbase_typeId (c))));
	  
	  llassert (ret != ctbase_undefined);
	  return ret;
	}
    }
  else
    {
      return c;
    }
}

static bool
ctbase_isVoidPointer (/*@dependent@*/ /*@notnull@*/ ctbase c)
{
  ctbase r = ctbase_realType (c);

  return (ctbase_isKind (r, CT_PTR) &&
	  ctype_isVoid (r->contents.base));
}

static bool
ctbase_isPointer (/*@notnull@*/ /*@dependent@*/ ctbase c)
{
  ctbase r = ctbase_realType (c);

  return (ctbase_isKind (r, CT_PTR));
}

static bool
ctbase_isEitherArray (/*@notnull@*/ ctbase c)
{
  ctbase r = ctbase_realType (c);

  return (ctbase_isKind (r, CT_ARRAY)
	  || ctbase_isKind (r, CT_FIXEDARRAY));
}

static bool
ctbase_isFixedArray (/*@notnull@*/ ctbase c)
{
  ctbase r = ctbase_realType (c);

  return (ctbase_isKind (r, CT_FIXEDARRAY));
}

static bool
ctbase_isStruct (/*@notnull@*/ ctbase c)
{
  ctbase r = ctbase_realType (c);

  return (ctbase_isKind (r, CT_STRUCT));
}

static bool
ctbase_isUnion (/*@notnull@*/ ctbase c)
{
  ctbase r = ctbase_realType (c);

  return (ctbase_isKind (r, CT_UNION));
}

/*
** clean this up -> typeTable should store ctype
*/

static typeId
ctbase_typeBaseUid (ctbase c)
{
  ctuid ct;

  if (ctbase_isDefined (c))
    {
      ct = c->type;

      if (ctuid_isAP (ct))
	{
	  return ctbase_typeBaseUid (ctype_getCtbase (c->contents.base));
	}
      else if (ct == CT_USER || ct == CT_ABST || ct == CT_NUMABST)
	{
	  return c->contents.tid;
	}
      else if (ct == CT_FIXEDARRAY)
	{
	  return ctbase_typeBaseUid (ctype_getCtbase (c->contents.farray->base));
	}
      else
	{
	  llcontbuglit ("ctbase_typeBaseUid: bad call");
	  return typeId_invalid;
	}
    }
  return typeId_invalid;
}

static bool
ctbase_isBaseUA (ctbase c)
{
  ctuid ct;

  if (ctbase_isDefined (c))
    {
      ct = c->type;

      if (ctuid_isAP (ct))
	{
	  return ctbase_isBaseUA (ctype_getCtbase (c->contents.base));
	}
      else if (ct == CT_FIXEDARRAY)
	{
	  return ctbase_isBaseUA (ctype_getCtbase (c->contents.farray->base));
	}
      else
	return (ct == CT_USER || ct == CT_ABST || ct == CT_NUMABST);
    }
  return FALSE;
}

static typeId
ctbase_typeId (ctbase c)
{
  if (ctbase_isUA (c))
    {
      return c->contents.tid;
    }
  else
    {
      if (ctbase_isConj (c)) 
	{
	  if (ctype_isUA (ctbase_getConjA (c))) {
	    return ctbase_typeId (ctype_getCtbase (ctbase_getConjA (c)));
	  } else if (ctype_isUA (ctbase_getConjB (c))) {
	    return ctbase_typeId (ctype_getCtbase (ctbase_getConjB (c)));
	  } else {
	    llcontbug (message ("ctbase_typeId: bad call: %q", ctbase_unparse (c)));
	    return typeId_invalid;
	  }
	}
      else
	{
	  llcontbug (message ("ctbase_typeId: bad call: %q", ctbase_unparse (c)));
	  return typeId_invalid;
	}
    }
}

static /*@only@*/ cstring
ctbase_unparse (ctbase c)
{
  if (ctbase_isUndefined (c)) {
    return cstring_makeLiteral ("<<undef>>");
  }
  
  switch (c->type)
    {
    case CT_UNKNOWN:
      return cstring_makeLiteral ("?");
    case CT_BOOL:
      return cstring_copy (context_printBoolName ());
    case CT_PRIM:
      return (cprim_unparse (c->contents.prim));
    case CT_USER:
    case CT_ABST:
    case CT_NUMABST:
      return (usymtab_getTypeEntryName (c->contents.tid));
    case CT_EXPFCN:
      return (message ("<expf: %t>", c->contents.base));
    case CT_PTR:
      /* no spaces for multiple pointers */

      if (ctype_isPointer (c->contents.base))
	{
	  return (cstring_appendChar (cstring_copy (ctype_unparse (c->contents.base)), '*'));
	}
      else
	{
	  return (message ("%t *", c->contents.base));
	}
    case CT_FIXEDARRAY:
      /*
      ** C prints out array declarations backwards, if
      ** base is an array need to print out in reverse order.
      */

      if (ctype_isArray (c->contents.farray->base)) 
	{
	  ctype base = c->contents.farray->base;
	  cstring res = message ("[%d]", (int) c->contents.farray->size);

	  while (ctype_isArray (base)) 
	    {
	      if (ctype_isFixedArray (base)) 
		{
		  res = message ("%q[%d]", 
				 res, (int) ctype_getArraySize (base));
		}
	      else
		{
		  res = message ("%q[]", res);
		}

	      base = ctype_baseArrayPtr (base);
	    }

	  return (message ("%t %q", base, res));
	} 
      else 
	{
	  return (message ("%t [%d]", 
			   c->contents.farray->base, 
			   (int) c->contents.farray->size));
	}
    case CT_ARRAY:
      if (ctype_isArray (c->contents.base)) 
	{
	  ctype base = c->contents.base;
	  cstring res = cstring_makeLiteral ("[]");

	  while (ctype_isArray (base)) 
	    {
	      if (ctype_isFixedArray (base)) 
		{
		  res = message ("%q[%d]", 
				 res, (int) ctype_getArraySize (base));
		}
	      else
		{
		  res = message ("%q[]", res);
		}

	      base = ctype_baseArrayPtr (base);
	    }

	  return (message ("%t %q", base, res));

	}
      else
	{
	  return (message ("%t []", c->contents.base));
	}
    case CT_FCN:
      return (message ("[function (%q) returns %t]",
		       uentryList_unparseParams (c->contents.fcn->params),
		       c->contents.fcn->rval));
    case CT_STRUCT:
      if (cstring_isDefined (c->contents.su->name) &&
	  !cstring_isEmpty (c->contents.su->name) &&
	  !isFakeTag (c->contents.su->name))
	{
	  return (message ("struct %s", c->contents.su->name));
	}
      else
	{
	  return (message ("struct { %q }", 
			   uentryList_unparseAbbrev (c->contents.su->fields)));	
	}
    case CT_UNION:
      if (cstring_isDefined (c->contents.su->name) &&
	  !cstring_isEmpty (c->contents.su->name) &&
	  !isFakeTag (c->contents.su->name))
	{
	  return (message ("union %s", c->contents.su->name));
	}
      else
	{
	  return (message ("union { %q }", 
			   uentryList_unparseAbbrev (c->contents.su->fields)));
	}
    case CT_ENUM:
      if (isFakeTag (c->contents.cenum->tag))
	{
	  return (message ("enum { %q }", 
			   enumNameList_unparseBrief (c->contents.cenum->members)));
	}
      else
	{
	  return (message ("enum %s { %q }", 
			   c->contents.cenum->tag,
			   enumNameList_unparseBrief (c->contents.cenum->members)));
	}
    case CT_CONJ:
      if (ctbase_isAnytype (c))
	{
	  return (cstring_makeLiteral ("<any>"));
	}
      else if (c->contents.conj->isExplicit || context_getFlag (FLG_SHOWALLCONJS))
	{
	  if (!ctype_isSimple (c->contents.conj->a) ||
	      !ctype_isSimple (c->contents.conj->b))
	    {
	      return (message ("<%t> | <%t>", c->contents.conj->a, c->contents.conj->b));
	    }
	  else
	    {
	      return (message ("%t | %t", c->contents.conj->a, c->contents.conj->b));
	    }
	}
      else
	{
	  return (cstring_copy (ctype_unparse (c->contents.conj->a)));
	}
    BADDEFAULT;
    }
  BADEXIT;
}

static /*@only@*/ cstring
  ctbase_unparseDeep (ctbase c)
{
  if (ctbase_isUndefined (c))
    {
      return cstring_makeLiteral ("<<undef>>");
    }

  switch (c->type)
    {
    case CT_UNKNOWN:
      return cstring_makeLiteral ("?");
    case CT_BOOL:
      return cstring_copy (context_printBoolName ());
    case CT_PRIM:
      return (cprim_unparse (c->contents.prim));
    case CT_ENUM:
      if (cstring_isNonEmpty (c->contents.cenum->tag))
	{
	  return (message ("enum %s { %q }",
			   c->contents.cenum->tag,
			   enumNameList_unparse (c->contents.cenum->members)));
	}
      else
	{
	  return (message ("enum { %q }",
			   enumNameList_unparse (c->contents.cenum->members)));
	}
    case CT_USER:
    case CT_ABST:
    case CT_NUMABST:
      return (usymtab_getTypeEntryName (c->contents.tid));
    case CT_EXPFCN:
      return (message ("<expf: %t>", c->contents.base));
    case CT_PTR:
      return (message ("%t *", c->contents.base));
    case CT_FIXEDARRAY:
      return (message ("%t [%d]", c->contents.farray->base, 
		       (int) c->contents.farray->size));
    case CT_ARRAY:
      return (message ("%t []", c->contents.base));
    case CT_FCN:
      return (message ("[function (%q) returns %t]",
		       uentryList_unparse (c->contents.fcn->params),
		       c->contents.fcn->rval));
    case CT_STRUCT:
      return (message ("struct %s { ... } ", c->contents.su->name));
    case CT_UNION:
      return (message ("union %s { ... }", c->contents.su->name));
    case CT_CONJ:
      if (ctbase_isAnytype (c))
	{
	  return (cstring_makeLiteral ("<any>"));
	}
      else
	{
	  return (message ("%t", c->contents.conj->a));
	}
    BADDEFAULT;
    }
  BADEXIT;
}

static /*@only@*/ cstring
ctbase_unparseNotypes (ctbase c)
{
  llassert (ctbase_isDefined (c));

  switch (c->type)
    {
    case CT_UNKNOWN:
      return cstring_makeLiteral ("?");
    case CT_BOOL:
      return cstring_copy (context_printBoolName ());
    case CT_PRIM:
      return (cprim_unparse (c->contents.prim));
    case CT_ENUM:
      if (typeId_isInvalid (c->contents.tid))
	{
	  return cstring_makeLiteral ("enum");
	}
      else
	{
	  return (message ("T#%d", c->contents.tid));
	}
    case CT_USER:
      return (message ("uT#%d", c->contents.tid));
    case CT_ABST:
      return (message ("aT#%d", c->contents.tid));
    case CT_NUMABST:
      return (message ("nT#%d", c->contents.tid));
    case CT_EXPFCN:
      return (message ("<expf: %q >", ctbase_unparseNotypes (ctype_getCtbase (c->contents.base))));
    case CT_PTR:
      return (message ("%q  *", ctbase_unparseNotypes (ctype_getCtbase (c->contents.base))));
    case CT_ARRAY:
      return (message ("%q []", ctbase_unparseNotypes (ctype_getCtbase (c->contents.base))));
    case CT_FCN:
      return (message ("[function (%d) returns %q]", uentryList_size (c->contents.fcn->params),
		       ctbase_unparseNotypes (ctype_getCtbase (c->contents.fcn->rval))));
    case CT_STRUCT:
      return (message ("struct %s", c->contents.su->name));
    case CT_UNION:
      return (message ("union %s", c->contents.su->name));
    case CT_ENUMLIST:
      return (message ("[enumlist]"));
    case CT_CONJ:
      if (ctbase_isAnytype (c))
	{
	  return (cstring_makeLiteral ("<any>"));
	}
      else
	{
	  return (message ("%q/%q", 
			   ctbase_unparseNotypes (ctype_getCtbase (c->contents.conj->a)),
			   ctbase_unparseNotypes (ctype_getCtbase (c->contents.conj->b))));
	}
    BADDEFAULT;
    }
  BADEXIT;
}

static /*@only@*/ cstring
ctbase_unparseDeclaration (ctbase c, /*@only@*/ cstring name) /*@*/ 
{
  if (ctbase_isUndefined (c))
    {
      return name;
    }
  
  switch (c->type)
    {
    case CT_UNKNOWN:
      return (message ("? %q", name));
    case CT_BOOL:
      return (message ("%s %q", context_printBoolName (), name));
    case CT_PRIM:
      return (message ("%q %q", cprim_unparse (c->contents.prim), name));
    case CT_USER:
    case CT_ABST:
    case CT_NUMABST:
      return (message ("%q %q", usymtab_getTypeEntryName (c->contents.tid), name));
    case CT_EXPFCN:
      llcontbuglit ("ctbase_unparseDeclaration: expfcn");
      return name;
    case CT_PTR:
      if (ctype_isFunction (c->contents.base))
	{
	  return ctbase_unparseDeclaration (ctype_getCtbase (c->contents.base), name);
	}
      else
	{
	  cstring s = cstring_prependChar ('*', name);
	  cstring ret = ctbase_unparseDeclaration (ctype_getCtbase (c->contents.base), s);
	  cstring_free (name);
	  return (ret);
	}
    case CT_FIXEDARRAY:
      return (message ("%q[%d]", 
		       ctbase_unparseDeclaration (ctype_getCtbase (c->contents.farray->base), name),
		       (int) c->contents.farray->size));
    case CT_ARRAY:
      return (message ("%q[]", 
		       ctbase_unparseDeclaration (ctype_getCtbase (c->contents.base), name)));
    case CT_FCN:
	{
	  cstring s = message ("%q(%q)", name, 
			       uentryList_unparseParams (c->contents.fcn->params));

	  return (ctbase_unparseDeclaration 
		  (ctype_getCtbase (c->contents.fcn->rval), s));
	}
    case CT_STRUCT:
      if (cstring_isDefined (c->contents.su->name) &&
	  !cstring_isEmpty (c->contents.su->name) &&
	  !isFakeTag (c->contents.su->name))
	{
	  return (message ("struct %s %q", c->contents.su->name, name));
	}
      else
	{
	  return (message ("struct { %q } %q", 
			   uentryList_unparseAbbrev (c->contents.su->fields),
			   name));
	}
    case CT_UNION:
      if (cstring_isDefined (c->contents.su->name) &&
	  !cstring_isEmpty (c->contents.su->name) &&
	  !isFakeTag (c->contents.su->name))
	{
	  return (message ("union %s %q", c->contents.su->name, name));
	}
      else
	{
	  return (message ("union { %q } %q", 
			   uentryList_unparseAbbrev (c->contents.su->fields),
			   name));
	}
    case CT_ENUM:
      if (isFakeTag (c->contents.cenum->tag))
	{
	  return (message ("enum { %q } %q", 
			   enumNameList_unparseBrief (c->contents.cenum->members),
			   name));
	}
      else
	{
	  return (message ("enum %s { %q } %q", 
			   c->contents.cenum->tag,
			   enumNameList_unparseBrief (c->contents.cenum->members),
			   name));
	}
    case CT_CONJ:
      if (ctbase_isAnytype (c))
	{
	  return (message ("<any> %q", name));
	}
      else if (c->contents.conj->isExplicit || context_getFlag (FLG_SHOWALLCONJS))
	{
	  if (!ctype_isSimple (c->contents.conj->a) ||
	      !ctype_isSimple (c->contents.conj->b))
	    {
	      cstring name1 = cstring_copy (name);

	      return 
		(message 
		 ("<%q> | <%q>", 
		  ctbase_unparseDeclaration 
		  (ctype_getCtbase (c->contents.conj->a), name1),
		  ctbase_unparseDeclaration 
		  (ctype_getCtbase (c->contents.conj->b), name)));
	    }
	  else
	    {
	      cstring s1 = ctbase_unparseDeclaration (ctype_getCtbase (c->contents.conj->a), 
						      cstring_copy (name));
	      return
		(message ("%q | %q", s1, 
			  ctbase_unparseDeclaration (ctype_getCtbase (c->contents.conj->b),
						     name)));
	    }
	}
      else
	{
	  cstring_free (name);
	  return (cstring_copy (ctype_unparse (c->contents.conj->a)));
	}
      BADDEFAULT;
    }
  BADEXIT;
}

static ctbase ctbase_undump (d_char *c) /*@requires maxRead(*c) >= 2 @*/
{
  ctbase res;
  char p = **c;

  (*c)++;

  switch (p)
    {
    case '?':
      return (ctbase_undefined);
    case 'u':
      return (ctbase_createUnknown ());
    case 'b':
      return (ctbase_createBool ());
    case 'p':
      res = ctbase_createPrim (cprim_fromInt (reader_getInt (c)));
      reader_checkChar (c, '|');
      return res;
    case 's':
      res = ctbase_createUser (typeId_fromInt (reader_getInt (c)));
      reader_checkChar (c, '|');
      return res;
    case 'a':
      res = ctbase_createAbstract (typeId_fromInt (reader_getInt (c)));
      reader_checkChar (c, '|');
      return res;
    case 'n':
      res = ctbase_createNumAbstract (typeId_fromInt (reader_getInt (c)));
      reader_checkChar (c, '|');
      return res;
    case 't':
      res = ctbase_makePointer (ctype_undump (c));
      reader_checkChar (c, '|');
      return res;
    case 'y':
      res = ctbase_makeArray (ctype_undump (c));
      reader_checkChar (c, '|');
      return res;
    case 'F':
      {
	ctype ct = ctype_undump (c);
	size_t size;

	reader_checkChar (c, '/');
	size = size_fromInt (reader_getInt (c));
	reader_checkChar (c, '|');
	return (ctbase_makeFixedArray (ct, size));
      }
    case 'f':
      {
	ctype ct;
	char *lp = strchr (*c, '(');

	llassertfatal (lp != NULL);

	*lp = '\0';
	ct = ctype_undump (c);
	*c = lp + 1;

	return (ctbase_makeLiveFunction (ct, uentryList_undump (c)));
      }
    case 'S':
      {
	uentryList fields;
	ctbase ctb;
	char *sname;
	char *lc = strchr (*c, '{');

	llassert (lc != NULL);
	*lc = '\0';

	sname = mstring_copy (*c);

	*c = lc + 1;

	if (*sname == '!')
	  {
	    unsigned int i;

	    i = (unsigned) atoi (sname + 1);

	    setTagNo (i);
	  }

	fields = uentryList_undumpFields (c, g_currentloc);

	ctb = ctbase_createStruct (cstring_fromCharsO (sname), fields);
	return ctb;
      }
    case 'U':
      {
	char *sname;
	char *lc = strchr (*c, '{');

	llassert (lc != NULL);

	*lc = '\0';
	sname = mstring_copy (*c);
	llassert (sname != NULL);

	*c = lc + 1;

	if (*sname == '!')
	  {
	    unsigned int i;

	    i = (unsigned) atoi (sname + 1);
	    setTagNo (i);
	  }

	return (ctbase_createUnion (cstring_fromCharsO (sname),
				    uentryList_undumpFields (c, g_currentloc)));
      }
    case 'e':
      {
	ctbase ret;
	char *sname;
	char *lc = strchr (*c, '{');

	llassert (lc != NULL);

	*lc = '\0';
	sname = mstring_copy (*c);
	*c = lc + 1;

	if (*sname == '!')
	  {
	    unsigned int i;

	    i = (unsigned) atoi (sname + 1);
	    setTagNo (i);
	  }

	ret = ctbase_createEnum (cstring_fromCharsO (sname),
				 enumNameList_undump (c));
	return ret;
      }
    case 'C':
      {
	bool isExplicit;
	ctype c1, c2;

	isExplicit = bool_fromInt (reader_getInt (c));
	reader_checkChar (c, '.');
	c1 = ctype_undump (c);
	reader_checkChar (c, '/');
	c2 = ctype_undump (c);
	reader_checkChar (c, '|');

	return (ctbase_makeConj (c1, c2, isExplicit));
      }

    default:
      (*c)--;
      llerror (FLG_SYNTAX, 
	       message ("Bad Library line (type): %s", cstring_fromChars (*c)));

  /*drl bee: pbr*/      while (**c != '\0')
	{
	  (*c)++;
	}

      return ctbase_createUnknown ();
    }
}

/* first letter of c encodes type:     */
/*    u  unknown                       */
/*    b  bool                          */
/*    p  prim                          */
/*    e  enum                          */
/*    l  enumList                      */
/*    s  uSer                          */
/*    a  abstract                      */
/*    t  poinTer                       */
/*    y  arraY                         */
/*    F  Fixed array                   */
/*    f  function                      */
/*    S  structure                     */
/*    U  union                         */
/*    C  conj                          */

static /*@only@*/ cstring
ctbase_dump (ctbase c)
{
  if (!ctbase_isDefined (c))
    {
      return cstring_makeLiteral ("?");
    }

  switch (c->type)
    {
    case CT_UNKNOWN:
      return cstring_makeLiteral ("u");
    case CT_BOOL:
      return cstring_makeLiteral ("b");
    case CT_PRIM:
      return (message ("p%d|", c->contents.prim));
    case CT_USER:
      return (message ("s%d|", usymtab_convertTypeId (c->contents.tid)));
    case CT_ABST:
      return (message ("a%d|", usymtab_convertTypeId (c->contents.tid)));
    case CT_NUMABST:
      return (message ("n%d|", usymtab_convertTypeId (c->contents.tid)));
    case CT_PTR:
      return (message ("t%q|", ctype_dump (c->contents.base)));
    case CT_ARRAY:
      return (message ("y%q|", ctype_dump (c->contents.base)));
    case CT_FIXEDARRAY:
      return (message ("F%q/%d|", 
		       ctype_dump (c->contents.farray->base),
		       (int) c->contents.farray->size));
    case CT_FCN:
      DPRINTF (("Dump function: %s", ctbase_unparse (c)));
      return (message ("f%q (%q)", ctype_dump (c->contents.fcn->rval),
		       uentryList_dumpParams (c->contents.fcn->params)));
    case CT_STRUCT:
      return (message ("S%s{%q}", c->contents.su->name,
		       uentryList_dumpFields (c->contents.su->fields)));
    case CT_UNION:
      return (message ("U%s{%q}", c->contents.su->name,
		       uentryList_dumpFields (c->contents.su->fields)));
    case CT_ENUM:
      {
	cstring s;
	
	if (cstring_isNonEmpty (c->contents.cenum->tag))
	  {
	    s = message ("e%s{%q}", 
			 c->contents.cenum->tag,
			 enumNameList_dump (c->contents.cenum->members));
	  }
	else
	  {
	    s = message ("e{%q}", 
			 enumNameList_dump (c->contents.cenum->members));
	  }
	return (s);
      }
    case CT_CONJ:
      return (message ("C%d.%q/%q|", 
		       bool_toInt (c->contents.conj->isExplicit),
		       ctype_dump (c->contents.conj->a),
		       ctype_dump (c->contents.conj->b)));
    case CT_EXPFCN:
      /* should clean them up! */
      return (cstring_makeLiteral ("?"));
    case CT_ENUMLIST:
      llcontbug (message ("Cannot dump: %q", ctbase_unparse (c))); 
      return (message ("u"));
    BADDEFAULT;
    }

  BADEXIT;
}

static /*@only@*/ ctbase
ctbase_copy (/*@notnull@*/ ctbase c)
{
  switch (c->type)
    {
    case CT_UNKNOWN:
      return (ctbase_createUnknown ());
    case CT_BOOL:
      return (ctbase_createBool ());
    case CT_ENUM:
      return (ctbase_createEnum (cstring_copy (c->contents.cenum->tag), 
				 enumNameList_copy (c->contents.cenum->members)));
    case CT_PRIM:
      return (ctbase_createPrim (c->contents.prim));
    case CT_USER:
      return (ctbase_createUser (c->contents.tid));
    case CT_ABST:
      return (ctbase_createAbstract (c->contents.tid));
    case CT_NUMABST:
      return (ctbase_createNumAbstract (c->contents.tid));
    case CT_EXPFCN:
      return (ctbase_expectFunction (c->contents.base));
    case CT_PTR:
      return (ctbase_makePointer (c->contents.base));
    case CT_ARRAY:
      return (ctbase_makeArray (c->contents.base));
    case CT_FCN:
      return (ctbase_makeLiveFunction (c->contents.fcn->rval,
				       uentryList_copy (c->contents.fcn->params)));	
    case CT_STRUCT:
      return (ctbase_createStruct (cstring_copy (c->contents.su->name),
				   uentryList_copy (c->contents.su->fields)));
    case CT_UNION:
      return (ctbase_createUnion (cstring_copy (c->contents.su->name),
				  uentryList_copy (c->contents.su->fields)));
    case CT_CONJ:
      /*@i@*/ return (c); /* not a real copy for conj's */
    default:
      llbug (message ("ctbase_copy: %q", ctbase_unparse (c)));
    }

  BADEXIT;
}
				
static enumNameList
ctbase_elist (ctbase c)
{
  llassert (ctbase_isDefined (c));
  llassert (c->type == CT_ENUM);

  return (c->contents.cenum->members);
}

static void
ctbase_free (/*@only@*/ ctbase c)
{
  if (c == ctbase_bool || c == ctbase_unknown)
    {
      /*@-mustfree@*/ return; /*@=mustfree@*/
    }

  --nctbases;
  
    
  if (ctbase_isDefined (c))
    {
      switch (c->type)
	{
	case CT_UNKNOWN:
	  sfree (c);
	  break;
	case CT_PRIM:
	  sfree (c);
	  break;
	case CT_ENUM:
	  sfree (c);
	  break;
	case CT_ENUMLIST:
	  /* sfree list? */
	  sfree (c);
	  break;
	case CT_USER:
	case CT_ABST:
	case CT_NUMABST:
	  sfree (c);
	  break;
	case CT_PTR:
	case CT_ARRAY:
	  sfree (c);
	  break;
	case CT_FCN:
	  /* Cannot free params: uentryList_free (c->contents.fcn->params);  */ 
	  uentryList_freeShallow (c->contents.fcn->params);
	  sfree (c);
	  break;
	case CT_STRUCT:
	case CT_UNION:
	  cstring_free (c->contents.su->name);
	  uentryList_free (c->contents.su->fields); 
	  sfree (c);
	  break;
	case CT_CONJ:
	  /* Don't bree conj's, */
	  break;
	default:
	  sfree (c);
	  break;
	}
    }
}

/*
** c should be * <unknown>
*/

static /*@only@*/ ctbase
ctbase_expectFunction (ctype c)
{
  ctbase f = ctbase_new ();

  f->type = CT_EXPFCN;
  f->contents.base = c;

  return (f);
}

static bool
ctbase_isExpectFunction (/*@notnull@*/ ctbase ct) /*@*/
{
  return (ct->type == CT_EXPFCN);
}

static ctype
ctbase_getExpectFunction (/*@notnull@*/ ctbase ct)
{
  llassert (ctbase_isExpectFunction (ct));
  return ct->contents.base;
}

static bool
ctbase_genMatch (ctbase c1, ctbase c2, bool force, bool arg, bool def, bool deep)
{
  ctuid c1tid, c2tid;
  
  /* undefined types never match */
  
  if (ctbase_isUndefined (c1) || ctbase_isUndefined (c2))
    return FALSE;
  
  /* abstract types match user types of same name */

  c1 = ctbase_realType (c1);
  c2 = ctbase_realType (c2);
  
  DPRINTF (("Matching: %s / %s", ctbase_unparse (c1),
	    ctbase_unparse (c2)));
  
  c1tid = c1->type;
  c2tid = c2->type;
  
  if (c1tid == CT_CONJ)
    {
      return (ctbase_genMatch (ctype_getCtbase (c1->contents.conj->a), c2,
			       force, arg, def, deep)
	      || ctbase_genMatch (ctype_getCtbase (c1->contents.conj->b), c2,
				  force, arg, def, deep));
    }
  
  if (c2tid == CT_CONJ)
    {
      return (ctbase_genMatch (c1, ctype_getCtbase (c2->contents.conj->a),
			       force, arg, def, deep)
	      || ctbase_genMatch (c1, ctype_getCtbase (c2->contents.conj->b),
				  force, arg, def, deep));
    }

  /*
  ** if the types don't match, there are some special cases...
  */

  if (c1tid != c2tid)
    {
      /* unknowns match anything */
      
      if (c1tid == CT_UNKNOWN || c2tid == CT_UNKNOWN)
	{
	  return TRUE;
	}

      if (c1tid == CT_FIXEDARRAY 
	  && (c2tid == CT_ARRAY || (!def && c2tid == CT_PTR)))
	{
	  if (ctype_isVoid (c2->contents.base))
	    {
	      return (context_getFlag (FLG_ABSTVOIDP) ||
		      (!(ctype_isRealAbstract (c1->contents.farray->base)) &&
		       !(ctype_isRealAbstract (c2->contents.base))));
	    }

	  return (ctbase_genMatch (ctype_getCtbase (c1->contents.farray->base),
				   ctype_getCtbase (c2->contents.base),
				   force, arg, def, deep));
	}


      if (c2tid == CT_FIXEDARRAY 
	  && (c1tid == CT_ARRAY || (!def && c1tid == CT_PTR)))
	{
	  if (ctype_isVoid (c1->contents.base))
	    {
	      return (context_getFlag (FLG_ABSTVOIDP) ||
		      (!(ctype_isRealAbstract (c2->contents.farray->base)) &&
		       !(ctype_isRealAbstract (c1->contents.base))));
	    }

	  return (ctbase_genMatch (ctype_getCtbase (c1->contents.base),
				   ctype_getCtbase (c2->contents.farray->base),
				   force, arg, def, deep));
	}
      
      /* evs 2000-07-25: Bool's may match user/abstract types */

      if ((c1tid == CT_BOOL 
	   && (c2tid == CT_PRIM && cprim_isInt (c2->contents.prim))) ||
	  (c2tid == CT_BOOL 
	   && (c1tid == CT_PRIM && cprim_isInt (c1->contents.prim))))
	{
	  return (context_msgBoolInt ());
	}

      if ((c1tid == CT_BOOL && (ctuid_isAnyUserType (c2tid)))) {
	ctype t2c = c2->contents.base;
	return (ctype_isBool (t2c));
      }

      if ((c2tid == CT_BOOL && (ctuid_isAnyUserType (c1tid)))) {
	ctype t1c = c1->contents.base;

	return (ctype_isBool (t1c));
      }

      if ((c1tid == CT_ENUM
	   && (c2tid == CT_PRIM && cprim_isInt (c2->contents.prim))) ||
	  (c2tid == CT_ENUM
	   && (c1tid == CT_PRIM && cprim_isInt (c1->contents.prim))))
	{
	  return (context_msgEnumInt ());
	}

      /*
      ** arrays and pointers...yuk!
      **
      ** Considered equivalent except in definitions.  
      ** (e.g., function parameters are equivalent)
      **
      */
      
      if (!def)
	{
	  if (ctuid_isAP (c1tid) && ctuid_isAP (c2tid))
	    {
	      c2tid = c1tid;
	    }
	}

      /*
      ** Function pointers can be removed.
      **
      ** [function ..] is equivalent to [function ..] *
      */

      if (c1tid == CT_PTR && c2tid == CT_FCN)
	{
	  if (ctype_isFunction (ctype_realType (c1->contents.base)))
	    {
	      c1 = ctbase_realType (ctype_getCtbaseSafe (c1->contents.base));
	      c1tid = c1->type;
	    }
	}

      if (c2tid == CT_PTR && c1tid == CT_FCN)
	{
	  if (ctype_isFunction (ctype_realType (c2->contents.base)))
	    {
	      c2 = ctbase_realType (ctype_getCtbaseSafe (c2->contents.base));
	      c2tid = c2->type;
	    }
	}

      /*
      ** we allow forward declarations to structures like,
      **
      **          typedef struct _t *t;
      ** 
      ** to allow,
      **           struct _t * to match t
      */
  
      if (context_getFlag (FLG_FORWARDDECL))
	{
	  if (ctuid_isAnyUserType (c1tid))
	    {
	      if (ctuid_isAP (c2tid))
		{
		  ctype ts = c2->contents.base;
		  
		  if (ctype_isUA (ts))
		    {
		      typeId ttid = ctype_typeId (ts);
		      typeId ctid = c1->contents.tid ;
		      
		      if (usymtab_matchForwardStruct (ctid, ttid))
			{
			  return TRUE;
			}
		    }
		}
	    }
	  
	  if (ctuid_isAnyUserType (c2tid))
	    {
	      if (ctuid_isAP (c1tid))
		{
		  ctype ts = c1->contents.base;
		  
		  if (ctype_isUA (ts))
		    {
		      typeId ttid = ctype_typeId (ts);
		      typeId ctid = c2->contents.tid ;
		      
		      if (usymtab_matchForwardStruct (ctid, ttid))
			{
			  return TRUE;
			}
		    }
		}
	    }
	}
    }

  if (c1tid != c2tid)
    return FALSE;

  switch (c1tid)
    {
    case CT_UNKNOWN:
      return (TRUE);
    case CT_PRIM:
      if (deep) {
	return (cprim_closeEnoughDeep (c1->contents.prim, c2->contents.prim));
      } else {
	return (cprim_closeEnough (c1->contents.prim, c2->contents.prim));
      }
    case CT_BOOL:
      return (TRUE);
    case CT_ABST:
    case CT_NUMABST:
    case CT_USER:
      return (typeId_equal (c1->contents.tid, c2->contents.tid));
    case CT_ENUM:	
      return (cstring_equal (c1->contents.cenum->tag, c2->contents.cenum->tag));
    case CT_PTR:
      if (ctype_isVoid (c1->contents.base)
	  || (ctype_isVoid (c2->contents.base)))
	{
	  if (ctype_isFunction (ctype_realType (c1->contents.base))
	      || ctype_isFunction (ctype_realType (c2->contents.base)))
	    {
	      return (!context_getFlag (FLG_CASTFCNPTR));
	    }
	  else
	    {
	      return (context_getFlag (FLG_ABSTVOIDP) ||
		      (!(ctype_isRealAbstract (c1->contents.base)) &&
		       !(ctype_isRealAbstract (c2->contents.base))));
	    }
	}
      else
	{
	  /* Only allow one implicit function pointer. */
			  
	  if (!bool_equal (ctype_isRealPointer (c1->contents.base),
			   ctype_isRealPointer (c2->contents.base))
	      && (ctype_isRealFunction (c1->contents.base)
		  || ctype_isRealFunction (c2->contents.base)))
	    {
	      return FALSE;
	    }

	  return (ctype_genMatch (c1->contents.base,
				  c2->contents.base, force, arg, def, TRUE));
	}
    case CT_FIXEDARRAY:
      if (ctype_isVoid (c1->contents.farray->base) 
	  || ctype_isVoid (c2->contents.farray->base))
	return TRUE;
      return (ctype_genMatch (c1->contents.farray->base, 
			      c2->contents.farray->base, 
			      force, arg, def, deep));
    case CT_ARRAY:
      if (ctype_isVoid (c1->contents.base) || ctype_isVoid (c2->contents.base))
	return TRUE;
      return (ctype_genMatch (c1->contents.base, c2->contents.base, force, arg, def, TRUE));
    case CT_FCN:
      return (ctype_genMatch (c1->contents.fcn->rval, 
			      c2->contents.fcn->rval, 
			      force, arg, def, TRUE) 
	      && uentryList_matchParams (c1->contents.fcn->params, 
					 c2->contents.fcn->params, 
					 force, TRUE));
    case CT_STRUCT:
    case CT_UNION:
      DPRINTF (("Struct: %s / %s",
		c1->contents.su->name,
		c2->contents.su->name));

      if (isFakeTag (c1->contents.su->name)
	  && isFakeTag (c2->contents.su->name))
	{
	  /* Both fake tags, check structure */
	  if (cstring_equal (c1->contents.su->name, c2->contents.su->name))
	    {
	      return TRUE;
	    }
	  else
	    {
	      return uentryList_matchFields (c1->contents.su->fields, 
					     c2->contents.su->fields);
	    }
	}
      else
	{
	  if (!cstring_isEmpty (c1->contents.su->name))
	    {
	      return (cstring_equal (c1->contents.su->name, c2->contents.su->name));
	    }
	  else
	    {
	      if (!cstring_isEmpty (c2->contents.su->name))
		{
		  return FALSE;
		}
	      
	      llcontbuglit ("ctbase_genMatch: match fields");
	      return (FALSE);
	    }
	}
    default:
      llcontbug (message ("ctbase_genMatch: unknown type: %d\n", (int)c1tid));
      return (FALSE);
    }
}

/*
** like ctbase_match, except for conjuncts:
**   modifies conjuncts to match only
*/

static bool
ctbase_forceMatch (ctbase c1, ctbase c2) /*@modifies c1, c2@*/
{
  return (ctbase_genMatch (c1, c2, TRUE, FALSE, FALSE, FALSE));
}

static bool
ctbase_match (ctbase c1, ctbase c2) /*@modifies nothing@*/
{
  return (ctbase_genMatch (c1, c2, FALSE, FALSE, FALSE, FALSE));
}

static bool
ctbase_matchDef (ctbase c1, ctbase c2) /*@modifies nothing@*/
{
  return (ctbase_genMatch (c1, c2, FALSE, FALSE, TRUE, FALSE));
}

static bool
ctbase_matchArg (ctbase c1, ctbase c2)
{
  return (ctbase_genMatch (c1, c2, FALSE, TRUE, FALSE, FALSE));
}

static /*@out@*/ /*@only@*/ /*@notnull@*/ ctbase 
ctbase_new ()
{
  ctbase c = (ctbase) dmalloc (sizeof (*c));

  nctbases++;
 /*
   if (nctbases % 100 == 0 && nctbases > lastnc)
     {
       llmsg (message ("ctbases: %d", nctbases));
       lastnc = nctbases;
     }
 */
  return (c);
}

static /*@only@*/ ctbase
ctbase_createPrim (cprim p)
{
  ctbase c = ctbase_new ();

  c->type = CT_PRIM;
  c->contents.prim = p;

  return (c);
}

static /*@observer@*/ ctbase
ctbase_getBool (void)
{
  /*@i@*/ return ctbase_createBool ();
}

static ctbase
ctbase_createBool ()
{
  if (!ctbase_isDefined (ctbase_bool))
    {
      ctbase_bool = ctbase_new ();
      ctbase_bool->type = CT_BOOL;
      ctbase_bool->contents.prim = CTX_BOOL;
    }

  /*@-retalias@*/ /*@-globstate@*/
  return ctbase_bool;
  /*@=retalias@*/ /*@=globstate@*/
}

static /*@only@*/ ctbase
ctbase_createUser (typeId u)
{
  ctbase c = ctbase_new ();

  c->type = CT_USER;
  c->contents.tid = u;

  llassert (typeId_isValid (u));

  return (c);
}

static /*@only@*/ ctbase
ctbase_createEnum (/*@keep@*/ cstring etag, /*@keep@*/ enumNameList emembers)
{
  ctbase c = ctbase_new ();

  c->type = CT_ENUM;

  if (cstring_isUndefined (etag))
    {
      llcontbuglit ("Undefined enum tag!");
      etag = fakeTag ();
    }

  c->contents.cenum = (tenum) dmalloc (sizeof (*c->contents.cenum));
  c->contents.cenum->tag = etag;
  c->contents.cenum->members = emembers;

  return (c);
}

static /*@observer@*/ cstring
ctbase_enumTag (/*@notnull@*/ ctbase ct)
{
  return (ct->contents.cenum->tag);
}

static /*@only@*/ ctbase
ctbase_createAbstract (typeId u)
{
  ctbase c = ctbase_new ();

  c->type = CT_ABST;
  c->contents.tid = u;

 /* also check its abstract? */

  llassert (typeId_isValid (c->contents.tid));

  return (c);
}

static /*@only@*/ ctbase
ctbase_createNumAbstract (typeId u)
{
  ctbase c = ctbase_new ();

  c->type = CT_NUMABST;
  c->contents.tid = u;
  
  /* also check its abstract? */
  
  llassert (typeId_isValid (c->contents.tid));
  return (c);
}

static /*@only@*/ ctbase
ctbase_createUnknown (void)
{
  if (!ctbase_isDefined (ctbase_unknown))
    {
      ctbase_unknown = ctbase_new ();
      ctbase_unknown->type = CT_UNKNOWN;
      ctbase_unknown->contents.prim = CTX_UNKNOWN;
    }

  /*@-retalias@*/ /*@-globstate@*/
  return ctbase_unknown;
  /*@=retalias@*/ /*@=globstate@*/
}

/*
** requires: result is not assigned to b
**           (should copy, but no way to reclaim storage)
*/

static /*@only@*/ ctbase
ctbase_makePointer (ctype b)
{
  ctbase c = ctbase_new ();

  c->type = CT_PTR;
  c->contents.base = b;

  return (c);
}

static /*@only@*/ ctbase
ctbase_makeArray (ctype b)
{
  ctbase c = ctbase_new ();

  c->type = CT_ARRAY;
  c->contents.base = b;

  return (c);
}

static /*@notnull@*/ /*@only@*/ ctbase
ctbase_makeFixedArray (ctype b, size_t size)
{
  ctbase c = ctbase_new ();

  c->type = CT_FIXEDARRAY;

  c->contents.farray = (tfixed) dmalloc (sizeof (*c->contents.farray));
  c->contents.farray->base = b;
  c->contents.farray->size = size;

  return (c);
}

static ctype
ctbase_makeFunction (ctype b, /*@only@*/ uentryList p)
{
  ctbase c = ctbase_new ();
  ctype ct;
  
  c->type = CT_FCN;
  c->contents.fcn = (cfcn) dmalloc (sizeof (*c->contents.fcn));

  if (ctype_isFunction (b)) /* was: && ctype_isPointer (b)) */
    {
      ctbase ctb;
      ctype rval;
      
      if (ctype_isPointer (b))
	{
	  ctb  = ctype_getCtbase (ctype_baseArrayPtr (b));
	}
      else
	{
	  ctb = ctype_getCtbase (b);
	}
      
      llassert (ctbase_isDefined (ctb));
      llassert (ctb->type == CT_FCN);
      
      rval = ctype_makeFunction (ctb->contents.fcn->rval, p);

      c->contents.fcn->rval = rval;
      c->contents.fcn->params = uentryList_copy (ctb->contents.fcn->params); /* no copy before */
    }
  else
    {
      c->contents.fcn->rval = b;
      c->contents.fcn->params = uentryList_copy (p); /* no copy before */
      /*@-branchstate@*/ /* p is really released on this branch */
    } 
  /*@=branchstate@*/

  ct = cttable_addComplex (c);  
  return (ct); /* was: ctype_makePointer (ct)); */
}

static ctype
ctbase_makeNFFunction (ctype b, /*@only@*/ uentryList p)
{
  ctbase c = ctbase_new ();
  ctype ct;

  c->type = CT_FCN;
  c->contents.fcn = (cfcn) dmalloc (sizeof (*c->contents.fcn));

  if (ctype_isFunction (b)) /* was && ctype_isPointer (b)) */
    {
      ctbase ctb;
      ctype rval;
      
      if (ctype_isPointer (b))
	{
	  ctb  = ctype_getCtbase (ctype_baseArrayPtr (b));
	}
      else
	{
	  ctb = ctype_getCtbase (b);
	}

      llassert (ctbase_isDefined (ctb));
      llassert (ctb->type == CT_FCN);
      
      rval = ctype_makeNFParamsFunction (ctb->contents.fcn->rval, p);
      
      c->contents.fcn->rval = rval;
      c->contents.fcn->params = uentryList_copy (ctb->contents.fcn->params);
    }
  else
    {
      c->contents.fcn->rval = b;
      c->contents.fcn->params = uentryList_copy (p);
      /*@-branchstate@*/ 
    }
  /*@=branchstate@*/

  ct = cttable_addComplex (c);
  return (ct); /* was: ctype_makePointer (ct)); */
}

static /*@only@*/ ctbase
  ctbase_makeLiveFunction (ctype b, /*@only@*/ uentryList p)
{
  ctbase c = ctbase_new ();

  c->type = CT_FCN;

  c->contents.fcn = (cfcn) dmalloc (sizeof (*c->contents.fcn));
  c->contents.fcn->rval = b;
  c->contents.fcn->params = p;

  /*@-mustfree@*/ return (c); /*@=mustfree@*/
}

static /*@observer@*/ /*@notnull@*/ ctbase
ctbase_realFunction (/*@dependent@*/ /*@notnull@*/ ctbase c)
{
  ctbase res;

  if (c->type == CT_FCN)
    {
      return c;
    }

  llassert (ctbase_isFunction (c));

  res = ctype_getCtbase (c->contents.base);
  
  llassert (ctbase_isDefined (res));

  return (res);
}

static bool
ctbase_isFunction (ctbase c)
{
  llassert (c != ctbase_undefined);

  if (c->type == CT_FCN)
    {
      return TRUE;
    }
  else
    {
      if (c->type == CT_PTR)
	{
	  ctbase fcn = ctype_getCtbase (ctbase_baseArrayPtr (c));

	  return (ctbase_isDefined (fcn) && fcn->type == CT_FCN);
	}

      return FALSE;
    }
}

/* doesn't copy c1 and c2 */

static /*@only@*/ ctbase
  ctbase_makeConj (ctype c1, ctype c2, bool isExplicit)
{
  ctbase c = ctbase_new ();

  c->type = CT_CONJ;

  c->contents.conj = (tconj) dmalloc (sizeof (*c->contents.conj));
  c->contents.conj->a = c1;
  c->contents.conj->b = c2;
  c->contents.conj->isExplicit = isExplicit;

  return (c);
}

static bool ctbase_isAnytype (/*@notnull@*/ ctbase b)
{
  /*
  ** A unknown|dne conj is a special representation for an anytype.
  */

  if (b->type == CT_CONJ)
    {
      /*@access ctype@*/
      return (b->contents.conj->a == ctype_unknown
	      && b->contents.conj->b == ctype_dne);
      /*@noaccess ctype@*/ 
    }
  
  return FALSE;
}

static ctype
ctbase_getConjA (/*@notnull@*/ ctbase c)
{
  llassert (c->type == CT_CONJ);
  return (c->contents.conj->a);
}

static ctype
ctbase_getConjB (/*@notnull@*/ ctbase c)
{
  llassert (c->type == CT_CONJ);
  return (c->contents.conj->b);
}

static bool
ctbase_isExplicitConj (/*@notnull@*/ ctbase c)
{
  llassert (c->type == CT_CONJ);
  return (c->contents.conj->isExplicit);
}

static /*@only@*/ ctbase
ctbase_createStruct (/*@only@*/ cstring n, /*@only@*/ uentryList f)
{
  ctbase c = ctbase_new ();

  c->type = CT_STRUCT;

  c->contents.su = (tsu) dmalloc (sizeof (*c->contents.su));
  c->contents.su->name = n;
  c->contents.su->fields = f;

  return (c);
}

static /*@observer@*/ uentryList
ctbase_getuentryList (/*@notnull@*/ ctbase c)
{
  c = ctbase_realType (c);

  if (!(c->type == CT_STRUCT || c->type == CT_UNION))
    llfatalbug (message ("ctbase_getuentryList: bad invocation: %q", ctbase_unparse (c)));

  return (c->contents.su->fields);
}

static ctbase
ctbase_createUnion (/*@keep@*/ cstring n, /*@only@*/ uentryList f)
{
  ctbase c = ctbase_new ();

  c->type = CT_UNION;

  c->contents.su = (tsu) dmalloc (sizeof (*c->contents.su));
  c->contents.su->name = n;
  c->contents.su->fields = f;

  return (c);
}

static ctype
ctbase_baseArrayPtr (/*@notnull@*/ ctbase c)
{
  ctuid ct;
  c = ctbase_realType (c);
  ct = c->type;

  if (ct == CT_FIXEDARRAY)
    {
      return c->contents.farray->base;
    }
  else
    {
      llassert (ctuid_isAP (ct));

      return c->contents.base;
    }
}

static ctype
ctbase_baseFunction (/*@notnull@*/ ctbase c)
{
  ctbase_fixUser (c);
  c = ctbase_realFunction (c);

  if (c->type != CT_FCN)
    {
      llfatalbug (message ("ctbase_baseFunction: bad call: %q", ctbase_unparse (c)));
    }

  return (c->contents.fcn->rval);
}

static uentryList
ctbase_argsFunction (/*@notnull@*/ ctbase c)
{
  ctbase_fixUser (c);
  c = ctbase_realFunction (c);

  if (c->type != CT_FCN)
    {
      llfatalbug (message ("ctbase_argsFunction: bad call: %q", 
			   ctbase_unparse (c)));
    }
  return (c->contents.fcn->params);
}

static bool
ctbase_baseisExpFcn (ctype c)
{
  ctbase cb;
  c = ctype_removePointers (c);

  cb = ctype_getCtbase (c);
  llassert (ctbase_isDefined (cb));

  if (cb->type == CT_FCN)
    {
      c = ctype_removePointers (ctype_getReturnType (c));

      cb = ctype_getCtbase (c);
      llassert (ctbase_isDefined (cb));

      return (cb->type == CT_EXPFCN);
    }
  return FALSE;
}

/*
** ctbase_newBase behaves specially when p is a CONJ:
**
**    c -> conj (newBase (c, p.a), p.b)
*/

static ctype
ctbase_newBase (ctype c, ctype p)
{
  ctbase cb;

  DPRINTF (("New base: %s / %s", ctype_unparse (c), ctype_unparse (p)));

  if (ctype_isUndefined (c) || ctype_isUnknown (c))
    {
      return p;
    }

  cb = ctype_getCtbase (c);

  if (ctype_isConj (p))
    {
      ctbase pb = ctype_getCtbase (p);

      llassert (ctbase_isDefined (pb));

      if (pb->contents.conj->isExplicit)
	{
	  return (ctype_makeExplicitConj (ctype_newBase (c, pb->contents.conj->a),
					  pb->contents.conj->b)); 
					  
	}
      else
	{
	  return (ctype_makeConj (ctype_newBase (c, pb->contents.conj->a),
				  pb->contents.conj->b));
				  
	}
    }

  if (ctbase_baseisExpFcn (c))
    {
      return (ctbase_newBaseExpFcn (c, p));
    }

  llassert (ctbase_isDefined (cb));

  switch (cb->type)
    {
    case CT_UNKNOWN:
    case CT_PRIM:
    case CT_USER:
    case CT_ENUM:
    case CT_ABST:
    case CT_NUMABST:
    case CT_STRUCT:
    case CT_UNION:
    case CT_EXPFCN:
      return (p);

    case CT_PTR:
      {
	ctype ret;
	ctype cbn;

	cbn = ctbase_newBase (cb->contents.base, p);
	ret = ctype_makePointer (cbn);

	return ret;
      }
    case CT_FIXEDARRAY:
      return (ctype_makeFixedArray (ctbase_newBase (cb->contents.farray->base, p),
				    cb->contents.farray->size));
    case CT_ARRAY:
      return (ctype_makeArray (ctbase_newBase (cb->contents.base, p)));
    case CT_FCN:
      return (ctype_makeRawFunction (ctbase_newBase (cb->contents.fcn->rval, p),
				      cb->contents.fcn->params));
    case CT_CONJ:
      return (ctype_makeConjAux (ctbase_newBase (cb->contents.conj->a, p),
				 ctbase_newBase (cb->contents.conj->b, p),
				 cb->contents.conj->isExplicit));
    default:
      llcontbug (message ("ctbase_newBase: bad ctbase: %q", ctbase_unparse (cb))); 
      return (p); 
    }
  BADEXIT;
}

static ctype
ctbase_newBaseExpFcn (ctype c, ctype p)
{
  ctbase cb = ctype_getCtbase (c);
  ctbase tcb;
  ctype ret, tmpct;
  ctype fp = ctype_unknown;
  uentryList ctargs = ctype_argsFunction (c);

  /*
  ** okay, this is really ugly...
  **
  ** pointers inside <expf> mean pointers to the function;
  ** pointers outside <expf> are pointers to the return value;
  ** because its a function there is one superfluous pointer.
  */
  
  /*
  ** bf is a ctype, used to derived structure of cb
  */
  
  if (!ctbase_isFunction (cb))
    llbuglit ("ctbase_newBaseExpFcn: expFcn -> not a function");
  
  tmpct = ctype_getBaseType (ctype_getReturnType (c));
  
  /*
  ** pointers before expfcn -> p are pointers to function, not result
  **
  */
  
  tcb = ctype_getCtbase (tmpct);

  llassert (ctbase_isDefined (tcb));
  tmpct = tcb->contents.base;
  
  /*
  ** record pointers to base in fp
  */
  
  while (!ctype_isUnknown (tmpct))
    {
      if (ctype_isExpFcn (tmpct)) {
	ctbase ttcb = ctype_getCtbase (tmpct);

	/*
	** evs 2000-05-16: This is necessary to deal with function pointers in parens.  
	** The whole function pointer parsing is a major kludge, but it seems to work,
	** and I'm only embarrassed by it when I haven't look at the C spec recently...
	*/
	   
	llassert (ctbase_isDefined (ttcb));
	tmpct = ttcb->contents.base;
	llassert (!ctype_isUnknown (tmpct));
      }

      switch (ctype_getCtKind (tmpct))
	{
	case CTK_PTR:
	  fp = ctype_makePointer (fp);
	  /*@switchbreak@*/ break;
	case CTK_ARRAY:
	  fp = ctype_makeArray (fp);
	  /*@switchbreak@*/ break;
	case CTK_COMPLEX:
	  {
	    ctbase fbase = ctype_getCtbase (tmpct);

	    if (ctbase_isFunction (fbase))
	      {
		fp = ctype_makeFunction (fp, uentryList_copy (ctargs));
		ctargs = ctbase_argsFunction (fbase);
	      }
	    else
	      {
		llbug 
		  (message
		   ("ctbase_newBaseExpFcn: fixing expfunction: bad complex type: %s [base: %q]",
		    ctype_unparse (tmpct), ctbase_unparse (fbase)));
	      }
	    goto exitLoop;
	  }
	default:
	  {
	    llcontbug 
	      (message ("ctbase_newBaseExpFcn: fixing expfunction: bad type: %s",
			ctype_unparse (tmpct)));
	    goto exitLoop; 
	  }
	}
      tmpct = ctype_baseArrayPtr (tmpct);
    }

 exitLoop:
  tmpct = ctype_getReturnType (c);

  /*
  ** pointers to expf are pointers to return value
  */
  
  while (!ctype_isExpFcn (tmpct))
    {
      switch (ctype_getCtKind (tmpct))
	{
	case CTK_PTR:
	  p = ctype_makePointer (p);
	  /*@switchbreak@*/ break;
	case CTK_ARRAY:
	  p = ctype_makeArray (p);
	  /*@switchbreak@*/ break;
	case CTK_COMPLEX:
	  {
	    ctbase fbase = ctype_getCtbase (tmpct);
	  
	    if (ctbase_isFunction (fbase))
	      {
		p = ctype_makeFunction (p, uentryList_copy (ctbase_argsFunction (fbase)));
	      }
	    else
	      {
		llbug 
		  (message
		   ("ctbase_newBaseExpFcn: fixing expfunction: bad complex type: %s",
		    ctype_unparse (tmpct)));
	      }
	    goto exitLoop2;
	  }

	default:
	  {
	    llcontbug 
	      (message ("ctbase_newBaseExpFcn: fixing expfunction2: bad type: %t", 
			tmpct));
	    goto exitLoop2; 
	  }
	}
      tmpct = ctype_baseArrayPtr (tmpct);
    }
  
 exitLoop2:
  
  /*
  ** pointers to fp are pointers to function type
  */

  ret = ctype_makeRawFunction (p, uentryList_copy (ctargs));
  
  while (ctype_getCtKind (fp) > CTK_PLAIN)
    {
      switch (ctype_getCtKind (fp))
	{
	case CTK_PTR:
	  ret = ctype_makePointer (ret);
	  /*@switchbreak@*/ break;
	case CTK_ARRAY:
	  ret = ctype_makeArray (ret);
	  /*@switchbreak@*/ break;
	case CTK_COMPLEX:
	  {
	    ctbase fbase = ctype_getCtbase (fp);
	    
	    if (ctbase_isFunction (fbase))
	      {
		ret = 
		  ctype_makeFunction (ret,
				      uentryList_copy (ctbase_argsFunction (fbase)));
	      }
	    else
	      {
		BADBRANCH;
	      }
	    goto exitLoop3;
	  }

	default:
	  {
	    llcontbug (message ("post-fixing expfunction: bad type: %t", fp));
	    goto exitLoop3;
	  }
	}
      fp = ctype_baseArrayPtr (fp);
    }
  
 exitLoop3:
  return (ret);
}

/*
** returns lowest level base of c: plain type
*/

static /*@notnull@*/ /*@only@*/ ctbase
ctbase_getBaseType (/*@notnull@*/ ctbase c)
{
  switch (c->type)
    {
    case CT_UNKNOWN:
    case CT_PRIM:
    case CT_USER:
    case CT_ENUM:
    case CT_ENUMLIST:
    case CT_BOOL:
    case CT_ABST:
    case CT_NUMABST:
    case CT_FCN:
    case CT_STRUCT:
    case CT_UNION:
      return (ctbase_copy (c));

    case CT_PTR:
    case CT_ARRAY:
      return (ctbase_getBaseType (ctype_getCtbaseSafe (c->contents.base)));

    case CT_FIXEDARRAY:
      return (ctbase_getBaseType (ctype_getCtbaseSafe (c->contents.farray->base)));
    case CT_CONJ:		/* base type of A conj branch? */
      return (ctbase_getBaseType (ctype_getCtbaseSafe (c->contents.conj->a)));
    case CT_EXPFCN:
      return (ctbase_copy (c));

    default:
      llfatalbug (message ("ctbase_getBaseType: bad ctbase: %q", ctbase_unparse (c)));
    }

  BADEXIT;
}

static int
ctbase_compare (ctbase c1, ctbase c2, bool strict)
{
  ctuid c1tid, c2tid;

  if (ctbase_isUndefined (c1) || ctbase_isUndefined (c2))
    {
      llcontbuglit ("ctbase_compare: undefined ctbase");
      return -1;
    }

  c1tid = c1->type;
  c2tid = c2->type;

  if (c1tid < c2tid)
    return -1;
  if (c1tid > c2tid)
    return 1;

  switch (c1tid)
    {
    case CT_UNKNOWN:
      return 0;
    case CT_PRIM:
      return (int_compare (c1->contents.prim, c2->contents.prim));
    case CT_BOOL:
      return 0;
    case CT_USER:
      return (typeId_compare (c1->contents.tid, c2->contents.tid));
    case CT_ENUMLIST:	     
      return 1;
    case CT_ENUM:		/* for now, keep like abstract */
    case CT_ABST:
    case CT_NUMABST:
      return (typeId_compare (c1->contents.tid, c2->contents.tid));
    case CT_PTR:
      return (ctype_compare (c1->contents.base, c2->contents.base));
    case CT_FIXEDARRAY:
      INTCOMPARERETURN (c1->contents.farray->size, c2->contents.farray->size);

      return (ctype_compare (c1->contents.farray->base,
			     c2->contents.farray->base));
    case CT_ARRAY:
      return (ctype_compare (c1->contents.base, c2->contents.base));
    case CT_FCN:
      {
	COMPARERETURN (ctype_compare (c1->contents.fcn->rval, c2->contents.fcn->rval));

	if (strict)
	  {
	    return (uentryList_compareStrict (c1->contents.fcn->params, 
					      c2->contents.fcn->params));
	  }
	else
	  {
	    return (uentryList_compareParams (c1->contents.fcn->params, 
					      c2->contents.fcn->params));
	  }
      }
    case CT_EXPFCN:
      return (ctype_compare (c1->contents.base, c2->contents.base));
    case CT_STRUCT:
    case CT_UNION:
      /* evs 2000-07-28: this block was missing! */
      if (strict) {
	int ncmp = cstring_compare (c1->contents.su->name,
				    c2->contents.su->name);

	if (ncmp != 0) {
	  if (isFakeTag (c1->contents.su->name) 
	      && isFakeTag (c2->contents.su->name)) {
	    ; /* If they are both fake struct tags, don't require match. */
	  } else {
	    return ncmp;
	  }
	}
      }

      DPRINTF (("Comparing fields: %s / %s",
		ctbase_unparse (c1),
		ctbase_unparse (c2)));

      return (uentryList_compareFields (c1->contents.su->fields,
					c2->contents.su->fields));
    case CT_CONJ:
      {
	COMPARERETURN (ctype_compare (c1->contents.conj->a,
				      c2->contents.conj->a));
	COMPARERETURN (ctype_compare (c1->contents.conj->b,
				      c2->contents.conj->b));
	return (bool_compare (c1->contents.conj->isExplicit,
			      c2->contents.conj->isExplicit));
      }
    }
  BADEXIT;
}

static int
ctbase_compareStrict (/*@notnull@*/ ctbase c1, /*@notnull@*/ ctbase c2)
{
 return (ctbase_compare (c1, c2, TRUE));
}

static bool ctbase_equivStrict (/*@notnull@*/ ctbase c1, /*@notnull@*/ ctbase c2)
{
  return (ctbase_compareStrict (c1,c2) == 0);
}

static bool ctbase_equiv (/*@notnull@*/ ctbase c1, /*@notnull@*/ ctbase c2)
{
  return (ctbase_compare (c1, c2, FALSE) == 0);
}

static bool
ctbase_isKind (/*@notnull@*/ ctbase c, ctuid kind)
{
  ctuid ck = c->type;

  if (ck == kind)
    return TRUE;

  if (ck == CT_CONJ)
    return (ctbase_isKind (ctype_getCtbaseSafe (c->contents.conj->a), kind) ||
	    ctbase_isKind (ctype_getCtbaseSafe (c->contents.conj->b), kind));

  return FALSE;
}

static bool
ctbase_isKind2 (/*@notnull@*/ ctbase c, ctuid kind1, ctuid kind2)
{
  ctuid ck = c->type;

  if (ck == kind1 || ck == kind2)
    return TRUE;

  if (ck == CT_CONJ)
    return (ctbase_isKind2 (ctype_getCtbaseSafe (c->contents.conj->a), kind1, kind2) ||
       ctbase_isKind2 (ctype_getCtbaseSafe (c->contents.conj->b), kind1, kind2));

  return FALSE;
}

static bool
ctbase_isAbstract (/*@notnull@*/ ctbase c)
{
  return (c->type == CT_ABST || c->type == CT_NUMABST);
}

static bool
ctbase_isNumAbstract (/*@notnull@*/ ctbase c)
{
  return (c->type == CT_NUMABST);
}

static bool ctbase_isUA (ctbase c) 
{
  return (ctbase_isDefined (c) && (ctuid_isAnyUserType (c->type)));
}

static bool
ctbase_almostEqual (ctbase c1, ctbase c2)
{
  ctuid c1tid, c2tid;
  
  /* undefined types never match */
  
  if (ctbase_isUndefined (c1) || ctbase_isUndefined (c2))
    return FALSE;
  
  c1tid = c1->type;
  c2tid = c2->type;
  
  if (c1tid == CT_FIXEDARRAY && c2tid == CT_ARRAY)
    {
      return (ctbase_almostEqual (ctype_getCtbase (c1->contents.farray->base),
				  ctype_getCtbase (c2->contents.base)));
    }
  
  if (c2tid == CT_FIXEDARRAY && c1tid == CT_ARRAY)
    {
      return (ctbase_almostEqual (ctype_getCtbase (c1->contents.base),
				  ctype_getCtbase (c2->contents.farray->base)));
    }
  
  if (c1tid != c2tid)
    return FALSE;

  switch (c1tid)
    {
    case CT_UNKNOWN:
      return TRUE;
    case CT_PRIM:
      return (cprim_equal (c1->contents.prim, c2->contents.prim));
    case CT_BOOL:
      return TRUE;
    case CT_ABST:
    case CT_NUMABST:
    case CT_USER:
      return (typeId_equal (c1->contents.tid, c2->contents.tid));
    case CT_ENUM:	
      return (cstring_equal (c1->contents.cenum->tag, c2->contents.cenum->tag));
    case CT_PTR:
      return (ctype_almostEqual (c1->contents.base, c2->contents.base));
    case CT_FIXEDARRAY:
      return (ctype_almostEqual (c1->contents.farray->base, 
				 c2->contents.farray->base));
    case CT_ARRAY:
      return (ctype_almostEqual (c1->contents.base, c2->contents.base));
    case CT_FCN:
      return (ctype_almostEqual (c1->contents.fcn->rval, c2->contents.fcn->rval)
	      && uentryList_matchParams (c1->contents.fcn->params, 
					 c2->contents.fcn->params, FALSE, TRUE));
    case CT_STRUCT:
    case CT_UNION:
      if (!cstring_isEmpty (c1->contents.su->name))
	{
	  return (cstring_equal (c1->contents.su->name, c2->contents.su->name));
	}
      else
	{
	  if (!cstring_isEmpty (c2->contents.su->name))
	    {
	      return FALSE;
	    }

	  llcontbuglit ("ctbase_almostEqual: match fields");
	  return (FALSE);
	}
    default:
      llcontbug (message ("ctbase_almostEqual: unknown type: %d\n", (int)c1tid));
      return (FALSE);
    }
}

/*drl added July 02, 001
  called by ctype_getArraySize
*/

size_t ctbase_getArraySize (ctbase ctb)
{
  /*drl 1/25/2002 fixed discovered by Jim Francis */
  ctbase r;
  
  llassert (ctbase_isDefined (ctb) );
  r = ctbase_realType (ctb);
  llassert (ctbase_isFixedArray(r) );

  return (r->contents.farray->size);
}

bool ctbase_isBigger (ctbase ct1, ctbase ct2)
{
  if (ct1 != NULL && ct2 != NULL
      && (ct1->type == CT_PRIM && ct2->type == CT_PRIM))
    {
      /* Only compare sizes for primitives */
      cprim cp1 = ct1->contents.prim;
      cprim cp2 = ct2->contents.prim;
      int nbits1 = cprim_getExpectedBits (cp1);
      int nbits2 = cprim_getExpectedBits (cp2);

      if (nbits1 > nbits2) {
	return TRUE;
      } else {
	return FALSE;
      }
    }
  else
    {
      return FALSE;
    }
}

int ctbase_getSize (ctbase ct)
{
  if (ct == NULL) 
    {
      return 0;
    }
  
  switch (ct->type) 
    {
    case CT_UNKNOWN:
    case CT_BOOL:
    case CT_PRIM:
      {
	cprim cp = ct->contents.prim;
	int nbits = cprim_getExpectedBits (cp);
	return nbits;
      }
    case CT_USER:
    case CT_ABST:
    case CT_NUMABST:
    case CT_EXPFCN:
      {
	return 0;
      }
    case CT_PTR:
      {
	/* Malloc returns void *, but they are bytes.  Normal void * is pointer size. */
	if (ctype_isVoid (ct->contents.base)) 
	  {
	    return 8;
	  }
	else
	  {
	    return ctype_getSize (ct->contents.base);
	  }
      }
    case CT_FIXEDARRAY: 
    case CT_ARRAY:
    case CT_FCN:
    case CT_STRUCT:
    case CT_UNION:
    case CT_ENUM:
    case CT_CONJ:
      break;
      BADDEFAULT;
    }

  return 0;
      
}
