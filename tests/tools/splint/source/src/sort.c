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
** sort.c
**
** sort abstraction
**
**      NOTE: The structure of this module follows a similar one
**            used in the previous LCL checker.  However, all other
**            details are quite different.
**
**  AUTHOR:
**	Yang Meng Tan,
**         Massachusetts Institute of Technology
*/

# include "splintMacros.nf"
# include "basic.h"
# include "llgrammar.h"
# include "lclscan.h"

/*@+ignorequals@*/

static lsymbol newStructTag (void) /*@*/ ;
static lsymbol newEnumTag (void) /*@*/ ;
static lsymbol newUnionTag (void) /*@*/ ;

/*@constant static int MAXBUFFLEN; @*/
# define MAXBUFFLEN 1024

/*@constant static int DELTA; @*/
# define DELTA 100

/*@constant static int NOSORTHANDLE; @*/
# define NOSORTHANDLE 0

/*@constant static int HOFSORTHANDLE; @*/
# define HOFSORTHANDLE 1

/* local routines */

static void sort_addTupleMembers (sort p_tupleSort, sort p_strSort)  
   /*@modifies internalState@*/ ;

static bool sort_isNewEntry (sortNode p_s) /*@*/ ;  

static sort sort_enterNew (/*@only@*/ sortNode p_s) 
   /*@modifies internalState@*/ ;

static sort sort_enterGlobal (/*@only@*/ sortNode p_s) /*@modifies internalState@*/ ;

static sort sort_enterNewForce (/*@only@*/ sortNode p_s) 
   /*@modifies internalState@*/ ;

static void genPtrOps (sort p_baseSort, sort p_ptrSort, sort p_arraySort);
static void genArrOps (sort p_baseSort, sort p_arraySort, int p_dim,
		       sort p_vecSort);
static void genVecOps (sort p_baseSort, sort p_vecSort, int p_dim);
static void genTupleOps (sort p_tupleSort);
static void genUnionOps (sort p_tupleSort);
static void genStrOps (sort p_strSort, sort p_tupleSort);
static void genEnumOps (sort p_enumSort);

static void overloadPtrFcns (sort p_ptrSort);
static void overloadIsSub (sort p_s, int p_dim);
static void overloadSizeof (sort p_domainSort);

/*@observer@*/ static cstring sort_unparseKind (sortKind p_k) /*@*/ ;

static /*@observer@*/ cstring
  sort_unparseKindName (sortNode p_s) /*@*/ ;

static lsymbol
  sortTag_toSymbol (char *p_kind, ltoken p_tagid, /*@out@*/ bool *p_isNew);

static void 
  overloadUnaryTok (/*@only@*/ nameNode p_nn, 
		    sort p_domainSort, /*@only@*/ ltoken p_range);
static void 
  overloadUnary (/*@only@*/ nameNode p_nn, 
		 sort p_domainSort, sort p_rangeSort);
static void 
  overloadBinary (/*@only@*/ nameNode p_nn, 
		  sort p_s, /*@only@*/ ltoken p_dTok, sort p_rs);
static /*@only@*/ nameNode makeFieldOp (lsymbol p_field);
static /*@only@*/ nameNode makeArrowFieldOp (lsymbol p_field);

# undef sp
static lsymbol sp (lsymbol p_s1, lsymbol p_s2);
static void sortError (ltoken p_t, sort p_oldsort, sortNode p_newnode);

sort g_sortBool;
sort g_sortCapBool;
sort g_sortInt;
sort g_sortChar;
sort g_sortFloat;
sort g_sortDouble;
sort g_sortCstring;

static sort sort_void;
static sort char_obj_ptrSort;
static sort char_obj_ArrSort;

/* This is used to uniqueize sort names, for anonymous C types */
static int sortUID = 1;

typedef /*@only@*/ sortNode o_sortNode;

static /*@only@*/ /*@null@*/ o_sortNode *sortTable = (sortNode *) 0;

static int sortTableSize = 0;
static int sortTableAlloc = 0;

/* Important to keep sorts in some order because importing routines
for sorts rely on this order to ensure that when we encounter a sort
S1 that is based on sort S2, S2 is before S1 in the imported file. */

static bool exporting = TRUE;

static lsymbol underscoreSymbol;
static /*@only@*/ ltoken intToken;

static /*@owned@*/ nameNode arrayRefNameNode;
static /*@owned@*/ nameNode ptr2arrayNameNode;
static /*@owned@*/ nameNode deRefNameNode;
static /*@owned@*/ nameNode nilNameNode;
static /*@owned@*/ nameNode plusNameNode;
static /*@owned@*/ nameNode minusNameNode;
static /*@owned@*/ nameNode condNameNode;
static /*@owned@*/ nameNode eqNameNode;
static /*@owned@*/ nameNode neqNameNode;

static ob_mstring sortKindName[] =
{
  "FIRSTSORT", "NOSORT", "HOFSORT",
  "PRIMITIVE", "SYNONYM", "POINTER", "OBJ", "ARRAY", "VECTOR",
  "STRUCT", "TUPLE", "UNION", "UNIONVAL", "ENUM", "LASTSORT"
} ;

static void smemberInfo_free (/*@null@*/ /*@only@*/ smemberInfo *mem)
{
  sfree (mem);
}

static void sortNode_free (/*@only@*/ sortNode sn)
{
  smemberInfo_free (sn->members);
  sfree (sn);
}

void
sort_destroyMod (void)
   /*@globals killed sortTable, killed arrayRefNameNode,
              killed ptr2arrayNameNode,killed deRefNameNode,
 	      killed nilNameNode, killed plusNameNode,
	      killed minusNameNode, killed condNameNode,
	      killed eqNameNode, killed neqNameNode @*/
{
  if (sortTable != NULL)  
    {
      int i;

      nameNode_free (arrayRefNameNode);
      nameNode_free (ptr2arrayNameNode);
      nameNode_free (deRefNameNode);
      nameNode_free (nilNameNode);
      nameNode_free (plusNameNode);
      nameNode_free (minusNameNode);
      nameNode_free (condNameNode);
      nameNode_free (eqNameNode);
      nameNode_free (neqNameNode);

      for (i = 0; i < sortTableSize; i++)
	{
	  sortNode_free (sortTable[i]);
	}

      sfree (sortTable);
      /*@-branchstate@*/
    }
} /*@=branchstate@*/

sort
sort_makeNoSort (void)
{
  return NOSORTHANDLE;
}

sort
sort_makeHOFSort (sort base)
{
  sortNode outSort;
  sort handle;

  outSort = (sortNode) dmalloc (sizeof (*outSort));
  outSort->kind = SRT_HOF;
  outSort->name = cstring_toSymbol (message ("_HOF_sort_%d", sortTableSize));
  outSort->tag = lsymbol_undefined;
  outSort->baseSort = base;
  outSort->objSort = NOSORTHANDLE;
  outSort->members = smemberInfo_undefined;
  outSort->export = exporting;
  outSort->imported = context_inImport ();
  outSort->mutable = FALSE;
  outSort->abstract = FALSE;

  llassert (sortTable != NULL);

  outSort->handle = handle = sortTableSize;
  sortTable[handle] = outSort;

  sortTableSize++;
  return handle;
}

static sort
sort_construct (lsymbol name, sortKind kind, sort baseSort,
		lsymbol tagName,
		bool mut, sort objSort, /*@null@*/ /*@only@*/ smemberInfo *members)
{
  sortNode outSort;
  sort handle;

  handle = sort_lookupName (name);

  outSort = (sortNode) dmalloc (sizeof (*outSort));
  outSort->kind = kind;
  outSort->name = name;
  outSort->tag = tagName;
  outSort->realtag = TRUE; 
  outSort->baseSort = baseSort;
  outSort->objSort = objSort;
  outSort->members = members;
  outSort->mutable = mut;
  outSort->export = exporting;
  outSort->imported = context_inImport ();
  outSort->abstract = FALSE;
  outSort->handle = handle;

  if (handle == NOSORTHANDLE)
    {
      outSort->handle = handle = sort_enterNew (outSort);
      return handle;
    }
  else
    {
      llassert (sortTable != NULL);

      if (sortTable[handle]->kind != kind)
	{
	  sortError (ltoken_undefined, handle, outSort);
	  sortNode_free (outSort);
	  return handle;
	}
      else
	{
	  /* evs --- added 11 Mar 1994
	  ** the new entry should supercede the old one, since
          ** it could be a forward reference to a struct, etc.
          */

	  sortTable[handle] = outSort;
	  return handle;
	}
    }
}

static sort
  sort_constructAbstract (lsymbol name, bool mut, sort baseSort)
{
  sortNode outSort;
  sortKind kind;
  sort handle;

  if (mut)
    kind = SRT_OBJ;
  else
    kind = SRT_PRIM;

  handle = sort_lookupName (name);
  outSort = (sortNode) dmalloc (sizeof (*outSort));
  outSort->kind = kind;
  outSort->name = name;
  outSort->tag = lsymbol_undefined;
  outSort->baseSort = baseSort;
  outSort->objSort = NOSORTHANDLE;
  outSort->members = smemberInfo_undefined;
  outSort->mutable = mut;
  outSort->export = exporting;
  outSort->imported = context_inImport ();
  outSort->abstract = TRUE;
  outSort->handle = handle;

  if (handle == NOSORTHANDLE)
    {
      outSort->handle = handle = sort_enterNew (outSort);
      /* do not make sort operators. */
    }
  else
    {
      llassert (sortTable != NULL);

      if (sortTable[handle]->kind != kind)
	{
	  sortError (ltoken_undefined, handle, outSort);
	}

      sortNode_free (outSort);
    }

  return handle;
}

sort
sort_makeSort (/*@unused@*/ ltoken t, lsymbol n)
{
  /*
  ** Expects n to be a new sort.
  ** Generate a sort with the given name.  Useful for LSL sorts. 
  */

  sort handle = sort_lookupName (n);

  if (handle == NOSORTHANDLE)
    {
      sortNode outSort;

      outSort = (sortNode) dmalloc (sizeof (*outSort));
      outSort->handle = handle;      
      outSort->kind = SRT_PRIM;
      outSort->name = n;
      outSort->tag = lsymbol_undefined;
      outSort->baseSort = NOSORTHANDLE;
      outSort->objSort = NOSORTHANDLE;
      outSort->members = smemberInfo_undefined;
      outSort->export = exporting;
      outSort->mutable = FALSE;
      outSort->imported = context_inImport ();
      outSort->abstract = FALSE;

      /* Put into sort table, sort_enter checks for duplicates. */
      handle = sort_enterNew (outSort);
    }
  else
    {
      /* don't override old info */
     ;
    }

  return handle;
}

static sort
sort_makeSortNoOps (/*@unused@*/ ltoken t, lsymbol n) /*@modifies internalState@*/ 
{
  sort handle;
  
  handle = sort_lookupName (n);

  if (handle == NOSORTHANDLE)
    {
      sortNode outSort;

      outSort = (sortNode) dmalloc (sizeof (*outSort));
      outSort->handle = handle;
      outSort->kind = SRT_PRIM;
      outSort->name = n;
      outSort->tag = lsymbol_undefined;
      outSort->baseSort = NOSORTHANDLE;
      outSort->objSort = NOSORTHANDLE;
      outSort->members = smemberInfo_undefined;
      outSort->export = exporting;
      outSort->mutable = FALSE;
      outSort->imported = context_inImport ();
      outSort->abstract = FALSE;
      /* Put into sort table, sort_enter checks for duplicates. */
      handle = sort_enterNew (outSort);
    } /* Don't override old info */

  return handle;
}

static sort
sort_makeLiteralSort (ltoken t, lsymbol n) 
   /*@modifies internalState@*/
{
  /*
  ** Like sort_makeSort, in addition, generate sizeof operator 
  ** t not currently used, may be useful for generating error msgs later 
  ** Also useful for abstract types, need sizeof operator.
  */

  sort handle = sort_makeSort (t, n);

  overloadSizeof (handle);
  return handle;
}

sort
sort_makeSyn (ltoken t, sort s, lsymbol n)
{
  /* make a synonym sort with name n that is == to sort s */
  /* expect n to be a new sort name */
  sortNode outSort;
  sort handle;
  /* must not clash with any LSL sorts */
  lsymbol newname = sp (underscoreSymbol, n);
  
  if (n == lsymbol_undefined)
    {
      llbuglit ("sort_makeSyn: synonym must have name");
    }

  handle = sort_lookupName (newname);

  outSort = (sortNode) dmalloc (sizeof (*outSort));
  outSort->kind = SRT_SYN;
  outSort->name = newname;
  outSort->baseSort = s;
  outSort->objSort = NOSORTHANDLE;
  /* info is not duplicated */
  outSort->tag = lsymbol_undefined;
  outSort->members = smemberInfo_undefined;
  outSort->export = exporting;
  outSort->mutable = FALSE;
  outSort->imported = context_inImport ();
  outSort->abstract = FALSE;
  outSort->handle = handle;

  if (handle == NOSORTHANDLE)
    {
      outSort->handle = handle = sort_enterNew (outSort);
      /* No operators to generate for synonyms */
    }
  else
    {
      llassert (sortTable != NULL);
      
      if (sortTable[handle]->kind != SRT_SYN)
	{
	  sortError (t, handle, outSort);
	}

      sortNode_free (outSort);
    }

  return handle;
}

sort
sort_makeFormal (sort insort)
{
  sortNode s;
  sort sor, handle;

  sor = sort_getUnderlying (insort);
  handle = sor;
  s = sort_lookup (sor);

  switch (s->kind)
    {
    case SRT_STRUCT:
      handle = sort_makeTuple (ltoken_undefined, sor);
      break;
    case SRT_UNION:
      handle = sort_makeUnionVal (ltoken_undefined, sor);
      break;
    default:
      break;
    }

  return handle;
}

sort
sort_makeGlobal (sort insort)
{
  /* Make a Obj if not an array or a struct */
  sortNode s;
  sort sor, handle;
  sor = sort_getUnderlying (insort);
  handle = sor;
  s = sort_lookup (sor);

  switch (s->kind)
    {
    case SRT_ARRAY:
    case SRT_STRUCT:
    case SRT_UNION:
    case SRT_HOF:
    case SRT_NONE:
      break;
    case SRT_VECTOR:
    case SRT_TUPLE:
    case SRT_UNIONVAL:
      llcontbuglit ("sort_makeGlobal: can't make vectors, tuples, or unionvals global");
      break;
    default:
      handle = sort_makeObj (sor);
      break;
    }
  return handle;
}

sort
sort_makeObj (sort sor)
{
  sortNode baseSortNode, outSort;
  sort baseSort, handle;
  lsymbol name;

 /* skip the synonym sort */
  baseSort = sort_getUnderlying (sor);
  baseSortNode = sort_quietLookup (baseSort);
  switch (baseSortNode->kind)
    {
    case SRT_HOF:
    case SRT_NONE:
      return baseSort;
    case SRT_VECTOR:
      if (baseSortNode->objSort != 0)
	return baseSortNode->objSort;
      else			/* must have well-defined objSort field */
	{
	  llcontbuglit ("sort_makeObj: Inconsistent vector reps:invalid objSort field");
	  return baseSort;
	}
    case SRT_TUPLE:
    case SRT_UNIONVAL:
     /* need to map *_Struct_Tuple to *_Struct and *_Union_UnionVal to
      *_Union, according to sort naming conventions */
      if (baseSortNode->baseSort != NOSORTHANDLE)
       /* for tuples and unionvals, baseSort field keeps the map from
          value sort to obj sort-> */
	return baseSortNode->baseSort;
      else			/* valid tuples and unionvals must have baseSort fields */
	{
	  llcontbuglit ("sort_makeObj: Inconsistent tuples or unionvals reps: invalid baseSort field");
	  return baseSort;
	}
    default:
      name = sp (sp (underscoreSymbol, sort_getLsymbol (baseSort)),
		 lsymbol_fromChars ("_Obj"));
      handle = sort_lookupName (name);

      outSort = (sortNode) dmalloc (sizeof (*outSort));
      outSort->kind = SRT_OBJ;
      /* must not clash with any LSL sorts */
      outSort->name = name;
      outSort->tag = lsymbol_undefined;
      outSort->baseSort = baseSort;
      outSort->objSort = NOSORTHANDLE;
      outSort->members = smemberInfo_undefined;
      outSort->mutable = TRUE;
      outSort->export = exporting;
      outSort->abstract = FALSE;
      outSort->handle = handle;
      outSort->imported = TRUE;

      if (handle == NOSORTHANDLE)
	{
	  if (sort_isNewEntry (outSort))
	    {
	      outSort->handle = handle = sort_enterNew (outSort);
	    }
	  else
	    {
	      outSort->handle = handle = sort_enterNew (outSort);
	    }
	}
      else
	{
	  llassert (sortTable != NULL);

	  if (sortTable[handle]->kind != SRT_OBJ)
	    {
	      sortError (ltoken_undefined, handle, outSort);
	    }

	  sortNode_free (outSort);
	}

      return handle;
    }
}

sort
sort_makePtr (ltoken t, sort baseSort)
{
  sortNode s, outSort;
  sort handle, arrayHandle;
  lsymbol name;

  s = sort_lookup (baseSort);

  if (s->kind == SRT_HOF)
    {
      return baseSort;
    }
  if (s->kind == SRT_NONE)
    {
      return baseSort;
    }

  if (s->kind != SRT_ARRAY && s->kind != SRT_STRUCT &&
      s->kind != SRT_UNION)
    /* && s->kind != SRT_OBJ) */
    /* base is not an SRT_ARRAY, struct or union.  Need to insert a obj. */
    baseSort = sort_makeObj (baseSort);
  
  name = sp (sp (underscoreSymbol, sort_getLsymbol (baseSort)),
	     lsymbol_fromChars ("_Ptr"));
  handle = sort_lookupName (name);
  
  outSort = (sortNode) dmalloc (sizeof (*outSort));
  outSort->kind = SRT_PTR;
  outSort->name = name;
  outSort->tag = lsymbol_undefined;
  outSort->baseSort = baseSort;
  outSort->objSort = NOSORTHANDLE;
  outSort->members = smemberInfo_undefined;
  outSort->mutable = FALSE;
  outSort->export = exporting;
  outSort->imported = context_inImport ();
  outSort->abstract = FALSE;
  outSort->handle = handle;
  
  if (handle == NOSORTHANDLE)
    {
      if (sort_isNewEntry (outSort))
	{
	  outSort->handle = handle = sort_enterNew (outSort);
	  arrayHandle = sort_makeArr (t, baseSort);
	  genPtrOps (baseSort, handle, arrayHandle);
	}
      else
	{
	  outSort->handle = handle = sort_enterNew (outSort);
	}
    }
  else
    {
      llassert (sortTable != NULL);

      if (sortTable[handle]->kind != SRT_PTR)
	{
	  sortError (t, handle, outSort);
	}
      
      sortNode_free (outSort);
    }

  return handle;
}

sort
sort_makePtrN (sort s, pointers p)
{
  if (pointers_isUndefined (p))
    {
      return s;
    }
  else
    {
      return sort_makePtrN (sort_makePtr (ltoken_undefined, s), 
			    pointers_getRest (p));
    }
}

sort
sort_makeArr (ltoken t, sort baseSort)
{
  sortNode s, outSort, old;
  sort handle, vecHandle;
  int dim;
  lsymbol name;

  s = sort_lookup (baseSort);

  if (s->kind == SRT_HOF)
    return baseSort;
  if (s->kind == SRT_NONE)
    return baseSort;

  if (s->kind != SRT_ARRAY && s->kind != SRT_STRUCT &&
      s->kind != SRT_UNION && s->kind != SRT_OBJ)
   /* base is not an array, struct or obj.  Need to insert a Obj. */
    baseSort = sort_makeObj (baseSort);

  name = sp (sp (underscoreSymbol, sort_getLsymbol (baseSort)),
	     lsymbol_fromChars ("_Arr"));
  handle = sort_lookupName (name);

  /* must not clash with any LSL sorts */
  outSort = (sortNode) dmalloc (sizeof (*outSort));  
  outSort->name = name;
  outSort->kind = SRT_ARRAY;
  outSort->baseSort = baseSort;
  outSort->objSort = NOSORTHANDLE;
  outSort->members = smemberInfo_undefined;
  outSort->mutable = TRUE;
  outSort->export = exporting;
  outSort->imported = context_inImport ();
  outSort->abstract = FALSE;
  outSort->handle = handle;
  
  if (handle == NOSORTHANDLE)
    {
      if (sort_isNewEntry (outSort))
	{
	  handle = sort_enterNew (outSort);
	  outSort = sort_lookup (handle);

	  for (old = outSort, dim = 0;
	       old->kind == SRT_ARRAY;
	       dim++, old = sort_lookup (old->baseSort))
	    {
	      ;
	    }

	  vecHandle = sort_makeVec (t, handle);
	  genArrOps (baseSort, handle, dim, vecHandle);
	}
      else
	{
	  outSort->handle = handle = sort_enterNew (outSort);
	}
    }
  else
    {
      llassert (sortTable != NULL);

      if (sortTable[handle]->kind != SRT_ARRAY)
	{
	  sortError (t, handle, outSort);
	}

      sortNode_free (outSort);
    }

  return handle;
}

sort
sort_makeVec (ltoken t, sort arraySort)
{
  sortNode s, outSort, old;
  sort baseSort, handle, elementSort;
  int dim;			/* array dimension count. */
  lsymbol name;

  s = sort_lookup (arraySort);

  if (s->kind == SRT_HOF)
    return arraySort;
  if (s->kind == SRT_NONE)
    return arraySort;

  if (s->kind != SRT_ARRAY)
    {
      llbug (message ("sort_makeVec: only arrays can become vectors: given sort is %s",
		      sort_unparseKind (s->kind)));
    }

  if (s->baseSort == NOSORTHANDLE)
    llbuglit ("sort_makeVec: arrays must have base (element) sort");

 /* Vectors return "values", so make array elements values. */

  baseSort = s->baseSort;
  elementSort = sort_makeVal (baseSort);

  name = sp (sp (underscoreSymbol, sort_getLsymbol (elementSort)),
	     lsymbol_fromChars ("_Vec"));
  handle = sort_lookupName (name);

  outSort = (sortNode) dmalloc (sizeof (*outSort));
  outSort->baseSort = elementSort;
  outSort->name = name;
  outSort->objSort = arraySort;
  outSort->kind = SRT_VECTOR;
  outSort->members = smemberInfo_undefined;
  outSort->mutable = FALSE;
  outSort->export = exporting;
  outSort->imported = context_inImport ();
  outSort->abstract = FALSE;
  outSort->handle = handle;

  if (handle == NOSORTHANDLE)
    {
      if (sort_isNewEntry (outSort))
	{
	  outSort = sort_lookup (handle = sort_enterNew (outSort));

	  for (old = outSort, dim = 0;
	       old->kind == SRT_VECTOR;
	       dim++, old = sort_lookup (old->baseSort))
	    {
	      ;
	    }

	  genVecOps (elementSort, handle, dim);
	}
      else
	{
	  outSort->handle = handle = sort_enterNew (outSort);
	}
    }
  else
    {
      llassert (sortTable != NULL);

      if (sortTable[handle]->kind != SRT_VECTOR)
	{
	  sortError (t, handle, outSort);
	}

      sortNode_free (outSort);
    }

  return handle;
}

sort
sort_makeVal (sort sor)
{
  sort retSort = sor;
  sortNode rsn, s;

  llassert (sortTable != NULL);
  s = sort_quietLookup (sor);

  switch (s->kind)
    {
    case SRT_PRIM:
    case SRT_ENUM:
    case SRT_PTR:
    case SRT_TUPLE:
    case SRT_UNIONVAL:
    case SRT_VECTOR:
    case SRT_HOF:
    case SRT_NONE:
     /* Do nothing for basic types and pointers. */
      retSort = sor;
      break;
    case SRT_SYN:
      return sort_makeVal (sortTable[sor]->baseSort);
    case SRT_OBJ:
     /* Strip out the last Obj's */
      if (s->baseSort == NOSORTHANDLE)
	{
	  llbuglit ("sort_makeVal: expecting a base sort for Obj");
	}
      retSort = s->baseSort;
      break;
    case SRT_ARRAY:
      retSort = sort_makeVec (ltoken_undefined, sor);
      break;
    case SRT_STRUCT:
      retSort = sort_makeTuple (ltoken_undefined, sor);
      break;
    case SRT_UNION:
      retSort = sort_makeUnionVal (ltoken_undefined, sor);
      break;
    default:
      llbuglit ("sort_makeVal: invalid sort kind");
    }
  rsn = sort_quietLookup (retSort);
  if (rsn->kind == SRT_NONE)
    {
      llfatalbug (message ("sort_makeVal: invalid return sort kind: %d", (int)rsn->kind));
    }
  return retSort;
}

sort
sort_makeImmutable (ltoken t, lsymbol name)
{
  sortNode outSort;
  sort handle;

  handle = sort_lookupName (name);

  outSort = (sortNode) dmalloc (sizeof (*outSort));
  outSort->kind = SRT_PRIM;
  outSort->name = name;
  outSort->baseSort = NOSORTHANDLE;
  outSort->objSort = NOSORTHANDLE;
  outSort->members = smemberInfo_undefined;
  outSort->export = exporting;
  outSort->mutable = FALSE;
  outSort->imported = context_inImport ();
  outSort->abstract = TRUE;
  outSort->handle = handle;

  if (handle == NOSORTHANDLE)
    {
      handle = sort_enterNew (outSort);
      outSort = sort_lookup (handle);
      overloadSizeof (handle);
    }
  else
    {				/* complain */
      llassert (sortTable != NULL);

      if ((sortTable[handle]->kind != SRT_PRIM) &&
	  (sortTable[handle]->abstract) &&
	  (!sortTable[handle]->mutable))
	{
	  sortError (t, handle, outSort);
	}

      sortNode_free (outSort);
    }

  return handle;
}

sort
sort_makeMutable (ltoken t, lsymbol name)
{
  sort immutable_old, handle, baseSort;
  lsymbol objName;

  immutable_old = sort_lookupName (name);

 /* First generate the value sort */
  baseSort = sort_makeImmutable (t, name);

  llassert (sortTable != NULL);

  /* to prevent duplicate error messages */
  if (immutable_old != NOSORTHANDLE &&
      (sortTable[baseSort]->kind != SRT_PRIM) &&
      (sortTable[baseSort]->abstract) &&
      (!sortTable[baseSort]->mutable))
    {
     /* already complained */
      handle = NOSORTHANDLE;
    }
  else
    {				/* sort_makeImmutable must have succeeded */
      sortNode outSort;

     /* must not clash with any LSL sorts */
      objName = sp (sp (underscoreSymbol, name),
		    lsymbol_fromChars ("_Obj"));
      handle = sort_lookupName (objName);

      outSort = (sortNode) dmalloc (sizeof (*outSort));
      outSort->kind = SRT_OBJ;
      outSort->name = objName;
      outSort->tag = lsymbol_undefined;
      outSort->baseSort = baseSort;
      outSort->objSort = NOSORTHANDLE;
      outSort->members = smemberInfo_undefined;
      outSort->mutable = TRUE;
      outSort->export = exporting;
      outSort->imported = context_inImport ();
      outSort->abstract = TRUE;
      outSort->handle = handle;

      if (handle == NOSORTHANDLE)
	{
	  if (sort_isNewEntry (outSort))
	    {
	      outSort->handle = handle = sort_enterNew (outSort);
	    }
	  else
	    {
	      handle = sort_enterNew (outSort);
	    }
	}
      else 
	{
	  llassert (sortTable != NULL);

	  if ((sortTable[handle]->kind != SRT_OBJ) 
	      && sortTable[handle]->abstract
	      && sortTable[handle]->mutable)
	    {
	      sortError (t, handle, outSort);
	    }

	  sortNode_free (outSort);
	}
    }
  return handle;
}

sort
sort_makeStr (ltoken opttagid)
{
  sortNode outSort;
  sort handle;
  bool isNewTag;
  lsymbol name;

  outSort = (sortNode) dmalloc (sizeof (*outSort));

  /* must not clash with any LSL sorts, tag2sortname adds "_" prefix */
  /* isNewTag true means that the name generated is new */

  if (ltoken_isUndefined (opttagid))
    {
      opttagid = ltoken_create (simpleId, newStructTag ());

      outSort->realtag = FALSE;
    }
  else
    {
      outSort->realtag = TRUE;
    }
  
  name = sortTag_toSymbol ("Struct", opttagid, &isNewTag);
  
  llassert (sortTable != NULL);
  handle = sort_lookupName (name);
  outSort->name = name;
  outSort->kind = SRT_STRUCT;
  outSort->tag = ltoken_getText (opttagid);
  outSort->baseSort = NOSORTHANDLE;
  outSort->objSort = NOSORTHANDLE;
  outSort->members = smemberInfo_undefined;
  outSort->export = exporting;
  outSort->mutable = TRUE;
  outSort->imported = context_inImport ();
  outSort->abstract = FALSE;
  outSort->handle = handle;

  if (handle == NOSORTHANDLE)
    {
      if (sort_isNewEntry (outSort))
	{
	  outSort->handle = handle = sort_enterNew (outSort);
	}
      else
	{
	  outSort->handle = handle = sort_enterNewForce (outSort);
	}
    }
  else 
    {
      if (sortTable[handle]->kind != SRT_STRUCT)
	{
	  sortError (opttagid, handle, outSort);
	}

      sortNode_free (outSort);
    }

  return handle;
}

bool
sort_updateStr (sort strSort, /*@only@*/ smemberInfo *info)
{
  /* expect strSort to be in sort table but not yet filled in */
  /* return TRUE if it is "new" */
  sort tupleSort;
  sortNode sn;
  
  llassert (sortTable != NULL);
  sn = sort_lookup (strSort);

  if (sn->members == (smemberInfo *) 0)
    {
      sortTable[strSort]->members = info;
      tupleSort = sort_makeTuple (ltoken_undefined, strSort);
      genStrOps (strSort, tupleSort);
      return TRUE;
    }
  else
    {
      smemberInfo_free (info);
      return FALSE;
    }
}

sort
sort_makeTuple (ltoken t, sort strSort)
{
  sort handle;
  sortNode outSort, s = sort_lookup (strSort);
  lsymbol name;

  if (s->kind != SRT_STRUCT)
    {
      llfatalbug (message ("sort_makeTuple: Only structs can become tuples: given sort is %s",
			   sort_unparseKind (s->kind)));
    }

  name = sp (s->name, lsymbol_fromChars ("_Tuple"));
  llassert (sortTable != NULL);
  handle = sort_lookupName (name);

  outSort = (sortNode) dmalloc (sizeof (*outSort));
  outSort->kind = SRT_TUPLE;
  outSort->name = name;
  outSort->tag = s->tag;
  outSort->realtag = s->realtag;
  outSort->baseSort = strSort;
  outSort->objSort = NOSORTHANDLE;
  outSort->members = smemberInfo_undefined;
  outSort->export = exporting;
  outSort->abstract = FALSE;
  outSort->imported = context_inImport ();
  outSort->mutable = FALSE;
  outSort->handle = handle;

  if (handle == NOSORTHANDLE)
    {
      if (sort_isNewEntry (outSort))
	{
	  outSort->handle = handle = sort_enterNew (outSort);

	  sort_addTupleMembers (handle, strSort);
	  genTupleOps (handle);
	}
      else
	{
	  outSort->handle = handle = sort_enterNew (outSort);
	}
    }
  else 
    {
      if (sortTable[handle]->kind != SRT_TUPLE)
	{
	  sortError (t, handle, outSort);
	}

      sortNode_free (outSort);
    }

  return handle;
}

static void
sort_addTupleMembers (sort tupleSort, sort strSort)
{
  smemberInfo *mem, *tail = smemberInfo_undefined;
  smemberInfo *top = smemberInfo_undefined;
  smemberInfo *newinfo;
  
  /* make sure it works for empty smemberInfo */
  
  llassert (sortTable != NULL);
  
  for (mem = sortTable[strSort]->members;
       mem != smemberInfo_undefined; mem = mem->next)
    {
      newinfo = (smemberInfo *) dmalloc (sizeof (*newinfo));
      newinfo->name = mem->name;
      newinfo->sort = sort_makeVal (mem->sort);
      newinfo->next = smemberInfo_undefined;

      if (top == smemberInfo_undefined)
	{			/* start of iteration */
	  top = newinfo;
	  tail = newinfo;
	}
      else
	{
	  llassert (tail != smemberInfo_undefined);

	  tail->next = newinfo;
	  tail = newinfo;
	  /*@-branchstate@*/ /* tail is dependent */
	} 
      /*@=branchstate@*/
    }

  sortTable[tupleSort]->members = top;
}

static 
void genTupleOps (sort tupleSort)
{
  ltoken range, dom;
  sort fieldsort;
  smemberInfo *m;
  unsigned int memCount;
  ltokenList domain = ltokenList_new ();
  sigNode signature;
  opFormUnion u;
  opFormNode opform;
  nameNode nn;

  memCount = 0;
  range = ltoken_createType (simpleId, SID_SORT, sort_getLsymbol (tupleSort));

  llassert (sortTable != NULL);
  for (m = sortTable[tupleSort]->members;
       m != smemberInfo_undefined; m = m->next)
    {
      fieldsort = sort_makeVal (m->sort);
      overloadUnary (makeFieldOp (m->name), tupleSort, fieldsort);

      dom = ltoken_createType (simpleId, SID_SORT,
			       sort_getLsymbol (fieldsort));
      ltokenList_addh (domain, dom);
      memCount++;
    }

  /* For tuples only: [__, ...]: memSorts, ... -> tupleSort */
  signature = makesigNode (ltoken_undefined, domain, range);
  u.middle = memCount;

  opform = makeOpFormNode (ltoken_copy (ltoken_lbracked),
			   OPF_BMIDDLE, u, ltoken_copy (ltoken_rbracket));

  nn = makeNameNodeForm (opform);
  symtable_enterOp (g_symtab, nn, signature);
  
  /*
  ** should not be able to take sizeof (struct^) ...
  */
}

static 
void genUnionOps (sort tupleSort)
{
 /* like genTupleOps but no constructor [ ...]: -> unionSort */
  smemberInfo *m;
  sort sort;

  llassert (sortTable != NULL);
  for (m = sortTable[tupleSort]->members;
       m != smemberInfo_undefined; m = m->next)
    {
     /* Generate __.memName: strSort ->memSortObj */
      overloadUnary (makeFieldOp (m->name), tupleSort, m->sort);
     /*    printf ("making __.%s: %s -> %s\n", lsymbol_toChars (m->name),
                sort_getName (tupleSort), sort_getName (m->sort)); */
     /* __->memName : Union_Ptr -> memSortObj */
      sort = sort_makePtr (ltoken_undefined, tupleSort);
      overloadUnary (makeArrowFieldOp (m->name), sort, m->sort);
     /*    printf ("making __->%s: %s -> %s\n", lsymbol_toChars (m->name),
                sort_getName (sort), sort_getName (m->sort)); */
    }
}

static 
void genStrOps (sort strSort, /*@unused@*/ sort tupleSort)
{
  smemberInfo *m;
  sort sort;
  
  llassert (sortTable != NULL);
  for (m = sortTable[strSort]->members;
       m != smemberInfo_undefined; m = m->next)
    {
     /* Generate __.memName: strSort ->memSortObj */
      overloadUnary (makeFieldOp (m->name), strSort, m->sort);
      /*    printf ("making __.%s: %s -> %s\n", lsymbol_toChars (m->name),
	    sort_getName (strSort), sort_getName (m->sort)); */
      /* __->memName : Struct_Ptr -> memSortObj */
      sort = sort_makePtr (ltoken_undefined, strSort);
      overloadUnary (makeArrowFieldOp (m->name), sort, m->sort);
      /*    printf ("making __->%s: %s -> %s\n", lsymbol_toChars (m->name),
	    sort_getName (sort), sort_getName (m->sort)); */
    }
  /* Generate fresh, trashed, modifies, unchanged: struct/union -> bool */
  /* Generate __any, __pre, __post: nStruct -> nTuple */
  /* Generate sizeof: strSort -> int */
  /* overloadStateFcns (strSort, tupleSort); */
}

sort
sort_makeUnion (ltoken opttagid)
{
  sortNode outSort;
  sort handle;
  bool isNewTag; 
  lsymbol name;

  /* must not clash with any LSL sorts, tag2sortname adds "_" prefix */
  /* isNewTag true means that the name generated is new */

  outSort = (sortNode) dmalloc (sizeof (*outSort));

  if (ltoken_isUndefined (opttagid))
    {
      opttagid = ltoken_create (simpleId, newUnionTag ());
      outSort->realtag = FALSE;
    }
  else
    {
      outSort->realtag = TRUE;
    }

  llassert (sortTable != NULL);
  name = sortTag_toSymbol ("Union", opttagid, &isNewTag);
  handle = sort_lookupName (name);
  outSort->name = name;
  outSort->kind = SRT_UNION;
  outSort->tag = ltoken_getText (opttagid);
  outSort->baseSort = NOSORTHANDLE;
  outSort->objSort = NOSORTHANDLE;
  outSort->members = smemberInfo_undefined;
  outSort->export = exporting;
  outSort->mutable = TRUE;
  outSort->imported = context_inImport ();
  outSort->abstract = FALSE;
  outSort->handle = handle;
  
  if (handle == NOSORTHANDLE)
    {
      if (sort_isNewEntry (outSort))
	{
	  outSort->handle = handle = sort_enterNew (outSort);
	}
      else
	{
	  outSort->handle = handle = sort_enterNewForce (outSort);
	}
    }
  else 
    {
      if (sortTable[handle]->kind != SRT_UNION)
	{
	  sortError (opttagid, handle, outSort);
	}

      sortNode_free (outSort);
    }

  return handle;
}

bool
sort_updateUnion (sort unionSort, /*@only@*/ smemberInfo *info)
{
 /* expect unionSort to be in sort table but not yet filled in */
 /* return TRUE if it is "new" */
  sort uValSort;
  sortNode sn;

  llassert (sortTable != NULL);

  sn = sort_lookup (unionSort);

  if (sn->members == (smemberInfo *) 0)
    {
      sortTable[unionSort]->members = info;
      uValSort = sort_makeUnionVal (ltoken_undefined, unionSort);
      /* same as struct operations */
      genStrOps (unionSort, uValSort);
      return TRUE;
    }
  else
    {
      smemberInfo_free (info);
      return FALSE;
    }
}

sort
sort_makeUnionVal (ltoken t, sort unionSort)
{
  sort handle;
  sortNode outSort, s = sort_lookup (unionSort);
  lsymbol name;

  if (s->kind != SRT_UNION)
    {
      llfatalbug (message ("sort_makeUnion: only unions can become unionVals: given sort is: %s",
			   sort_unparseKind (s->kind)));
    }

  llassert (sortTable != NULL);

  name = sp (s->name, lsymbol_fromChars ("_UnionVal"));
  handle = sort_lookupName (name);

  outSort = (sortNode) dmalloc (sizeof (*outSort));
  outSort->kind = SRT_UNIONVAL;
  outSort->name = name;
  outSort->tag = s->tag;
  outSort->realtag = s->realtag;
  outSort->baseSort = unionSort;
  outSort->objSort = NOSORTHANDLE;
  outSort->members = smemberInfo_undefined;
  outSort->export = exporting;
  outSort->abstract = FALSE;
  outSort->imported = context_inImport ();
  outSort->mutable = FALSE;
  outSort->handle = handle;

  if (handle == NOSORTHANDLE)
    {
      if (sort_isNewEntry (outSort))
	{
	  outSort->handle = handle = sort_enterNew (outSort);

	  /* Add members to the unionVal's. */
	  /* same as structs and tuples */

	  sort_addTupleMembers (handle, unionSort);
	  genUnionOps (handle);
	}
      else
	{
	  outSort->handle = handle = sort_enterNew (outSort);
	}
    }
  else 
    {
      if (sortTable[handle]->kind != SRT_UNIONVAL)
	{
	  sortError (t, handle, outSort);
	}

      sortNode_free (outSort);
    }

  return handle;
}

static lsymbol
newEnumTag ()
{
  static int ecount = 0;

  return (cstring_toSymbol (message ("e%s%de", context_moduleName (), ecount++)));
}

static lsymbol
newStructTag ()
{
  static int ecount = 0;

  return (cstring_toSymbol (message ("s%s%ds", context_moduleName (), ecount++)));
}

static lsymbol
newUnionTag ()
{
  static int ecount = 0;

  return (cstring_toSymbol (message ("u%s%du", context_moduleName (), ecount++)));
}

sort
sort_makeEnum (ltoken opttagid)
{
  sortNode outSort;
  sort handle;
  bool isNew;
  lsymbol name;

  llassert (sortTable != NULL);

  outSort = (sortNode) dmalloc (sizeof (*outSort));

  if (ltoken_isUndefined (opttagid))
    {
      opttagid = ltoken_create (simpleId, newEnumTag ());
      outSort->realtag = FALSE;
    }
  else
    {
      outSort->realtag = TRUE;
    }
  
  /* must not clash with any LSL sorts, tag2sortname adds "_" prefix */

  name = sortTag_toSymbol ("Enum", opttagid, &isNew);
  handle = sort_lookupName (name);
  outSort->name = name;
  outSort->kind = SRT_ENUM;
  outSort->tag = ltoken_getText (opttagid);
  outSort->baseSort = NOSORTHANDLE;
  outSort->objSort = NOSORTHANDLE;
  outSort->members = smemberInfo_undefined;
  outSort->export = exporting;
  outSort->mutable = FALSE;
  outSort->imported = context_inImport ();
  outSort->abstract = FALSE;
  outSort->handle = handle;

  if (handle == NOSORTHANDLE)
    {
      if (sort_isNewEntry (outSort))
	{
	  outSort->handle = handle = sort_enterNew (outSort);
	}
      else
	{
	  outSort->handle = handle = sort_enterNewForce (outSort);
	}
    }
  else 
    {
      if (sortTable[handle]->kind != SRT_ENUM)
	{
	  sortError (opttagid, handle, outSort);
	}

      sortNode_free (outSort);
    }

  return handle;
}

bool
sort_updateEnum (sort enumSort, /*@only@*/ smemberInfo *info)
{
  /*
  ** Expect enumSort to be in sort table but not yet filled in.
  ** Return TRUE if it is "new" 
  */

  sortNode sn;

  llassert (sortTable != NULL);

  sn = sort_lookup (enumSort);
  if (sn->members == (smemberInfo *) 0)
    {
      sortTable[enumSort]->members = info;
      genEnumOps (enumSort);
      return TRUE;
    }
  else
    {
      smemberInfo_free (info);
      return FALSE;
    }
}

static 
void genEnumOps (sort enumSort)
{
  smemberInfo *ei;
  ltokenList domain = ltokenList_new ();
  ltoken range, mem;
  nameNode nn;
  sigNode signature;

  range = ltoken_createType (simpleId, SID_SORT, sort_getLsymbol (enumSort));
  signature = makesigNode (ltoken_undefined, domain, range);

  llassert (sortTable != NULL);

  for (ei = sortTable[enumSort]->members;
       ei != (smemberInfo *) 0; ei = ei->next)
    {
      mem = ltoken_createType (simpleId, SID_OP, ei->name);
      nn = makeNameNodeId (mem);
      symtable_enterOp (g_symtab, nn, sigNode_copy (signature));
    }

  sigNode_free (signature);
  overloadSizeof (enumSort);
}

static void
genPtrOps (/*@unused@*/ sort baseSort, sort ptrSort, sort arraySort)
{
  /* Generate *__: xPtr -> x */

  /* overloadUnary (deRefNameNode, ptrSort, baseSort); */

  /* Generate maxIndex, minIndex: xPtr -> int */
  /* overloadUnaryTok (maxIndexNameNode, ptrSort, intToken); */
  /* overloadUnaryTok (minIndexNameNode, ptrSort, intToken); */

  /* Generate __[]: pointer -> array  */
  overloadUnary (nameNode_copySafe (ptr2arrayNameNode), ptrSort, arraySort);

  /* Generate __+__, __-__: pointer, int -> pointer  */
  overloadBinary (nameNode_copySafe (plusNameNode), ptrSort, 
		  ltoken_copy (intToken), ptrSort);

  overloadBinary (nameNode_copySafe (minusNameNode), ptrSort, 
		  ltoken_copy (intToken), ptrSort);

  /* Generate NIL: -> xPtr */
  /* Generate __+__: int, pointer -> pointer  */
  /* Generate __-__: pointer, pointer -> int  */
  overloadPtrFcns (ptrSort);
}

static void
genArrOps (sort baseSort, sort arraySort, int dim, /*@unused@*/ sort vecSort)
{
  /* Generate __[__]: nArr, int -> n */
  overloadBinary (nameNode_copySafe (arrayRefNameNode), arraySort, 
		  ltoken_copy (intToken), baseSort);
  
  /* Generate maxIndex, minIndex: sort -> int */
  /* overloadUnaryTok (maxIndexNameNode, arraySort, intToken); */
  /* overloadUnaryTok (minIndexNameNode, arraySort, intToken); */
  
  /* Generate isSub: arraySort, int, ... -> bool */
  overloadIsSub (arraySort, dim); 
  
  /* Generate fresh, trashed, modifies, unchanged: array -> bool  */
  /* Generate any, pre, post: array -> vector */
  
  /* overloadStateFcns (arraySort, vecSort); */
  /* overloadObjFcns (arraySort); */
}

/*
** overloadPtrFcns:
**   generate NIL: -> ptrSort
**            __+__: int, ptrSort -> ptrSort  
**            __-__: ptrSort, ptrSort -> int  
*/
static void
overloadPtrFcns (sort ptrSort)
{
  ltokenList domain = ltokenList_new ();
  ltoken range;
  sigNode signature;
  
  /* NIL: -> ptrSort */
  
  range = ltoken_createType (simpleId, SID_SORT, sort_getLsymbol (ptrSort));
  signature = makesigNode (ltoken_undefined, ltokenList_new (), ltoken_copy (range));
  symtable_enterOp (g_symtab, nameNode_copySafe (nilNameNode), signature);
  
  /* __+__: int, ptrSort -> ptrSort  */
  
  ltokenList_addh (domain, ltoken_copy (intToken));
  ltokenList_addh (domain, ltoken_copy (range));

  signature = makesigNode (ltoken_undefined, domain, ltoken_copy (range));
  symtable_enterOp (g_symtab, nameNode_copySafe (plusNameNode), signature);
  
  /* __-__: ptrSort, ptrSort -> int  */

  domain = ltokenList_new ();
  ltokenList_addh (domain, ltoken_copy (range));
  ltokenList_addh (domain, range);
  range = ltoken_copy (intToken);
  signature = makesigNode (ltoken_undefined, domain, range);
  symtable_enterOp (g_symtab, nameNode_copySafe (minusNameNode), signature);
}

static void
genVecOps (sort baseSort, sort vecSort, int dim)
{
  /* Generate __[__]: vecSort, int -> baseSort */

  overloadBinary (nameNode_copySafe (arrayRefNameNode), vecSort, 
		  ltoken_copy (intToken), baseSort);

  /*          sizeof: vecSort -> int */
  /* Generate isSub: vecSort, int, ... -> bool */

  overloadIsSub (vecSort, dim);
}

static void
overloadIsSub (sort s, int dim)
{
  /* Generate isSub: s, int, ... -> bool */
  int j, i;
  ltoken dom, nulltok = ltoken_undefined;
  ltokenList domain;
  sigNode signature;

  for (j = 1; j <= dim; j++)
    {
      nameNode isSubNameNode = (nameNode) dmalloc (sizeof (*isSubNameNode));

      isSubNameNode->isOpId = TRUE;
      isSubNameNode->content.opid = ltoken_createType (simpleId, SID_OP, 
							 lsymbol_fromChars ("isSub"));
      dom = ltoken_createType (simpleId, SID_SORT, sort_getLsymbol (s));

      domain = ltokenList_singleton (dom);

      for (i = 1; i <= j; i++)
	{
	  ltokenList_addh (domain, ltoken_copy (intToken));
	}

      signature = makesigNode (nulltok, domain, ltoken_copy (ltoken_bool));
      symtable_enterOp (g_symtab, isSubNameNode, signature);
    }
}

static void
overloadUnaryTok (/*@only@*/ nameNode nn, sort domainSort, /*@only@*/ ltoken range)
{
  /* Generate <nn>: domainSort -> rangeTok */
  sigNode signature;
  ltoken dom;
  ltokenList domain;

  dom = ltoken_createType (simpleId, SID_SORT, sort_getLsymbol (domainSort));
  domain = ltokenList_singleton (dom);
  signature = makesigNode (ltoken_undefined, domain, range);
  symtable_enterOp (g_symtab, nn, signature);
}

static void
overloadSizeof (sort domainSort)
{
  nameNode sizeofNameNode = (nameNode) dmalloc (sizeof (*sizeofNameNode));
  
  sizeofNameNode->isOpId = TRUE;
  sizeofNameNode->content.opid = ltoken_createType (simpleId, SID_OP, 
						      lsymbol_fromChars ("sizeof"));
  
  overloadUnaryTok (sizeofNameNode, domainSort, ltoken_copy (intToken));
}

static void
overloadUnary (/*@only@*/ nameNode nn, sort domainSort, sort rangeSort)
{
  ltoken range = ltoken_createType (simpleId, SID_SORT, sort_getLsymbol (rangeSort));

  overloadUnaryTok (nn, domainSort, range);
}

static void
overloadBinary (/*@only@*/ nameNode nn, sort s, /*@only@*/ ltoken dTok, sort rs)
{
  /* Generate <nn>: s, dTok -> rs */
  sigNode signature;
  ltoken range, dom;
  ltokenList domain = ltokenList_new ();

  range = ltoken_createType (simpleId, SID_SORT, sort_getLsymbol (rs));
  dom = ltoken_createType (simpleId, SID_SORT, sort_getLsymbol (s));
  
  ltokenList_addh (domain, dom);
  ltokenList_addh (domain, dTok);
  
  signature = makesigNode (ltoken_undefined, domain, range);
      symtable_enterOp (g_symtab, nn, signature);
}

static /*@only@*/ nameNode
makeFieldOp (lsymbol field)
{
 /* operator: __.<field> */
  nameNode nn;
  opFormUnion u;
  opFormNode opform;

  u.id = ltoken_createType (simpleId, SID_OP, field);
  opform = makeOpFormNode (ltoken_undefined, OPF_MSELECT, u, ltoken_undefined);
  nn = makeNameNodeForm (opform);
  return nn;
}

static /*@only@*/ nameNode
makeArrowFieldOp (lsymbol field)
{
 /* operator: __-><field> */
  nameNode nn;
  opFormUnion u;
  opFormNode opform;

  u.id = ltoken_createType (simpleId, SID_OP, field);
  opform = makeOpFormNode (ltoken_undefined, OPF_MMAP, u, ltoken_undefined);
  nn = makeNameNodeForm (opform);
  return nn;
}

void
sort_init (void) 
   /*@globals undef arrayRefNameNode,
              undef ptr2arrayNameNode,
              undef deRefNameNode,
              undef nilNameNode,
              undef plusNameNode,
              undef minusNameNode,
              undef condNameNode,
              undef eqNameNode,
              undef neqNameNode,
              undef intToken; @*/
{
  /* on alpha, declaration does not allocate storage */
  sortNode noSort, HOFSort;
  opFormNode opform;
  opFormUnion u;
  underscoreSymbol = lsymbol_fromChars ("_");

  /*
  ** commonly used data for generating operators 
  */
  
  lsymbol_setbool (lsymbol_fromChars ("bool"));
  intToken = ltoken_createType (simpleId, SID_SORT, lsymbol_fromChars ("int"));
  
  /*
  ** __ \eq __: sort, sort -> bool 
  */

  u.anyop = ltoken_copy (ltoken_eq);
  opform = makeOpFormNode (ltoken_undefined, OPF_MANYOPM, u, ltoken_undefined);
  eqNameNode = makeNameNodeForm (opform);
  
  /*
  ** __ \neq __: sort, sort -> bool 
  */

  u.anyop = ltoken_copy (ltoken_neq);
  opform = makeOpFormNode (ltoken_undefined, OPF_MANYOPM, u, ltoken_undefined);
  neqNameNode = makeNameNodeForm (opform);
  
  /*
  **if __ then __ else __: bool, sort, sort -> sort 
  */

  opform = makeOpFormNode (ltoken_undefined, OPF_IF, 
			   opFormUnion_createMiddle (0), ltoken_undefined);
  condNameNode = makeNameNodeForm (opform);
  
  /* operator: __[__]: arraySort, int -> elementSort_Obj */
  u.middle = 1;
  opform = makeOpFormNode (ltoken_copy (ltoken_lbracked), OPF_BMMIDDLE, u,
			   ltoken_copy (ltoken_rbracket));
  arrayRefNameNode = makeNameNodeForm (opform);
  
  /* operator: __[]: ptrSort -> arraySort */
  u.middle = 0;
  opform = makeOpFormNode (ltoken_copy (ltoken_lbracked), 
			   OPF_BMMIDDLE, u,
			   ltoken_copy (ltoken_rbracket));
  ptr2arrayNameNode = makeNameNodeForm (opform);
  
  /* operator: *__ */
  u.anyop = ltoken_create (LLT_MULOP, lsymbol_fromChars ("*"));
  opform = makeOpFormNode (ltoken_undefined, OPF_ANYOPM, u, ltoken_undefined);
  deRefNameNode = makeNameNodeForm (opform);
  
  /* operator: __ + __ */
  u.anyop = ltoken_create (simpleOp, lsymbol_fromChars ("+"));
  opform = makeOpFormNode (ltoken_undefined, OPF_MANYOPM, u, ltoken_undefined);
  plusNameNode = makeNameNodeForm (opform);
  
  /* operator: __ - __ */
  u.anyop = ltoken_create (simpleOp, lsymbol_fromChars ("-"));
  opform = makeOpFormNode (ltoken_undefined, OPF_MANYOPM, u, ltoken_undefined);
  minusNameNode = makeNameNodeForm (opform);
  
  /* operator: NIL */
  nilNameNode = (nameNode) dmalloc (sizeof (*nilNameNode));
  nilNameNode->isOpId = TRUE;
  nilNameNode->content.opid = ltoken_createType (simpleId, SID_OP, 
						 lsymbol_fromChars ("NIL"));

  noSort = (sortNode) dmalloc (sizeof (*noSort));
  noSort->kind = SRT_NONE;
  noSort->name = lsymbol_fromChars ("_unknown");;
  noSort->tag = lsymbol_undefined;
  noSort->baseSort = NOSORTHANDLE;
  noSort->objSort = NOSORTHANDLE;
  noSort->members = smemberInfo_undefined;
  noSort->export = FALSE;
  noSort->mutable = FALSE;
  noSort->abstract = FALSE;
  noSort->imported = FALSE;
  noSort->handle = NOSORTHANDLE;
  
  HOFSort = (sortNode) dmalloc (sizeof (*HOFSort));
  HOFSort->kind = SRT_HOF;
  HOFSort->handle = HOFSORTHANDLE;
  HOFSort->name = lsymbol_undefined;
  HOFSort->tag = lsymbol_undefined;
  HOFSort->realtag = FALSE;
  HOFSort->baseSort = NOSORTHANDLE;
  HOFSort->objSort = NOSORTHANDLE;
  HOFSort->members = smemberInfo_undefined;
  HOFSort->export = FALSE;
  HOFSort->mutable = FALSE;
  HOFSort->abstract = FALSE;
  HOFSort->imported = FALSE;

  /*
  ** Store the null sort into table, and in the process initialize the sort table. 
  ** Must be the first sort_enter so NOSORTHANDLE is truly = 0. Similarly, 
  ** for HOFSORTHANDLE = 1.
  */
  
  (void) sort_enterGlobal (noSort);
  (void) sort_enterGlobal (HOFSort); 
  
  /* Other builtin sorts */
  
  g_sortBool = sort_makeImmutable (ltoken_undefined, lsymbol_fromChars ("bool"));
  g_sortCapBool = sort_makeSortNoOps (ltoken_undefined, lsymbol_fromChars ("Bool"));
  
  llassert (sortTable != NULL);

  /* make g_sortBool a synonym for g_sortBool */
  sortTable[g_sortCapBool]->kind = SRT_SYN;
  sortTable[g_sortCapBool]->baseSort = g_sortBool;
  sortTable[g_sortCapBool]->mutable = FALSE;
  sortTable[g_sortCapBool]->abstract = TRUE;
  
  g_sortInt = sort_makeLiteralSort (ltoken_undefined, 
				   lsymbol_fromChars ("int"));
  g_sortChar = sort_makeLiteralSort (ltoken_undefined,
				    lsymbol_fromChars ("char"));
  sort_void = sort_makeLiteralSort (ltoken_undefined,
				    lsymbol_fromChars ("void"));
  
  /* g_sortCstring is char__Vec, for C strings eg: "xyz" */
  char_obj_ptrSort = sort_makePtr (ltoken_undefined, g_sortChar);
  char_obj_ArrSort = sort_makeArr (ltoken_undefined, g_sortChar);
  
  g_sortCstring = sort_makeVal (char_obj_ArrSort);
  g_sortFloat = sort_makeLiteralSort (ltoken_undefined, lsymbol_fromChars ("float"));
  g_sortDouble = sort_makeLiteralSort (ltoken_undefined, lsymbol_fromChars ("double"));
}

sort
sort_lookupName (lsymbol name)
{
  long int i;

  if (name == lsymbol_undefined)
    {
      return NOSORTHANDLE;
    }

  llassert (sortTable != NULL);

  for (i = 0; i < sortTableSize; i++)
    {
      if (sortTable[i]->name == name)
	{
	  return i;
	}
    }

  return NOSORTHANDLE;
}

static bool
sort_isNewEntry (sortNode s)
{
  int i;
  
  for (i = 0; i < sortTableSize; i++)
    {
      llassert (sortTable != NULL);

      if (sortTable[i]->kind == s->kind && sortTable[i]->name == s->name)
	{
	  return FALSE;
	}
    }
  return TRUE;
}

static sort
sort_enterGlobal (sortNode s)
{
  return (sort_enterNew (s));
}

static sort
sort_enterNew (sortNode s)
{
  /*
  ** This ensures that the argument sortNode is not entered into
  ** the sort table more than once.  isNew flag will tell the
  ** caller this info, and the caller will decide whether to generate
  ** operators for this sort. 
  */

  long int i;
  
  for (i = 0; i < sortTableSize; i++)
    {
      llassert (sortTable != NULL);

      if (sortTable[i]->kind == s->kind && sortTable[i]->name == s->name)
	{
	  sortNode_free (s);
	  return i;
	}
    }

  if (sortTableSize >= sortTableAlloc)
    {
      sortNode *oldSortTable = sortTable;

      sortTableAlloc += DELTA;
      sortTable = (sortNode *) dmalloc (sortTableAlloc * sizeof (*sortTable));

      if (sortTableSize > 0)
	{
	  llassert (oldSortTable != NULL);      
	  for (i = 0; i < sortTableSize; i++)
	    {
	      sortTable[i] = oldSortTable[i];
	    }
	}

      sfree (oldSortTable);
    }

  llassert (sortTable != NULL);

  s->handle = sortTableSize;
  sortTable[sortTableSize++] = s;

  /*@-compdef@*/ 
  return s->handle;
} /*=compdef@*/

static sort sort_enterNewForce (sortNode s)
{
  sort sor = sort_lookupName (s->name);

  if (sort_isNoSort (sor))
    {
      sor = sort_enterNew (s);
      llassert (sortTable != NULL);
      /*@-usereleased@*/
      llassert (sortTable[sor] == s);
      /*@=usereleased@*/
    }
  else
    {
      s->handle = sor;
      llassert (sortTable != NULL);
      sortTable[sor] = s;
    }
  
  /*@-globstate@*/ return (sor); /*@=globstate@*/
}

void
sort_printStats (void)
{
  /* only for debugging */
  printf ("sortTableSize = %d; sortTableAlloc = %d\n", sortTableSize,
	  sortTableAlloc);
}

sortNode
sort_lookup (sort sor)
{
  /* ymtan: can sor be 0 ? */
  /* evs --- yup...0 should return noSort ? */
  
  if (sor > 0U && sor < (unsigned) sortTableSize)
    {
      llassert (sortTable != NULL);
      return sortTable[sor];
    }

  llassert (sor == 0);
  llassert (sor == NOSORTHANDLE);
  llassert (sortTable != NULL);
  return sortTable[NOSORTHANDLE];
}

sortNode
sort_quietLookup (sort sor)
{
  /* ymtan: can sor be 0 ? */
  if (sor > 0U && sor < (unsigned) sortTableSize)
    {
      llassert (sortTable != NULL);
      return (sortTable[sor]);
    }
  else
    {
      llassert (sortTable != NULL);
      return (sortTable[NOSORTHANDLE]);
    }
}

static cstring
printEnumMembers (/*@null@*/ smemberInfo *list)
{
  cstring out = cstring_undefined;
  smemberInfo *m;

  for (m = list; m != (smemberInfo *) 0; m = m->next)
    {
      out = cstring_concat (out, lsymbol_toString (m->name));

      if (m->next != (smemberInfo *) 0)
	{
	  out = cstring_concatChars (out, ", ");
	}
    }
  return out;
}

static /*@only@*/ cstring
printStructMembers (/*@null@*/ smemberInfo *list)
{
  cstring ret = cstring_undefined;
  smemberInfo *m;

  for (m = list; m != (smemberInfo *) 0; m = m->next)
    {
      ret = message ("%q%q %s; ",
		     ret, sort_unparse (m->sort), 
		     cstring_fromChars (lsymbol_toChars (m->name)));
    }

  return ret;
}

/*@only@*/ cstring
sort_unparse (sort s)
{
 /* printing routine for sorts */
  sortNode sn;
  lsymbol name;

  sn = sort_quietLookup (s);
  name = sn->name;

  switch (sn->kind)
    {
    case SRT_NONE:
      if (name == lsymbol_undefined)
	{
	  return cstring_makeLiteral ("_unknown");
	}

      return (cstring_fromCharsNew (lsymbol_toChars (name)));
    case SRT_HOF:
      return cstring_makeLiteral ("procedural");
    case SRT_PRIM:
      return (cstring_fromCharsNew (lsymbol_toChars (name)));
    case SRT_SYN:
      return (cstring_fromCharsNew (lsymbol_toChars (name)));

    case SRT_PTR:
      return (message ("%q *", sort_unparse (sort_makeVal (sn->baseSort))));
    case SRT_OBJ:
      return (message ("obj %q", sort_unparse (sn->baseSort)));
    case SRT_ARRAY:
      return (message ("array of %q", sort_unparse (sort_makeVal (sn->baseSort))));
    case SRT_VECTOR:
      return (message ("vector of %q", sort_unparse (sn->baseSort)));
    case SRT_TUPLE:
      if (sn->tag != lsymbol_undefined && sn->realtag)
	{
	  return (message ("struct %s", cstring_fromChars (lsymbol_toChars (sn->tag))));
	}
      else
	{
	  return (message ("struct {%q}", printStructMembers (sn->members)));
	}
    case SRT_UNIONVAL:
      if (sn->tag != lsymbol_undefined && sn->realtag)
	{
	  return (message ("union %s", cstring_fromChars (lsymbol_toChars (sn->tag))));
	}
      else
	{
	  return (message ("union {%q}", printStructMembers (sn->members)));
	}
    case SRT_ENUM:
      if (sn->tag != lsymbol_undefined && sn->realtag)
	{
	  return (message ("enum %s", cstring_fromChars (lsymbol_toChars (sn->tag))));
	}
      else
	{
	  return (message ("enum {%q}", printEnumMembers (sn->members)));
	}
    case SRT_STRUCT:
      if (sn->tag != lsymbol_undefined && sn->realtag)
	{
	  return (message ("obj struct %s", cstring_fromChars (lsymbol_toChars (sn->tag))));
	}
      else
	{
	  return (message ("obj struct {%q}", printStructMembers (sn->members)));
	}
    case SRT_UNION:
      if (sn->tag != lsymbol_undefined && sn->realtag)
	{
	  return (message ("obj union %s", cstring_fromChars (lsymbol_toChars (sn->tag))));
	}
      else
	{
	  return (message ("obj union {%q}", printStructMembers (sn->members)));
	}
    default:
      return (cstring_makeLiteral ("illegal"));
    }
}

static lsymbol
sp (lsymbol s1, lsymbol s2)
{
  char buff[MAXBUFFLEN];
  char *name1Ptr;
  char *name2Ptr;
  int temp_length;

  name1Ptr = lsymbol_toCharsSafe (s1);
  name2Ptr = lsymbol_toCharsSafe (s2);

  if (strlen (name1Ptr) + strlen (name2Ptr) + 1 > MAXBUFFLEN)
    {
      temp_length = strlen (name1Ptr) + strlen (name2Ptr) + 1;
      llfatalbug (message ("sp: name too long: %s%s", 
			   cstring_fromChars (name1Ptr), 
			   cstring_fromChars (name2Ptr)));
    }

  strcpy (&buff[0], name1Ptr);
  strcat (&buff[0], name2Ptr);

  return lsymbol_fromChars (&buff[0]);
}

static lsymbol
sortTag_toSymbol (char *kind, ltoken tagid, /*@out@*/ bool *isNew)
{
  /* 
  ** kind could be struct, union or enum.  Create a unique sort
  ** name based on the given info. But first check that tagid
  ** has not been defined already. (ok if it is a forward decl) 
  **/

  tagInfo to;

  if (ltoken_isUndefined (tagid))
    {
      *isNew = TRUE;
      return (cstring_toSymbol (message ("_anon_%s%d", cstring_fromChars (kind), sortUID++)));
    }
  else
    {
      to = symtable_tagInfo (g_symtab, ltoken_getText (tagid));

      if (tagInfo_exists (to))
	{
	  *isNew = FALSE;
	}
      else
	{
	  *isNew = TRUE;
	}

      return (cstring_toSymbol (message ("_%s_%s", 
					 ltoken_unparse (tagid), 
					 cstring_fromChars (kind))));
    }
}

/*@constant int MAX_SORT_DEPTH@*/
# define MAX_SORT_DEPTH 10

static sort
sort_getUnderlyingAux (sort s, int depth)
{
  sortNode sn = sort_quietLookup (s);
  
  if (sn->kind == SRT_SYN)
    {
      if (depth > MAX_SORT_DEPTH)
	{
	  llcontbug (message ("sort_getUnderlying: depth charge: %d", depth));
	  return s;
	}
      
      return sort_getUnderlyingAux (sn->baseSort, depth + 1);
    }
  
  return s;
}

sort
sort_getUnderlying (sort s)
{
  return sort_getUnderlyingAux (s, 0);
}

static lsymbol
underlyingSortName (sortNode sn)
{
  if (sn->kind == SRT_SYN)
    return underlyingSortName (sort_quietLookup (sn->baseSort));
  return sn->name;
}

static /*@observer@*/ sortNode
underlyingSortNode (sortNode sn)
{
  if (sn->kind == SRT_SYN)
    {
      return underlyingSortNode (sort_quietLookup (sn->baseSort));
    }

  return sn;
}

bool
sort_mutable (sort s)
{
 /* if s is not a valid sort, then returns false */
  sortNode sn = sort_quietLookup (s);
  if (sn->mutable)
    return TRUE;
  return FALSE;
}

bool
sort_setExporting (bool flag)
{
  bool old;
  old = exporting;
  exporting = flag;
  return old;
}

/*@observer@*/ static cstring 
sort_unparseKind (sortKind k)
{
  if (k > SRT_FIRST && k < SRT_LAST)
    return (cstring_fromChars (sortKindName[(int)k]));
  else
    return (cstring_makeLiteralTemp ("<unknown sort kind>"));
}

bool
sort_isValidSort (sort s)
{
  sortNode sn = sort_quietLookup (s);
  sortKind k = sn->kind;
  if (k != SRT_NONE && k > SRT_FIRST && k < SRT_LAST)
    return TRUE;
  else
    return FALSE;
}

void
sort_dump (FILE *f, bool lco)
{
  int i;
  sortNode s;
  smemberInfo *mem;

  fprintf (f, "%s\n", BEGINSORTTABLE);
  llassert (sortTable != NULL);

  for (i = 2; i < sortTableSize; i++)
    {
      /* skips 0 and 1, noSort and HOFSort */
      s = sortTable[i];
      
      /* if (lco && !s.export) continue; */
      /* Difficult to keep track of where each op and sort belong to
	 which LCL type.  Easiest to export them all (even private sorts and
	 op's) but for checking imported modules, we only use LCL types and
	 variables to check, i.e., we don't rely on sorts and op's for such
	 checking. */
      
      if (s->kind == SRT_NONE)
	continue;
      
      if (lco)
	{
	  fprintf (f, "%%LCL");
	}

      if (lsymbol_isDefined (s->name))
	{
	  fprintf (f, "sort %s ", lsymbol_toCharsSafe (s->name));
	}
      else
	{
	  llcontbug (message ("Invalid sort in sort_dump: sort %d; sortname: %s. "
			      "This may result from using .lcs files produced by an old version of Splint. "
			      "Remove the .lcs files, and rerun Splint.",
			      i, lsymbol_toString (s->name)));
	  fprintf (f, "sort _error_ ");
	}
      
      if (!lco && !s->export)
	fprintf (f, "private ");

      /*@-loopswitchbreak@*/
      switch (s->kind)
	{
	case SRT_HOF:
	  fprintf (f, "hof nil nil\n");
	  break;
	case SRT_PRIM:
	  if (s->abstract)
	    fprintf (f, "immutable nil nil\n");
	  else
	    fprintf (f, "primitive nil nil\n");
	  break;
	case SRT_OBJ:
	  if (s->abstract)
	    fprintf (f, "mutable %s nil\n",
		     lsymbol_toCharsSafe (sortTable[s->baseSort]->name));
	  else
	    fprintf (f, "obj %s nil\n",
		     lsymbol_toCharsSafe (sortTable[s->baseSort]->name));
	  break;
	case SRT_SYN:
	  fprintf (f, "synonym %s nil\n",
		   lsymbol_toCharsSafe (sortTable[s->baseSort]->name));
	  break;
	case SRT_PTR:
	  fprintf (f, "ptr %s nil\n", lsymbol_toCharsSafe (sortTable[s->baseSort]->name));
	  break;
	case SRT_ARRAY:
	  fprintf (f, "arr %s nil\n",
		   lsymbol_toCharsSafe (sortTable[s->baseSort]->name));
	  break;
	case SRT_VECTOR:
	  fprintf (f, "vec %s %s\n",
		   lsymbol_toCharsSafe (sortTable[s->baseSort]->name),
		   lsymbol_toCharsSafe (sortTable[s->objSort]->name));
	  break;
	case SRT_STRUCT:
	  if (s->tag == lsymbol_undefined)
	    {
	      /* we need to make up a tag to prevent excessive
		 growth of .lcs files when tags are overloaded
		 */
	      llbuglit ("Struct has no tag");
	    }
	  else
	    fprintf (f, "str %s nil\n", lsymbol_toCharsSafe (s->tag));

	  for (mem = s->members;
	       mem != smemberInfo_undefined; mem = mem->next)
	    {
	      if (lco)
		fprintf (f, "%%LCL");
	      fprintf (f, "sort %s strMem %s nil\n", lsymbol_toCharsSafe (mem->name),
		       lsymbol_toCharsSafe (sortTable[mem->sort]->name));
	    }
	  if (lco)
	    fprintf (f, "%%LCL");
	  fprintf (f, "sort strEnd nil nil nil\n");
	  break;
	case SRT_UNION:
	  if (s->tag == lsymbol_undefined)
	    llbuglit ("Union has no tag");
	  else
	    fprintf (f, "union %s nil\n", lsymbol_toCharsSafe (s->tag));
	  for (mem = s->members;
	       mem != smemberInfo_undefined; mem = mem->next)
	    {
	      if (lco)
		fprintf (f, "%%LCL");
	      fprintf (f, "sort %s unionMem %s nil\n", lsymbol_toCharsSafe (mem->name),
		       lsymbol_toCharsSafe (sortTable[mem->sort]->name));
	    }
	  if (lco)
	    fprintf (f, "%%LCL");
	  fprintf (f, "sort unionEnd nil nil nil\n");
	  break;
	case SRT_ENUM:
	  if (s->tag == lsymbol_undefined)
	    {
	      llbuglit ("Enum has no tag");
	    }

	  fprintf (f, "enum %s nil\n", lsymbol_toCharsSafe (s->tag));

	  for (mem = s->members;
	       mem != smemberInfo_undefined; mem = mem->next)
	    {
	      if (lco)
		fprintf (f, "%%LCL");
	      fprintf (f, "sort %s enumMem nil nil\n", lsymbol_toCharsSafe (mem->name));
	    }
	  if (lco)
	    fprintf (f, "%%LCL");
	  fprintf (f, "sort enumEnd nil nil nil\n");
	  break;
	case SRT_TUPLE:
	  fprintf (f, "tup %s nil\n", 
		   lsymbol_toCharsSafe (sortTable[s->baseSort]->name));
	  break;
	case SRT_UNIONVAL:
	  fprintf (f, "unionval %s nil\n",
		   lsymbol_toCharsSafe (sortTable[s->baseSort]->name));
	  break;
	default:
	  fprintf (f, "sort_dump: unexpected sort: %d", (int)s->kind);
	}			/* switch */
      /*@=loopswitchbreak@*/
    }

  fprintf (f, "%s\n", SORTTABLEEND);
}

static void
sort_loadOther (char *kstr, lsymbol sname, sort bsort)
{
  if (strcmp (kstr, "synonym") == 0)
    {
      (void) sort_construct (sname, SRT_SYN, bsort, lsymbol_undefined,
			     FALSE, NOSORTHANDLE, smemberInfo_undefined);
    }
  else if (strcmp (kstr, "mutable") == 0)
    {
      (void) sort_constructAbstract (sname, TRUE, bsort);
    }
  else if (strcmp (kstr, "obj") == 0)
    {
      (void) sort_construct (sname, SRT_OBJ, bsort, lsymbol_undefined,
			     TRUE, NOSORTHANDLE, smemberInfo_undefined);
    }
  else if (strcmp (kstr, "ptr") == 0)
    {
      (void) sort_construct (sname, SRT_PTR, bsort, lsymbol_undefined,
			     FALSE, NOSORTHANDLE, smemberInfo_undefined);
    }
  else if (strcmp (kstr, "arr") == 0)
    {
      (void) sort_construct (sname, SRT_ARRAY, bsort, lsymbol_undefined,
			     TRUE, NOSORTHANDLE, smemberInfo_undefined);
    }
  else if (strcmp (kstr, "tup") == 0)
    {
      (void) sort_construct (sname, SRT_TUPLE, bsort, lsymbol_undefined,
			     FALSE, NOSORTHANDLE, smemberInfo_undefined);
    }
  else if (strcmp (kstr, "unionval") == 0)
    {
      (void) sort_construct (sname, SRT_UNIONVAL, bsort, lsymbol_undefined,
			     FALSE, NOSORTHANDLE, smemberInfo_undefined);
    }
  else
    {
      llbug (message ("Unhandled: %s", cstring_fromChars (kstr)));
    }
}

static void
parseSortLine (char *line, ltoken t, inputStream  s,
	       mapping map, lsymbolList slist)
{
  /* caller expects that map and slist are updated */
  /* t and importfle are only used for error messages */
  static lsymbol strName = lsymbol_undefined;
  static smemberInfo *strMemList = NULL;
  static lsymbol unionName = lsymbol_undefined;
  static smemberInfo *unionMemList = NULL;
  static lsymbol enumName = lsymbol_undefined;
  static smemberInfo *enumMemList = NULL;
  static lsymbol tagName = lsymbol_undefined;
  
  cstring importfile = inputStream_fileName (s);
  char sostr[MAXBUFFLEN], kstr[10], basedstr[MAXBUFFLEN], objstr[MAXBUFFLEN];
  bool tmp;
  tagInfo ti;
  lsymbol sname, bname, new_name, objName;
  sort objSort;
  char *lineptr;
  int col;			/* for keeping column number */
  ltoken tagid;
  
  if (sscanf (line, "sort %s %s %s %s", &(sostr[0]), &(kstr[0]),
	      &(basedstr[0]), &(objstr[0])) != 4)
    {		
      /* if this fails, can have weird errors */
      /* strEnd, unionEnd, enumEnd won't return 4 args */
      lclplainerror 
	(message ("%q: Imported file contains illegal sort declaration.   "
		  "Skipping this line: \n%s\n",
		  fileloc_unparseRaw (importfile, inputStream_thisLineNumber (s)), 
		  cstring_fromChars (line)));
      return;
    }
  
  sname = lsymbol_fromChars (sostr);
  if (sname == lsymbol_fromChars ("nil"))
    {
      /* No given sort name.  Use lsymbol_undefined and generate sort name
	 in sort building routines. */
      sname = lsymbol_undefined;
      lclerror (t, message ("Illegal sort declaration in import file: %s:\n%s",
			    importfile, 
			    cstring_fromChars (line)));
    }
  
  /* Assume that when we encounter a sort S1 that is based on sort
     S2, S2 is before S1 in the imported file.  sort table is a
     linear list and we create base sorts before other sorts. */
  
  bname = lsymbol_fromChars (basedstr);
  if (strcmp (kstr, "primitive") == 0)
    {
      new_name = lsymbol_translateSort (map, sname);
      (void) sort_construct (new_name, SRT_PRIM, NOSORTHANDLE,
			     lsymbol_undefined, FALSE,
			     NOSORTHANDLE, smemberInfo_undefined);
    }
  else if (strcmp (kstr, "strMem") == 0)
    {
      smemberInfo *mem = (smemberInfo *) dmalloc (sizeof (*mem));
      mem->next = strMemList;
      mem->name = sname;
      mem->sortname = bname;
      mem->sort = NOSORTHANDLE;
      strMemList = mem;
    }
  else if (strcmp (sostr, "strEnd") == 0)
    {				/* now process it */
      if (strName != lsymbol_undefined && strMemList != NULL)
	{
	  sort asort = sort_construct (strName, SRT_STRUCT, NOSORTHANDLE, tagName,
				       TRUE, NOSORTHANDLE, strMemList);
	  
	  if (tagName != lsymbol_undefined)
	    {
	      tagid = ltoken_create (simpleId, tagName);

	      ti = (tagInfo) dmalloc (sizeof (*ti));
	      ti->sort = asort;
	      ti->kind = TAG_STRUCT;
	      ti->id = tagid;
	      ti->imported = FALSE;
	      
	      (void) symtable_enterTagForce (g_symtab, ti);
	    }
	}
      else
	{
	  if (strName == lsymbol_undefined)
	    {
	      lclbug (message ("%q: Imported file contains unexpected null struct sort",
			       fileloc_unparseRaw (importfile, inputStream_thisLineNumber (s))));
	    }
	  else
	    {
	      /*
	       ** no members -> its a forward struct
	       */
	      
	      if (tagName != lsymbol_undefined)
		{
		  tagid = ltoken_create (simpleId, tagName);
		  (void) checkAndEnterTag (TAG_FWDSTRUCT, tagid);
		}
	    }
	}
      strName = lsymbol_undefined;
      strMemList = NULL;
      tagName = lsymbol_undefined;
    }
  else if (strcmp (kstr, "str") == 0)
    {
      if (strName != lsymbol_undefined || strMemList != NULL)
	{
	  lclbug (message ("%q: unexpected non-null struct sort or "
			   "non-empty member list",
			   fileloc_unparseRaw (importfile, 
					       inputStream_thisLineNumber (s))));
	}
      /* see if a tag is associated with this sort */
      if (strcmp (basedstr, "nil") == 0)
	{
	  llfatalerror (message ("%s: Struct missing tag.  Obsolete .lcs file, remove and rerun lcl.",
				 importfile));
	  /*
	    strName = sortTag_toSymbol ("Struct", nulltok, &tmp);
	    tagName = lsymbol_undefined;
	    mapping_bind (map, sname, strName);
	    */
	}
      else /* a tag exists */
	{  /* create tag in symbol table and add tagged sort in sort table */
	  tagName = bname;
	  tagid = ltoken_create (simpleId, bname);

	  strName = sortTag_toSymbol ("Struct", tagid, &tmp);
	  ti = symtable_tagInfo (g_symtab, tagName);

	  /*
	  ** No error for redefining a tag in an import.
	  */
	}
      /* to be processed later in sort_import */
      lsymbolList_addh (slist, strName);
    }
  else if (strcmp (kstr, "enumMem") == 0)
    {
      smemberInfo *mem = (smemberInfo *) dmalloc (sizeof (*mem));
      mem->next = enumMemList;
      mem->sortname = enumName;
      mem->name = sname;
      mem->sort = NOSORTHANDLE;
      enumMemList = mem;
    }
  else if (strcmp (sostr, "enumEnd") == 0)
    {
      if (enumName != lsymbol_undefined && enumMemList != NULL)
	{
	  sort asort = sort_construct (enumName, SRT_ENUM, NOSORTHANDLE, tagName,
				       FALSE, NOSORTHANDLE, enumMemList);
	  
	  if (tagName != lsymbol_undefined)
	    {
	      tagid = ltoken_create (simpleId, tagName);

	      ti = (tagInfo) dmalloc (sizeof (*ti));
	      ti->sort = asort;
	      ti->kind = TAG_ENUM;
	      ti->id = tagid;
	      ti->imported = FALSE;

	      (void) symtable_enterTagForce (g_symtab, ti);
	    }
	}
      else
	{
	  lclbug (message ("%q: unexpected null enum sort or empty member list",
			   fileloc_unparseRaw (importfile, inputStream_thisLineNumber (s))));
	}
      enumName = lsymbol_undefined;
      enumMemList = NULL;
      tagName = lsymbol_undefined;
    }
  else if (strcmp (kstr, "enum") == 0)
    {
      if (enumName != lsymbol_undefined || enumMemList != NULL)
	{
	  lclbug (message ("%q: Unexpected non-null enum sort or "
			   "non-empty member list",
			   fileloc_unparseRaw (importfile, 
					       inputStream_thisLineNumber (s))));
	}

      /* see if a tag is associated with this sort */
      if (strcmp (basedstr, "nil") == 0)
	{
	  llfatalerror (message ("%s: Enum missing tag.  Obsolete .lcs file, "
				 "remove and rerun lcl.",
				 importfile));
	}
      else
	{			/* a tag exists */
	  tagName = bname;
	  tagid = ltoken_create (simpleId, bname);
	  enumName = sortTag_toSymbol ("Enum", tagid, &tmp);
	  ti = symtable_tagInfo (g_symtab, bname);
	}
    }
  else if (strcmp (kstr, "unionMem") == 0)
    {
      smemberInfo *mem = (smemberInfo *) dmalloc (sizeof (*mem));
      mem->next = unionMemList;
      mem->sortname = bname;
      mem->name = sname;
      mem->sort = NOSORTHANDLE;
      unionMemList = mem;
    }
  else if (strcmp (sostr, "unionEnd") == 0)
    {
      if (unionName != lsymbol_undefined && unionMemList != NULL)
	{
	  sort asort = sort_construct (unionName, SRT_UNION, NOSORTHANDLE, tagName,
				       FALSE, NOSORTHANDLE, unionMemList);

	  if (tagName != lsymbol_undefined)
	    {
	      tagid = ltoken_create (simpleId, tagName);

	      ti = (tagInfo) dmalloc (sizeof (*ti));
	      ti->sort = asort;
	      ti->kind = TAG_UNION;
	      ti->id = tagid;
	      ti->imported = FALSE;

	      (void) symtable_enterTagForce (g_symtab, ti);
	    }
	}
      else
	{
	  if (unionName == lsymbol_undefined)
	    {
	      lclbug
		(message ("%q: Imported file contains unexpected null union sort",
			  fileloc_unparseRaw (importfile, inputStream_thisLineNumber (s))));
	    }
	  else
	    {
	      /*
	       ** no members -> its a forward struct
	       */
	      
	      if (tagName != lsymbol_undefined)
		{
		  tagid = ltoken_create (simpleId, tagName);

		  (void) checkAndEnterTag (TAG_FWDUNION, tagid);
		}
	    }
	}

      unionName = lsymbol_undefined;
      unionMemList = NULL;
      tagName = lsymbol_undefined;
    }
  else if (strcmp (kstr, "union") == 0)
    {
      if (unionName != lsymbol_undefined || unionMemList != NULL)
	{
	  lclbug
	    (message 
	     ("%q: Unexpected non-null union sort or non-empty "
	      "member list",
	      fileloc_unparseRaw (importfile, inputStream_thisLineNumber (s))));
	}
      /* see if a tag is associated with this sort */
      if (strcmp (basedstr, "nil") == 0)
	{
	  llfatalerror
	    (message ("%s: Union missing tag.  Obsolete .lcs file, "
		      "remove and rerun lcl.",
	      importfile));
	}
      else
	{			/* a tag exists */
	  tagName = bname;
	  tagid = ltoken_create (simpleId, bname);

	  unionName = sortTag_toSymbol ("Union", tagid, &tmp);
	  ti = symtable_tagInfo (g_symtab, bname);
	}
      lsymbolList_addh (slist, unionName);
    }
  else if (strcmp (kstr, "immutable") == 0)
    {
      (void) sort_constructAbstract (sname, FALSE, NOSORTHANDLE);
    }
  else if (strcmp (kstr, "hof") == 0)
    {
      (void) sort_construct (sname, SRT_HOF, NOSORTHANDLE, lsymbol_undefined,
			     FALSE, NOSORTHANDLE, smemberInfo_undefined);
    }
  else
    {
      sort bsort = sort_lookupName (lsymbol_translateSort (map, bname));

      if (sort_isNoSort (bsort))
	{
	  lineptr = strchr (line, ' ');	/* go past "sort" */
	  llassert (lineptr != NULL);
	  lineptr = strchr (lineptr + 1, ' ');	/* go past sostr */
	  llassert (lineptr != NULL);
	  lineptr = strchr (lineptr + 1, ' ');	/* go past kstr */
	  llassert (lineptr != NULL);
	  col = 5 + lineptr - line;	/* 5 for initial "%LCL "*/

	  llbug 
	    (message ("%q: Imported file contains unknown base sort: %s",
		      fileloc_unparseRawCol (importfile, 
					     inputStream_thisLineNumber (s), col),
		      cstring_fromChars (lsymbol_toCharsSafe (bname))));
	}
      
      if (strcmp (kstr, "vec") == 0)
	{			
	  objName = lsymbol_fromChars (objstr);
	  objSort = sort_lookupName (lsymbol_translateSort (map, objName));
	  (void) sort_construct (sname, SRT_VECTOR, bsort, lsymbol_undefined,
				 FALSE, objSort, smemberInfo_undefined);
	}
      else
	{
	  sort_loadOther (kstr, sname, bsort);
	}
    } 
}

void
sort_import (inputStream imported, ltoken tok, mapping map)
{
  /* tok is only used for error message line number */
  char *buf;
  cstring importfile;
  inputStream lclsource;
  sort bsort;
  lsymbolList slist = lsymbolList_new ();

  buf = inputStream_nextLine (imported);

  llassert (buf != NULL);

  importfile = inputStream_fileName (imported);

  if (!firstWord (buf, "%LCLSortTable"))
    {
      lclsource = LCLScanSource ();

      lclfatalerror (tok, message ("Expecting \"%%LCLSortTable\" line "
				   "in import file %s:\n%s",
				   importfile, 
				   cstring_fromChars (buf)));
      
    }

  for (;;)
    {
      buf = inputStream_nextLine (imported);

      llassert (buf != NULL);

      if (firstWord (buf, "%LCLSortTableEnd"))
	{
	  break;
	}
      else
	{ /* a good line, remove %LCL from line first */
	  if (firstWord (buf, "%LCL"))
	    {
	      parseSortLine (buf + 4, tok, imported, map, slist);
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

  /* now process the smemberInfo in the sort List */
  lsymbolList_elements (slist, s)
    {
      if (s != lsymbol_undefined)
	{
	  sort sor;
	  sortNode sn;

	  sor = sort_lookupName (s);
	  sn = sort_quietLookup (sor);
	  
	  switch (sn->kind)
	    {
	    case SRT_ENUM:
	      {			/* update the symbol table with members of enum */
		varInfo vi;
		smemberInfo *mlist = sn->members;
		for (; mlist != NULL; mlist = mlist->next)
		  {
		    /* check that enumeration constants are unique */
		    vi = symtable_varInfo (g_symtab, mlist->name);
		    if (!varInfo_exists (vi))
		      {	/* put info into symbol table */
			vi = (varInfo) dmalloc (sizeof (*vi));
			vi->id = ltoken_create (NOTTOKEN, mlist->name);
			vi->kind = VRK_ENUM;
			vi->sort = sor;
			vi->export = TRUE;

			(void) symtable_enterVar (g_symtab, vi);
			varInfo_free (vi);
		      }
		    else
		      {
			lclplainerror 
			  (message ("%s: enum member %s of %s has already been declared",
				    importfile, 
				    lsymbol_toString (mlist->name),
				    lsymbol_toString (sn->name)));
		      }
		  }
		/*@switchbreak@*/ break;
	      }
	    case SRT_STRUCT:
	    case SRT_UNION:
	      {
		smemberInfo *mlist = sn->members;

		for (; mlist != NULL; mlist = mlist->next)
		  {
		    bsort = sort_lookupName (lsymbol_translateSort (map, mlist->sortname));
		    if (sort_isNoSort (bsort))
		      {
			lclbug (message ("%s: member %s of %s has unknown sort\n",
					 importfile, 
					 cstring_fromChars (lsymbol_toChars (mlist->name)),
					 cstring_fromChars (lsymbol_toChars (sn->name))));
		      }
		    else
		      {
			mlist->sort = bsort;
		      }
		  }
		/*@switchbreak@*/ break;
	      }
	    default:
	      lclbug (message ("%s: %s has unexpected sort kind %s",
			       importfile, 
			       cstring_fromChars (lsymbol_toChars (sn->name)),
			       sort_unparseKind (sn->kind)));
	    }
	}
    } end_lsymbolList_elements;
  
  /* list and sorts in it are not used anymore */
  lsymbolList_free (slist);
}

bool
sort_equal (sort s1, sort s2)
{
  sort syn1, syn2;

  if (s1 == s2) return TRUE;
  
  /* handle synonym sorts */
  syn1 = sort_getUnderlying (s1);
  syn2 = sort_getUnderlying (s2);
  
  if (syn1 == syn2) return TRUE;
  /* makes bool and Bool equal */
  
  return FALSE;
}

bool
sort_compatible (sort s1, sort s2)
{
  sort syn1, syn2;
  /* later: might consider "char" and enum types the same as "int" */
  if (s1 == s2)
    return TRUE;
  /* handle synonym sorts */
  syn1 = sort_getUnderlying (s1);
  syn2 = sort_getUnderlying (s2);
  if (syn1 == syn2)
    return TRUE;
  /* makes bool and Bool equal */
  return FALSE;
}

bool
sort_compatible_modulo_cstring (sort s1, sort s2)
{
 /* like sort_compatible but also handles special cstring inits,
    allows the following 2 cases:
    char c[] = "abc"; (LHS: char_Obj_Arr, RHS = char_Vec)
    (c as implicitly coerced into c^)
    char *d = "abc";  (LHS: char_Obj_Ptr, RHS = char_Vec)
    (d as implicitly coerced into d[]^)
 */
  sort syn1, syn2;
  if (sort_compatible (s1, s2))
    return TRUE;
  syn1 = sort_getUnderlying (s1);
  syn2 = sort_getUnderlying (s2);
  if (g_sortCstring == syn2 &&
      (syn1 == char_obj_ptrSort || syn1 == char_obj_ArrSort))
    return TRUE;
  return FALSE;
}

lsymbol
sort_getLsymbol (sort sor)
{
  sortNode sn = sort_quietLookup (sor);
  return sn->name;
}

/* a few handy routines for debugging */

char *sort_getName (sort s)
{
  return (lsymbol_toCharsSafe (sort_getLsymbol (s)));
}

/*@exposed@*/ cstring
sort_unparseName (sort s)
{
  return (cstring_fromChars (sort_getName (s)));
}

static void
sortError (ltoken t, sort oldsort, sortNode newnode)
{
  sortNode old = sort_quietLookup (oldsort);
  
  if ((old->kind <= SRT_FIRST || old->kind >= SRT_LAST) ||
      (newnode->kind <= SRT_FIRST || newnode->kind >= SRT_LAST))
    {
      llbuglit ("sortError: illegal sort kind");
    }

  llassert (sortTable != NULL);

  lclerror (t, message ("Sort %s defined as %s cannot be redefined as %s",
			cstring_fromChars (lsymbol_toChars (newnode->name)),
			sort_unparseKindName (sortTable[oldsort]),
			sort_unparseKindName (newnode)));
}

static /*@observer@*/ cstring
  sort_unparseKindName (sortNode s)
{
  switch (s->kind)
    {
    case SRT_NONE:
      return cstring_fromChars (sortKindName[(int)s->kind]);
    default:
      if (s->abstract)
	{
	  if (s->mutable)
	    {
	      return cstring_makeLiteralTemp ("MUTABLE");
	    }
	  else
	    {
	      return cstring_makeLiteralTemp ("IMMUTABLE");
	    }
	}
      else
	return cstring_fromChars (sortKindName[(int)s->kind]);
    }
  
  BADEXIT;
}

sort
sort_fromLsymbol (lsymbol sortid)
{
 /* like sort_lookupName but creates sort if not already present */
  sort sort = sort_lookupName (sortid);
  if (sort == NOSORTHANDLE)
    sort = sort_makeSort (ltoken_undefined, sortid);
  return sort;
}

bool
sort_isHOFSortKind (sort s)
{
  sortNode sn = sort_quietLookup (s);
  if (sn->kind == SRT_HOF)
    return TRUE;
  return FALSE;
}

/*
** returns TRUE iff s has State operators (', ~, ^)
*/

static bool
sort_hasStateFcns (sort s)
{
  sortNode sn = sort_quietLookup (s);
  sortKind kind = sn->kind;
  
  if (kind == SRT_SYN)
    {
      return (sort_hasStateFcns (sn->baseSort));
    }
  
  return ((kind == SRT_PTR) ||
	  (kind == SRT_OBJ) ||
	  (kind == SRT_ARRAY) ||
	  (kind == SRT_STRUCT) ||
	  (kind == SRT_UNION));
}


