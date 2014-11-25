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
** storeRef.c
**
** Memory management:
**    storeRef's are kept in allRefs for each function scope, and all are
**    free'd at the end of the function.  This relies on the constraint that
**    no storeRef created while checking a function is used outside that
**    function.
**
**    storeRefs in the file and global scopes are free'd by the uentry.
**
*/

# include "splintMacros.nf"
# include "basic.h"
# include "exprChecks.h"
# include "transferChecks.h"
# include "sRefTable.h"
# include "structNames.h"

/*
** Predicate functions that evaluate both arguments in order.
*/

/*@notfunction@*/
# define OR(a,b)  (a ? (b, TRUE) : b)

/*@notfunction@*/
# define AND(a,b) (a ? b : (b, FALSE))

static void sRef_checkValidAux (sRef p_s, sRefSet p_checkedsofar) /*@modifies p_checkedsofar@*/ ;

static bool sRef_isDerived (sRef p_s) /*@*/ ;

static /*@exposed@*/ sRef sRef_fixDirectBase (sRef p_s, sRef p_base) 
   /*@modifies p_base@*/ ;

static void sRef_updateNullState (sRef p_res, sRef p_other) /*@modifies p_res@*/ ;

static bool sRef_isAllocatedStorage (sRef p_s) /*@*/ ;
static void sRef_setNullErrorLoc (sRef p_s, fileloc) /*@*/ ;
static void sRef_setNullStateN (sRef p_s, nstate p_n) /*@modifies p_s@*/ ;

static int sRef_depth (sRef p_s) /*@*/ ;

static void
  sRef_innerAliasSetComplete (void (p_predf) (sRef, fileloc), sRef p_s, 
			      fileloc p_loc)
  /*@modifies p_s@*/ ;

static void
sRef_innerAliasSetCompleteParam (void (p_predf) (sRef, sRef), sRef p_s, sRef p_t)
     /*@modifies p_s@*/ ;
     
static speckind speckind_fromInt (int p_i);
static bool sRef_equivalent (sRef p_s1, sRef p_s2);
static bool sRef_isDeepUnionField (sRef p_s);
static void sRef_addDeriv (/*@notnull@*/ sRef p_s, /*@notnull@*/ /*@exposed@*/ sRef p_t);
static bool sRef_checkModify (sRef p_s, sRefSet p_sl) /*@*/ ;

/*
** If s is definitely null, it has no memory state.
*/

static void sRef_resetAliasKind (/*@notnull@*/ sRef s) /*@modifies s->aliaskind@*/
{
  if (s->nullstate == NS_DEFNULL)
    {
      /* s->aliaskind = AK_ERROR; */
    }
}

# ifdef DEBUGSPLINT
extern void sRef_checkCompletelyReasonable (sRef s) /*@*/ 
{
  DPRINTF (("Check sRef: [%p] %s / %s", s, sRef_unparse (s),
	    sRefSet_unparse (sRef_derivedFields (s))));

  if (sRef_isReasonable (s))
    {
      sRefSet_elements (s->deriv, el)
	{
	  llassert (el != s);
	  sRef_checkCompletelyReasonable (el);
	} end_sRefSet_elements ;
    }
}
# endif

/*@falsewhennull@*/ bool sRef_isReasonable (sRef s) /*@*/
{
  if (sRef_isValid (s))
    {
# ifdef DEBUGSPLINT
      if (!bool_isReasonable (s->safe)
	  || !bool_isReasonable (s->modified)
	  || !bool_isReasonable (s->immut))
	{
	  llcontbug (message ("Unreasonable sRef [%p]", s));
	  return FALSE;
	}

      if (!sstate_isValid (s->defstate))
	{
	  llcontbug (message ("Unreasonable sRef [%p]", s));
	  return FALSE;
	}

      if (!nstate_isValid (s->nullstate))
	{
	  llcontbug (message ("Unreasonable sRef [%p]", s));
	  return FALSE;
	}

      if (!alkind_isValid (s->aliaskind)
	  || !alkind_isValid (s->oaliaskind))
	{
	  llcontbug (message ("Unreasonable sRef [%p]", s));
	  return FALSE;
	}

      if (!exkind_isValid (s->expkind)
	  || !exkind_isValid (s->oexpkind))
	{
	  llcontbug (message ("Unreasonable sRef [%p]", s));
	  return FALSE;
	}
# endif

      return TRUE;
    }

  return FALSE;
}

static /*@nullwhentrue@*/ bool sRef_isUnreasonable (sRef s) /*@*/
{
  return (!sRef_isReasonable (s));
}

static void sRef_checkMutable (/*@unused@*/ sRef s)
{
  if (sRef_isReasonable (s) && s->immut)
    {
      llcontbug (message ("Modification to sRef marked immutable: %q", 
			  sRef_unparseFull (s)));
    }
}

static bool skind_isSimple (skind sk)
{
  switch (sk)
    {
    case SK_PARAM: case SK_CVAR: case SK_CONST:
    case SK_OBJECT: case SK_UNKNOWN: case SK_NEW:
      return TRUE;
    default:
      return FALSE;
    }
}

static void sinfo_free (/*@special@*/ /*@temp@*/ /*@notnull@*/ sRef p_s)
   /*@uses p_s->kind, p_s->info@*/
   /*@releases p_s->info@*/ ;

static /*@null@*/ sinfo sinfo_copy (/*@notnull@*/ sRef p_s) /*@*/ ;
static void sRef_setPartsFromUentry (sRef p_s, uentry p_ue)
   /*@modifies p_s@*/ ;
static bool checkDeadState (/*@notnull@*/ sRef p_el, /*@null@*/ sRef p_e2, 
			    bool p_tbranch, fileloc p_loc);
static /*@dependent@*/ sRef sRef_constructPointerAux (/*@notnull@*/ /*@exposed@*/ sRef p_t) /*@*/ ;

static void 
  sRef_combineExKinds (/*@notnull@*/ sRef p_res, /*@notnull@*/ sRef p_other)
  /*@modifies p_res@*/ ;

static void 
  sRef_combineAliasKinds (/*@notnull@*/ sRef p_res, /*@notnull@*/ sRef p_other, 
			  clause p_cl, fileloc p_loc)
  /*@modifies p_res@*/ ;

static void
  sRef_combineNullState (/*@notnull@*/ sRef p_res, /*@notnull@*/ sRef p_other)
  /*@modifies p_res@*/ ;

static void
  sRef_combineDefState (/*@notnull@*/ sRef p_res, /*@notnull@*/ sRef p_other)
  /*@modifies p_res@*/ ;

static void sRef_setStateFromAbstractUentry (sRef p_s, uentry p_ue) 
  /*@modifies p_s@*/ ;

static void 
  sinfo_update (/*@notnull@*/ /*@exposed@*/ sRef p_res, 
		/*@notnull@*/ /*@exposed@*/ sRef p_other);
static void sRef_setDefinedAux (sRef p_s, fileloc p_loc, bool p_clear)
   /*@modifies p_s@*/ ;
static void sRef_setDefinedNoClear (sRef p_s, fileloc p_loc)
   /*@modifies p_s@*/ ;
static void sRef_setStateAux (sRef p_s, sstate p_ss, fileloc p_loc)
   /*@modifies p_s@*/;

static /*@exposed@*/ sRef 
  sRef_buildNCField (/*@exposed@*/ sRef p_rec, /*@exposed@*/ cstring p_f);

static void 
  sRef_mergeStateAux (/*@notnull@*/ sRef p_res, /*@notnull@*/ sRef p_other, 
		      clause p_cl, bool p_opt, fileloc p_loc,
		      bool p_doDerivs)
  /*@modifies p_res, p_other@*/ ;

static /*@null@*/ sinfo sinfo_fullCopy (/*@notnull@*/ sRef p_s);
static bool sRef_doModify (sRef p_s, sRefSet p_sl) /*@modifies p_s@*/ ;
static bool sRef_doModifyVal (sRef p_s, sRefSet p_sl) /*@modifies p_s@*/;
static bool sRef_checkModifyVal (sRef p_s, sRefSet p_sl) /*@*/ ;

static /*@only@*/ sRefSet
  sRef_mergeDerivs (/*@only@*/ sRefSet p_res, sRefSet p_other, 
		    bool p_opt, clause p_cl, fileloc p_loc);

static /*@only@*/ sRefSet
  sRef_mergeUnionDerivs (/*@only@*/ sRefSet p_res, 
			 /*@exposed@*/ sRefSet p_other,
			 bool p_opt, clause p_cl, fileloc p_loc);

static /*@only@*/ sRefSet 
  sRef_mergePdefinedDerivs (/*@only@*/ sRefSet p_res, sRefSet p_other, bool p_opt,
			    clause p_cl, fileloc p_loc);

static /*@only@*/ cstring sRef_unparseWithArgs (sRef p_s, uentryList p_args);
static /*@only@*/ cstring sRef_unparseNoArgs (sRef p_s);

static /*@exposed@*/ sRef sRef_findDerivedPointer (sRef p_s);
static /*@exposed@*/ sRef sRef_findDerivedArrayFetch (/*@notnull@*/ sRef, bool, int, bool) ;
static /*@exposed@*/ sRef sRef_findDerivedField (/*@notnull@*/ sRef p_rec, cstring p_f);
static /*@exposed@*/ sRef
  sRef_getDeriv (/*@notnull@*/ /*@returned@*/ sRef p_set, sRef p_guide);

static bool inFunction = FALSE;
static /*@only@*/ sRefTable allRefs;

/* # define DEBUGREFS  */

# ifdef DEBUGREFS
static nsrefs = 0;
static totnsrefs = 0;
static maxnsrefs = 0;
static ntotrefers = 0;
static nrefers = 0;
# endif

static /*@checked@*/ bool protectDerivs = FALSE;

/*
** Result of sRef_alloc is dependent since allRefs may
** reference it.  It is only if !inFunction.
*/

static /*@dependent@*/ /*@out@*/ /*@notnull@*/ sRef
sRef_alloc (void)
{
  sRef s = (sRef) dmalloc (sizeof (*s));

  s->immut = FALSE;

  DPRINTF (("Alloc sref: [%p]", s));

  if (inFunction)
    {
      allRefs = sRefTable_add (allRefs, s);
      /*@-branchstate@*/ 
    }
  else
    {
      DPRINTF (("Not in function!"));
    }

  /*@=branchstate@*/

# ifdef DEBUGREFS
  if (nsrefs >= maxnsrefs)
    {
      maxnsrefs = nsrefs;
    }

  totnsrefs++;
  nsrefs++;
# endif

  /*@-mustfree@*/ /*@-freshtrans@*/
  return s;
  /*@=mustfree@*/ /*@=freshtrans@*/
}

static void sRef_checkValidAux (sRef s, sRefSet checkedsofar)
{
  llassert (FALSE);

  if (!sRef_isReasonable (s)) return;

  if (sRefSet_containsSameObject (checkedsofar, s))
    {
      return;
    }

  /*@-temptrans@*/
  checkedsofar = sRefSet_insert (checkedsofar, s);
  /*@=temptrans@*/ /* checksofar will be destroyed before checkValid returns */

  switch (s->kind)
    {
    case SK_UNCONSTRAINED:
      llassert (cstring_length (s->info->fname) < 100);
      break;

    case SK_CVAR:
      llassert (s->info->cvar->lexlevel >= 0);
      /* llassert (s->info->cvar->lexlevel <= usymtab_getCurrentDepth ()); */
      break;

    case SK_PARAM:
      llassert (s->info->paramno >= -1);
      llassert (s->info->paramno <= 999); /* sanity check */
      break;

    case SK_ARRAYFETCH:
      sRef_checkValidAux (s->info->arrayfetch->arr, checkedsofar);
      break;

    case SK_FIELD:
      sRef_checkValidAux (s->info->field->rec, checkedsofar);
      llassert (cstring_length (s->info->field->field) < 100);
      break;

    case SK_PTR:
      sRef_checkValidAux (s->info->ref, checkedsofar);
      break;
 
   case SK_ADR:
      sRef_checkValidAux (s->info->ref, checkedsofar);
      break;

    case SK_OBJECT:
      /* check ctype s->info->object */
      break;

    case SK_CONJ:
      sRef_checkValidAux (s->info->conj->a, checkedsofar);
      sRef_checkValidAux (s->info->conj->b, checkedsofar);
      break;

    case SK_NEW:
      llassert (cstring_length (s->info->fname) < 100);
      break;

    case SK_DERIVED:
      sRef_checkValidAux (s->info->ref, checkedsofar);
      break;

    case SK_EXTERNAL:
      sRef_checkValidAux (s->info->ref, checkedsofar);
      break;

    case SK_TYPE:
    case SK_CONST:
    case SK_RESULT:
      /* check ctyp s->type */
      break;

    case SK_SPECIAL:
      llassert (s->info->spec == SR_NOTHING 
		|| s->info->spec == SR_INTERNAL
		|| s->info->spec == SR_SPECSTATE 
		|| s->info->spec == SR_SYSTEM);
      break;

    case SK_UNKNOWN:
      break;

      BADDEFAULT;
    }
  

  sRefSet_elements (s->deriv, el)
    {
      sRef_checkValidAux (el, checkedsofar);
    } end_sRefSet_elements ;
}

void sRef_checkValid (/*@unused@*/ sRef s)
{
  return;
  /*
  sRefSet checkedsofar = sRefSet_new ();
  sRef_checkValidAux (s, checkedsofar);
  */
}

static /*@dependent@*/ /*@notnull@*/ /*@special@*/ sRef
  sRef_new (void)
  /*@defines result@*/
  /*@ensures isnull result->aliasinfo, result->definfo,
                    result->expinfo, result->info, result->deriv, result->state@*/
{
  sRef s = sRef_alloc ();

  s->kind = SK_UNKNOWN;
  s->safe = TRUE;
  s->modified = FALSE;
  s->immut = FALSE;
  s->val = multiVal_undefined;

  s->type = ctype_unknown;
  s->defstate = SS_UNKNOWN;

  /* start modifications */
  s->bufinfo.bufstate = BB_NOTNULLTERMINATED;
  s->bufinfo.size = -1; 
  s->bufinfo.len = -1; 
  /* end modifications */

  s->aliaskind = AK_UNKNOWN;
  s->oaliaskind = AK_UNKNOWN;

  s->nullstate = NS_UNKNOWN;

  s->expkind = XO_UNKNOWN;
  s->oexpkind = XO_UNKNOWN;

  s->aliasinfo = stateInfo_undefined;
  s->definfo = stateInfo_undefined;
  s->nullinfo = stateInfo_undefined;
  s->expinfo = stateInfo_undefined;

  s->info = NULL;
  s->deriv = sRefSet_undefined;

  s->state = valueTable_undefined;

  return s;
}

static /*@dependent@*/ /*@notnull@*/ /*@special@*/ sRef
  sRef_newRef (void)
  /*@defines result@*/
  /*@ensures isnull result->aliasinfo, result->definfo,
                    result->expinfo, result->info, result->deriv@*/
{
  sRef res = sRef_new ();
  res->immut = FALSE;
  res->state = valueTable_undefined;
  return res;
}


void sRef_protectDerivs (void) /*@modifies protectDerivs@*/
{
  llassert (!protectDerivs);
  protectDerivs = TRUE;
}

void sRef_clearProtectDerivs (void) /*@modifies protectDerivs@*/
{
  llassert (protectDerivs);
  protectDerivs = FALSE;
}

/*
** hmmm...here be kind of a hack.  This function mysteriously appeared
** in my code, but I'm sure I didn't write it.
*/

bool
sRef_isRecursiveField (sRef s)
{
  if (sRef_isField (s))
    {
      if (sRef_depth (s) > 13)
	{
	  sRef base;
	  cstring fieldname;
	  
	  fieldname = sRef_getField (s);
	  base = sRef_getBase (s);
	  
	  while (sRef_isReasonable (base))
	    {
	      if (sRef_isField (base))
		{
		  if (cstring_equal (fieldname, sRef_getField (base)))
		    {
		      return TRUE;
		    }
		}
	      
	      base = sRef_getBaseSafe (base);
	    }
	}
    }

  return FALSE;
}

static void
sRef_addDeriv (/*@notnull@*/ sRef s, /*@notnull@*/ /*@exposed@*/ sRef t)
{
  if (!context_inProtectVars () 
      && !protectDerivs
      && sRef_isReasonable (s)
      && sRef_isReasonable (t)
      && !sRef_isConst (s))
    {
      int sd = sRef_depth (s);
      int td = sRef_depth (t);
      
      if (sd >= td)
	{
	  return;
	}

      /* This sometimes fails: (evans 2001-07-12)
      if (sRef_isArrayFetch (t))
	{
	  DPRINTF (("Derived fetch: %s / %s / %s",
		    sRef_unparseFull (s), sRef_unparseFull (t),
		    sRef_unparseFull (t->info->arrayfetch->arr)));
	  llassert (t->info->arrayfetch->arr == s);
	}
      */

      if (sRef_isFileOrGlobalScope (s))
	{
	  if (context_inFunctionLike () 
	      && ctype_isKnown (sRef_getType (s))
	      && !ctype_isFunction (sRef_getType (s)))
	    {
	      globSet g = context_getUsedGlobs ();

	      if (!globSet_member (g, s))
		{
		  /* 
		  ** don't report as a bug 
		  ** 

		  llcontbug 
			(message ("sRef_addDeriv: global variable not in used "
				  "globs: %q / %s / %q",
				  sRef_unparse (s), 
				  ctype_unparse (sRef_getType (s)),
				  sRefSet_unparse (s->deriv)));
		  */
		}
	      else
		{
		  DPRINTF (("Add deriv: [%p] %s / [%p] %s",
			    s, sRef_unparse (s),
			    t, sRef_unparse (t)));
		  
		  s->deriv = sRefSet_insert (s->deriv, t);
		}
	    }
	}
      else
	{
	  DPRINTF (("Add deriv: [%p] %s / [%p] %s",
		    s, sRef_unparse (s),
		    t, sRef_unparse (t)));

	  s->deriv = sRefSet_insert (s->deriv, t);
	}
    }
}

bool
sRef_deepPred (bool (predf) (sRef), sRef s)
{
  if (sRef_isReasonable (s))
    {
      if ((*predf)(s)) return TRUE;

      switch (s->kind)
	{
	case SK_PTR:
	  return (sRef_deepPred (predf, s->info->ref));
	case SK_ARRAYFETCH:
	  return (sRef_deepPred (predf, s->info->arrayfetch->arr));
	case SK_FIELD:
	  return (sRef_deepPred (predf, s->info->field->rec));
	case SK_CONJ:
	  return (sRef_deepPred (predf, s->info->conj->a)
		  || sRef_deepPred (predf, s->info->conj->b));
	default:
	  return FALSE;
	}
    }

  return FALSE;
}

bool sRef_modInFunction (void)
{
  return inFunction;
}

void sRef_setStateFromType (sRef s, ctype ct)
{
  if (sRef_isReasonable (s))
    {
      if (ctype_isUser (ct))
	{
	  DPRINTF (("Here we are: %s", sRef_unparseFull (s)));
	  sRef_setStateFromUentry 
	    (s, usymtab_getTypeEntry (ctype_typeId (ct)));
	}
      else if (ctype_isAbstract (ct))
	{
	  DPRINTF (("Here we are: %s", sRef_unparseFull (s)));
	  sRef_setStateFromAbstractUentry 
	    (s, usymtab_getTypeEntry (ctype_typeId (ct)));
	}
      else
	{
	  ; /* not a user type */
	}
    }
}

static void sRef_setTypeState (sRef s)
{
  if (sRef_isReasonable (s))
    {
      sRef_setStateFromType (s, s->type);
    }
}

bool sRef_hasAliasInfoLoc (sRef s)
{
  return (sRef_isReasonable (s) && (s->aliasinfo != NULL)
	  && (fileloc_isDefined (s->aliasinfo->loc)));
}

/*@falsewhennull@*/ bool sRef_hasStateInfoLoc (sRef s)
{
  return (sRef_isReasonable (s) && (s->definfo != NULL) 
	  && (fileloc_isDefined (s->definfo->loc)));
}

/*@falsewhennull@*/ bool sRef_hasExpInfoLoc (sRef s)
{
  return (sRef_isReasonable (s) 
	  && (s->expinfo != NULL) && (fileloc_isDefined (s->expinfo->loc)));
}

# if 0
static /*@observer@*/ /*@unused@*/ stateInfo sRef_getInfo (sRef s, cstring key)
{
  stateValue sv;
  
  if (!sRef_isReasonable (s)) {
    return stateInfo_undefined;
  }
  
  sv = valueTable_lookup (s->state, key);
  
  if (stateValue_isDefined (sv)) 
    {
      return stateValue_getInfo (sv);
    }
  
  return stateInfo_undefined;
}
# endif

static bool
sRef_hasNullInfoLoc (sRef s)
{
  return (sRef_isReasonable (s) && s->nullinfo != NULL
	  && (fileloc_isDefined (s->nullinfo->loc)));
}

bool
sRef_hasAliasInfoRef (sRef s)
{
  return (sRef_isReasonable (s) && (s->aliasinfo != NULL) 
	  && (sRef_isReasonable (s->aliasinfo->ref)));
}

static /*@observer@*/ fileloc
sRef_getAliasInfoLoc (/*@exposed@*/ sRef s)
{
  llassert (sRef_isReasonable (s) && s->aliasinfo != NULL
	    && (fileloc_isDefined (s->aliasinfo->loc)));
  return (s->aliasinfo->loc);
}

static /*@observer@*/ fileloc
sRef_getStateInfoLoc (/*@exposed@*/ sRef s)
{
  llassert (sRef_isReasonable (s) && s->definfo != NULL 
	    && (fileloc_isDefined (s->definfo->loc)));
  return (s->definfo->loc);
}

static /*@observer@*/ fileloc
sRef_getExpInfoLoc (/*@exposed@*/ sRef s)
{
  llassert (sRef_isReasonable (s) && s->expinfo != NULL 
	    && (fileloc_isDefined (s->expinfo->loc)));
  return (s->expinfo->loc);
}

static /*@observer@*/ fileloc
sRef_getNullInfoLoc (/*@exposed@*/ sRef s)
{
  llassert (sRef_isReasonable (s) && s->nullinfo != NULL 
	    && (fileloc_isDefined (s->nullinfo->loc)));
  return (s->nullinfo->loc);
}

/*@observer@*/ sRef
  sRef_getAliasInfoRef (/*@temp@*/ sRef s)
{
  llassert (sRef_isReasonable (s) && s->aliasinfo != NULL);
  return (s->aliasinfo->ref);
}

bool sRef_inGlobalScope ()
{
  return !inFunction;
}

/*
** This function should be called before new sRefs are created
** somewhere where they will have a lifetime greater than the
** current function scope.
*/

void sRef_setGlobalScope ()
{
  llassert (inFunction);
  DPRINTF (("leave function"));
  inFunction = FALSE;
}

void sRef_clearGlobalScope ()
{
  llassert (!inFunction);
  DPRINTF (("enter function"));
  inFunction = TRUE;
}

static bool oldInFunction = FALSE;
static int nestedScope = 0;

void sRef_setGlobalScopeSafe ()
{
  if (nestedScope == 0)
    {
      oldInFunction = inFunction;
    }
  
  nestedScope++;
  DPRINTF (("leave function safe"));
  inFunction = FALSE;
}

void sRef_clearGlobalScopeSafe ()
{
  nestedScope--;
  llassert (nestedScope >= 0);
  
  if (nestedScope == 0)
    {
      inFunction = oldInFunction;
    }

  DPRINTF (("clear function: %s", bool_unparse (inFunction)));
}

void sRef_enterFunctionScope ()
{
  /* evans 2001-09-09 - cleanup if we are in a macro! */
  if (context_inMacro ())
    {
      if (inFunction) {
	sRef_exitFunctionScope ();
      }
    }

  llassert (!inFunction);
  llassert (sRefTable_isEmpty (allRefs));
  inFunction = TRUE;
  DPRINTF (("enter function"));
}

void sRef_exitFunctionScope ()
{  
  if (inFunction)
    {
      DPRINTF (("Exit function scope."));
      sRefTable_clear (allRefs);
      DPRINTF (("Exit function scope done."));
      inFunction = FALSE;
    }
  else
    {
      llbuglit ("sRef_exitFunctionScope: not in function");
    }
}
  
void sRef_destroyMod () /*@globals killed allRefs;@*/
{
# ifdef DEBUGREFS  
  llmsg (message ("Live: %d / %d ", nsrefs, totnsrefs));  
# endif

  sRefTable_free (allRefs);
}


static /*@notnull@*/ /*@exposed@*/ sRef
sRef_fixConj (/*@notnull@*/ sRef s)
{
  if (sRef_isConj (s))
    {
      do {
	s = sRef_getConjA (s);
      } while (sRef_isConj (s));
      
      llassert (sRef_isReasonable (s));
      return s; /* don't need to ref */
    }
  else
    {
      return s;
    }
}

static bool 
sRef_isExternallyVisibleAux (sRef s)
{
  bool res = FALSE;
  sRef base = sRef_getRootBase (s);

  if (sRef_isReasonable (base))
    {
      res = sRef_isParam (base) || sRef_isFileOrGlobalScope (base) || sRef_isExternal (base);
    }

  return res;
}

bool 
  sRef_isExternallyVisible (sRef s)
{
  return (sRef_aliasCheckSimplePred (sRef_isExternallyVisibleAux, s));
}

/*@exposed@*/ uentry
sRef_getBaseUentry (sRef s)
{
  sRef base = sRef_getRootBase (s);
  uentry res = uentry_undefined;
  
  if (sRef_isReasonable (base))
    {
      switch (base->kind)
	{
	case SK_PARAM:
	  res = usymtab_getRefQuiet (paramsScope, usymId_fromInt (base->info->paramno));
	  break;

	case SK_CVAR:
	  res = usymtab_getRefQuiet (base->info->cvar->lexlevel, 
				     base->info->cvar->index);
	  break;

	default:
	  break;
	}  
    }

  return res;
}

/*
** lookup the current uentry corresponding to s, and return the corresponding sRef.
** yuk yuk yuk yuk yuk yuk yuk yuk
*/

/*@exposed@*/ sRef
sRef_updateSref (sRef s)
{
  sRef inner;
  sRef ret;
  sRef res;

  if (!sRef_isReasonable (s)) return sRef_undefined;
  
  switch (s->kind)
    {
    case SK_UNKNOWN:
    case SK_OBJECT:
    case SK_NEW:
    case SK_TYPE:
    case SK_DERIVED:
    case SK_UNCONSTRAINED:
    case SK_CONST:
    case SK_SPECIAL:
    case SK_RESULT:
      return s; 
    case SK_EXTERNAL:
      {
	sRef r = sRef_updateSref (s->info->ref);

	if (r != s->info->ref)
	  {
	    return sRef_makeExternal (r);
	  }
	else
	  {
	    return s;
	  }
      }
    case SK_PARAM:
    case SK_CVAR:
      {
	uentry ue = sRef_getUentry (s);

	/* must be raw name!  (need the marker) */
	/* Must be in the correct scope! */

	ue = usymtab_lookupSafeScope (uentry_rawName (ue), sRef_lexLevel (s));
	
	if (uentry_isUndefined (ue))
	  {
	    return s;
	  }
	else
	  {
	    DPRINTF (("Update sref: %s", uentry_unparseFull (ue)));
	    return (uentry_getSref (ue));
	  }
      }
    case SK_ARRAYFETCH:
      /* special case if ind known */
      inner = s->info->arrayfetch->arr;
      ret = sRef_updateSref (inner);

      if (ret == inner) 
	{
	  res = s; 
	}
      else 
	{
	  res = sRef_makeArrayFetch (ret);
	}

      return res;

    case SK_FIELD:
      inner = s->info->field->rec;
      ret = sRef_updateSref (inner);

      if (ret == inner) 
	{
	  res = s; 
	}
      else 
	{
	  res = (sRef_makeField (ret, s->info->field->field));
	}

      return (res);
    case SK_PTR:
      inner = s->info->ref;
      ret = sRef_updateSref (inner);
      if (ret == inner) 
	{
	  res = s; 
	}
      else
	{
	  res = sRef_makePointer (ret);
	}

      return (res);

    case SK_ADR:
      inner = s->info->ref;
      ret = sRef_updateSref (inner);

      if (ret == inner)
	{
	  res = s; 
	}
      else 
	{
	  res = sRef_makeAddress (ret);
	}

      return (res);

    case SK_CONJ:
      {
	sRef innera = s->info->conj->a;
	sRef innerb = s->info->conj->b;
	sRef reta = sRef_updateSref (innera);
	sRef retb = sRef_updateSref (innerb);

	if (innera == reta && innerb == retb)
	  {
	    res = s;
	  }
	else 
	  {
	    res = sRef_makeConj (reta, retb);
	  }

	return (res);
      }
    }
  
  BADEXIT;
}

uentry
sRef_getUentry (sRef s)
{
  llassert (sRef_isReasonable (s));

  switch (s->kind)
    {
    case SK_PARAM:
      return (usymtab_getRefQuiet (paramsScope, usymId_fromInt (s->info->paramno)));
    case SK_CVAR:
      return (usymtab_getRefQuiet (s->info->cvar->lexlevel, s->info->cvar->index));
    case SK_CONJ:
      {
	if (sRef_isCvar (s->info->conj->a) || sRef_isParam (s->info->conj->a)
	    || sRef_isConj (s->info->conj->a))
	  {
	    return sRef_getUentry (s->info->conj->a);
	  }
	else 
	  {
	    return sRef_getUentry (s->info->conj->b);
	  }
      }
    case SK_FIELD: /* evans 2002-07-17: added case for SK_FIELD */
    case SK_UNKNOWN:
    case SK_SPECIAL:
      return uentry_undefined;
    BADDEFAULT;
    }
}

int
sRef_getParam (sRef s)
{
  llassert (sRef_isReasonable (s));
  llassert (s->kind == SK_PARAM);

  return s->info->paramno;
}

bool
sRef_isModified (sRef s)
{
    return (!sRef_isReasonable (s) || s->modified);
}

void sRef_setModified (sRef s)
{
  if (sRef_isReasonable (s))
    {
      s->modified = TRUE;

      
      if (sRef_isRefsField (s))
	{
	  sRef base = sRef_getBase (s);
	  
	  llassert (s->kind == SK_FIELD);
	  
	  if (sRef_isPointer (base))
	    {
	      base = sRef_getBase (base);
	    }
	  
	  if (sRef_isRefCounted (base))
	    {
	      base->aliaskind = AK_NEWREF;
	    }
	}
    }
}

/*
** note: this side-effects sRefSet to set modified to TRUE
** for any sRef similar to s.
*/

bool
sRef_canModifyVal (sRef s, sRefSet sl)
{
  if (context_getFlag (FLG_MUSTMOD))
    {
      return (sRef_doModifyVal (s, sl));
    }
  else
    {
      return (sRef_checkModifyVal (s, sl));
    }
}

bool
sRef_canModify (sRef s, sRefSet sl)
{
  if (context_getFlag (FLG_MUSTMOD))
    {
      return (sRef_doModify (s, sl));
    }
  else
    {
      return (sRef_checkModify (s, sl));
    }
}

/*
** No side-effects
*/

static
bool sRef_checkModifyVal (sRef s, sRefSet sl)
{
  DPRINTF (("Check modify val: %s", sRef_unparse (s)));

  if (sRef_isInvalid (s))
    {
      return TRUE;
    }
  
  switch (s->kind)
    {
    case SK_UNCONSTRAINED:
    case SK_CONST:
      return TRUE;
    case SK_CVAR:
      DPRINTF (("Modify var: %s", sRef_unparse (s)));

      if (sRef_isFileOrGlobalScope (s))
	{
	  if (context_checkGlobMod (s))
	    {
	      return (sRefSet_member (sl, s));
	    }

	  return TRUE;
	}
      else
	{
	  return TRUE;
	}
    case SK_PARAM:
            return (sRefSet_member (sl, s) 
	      || alkind_isOnly (sRef_getOrigAliasKind (s)));
    case SK_ARRAYFETCH: 
      /* special case if ind known */
      return (sRefSet_member (sl, s) ||
	      sRef_checkModifyVal (s->info->arrayfetch->arr, sl));
    case SK_FIELD:
      return (sRefSet_member (sl, s) || sRef_checkModifyVal (s->info->field->rec, sl));
    case SK_PTR:
      return (sRefSet_member (sl, s) || sRef_checkModifyVal (s->info->ref, sl));
    case SK_ADR:
      return (sRefSet_member (sl, s) || sRef_checkModifyVal (s->info->ref, sl));
    case SK_CONJ:
      return ((sRef_checkModifyVal (s->info->conj->a, sl)) &&
	      (sRef_checkModifyVal (s->info->conj->b, sl)));
    case SK_UNKNOWN:
    case SK_OBJECT:
    case SK_NEW:
    case SK_TYPE:
    case SK_DERIVED:
      return TRUE;
    case SK_EXTERNAL:
      return TRUE;
    case SK_SPECIAL:
      {
	switch (s->info->spec)
	  {
	  case SR_NOTHING:   return TRUE;
	  case SR_INTERNAL:  
	    if (context_getFlag (FLG_INTERNALGLOBS))
	      {
		return (sRefSet_member (sl, s));
	      }
	    else
	      {
		return TRUE;
	      }
	  case SR_SPECSTATE: return TRUE;
	  case SR_SYSTEM:    return (sRefSet_member (sl, s));
	  case SR_GLOBALMARKER: BADBRANCH;
	  }
      }
    case SK_RESULT: BADBRANCH;
    }
  BADEXIT;
}

/*
** this should probably be elsewhere...
**
** returns TRUE iff sl indicates that s can be modified
*/

static bool sRef_checkModify (sRef s, sRefSet sl)
{
  llassert (sRef_isReasonable (s));

  switch (s->kind)
    {
    case SK_UNCONSTRAINED:
    case SK_CONST:
      return TRUE;
    case SK_CVAR:
      if (sRef_isFileOrGlobalScope (s))
	{
	  if (context_checkGlobMod (s))
	    {
	      return (sRefSet_member (sl, s));
	    }

	  return TRUE;
	}
      else
	{
	  return TRUE;
	}
    case SK_PARAM:
      return TRUE;
    case SK_ARRAYFETCH:
      return (sRefSet_member (sl, s) ||
	      sRef_checkModifyVal (s->info->arrayfetch->arr, sl));
    case SK_FIELD:
      {
	sRef sr = s->info->field->rec;

	if (sr->kind == SK_PARAM)
	  return TRUE; /* structs are copied on call */

	return (sRefSet_member (sl, s) || sRef_checkModifyVal (s->info->field->rec, sl));
      }
    case SK_PTR:
      {
	bool sm;

	sm = sRefSet_member (sl, s);

	if (sm)
	  return TRUE;
	else
	  return (sRef_checkModifyVal (s->info->ref, sl));
      }
    case SK_ADR:
      return (sRefSet_member (sl, s) || sRef_checkModifyVal (s->info->ref, sl));
    case SK_CONJ:
      return ((sRef_checkModify (s->info->conj->a, sl)) &&
	      (sRef_checkModify (s->info->conj->b, sl)));
    case SK_NEW:
    case SK_OBJECT:
    case SK_UNKNOWN:
    case SK_TYPE:
    case SK_DERIVED:
    case SK_EXTERNAL:
      return TRUE;
    case SK_SPECIAL:
      {
	switch (s->info->spec)
	  {
	  case SR_NOTHING:   return TRUE;
	  case SR_INTERNAL:  
	    if (context_getFlag (FLG_INTERNALGLOBS))
	      {
		return (sRefSet_member (sl, s));
	      }
	    else
	      {
		return TRUE;
	      }
	  case SR_SPECSTATE: return TRUE;
	  case SR_SYSTEM:    return (sRefSet_member (sl, s));
	  case SR_GLOBALMARKER: BADBRANCH;
	  }
      }
    case SK_RESULT: BADBRANCH;
    }
  BADEXIT;
}

cstring sRef_stateVerb (sRef s)
{
  if (sRef_isDead (s))
    {
      return cstring_makeLiteralTemp ("released");
    }
  else if (sRef_isKept (s))
    {
      return cstring_makeLiteralTemp ("kept");
    }
  else if (sRef_isDependent (s))
    {
      return cstring_makeLiteralTemp ("dependent");
    }
  else
    {
      BADEXIT;
    }
}

cstring sRef_stateAltVerb (sRef s)
{
  if (sRef_isDead (s))
    {
      return cstring_makeLiteralTemp ("live");
    }
  else if (sRef_isKept (s))
    {
      return cstring_makeLiteralTemp ("not kept");
    }
  else if (sRef_isDependent (s))
    {
      return cstring_makeLiteralTemp ("independent");
    }
  else
    {
      BADEXIT;
    }
}

static 
bool sRef_doModifyVal (sRef s, sRefSet sl)
{
  llassert (sRef_isReasonable (s));

  
  switch (s->kind)
    {
    case SK_UNCONSTRAINED:
    case SK_CONST:
      return TRUE;
    case SK_CVAR:
      if (sRef_isFileOrGlobalScope (s))
	{
	  
	  if (context_checkGlobMod (s))
	    {
	      return (sRefSet_modifyMember (sl, s));
	    }
	  else
	    {
	      (void) sRefSet_modifyMember (sl, s);
	    }

	  	  return TRUE;
	}
      else
	{
	  return TRUE;
	}      
    case SK_PARAM:
      return (sRefSet_modifyMember (sl, s) 
	      || alkind_isOnly (sRef_getOrigAliasKind (s)));
    case SK_ARRAYFETCH:
      /* special case if ind known */
      /* unconditional OR, need side effect */
      return (OR (sRefSet_modifyMember (sl, s),
		  sRef_doModifyVal (s->info->arrayfetch->arr, sl)));
    case SK_FIELD:
      return (OR (sRefSet_modifyMember (sl, s),
		  sRef_doModifyVal (s->info->field->rec, sl)));
    case SK_PTR:
      return (OR (sRefSet_modifyMember (sl, s),
		  sRef_doModifyVal (s->info->ref, sl)));
    case SK_ADR:
      return (OR (sRefSet_modifyMember (sl, s),
		  sRef_doModifyVal (s->info->ref, sl)));
    case SK_CONJ:
      return (AND (sRef_doModifyVal (s->info->conj->a, sl) ,
		   sRef_doModifyVal (s->info->conj->b, sl)));
    case SK_OBJECT:
    case SK_DERIVED:
    case SK_EXTERNAL:
    case SK_UNKNOWN:
    case SK_NEW:
    case SK_TYPE:
      return TRUE;
    case SK_SPECIAL:
      {
	switch (s->info->spec)
	  {
	  case SR_NOTHING:   return TRUE;
	  case SR_INTERNAL:  
	    if (context_getFlag (FLG_INTERNALGLOBS))
	      {
		return (sRefSet_modifyMember (sl, s));
	      }
	    else
	      {
		(void) sRefSet_modifyMember (sl, s);
		return TRUE;
	      }
	  case SR_SPECSTATE: return TRUE;
	  case SR_SYSTEM:    return (sRefSet_modifyMember (sl, s));
	  case SR_GLOBALMARKER: BADBRANCH;

	  }
      }
    case SK_RESULT: BADBRANCH;
    }
  BADEXIT;
}

/*
** this should probably be elsewhere...
**
** returns TRUE iff sl indicates that s can be modified
*/

static 
bool sRef_doModify (sRef s, sRefSet sl)
{
  llassert (sRef_isReasonable (s));
  
  switch (s->kind)
    {
    case SK_UNCONSTRAINED:
    case SK_CONST:
      return TRUE;
    case SK_CVAR:
      if (sRef_isFileOrGlobalScope (s))
	{
	  if (context_checkGlobMod (s))
	    {
	      return (sRefSet_modifyMember (sl, s));
	    }
	  else
	    {
	      (void) sRefSet_modifyMember (sl, s);
	    }

	  return TRUE;
	}
      else
	{
	  return TRUE;
	}
    case SK_PARAM:
      return TRUE;
    case SK_ARRAYFETCH:
            return (OR (sRefSet_modifyMember (sl, s),
		  sRef_doModifyVal (s->info->arrayfetch->arr, sl)));
    case SK_FIELD:
      {
	sRef sr = s->info->field->rec;

	if (sr->kind == SK_PARAM)
	  {
	    return TRUE; /* structs are shallow-copied on call */
	  }
	
	return (OR (sRefSet_modifyMember (sl, s),
		    sRef_doModifyVal (s->info->field->rec, sl)));
      }
    case SK_PTR:
      {
	return (OR (sRefSet_modifyMember (sl, s),
		    sRef_doModifyVal (s->info->ref, sl)));
      }
    case SK_ADR:
      return (OR (sRefSet_modifyMember (sl, s),
		  sRef_doModifyVal (s->info->ref, sl)));
    case SK_CONJ:
      return (AND (sRef_doModify (s->info->conj->a, sl),
		  (sRef_doModify (s->info->conj->b, sl))));
    case SK_UNKNOWN:
    case SK_NEW:
    case SK_TYPE:
      return TRUE;
    case SK_OBJECT:
    case SK_DERIVED:
    case SK_EXTERNAL:
      return TRUE;
    case SK_SPECIAL:
      {
	switch (s->info->spec)
	  {
	  case SR_NOTHING:   return TRUE;
	  case SR_INTERNAL:  return TRUE;
	  case SR_SPECSTATE: return TRUE;
	  case SR_SYSTEM:    return (sRefSet_modifyMember (sl, s));
	  case SR_GLOBALMARKER: BADBRANCH;
	  }
      }
    case SK_RESULT: BADBRANCH;
    }
  BADEXIT;
}

static /*@exposed@*/ sRef
  sRef_leastCommon (/*@exposed@*/ sRef s1, sRef s2)
{
  llassert (sRef_similar (s1, s2));
  
  if (!sRef_isReasonable (s1)) return s1;
  if (!sRef_isReasonable (s2)) return s1;

  sRef_combineDefState (s1, s2);
  sRef_combineNullState (s1, s2);
  sRef_combineExKinds (s1, s2);
  
  if (s1->aliaskind != s2->aliaskind)
    {
      if (s1->aliaskind == AK_UNKNOWN)
	{
	  s1->aliaskind = s2->aliaskind;
	}
      else if (s2->aliaskind == AK_UNKNOWN)
	{
	  ;
	}
      else
	{
	  s1->aliaskind = AK_ERROR;
	}
    }

  return s1;
}

int sRef_compare (sRef s1, sRef s2)
{
  if (s1 == s2) return 0;

  if (sRef_isInvalid (s1)) return -1;
  if (sRef_isInvalid (s2)) return 1;
      
  INTCOMPARERETURN (s1->kind, s2->kind);
  INTCOMPARERETURN (s1->defstate, s2->defstate);
  INTCOMPARERETURN (s1->aliaskind, s2->aliaskind);

  DPRINTF (("Compare null state: %s / %s",
	    sRef_unparseFull (s1),
	    sRef_unparseFull (s2)));

  COMPARERETURN (nstate_compare (sRef_getNullState (s1),
				 sRef_getNullState (s2)));

  switch (s1->kind)
    {
    case SK_PARAM:
      return (int_compare (s1->info->paramno, s2->info->paramno));
    case SK_ARRAYFETCH:
      {
	COMPARERETURN (sRef_compare (s1->info->arrayfetch->arr, 
				     s2->info->arrayfetch->arr));
	
	if (s1->info->arrayfetch->indknown && s2->info->arrayfetch->indknown)
	  {
	    return (int_compare (s1->info->arrayfetch->ind, 
				 s2->info->arrayfetch->ind));
	  }
	if (!s1->info->arrayfetch->indknown && !s2->info->arrayfetch->indknown)
	  return 0;
	
	return 1;
      }
    case SK_FIELD:
      {
	COMPARERETURN (sRef_compare (s1->info->field->rec, s2->info->field->rec));
	
	if (cstring_equal (s1->info->field->field, s2->info->field->field))
	  return 0;

	return 1;
      }
    case SK_PTR:
    case SK_ADR:
      return (sRef_compare (s1->info->ref, s2->info->ref));
    case SK_CONJ:
      COMPARERETURN (sRef_compare (s1->info->conj->a, s2->info->conj->a));
      return (sRef_compare (s1->info->conj->b, s2->info->conj->b));
    case SK_UNCONSTRAINED:
      return (cstring_compare (s1->info->fname, s2->info->fname));
    case SK_NEW:
    case SK_CVAR:
    case SK_UNKNOWN:
    case SK_OBJECT:
    case SK_TYPE:
    case SK_DERIVED:
    case SK_EXTERNAL:
    case SK_CONST:
    case SK_RESULT:
      return 0;
    case SK_SPECIAL:
      return (generic_compare (s1->info->spec, s2->info->spec));
    }
  BADEXIT;
}

static bool cref_equal (cref c1, cref c2)
{
  return ((c1->lexlevel == c2->lexlevel) &&
	  (usymId_equal (c1->index, c2->index)));
}

/*
** returns true if s1 could be the same storage as s2.
** i.e., a[?] ~ a[3].  Note its not symmetric ... s1
** should be more specific.
*/

/*
** like similar, but matches objects <-> non-objects
*/

static bool 
sRef_uniqueReference (sRef s)
{
  return (sRef_isFresh (s) || sRef_isUnique (s) 
	  || sRef_isOnly (s) || sRef_isStack (s)
	  || sRef_isAddress (s)); 
}

static bool
sRef_similarRelaxedAux (sRef s1, sRef s2)
{
  if (s1 == s2)
    {
      if (sRef_isUnknownArrayFetch (s1))
	{
	  return FALSE;
	}
      else
	{
	  return TRUE;
	}
    }

  if (sRef_isInvalid (s1) || sRef_isInvalid (s2)) return FALSE;

  if (sRef_isConj (s2)) 
    return (sRef_similarRelaxedAux (s1, sRef_getConjA (s2)) ||
	    sRef_similarRelaxedAux (s1, sRef_getConjB (s2)));

  switch (s1->kind)
    {
    case SK_CVAR:
      return ((s2->kind == SK_CVAR)
	      && (cref_equal (s1->info->cvar, s2->info->cvar)));
    case SK_PARAM:
      return ((s2->kind == SK_PARAM)
	      && (s1->info->paramno == s2->info->paramno));
    case SK_ARRAYFETCH:
      if (s2->kind == SK_ARRAYFETCH)
	{
	  if (sRef_similarRelaxedAux (s1->info->arrayfetch->arr,
				      s2->info->arrayfetch->arr))
	    {
	      if (s1->info->arrayfetch->indknown)
		{
		  if (s2->info->arrayfetch->indknown)
		    {
		      return (s1->info->arrayfetch->ind == s2->info->arrayfetch->ind);
		    }
		  else 
		    {
		      return FALSE;
		    }
		}
	      else
		{
		  return FALSE;
		}
	    }
	}
      return FALSE;
    case SK_FIELD:
      return ((s2->kind == SK_FIELD
	       && (sRef_similarRelaxedAux (s1->info->field->rec,
					   s2->info->field->rec)
		   && cstring_equal (s1->info->field->field,
				     s2->info->field->field))));
    case SK_PTR:
      return ((s2->kind == SK_PTR)
	      && sRef_similarRelaxedAux (s1->info->ref, s2->info->ref));
    case SK_ADR:
      return ((s2->kind == SK_ADR)
	      && sRef_similarRelaxedAux (s1->info->ref, s2->info->ref));
    case SK_CONJ:
      return ((sRef_similarRelaxedAux (s1->info->conj->a, s2) ||
	      (sRef_similarRelaxedAux (s1->info->conj->b, s2))));
    case SK_SPECIAL:
      return (s1->info->spec == s2->info->spec);
    case SK_UNCONSTRAINED:
      return (cstring_equal (s1->info->fname, s2->info->fname));
    case SK_DERIVED:
    case SK_CONST:
    case SK_TYPE:
    case SK_NEW:
    case SK_UNKNOWN:
    case SK_OBJECT:
    case SK_EXTERNAL:
    case SK_RESULT:
      return FALSE;
    }
  BADEXIT;
}

bool
sRef_similarRelaxed (sRef s1, sRef s2)
{
  bool us1, us2;

  if (s1 == s2) 
    {
      if (sRef_isThroughArrayFetch (s1))
	{
	  return FALSE;
	}
      else
	{
	  return TRUE;
	}
    }

  if (sRef_isInvalid (s1) || sRef_isInvalid (s2)) return FALSE;

  us1 = sRef_uniqueReference (s1);
  us2 = sRef_uniqueReference (s2);

  if ((s1->kind == SK_EXTERNAL && (s2->kind != SK_EXTERNAL && !us2))
      || (s2->kind == SK_EXTERNAL && (s1->kind != SK_EXTERNAL && !us1)))
    {
      /*
      ** Previously, also:
      **   || (sRef_isExposed (s1) && !us2) || (sRef_isExposed (s2) && !us1)) ???? 
      **
      ** No clue why this was there?!
      */


      if (sRef_isExposed (s1) && sRef_isCvar (s1))
	{
	  uentry ue1 = sRef_getUentry (s1);

	  if (uentry_isRefParam (ue1))
	    {
	      return sRef_similarRelaxedAux (s1, s2);
	    }
	}
      
      if (sRef_isExposed (s2) && sRef_isCvar (s2))
	{
	  uentry ue2 = sRef_getUentry (s2);

	  if (uentry_isRefParam (ue2))
	    {
	      return sRef_similarRelaxedAux (s1, s2);
	    }
	}
      
            return (ctype_match (s1->type, s2->type));
    }
  else
    {
            return sRef_similarRelaxedAux (s1, s2);
    }
}

bool
sRef_similar (sRef s1, sRef s2)
{
  if (s1 == s2) return TRUE;
  if (sRef_isInvalid (s1) || sRef_isInvalid (s2)) return FALSE;

  if (sRef_isConj (s2)) 
    {
      return (sRef_similar (s1, sRef_getConjA (s2)) ||
	      sRef_similar (s1, sRef_getConjB (s2)));
    }

  if (sRef_isDerived (s2))
   {
     return (sRef_includedBy (s1, s2->info->ref));
   }

  switch (s1->kind)
    {
    case SK_CVAR:
      return ((s2->kind == SK_CVAR)
	      && (cref_equal (s1->info->cvar, s2->info->cvar)));
    case SK_PARAM:
      return ((s2->kind == SK_PARAM)
	      && (s1->info->paramno == s2->info->paramno));
    case SK_ARRAYFETCH:
      if (s2->kind == SK_ARRAYFETCH)
	{
	  if (sRef_similar (s1->info->arrayfetch->arr,
			    s2->info->arrayfetch->arr))
	    {
	      if (s1->info->arrayfetch->indknown)
		{
		  if (s2->info->arrayfetch->indknown)
		    {
		      return (s1->info->arrayfetch->ind == s2->info->arrayfetch->ind);
		    }
		  else 
		    {
		      return TRUE;
		    }
		}
	      else
		{
		  return TRUE;
		}
	    }
	}
      else 
	{
	  if (s2->kind == SK_PTR)
	    {
	      if (sRef_similar (s1->info->arrayfetch->arr,
				s2->info->ref))
		{
		  return TRUE; 
		}
	    }
	}

      return FALSE;
    case SK_FIELD:
      return ((s2->kind == SK_FIELD
	       && (sRef_similar (s1->info->field->rec,
				 s2->info->field->rec)
		   && cstring_equal (s1->info->field->field,
				     s2->info->field->field))));
    case SK_PTR:
      if (s2->kind == SK_PTR)
	{
	  return sRef_similar (s1->info->ref, s2->info->ref);
	}
      else 
	{
	  if (s2->kind == SK_ARRAYFETCH)
	    {
	      if (sRef_similar (s2->info->arrayfetch->arr,
				s1->info->ref))
		{
		  return TRUE; 
		}
	    }
	}

      return FALSE;
    case SK_ADR:
      return ((s2->kind == SK_ADR)
	      && sRef_similar (s1->info->ref, s2->info->ref));
    case SK_CONJ:
      return ((sRef_similar (s1->info->conj->a, s2) ||
	      (sRef_similar (s1->info->conj->b, s2))));
    case SK_DERIVED:
      return (sRef_includedBy (s2, s1->info->ref));
    case SK_UNCONSTRAINED:
      return (s2->kind == SK_UNCONSTRAINED
	      && cstring_equal (s1->info->fname, s2->info->fname));
    case SK_CONST:
    case SK_TYPE:
    case SK_NEW:
    case SK_UNKNOWN:
    case SK_OBJECT:
    case SK_EXTERNAL:
    case SK_RESULT:
      return FALSE;
    case SK_SPECIAL:
      return (s2->kind == SK_SPECIAL 
	      && (s1->info->spec == s2->info->spec));
    }

  /*@notreached@*/ DPRINTF (("Fell through for: %s / %s", sRef_unparse (s1), sRef_unparse (s2)));
  BADEXIT;
}

/*
** return TRUE iff small can be derived from big.
**
** (e.g. x, x.a is includedBy x;
**       x.a is included By x.a;
*/

bool
sRef_includedBy (sRef small, sRef big)
{
  if (small == big) return TRUE;
  if (sRef_isInvalid (small) || sRef_isInvalid (big)) return FALSE;

  if (sRef_isConj (big)) 
    return (sRef_similar (small, sRef_getConjA (big)) ||
	    sRef_similar (small, sRef_getConjB (big)));

  switch (small->kind)
    {
    case SK_CVAR:
    case SK_PARAM:
      return (sRef_same (small, big));
    case SK_ARRAYFETCH:
      if (big->kind == SK_ARRAYFETCH)
	{
	  if (sRef_same (small->info->arrayfetch->arr, big->info->arrayfetch->arr))
	    {
	      if (small->info->arrayfetch->indknown)
		{
		  if (big->info->arrayfetch->indknown)
		    {
		      return (small->info->arrayfetch->ind == big->info->arrayfetch->ind);
		    }
		  else 
		    {
		      return TRUE;
		    }
		}
	      else
		{
		  return TRUE;
		}
	    }
	}
      return (sRef_includedBy (small->info->arrayfetch->arr, big));
    case SK_FIELD:
      if (big->kind == SK_FIELD)
	{
	  return 
	    (sRef_same (small->info->field->rec, big->info->field->rec) &&
	     cstring_equal (small->info->field->field, big->info->field->field));
	}
      else
	{
	  return (sRef_includedBy (small->info->field->rec, big));
	}

    case SK_PTR:
      if (big->kind == SK_PTR)
	{
	  return sRef_same (small->info->ref, big->info->ref);
	}
      else
	{
	  return (sRef_includedBy (small->info->ref, big));
	}

    case SK_ADR:
      return ((big->kind == SK_ADR) && sRef_similar (small->info->ref, big->info->ref));
    case SK_CONJ:
      return ((sRef_includedBy (small->info->conj->a, big) ||
	      (sRef_includedBy (small->info->conj->b, big))));
    case SK_DERIVED:
      return (sRef_includedBy (small->info->ref, big));
    case SK_UNCONSTRAINED:
    case SK_CONST:
    case SK_TYPE:
    case SK_NEW:
    case SK_UNKNOWN:
    case SK_OBJECT:
    case SK_EXTERNAL:
    case SK_RESULT:
      return FALSE;
    case SK_SPECIAL:
      switch (small->info->spec)
	{
	case SR_NOTHING: return TRUE;
	case SR_SPECSTATE:
	case SR_INTERNAL: return (sRef_isSpecInternalState (big) ||
				  sRef_isFileStatic (big));
	case SR_SYSTEM: return (sRef_isSystemState (big));
	case SR_GLOBALMARKER: BADBRANCH;
	}
    }
  BADEXIT;
}

/*
** Same is similar to similar, but not quite the same. 
** same and realSame aren't the same, but they are really similar.
** similarly, same is the same as same. but realSame is
** not really the same as same, or similar to similar.
**
** Similarly to similar, same checks if two sRefs are the same.
** The similarities end, however, when same compares arrays
** with unknown indexes.  Similar returns false; same returns true.
**
** Similarly to similar and same, realSame is the same as same,
** except they do not behave the same when face with unknown
** sRefs.  Same thinks they are not the same, but realSame thinks
** the are.
**
*/

bool
sRef_realSame (sRef s1, sRef s2)
{
  if (s1 == s2) return TRUE;
  
  if (sRef_isUnreasonable (s1) 
      || sRef_isUnreasonable (s2)) 
    {
      return FALSE;
    }

  switch (s1->kind)
    {
    case SK_CVAR:
      return ((s2->kind == SK_CVAR) && (cref_equal (s1->info->cvar, s2->info->cvar)));
    case SK_PARAM:
      return ((s2->kind == SK_PARAM) && (s1->info->paramno == s2->info->paramno));
    case SK_ARRAYFETCH:
      if (s2->kind == SK_ARRAYFETCH)
	{
	  if (sRef_realSame (s1->info->arrayfetch->arr, s2->info->arrayfetch->arr))
	    {
	      if (s1->info->arrayfetch->indknown && s2->info->arrayfetch->indknown)
		{
		  return (s1->info->arrayfetch->ind == s2->info->arrayfetch->ind);
		}
	      if (!s1->info->arrayfetch->indknown && !s2->info->arrayfetch->indknown)
		{
		  return TRUE;
		}
	      return FALSE;
	    }
	}
      return FALSE;
    case SK_FIELD:
      return ((s2->kind == SK_FIELD &&
	       (sRef_realSame (s1->info->field->rec, s2->info->field->rec) &&
		cstring_equal (s1->info->field->field, s2->info->field->field))));
    case SK_PTR:
      return ((s2->kind == SK_PTR) && sRef_realSame (s1->info->ref, s2->info->ref));
    case SK_ADR:
      return ((s2->kind == SK_ADR) && sRef_realSame (s1->info->ref, s2->info->ref));
    case SK_CONJ:
      return ((sRef_realSame (s1->info->conj->a, s2) ||
	      (sRef_realSame (s1->info->conj->b, s2))));
    case SK_OBJECT:
      return ((s2->kind == SK_OBJECT) 
	      && ctype_match (s1->info->object, s2->info->object));
    case SK_EXTERNAL:
      return ((s2->kind == SK_EXTERNAL) 
	      && sRef_realSame (s1->info->ref, s2->info->ref));
    case SK_SPECIAL:
      return ((s2->kind == SK_SPECIAL) && s1->info->spec == s2->info->spec);
    case SK_DERIVED:
      return ((s2->kind == SK_DERIVED) && sRef_realSame (s1->info->ref, s2->info->ref));
    case SK_UNCONSTRAINED:
      return ((s2->kind == SK_UNCONSTRAINED) 
	      && (cstring_equal (s1->info->fname, s2->info->fname)));
    case SK_TYPE:
    case SK_CONST:
    case SK_NEW:
    case SK_UNKNOWN:
    case SK_RESULT:
      return TRUE; /* changed this! was false */
    }
  BADEXIT;
}

bool
sRef_sameObject (sRef s1, sRef s2)
{
  return (s1 == s2);
}

/*
** same is similar to similar, but not quite the same. 
**
** Similarly to similar, same checks is two sRefs are the same.
** The similarities end, however, when same compares arrays
** with unknown indexes.  Similar returns false; same returns true.
*/

bool
sRef_same (sRef s1, sRef s2)
{
  if (s1 == s2) return TRUE;
  if (sRef_isInvalid (s1) || sRef_isInvalid (s2)) return FALSE;

  switch (s1->kind)
    {
    case SK_CVAR:
      return ((s2->kind == SK_CVAR) && (cref_equal (s1->info->cvar, s2->info->cvar)));
    case SK_PARAM:
      return ((s2->kind == SK_PARAM) && (s1->info->paramno == s2->info->paramno));
    case SK_ARRAYFETCH:
      if (s2->kind == SK_ARRAYFETCH)
	{
	  llassert (s1->info->field->rec != s1);
	  if (sRef_same (s1->info->arrayfetch->arr, s2->info->arrayfetch->arr))
	    {
	      if (s1->info->arrayfetch->indknown && s2->info->arrayfetch->indknown)
		{
		  return (s1->info->arrayfetch->ind == s2->info->arrayfetch->ind);
		}
	      return TRUE;
	    }
	}
      return FALSE;
    case SK_FIELD:
      {
	llassert (s1->info->field->rec != s1);
	return ((s2->kind == SK_FIELD &&
		 (sRef_same (s1->info->field->rec, s2->info->field->rec) &&
		  cstring_equal (s1->info->field->field, s2->info->field->field))));
      }
    case SK_PTR:
      {
	llassert (s1->info->ref != s1);
	return ((s2->kind == SK_PTR) && sRef_same (s1->info->ref, s2->info->ref));
      }
    case SK_ADR:
      {
	llassert (s1->info->ref != s1);
	return ((s2->kind == SK_ADR) && sRef_same (s1->info->ref, s2->info->ref));
      }
    case SK_CONJ:
      llassert (s1->info->conj->a != s1);
      llassert (s1->info->conj->b != s1);
      return ((sRef_same (s1->info->conj->a, s2)) && /* or or and? */
	      (sRef_same (s1->info->conj->b, s2)));
    case SK_SPECIAL:
      return ((s2->kind == SK_SPECIAL) && s1->info->spec == s2->info->spec);
    case SK_DERIVED:
      llassert (s1->info->ref != s1);
      return ((s2->kind == SK_DERIVED) && sRef_same (s1->info->ref, s2->info->ref));
    case SK_CONST:
    case SK_UNCONSTRAINED:
    case SK_TYPE:
    case SK_UNKNOWN:
    case SK_NEW:
    case SK_OBJECT:
    case SK_EXTERNAL:
    case SK_RESULT:
      return FALSE; 
    }
  BADEXIT;
}

/*
** sort of similar, for use in def/use
*/

static bool
sRef_closeEnough (sRef s1, sRef s2)
{
  if (s1 == s2) return TRUE;
  if (sRef_isInvalid (s1) || sRef_isInvalid (s2)) return FALSE;

  switch (s1->kind)
    {
    case SK_CVAR:
      return (((s2->kind == SK_CVAR) &&
	       (cref_equal (s1->info->cvar, s2->info->cvar))) ||
	      (s2->kind == SK_UNCONSTRAINED && s1->info->cvar->lexlevel == 0));
    case SK_UNCONSTRAINED:
      return (s2->kind == SK_UNCONSTRAINED
	      || ((s2->kind == SK_CVAR) && (s2->info->cvar->lexlevel == 0)));
    case SK_PARAM:
      return ((s2->kind == SK_PARAM) 
	      && (s1->info->paramno == s2->info->paramno));
    case SK_ARRAYFETCH:
      if (s2->kind == SK_ARRAYFETCH)
	{
	  if (sRef_closeEnough (s1->info->arrayfetch->arr, s2->info->arrayfetch->arr))
	    {
	      if (s1->info->arrayfetch->indknown && s2->info->arrayfetch->indknown)
		{
		  return (s1->info->arrayfetch->ind == s2->info->arrayfetch->ind);
		}
	      return TRUE;
	    }
	}
      return FALSE;
    case SK_FIELD:
      return ((s2->kind == SK_FIELD &&
	       (sRef_closeEnough (s1->info->field->rec, s2->info->field->rec) &&
		cstring_equal (s1->info->field->field, s2->info->field->field))));
    case SK_PTR:
      return ((s2->kind == SK_PTR) && sRef_closeEnough (s1->info->ref, s2->info->ref));
    case SK_ADR:
      return ((s2->kind == SK_ADR) && sRef_closeEnough (s1->info->ref, s2->info->ref));
    case SK_DERIVED:
      return ((s2->kind == SK_DERIVED) && sRef_closeEnough (s1->info->ref, s2->info->ref));
    case SK_CONJ:
      return ((sRef_closeEnough (s1->info->conj->a, s2)) ||
	      (sRef_closeEnough (s1->info->conj->b, s2)));
    case SK_SPECIAL:
      return ((s2->kind == SK_SPECIAL) && s1->info->spec == s2->info->spec);
    case SK_TYPE:
    case SK_CONST:
    case SK_UNKNOWN:
    case SK_NEW:
    case SK_OBJECT:
    case SK_EXTERNAL:
    case SK_RESULT:

      return FALSE;
    }
  BADEXIT;
}

/*
  drl add 12/24/2000
  s is an sRef of a formal paramenter in a function call constraint
  we trys to return a constraint expression derived from the actual parementer of a function call.
*/

/*@only@*/ constraintExpr sRef_fixConstraintParam (/*@observer@*/  sRef s, /*@observer@*/ /*@temp@*/ exprNodeList args)
{
  constraintExpr ce;

  if (sRef_isInvalid (s))
    llfatalbug((message("Invalid sRef")));

  switch (s->kind)
    {
    case SK_RESULT:
      {
	ce = constraintExpr_makeTermsRef (s);
	return ce;
      }
    case SK_FIELD:
      {
	sRef temp;
	
	temp = (sRef_makeField (sRef_fixBaseParam (s->info->field->rec, args),
			      s->info->field->field));
	ce = constraintExpr_makeTermsRef (temp);
	return ce;
      }
    case SK_PTR:
      {
	sRef temp;
	temp = (sRef_makePointer (sRef_fixBaseParam (s->info->ref, args)));
	ce = constraintExpr_makeTermsRef (temp);
	return ce;
      }

    case SK_ARRAYFETCH:
       {
	sRef temp;
	temp = sRef_saveCopy(s);
	temp = sRef_fixBaseParam (temp, args);
	ce = constraintExpr_makeTermsRef (temp);

	sRef_free(temp);
	return ce;
      }
    case SK_CVAR:
      {
	sRef temp;
	temp = sRef_saveCopy(s);
	ce = constraintExpr_makeTermsRef (temp);
	sRef_free(temp);
	return ce;
      }
    case SK_PARAM:
      {
	exprNode e;
	llassert (exprNodeList_size (args) > s->info->paramno);
	e = exprNodeList_nth (args, s->info->paramno);
	
	llassert (!(exprNode_isError (e)));
	ce = constraintExpr_makeExprNode (e);
	return ce;
      }

    default:
      {
	sRef temp;
	llcontbug (message ("Trying to do fixConstraintParam on nonparam, nonglobal: %q for function with arguments %q",
			    sRef_unparse (s), exprNodeList_unparse(args)));
      temp = sRef_saveCopy(s);
      ce = constraintExpr_makeTermsRef (temp);

      sRef_free(temp);
      return ce;
      }
    }
}

/*@exposed@*/ sRef
sRef_fixBaseParam (/*@returned@*/ sRef s, exprNodeList args)
{
  if (sRef_isInvalid (s)) return (sRef_undefined);

  switch (s->kind)
    {
    case SK_UNCONSTRAINED:
    case SK_CVAR:
      return s;
    case SK_PARAM:
      {
	if (exprNodeList_size (args) > s->info->paramno)
	  {
	    exprNode e = exprNodeList_nth (args, s->info->paramno);

	    if (exprNode_isError (e))
	      {
		return sRef_makeUnknown ();
	      }
	    
	    return (exprNode_getSref (e));
	  }
	else
	  {
	    return sRef_makeUnknown ();
	  }
      }
    case SK_ARRAYFETCH:

      if (s->info->arrayfetch->indknown)
	{
	  return (sRef_makeArrayFetchKnown 
		  (sRef_fixBaseParam (s->info->arrayfetch->arr, args),
		   s->info->arrayfetch->ind));
	}
      else
	{
	  return (sRef_makeArrayFetch 
		  (sRef_fixBaseParam (s->info->arrayfetch->arr, args)));
	}
    case SK_FIELD:
      {
	sRef res;
	DPRINTF (("Fix field: %s", sRef_unparseFull (s)));
	res = sRef_makeField (sRef_fixBaseParam (s->info->field->rec, args),
			      s->info->field->field);
	DPRINTF (("Returns: %s", sRef_unparseFull (res)));
	return res;
      }
    case SK_PTR:
      return (sRef_makePointer (sRef_fixBaseParam (s->info->ref, args)));

    case SK_ADR:
      return (sRef_makeAddress (sRef_fixBaseParam (s->info->ref, args)));

    case SK_CONJ:
      return (sRef_makeConj (sRef_fixBaseParam (s->info->conj->a, args),
			     sRef_fixBaseParam (s->info->conj->b, args)));
    case SK_DERIVED:
    case SK_SPECIAL:
    case SK_TYPE:
    case SK_CONST:
    case SK_NEW:
    case SK_UNKNOWN:
    case SK_OBJECT:
    case SK_EXTERNAL:
    case SK_RESULT:
      return s;
    }
  BADEXIT;
}

/*@exposed@*/ sRef
sRef_undumpGlobal (char **c)
{
  char p = **c;

  (*c)++;

  switch (p)
    {
    case 'g':
      {
	usymId uid = usymId_fromInt (reader_getInt (c));
	sstate defstate;
	nstate nullstate;
	sRef ret;

	reader_checkChar (c, '@');
	defstate = sstate_fromInt (reader_getInt (c));

	reader_checkChar (c, '@');
	nullstate = nstate_fromInt (reader_getInt (c));

	ret = sRef_makeGlobal (uid, ctype_unknown, stateInfo_currentLoc ());
	sRef_setNullStateN (ret, nullstate);
	ret->defstate = defstate;
	return ret;
      }
    case 's':
      {
	int i = reader_getInt (c);
	speckind sk = speckind_fromInt (i);

	switch (sk)
	  {
	  case SR_NOTHING:   return (sRef_makeNothing ());
	  case SR_INTERNAL:  return (sRef_makeInternalState ());
	  case SR_SPECSTATE: return (sRef_makeSpecState ());
	  case SR_SYSTEM:    return (sRef_makeSystemState ());
	  case SR_GLOBALMARKER: BADBRANCH;
	  }
	BADEXIT;
      }
    case '-':
      return sRef_undefined;
    case 'u':
      return sRef_makeUnknown ();
    case 'x':
      return sRef_makeUnknown ();
    default:
      llfatalerror (message ("sRef_undumpGlobal: bad line: %s",
			     cstring_fromChars (*c)));
    }
  BADEXIT;
}

static /*@exposed@*/ sRef sRef_undumpBody (char **c)
{
  char p = **c;

  (*c)++;

  switch (p)
    {
    case 'g':
      return (sRef_makeGlobal (usymId_fromInt (reader_getInt (c)), 
			       ctype_unknown, stateInfo_currentLoc ()));
    case 'p':
      return (sRef_makeParam (reader_getInt (c), ctype_unknown, 
			      stateInfo_makeLoc (g_currentloc, SA_DECLARED)));
    case 'r':
      return (sRef_makeResult (ctype_undump (c)));
    case 'a':
      {
	if ((**c >= '0' && **c <= '9') || **c == '-')
	  {
	    int i = reader_getInt (c);
	    sRef arr = sRef_undump (c);
	    sRef ret = sRef_buildArrayFetchKnown (arr, i);

	    return ret;
	  }
	else
	  {
	    sRef arr = sRef_undump (c);
	    sRef ret = sRef_buildArrayFetch (arr);

	    return ret;
	  }
      }
    case 'f':
      {
	cstring fname = cstring_undefined;
	sRef ret;

	while (**c != '.')
	  {
	    fname = cstring_appendChar (fname, **c);
	    (*c)++;
	  }
	(*c)++;

	ret = sRef_buildField (sRef_undump (c), fname);
	cstring_markOwned (fname);
	return (ret);
      }
    case 's':
      {
	int i = reader_getInt (c);
	speckind sk = speckind_fromInt (i);

	switch (sk)
	  {
	  case SR_NOTHING:   return (sRef_makeNothing ());
	  case SR_INTERNAL:  return (sRef_makeInternalState ());
	  case SR_SPECSTATE: return (sRef_makeSpecState ());
	  case SR_SYSTEM:    return (sRef_makeSystemState ());
	  case SR_GLOBALMARKER: BADBRANCH;
	  }
	BADEXIT;
      }
    case 't':
      {
	sRef ptr = sRef_undump (c);
	sRef ret = sRef_makePointer (ptr);

	return (ret);
      }
    case 'd':
      {
	sRef adr = sRef_undump (c);
	sRef ret = sRef_makeAddress (adr);

	return (ret);
      }
    case 'o':
      {
	return (sRef_makeObject (ctype_undump (c)));
      }
    case 'c':
      {
	sRef s1 = sRef_undump (c);
	sRef s2 = ((*c)++, sRef_undump (c));
	sRef ret = sRef_makeConj (s1, s2);

	return (ret);
      }
    case '-':
      return sRef_undefined;
    case 'u':
      return sRef_makeUnknown ();
    case 'x':
      return sRef_makeUnknown ();
    default:
      llfatalerror (message ("sRef_undump: bad line: %s", cstring_fromChars (*c)));
    }
  BADEXIT;
}

/*@exposed@*/ sRef sRef_undump (char **c)
{
  sRef res = sRef_undumpBody (c);

  if (reader_optCheckChar (c, '='))
    {
      multiVal mv = multiVal_undump (c);
      sRef_setValue (res, mv);
      reader_checkChar (c, '=');
    }

  return res;
}

static /*@only@*/ cstring sRef_dumpBody (sRef s)
{
  if (sRef_isInvalid (s))
    {
      return (cstring_makeLiteral ("-"));
    }
  else
    {
      switch (s->kind)
	{
	case SK_PARAM:
	  return (message ("p%d", s->info->paramno));
	case SK_ARRAYFETCH:
	  if (s->info->arrayfetch->indknown)
	    {
	      return (message ("a%d%q", s->info->arrayfetch->ind,
			       sRef_dump (s->info->arrayfetch->arr)));
	    }
	  else
	    {
	      return (message ("a%q", sRef_dump (s->info->arrayfetch->arr)));
	    }
	case SK_FIELD:
	  return (message ("f%s.%q", s->info->field->field, 
			   sRef_dump (s->info->field->rec)));
	case SK_PTR:
	  return (message ("t%q", sRef_dump (s->info->ref)));
	case SK_ADR:
	  return (message ("d%q", sRef_dump (s->info->ref)));
	case SK_OBJECT:
	  return (message ("o%q", ctype_dump (s->info->object)));
	case SK_SPECIAL:
	  return (message ("s%d", (int) s->info->spec));
	case SK_CONJ:
	  return (message ("c%q.%q",
			   sRef_dump (s->info->conj->a),
			   sRef_dump (s->info->conj->b)));
	case SK_CVAR:
	  if (sRef_isFileOrGlobalScope (s))
	    {
	      return (message ("g%d", 
			       usymtab_convertId (s->info->cvar->index)));
	    }
	  else
	    {
	      llcontbug (message ("Dumping local variable: %q",
				  sRef_unparseDebug (s)));
	      return (cstring_makeLiteral ("u"));
	    }
	case SK_UNKNOWN:
	  return (cstring_makeLiteral ("u"));
	case SK_RESULT:
	  return (message ("r%q", ctype_dump (s->type)));
	case SK_TYPE:
	case SK_CONST:
	case SK_EXTERNAL:
	case SK_DERIVED:
	case SK_NEW:
	case SK_UNCONSTRAINED:
	  llcontbug (message ("sRef_dump: bad kind: %q",
			      sRef_unparseFull (s)));
	  return (cstring_makeLiteral ("x"));
	}
    }
     
  BADEXIT;
}

/*@only@*/ cstring sRef_dump (sRef s)
{
  cstring res = sRef_dumpBody (s);

  if (sRef_hasValue (s))
    {
      res = message ("%q=%q=", res, multiVal_dump (sRef_getValue (s)));
    }

  return res;
}

cstring sRef_dumpGlobal (sRef s)
{
  llassert (!sRef_hasValue (s));

  if (sRef_isInvalid (s))
    {
      return (cstring_makeLiteral ("-"));
    }
  else
    {
      switch (s->kind)
	{
	case SK_CVAR:
	  if (sRef_isFileOrGlobalScope (s))
	    {
	      return (message ("g%d@%d@%d", 
			       usymtab_convertId (s->info->cvar->index),
			       (int) s->defstate,
			       (int) sRef_getNullState (s)));
	    }
	  else
	    {
	      llcontbug (message ("Dumping local variable: %q",
				  sRef_unparseDebug (s)));
	      return (cstring_makeLiteral ("u"));
	    }
	case SK_UNKNOWN:
	  return (cstring_makeLiteral ("u"));
	case SK_SPECIAL:
	  return (message ("s%d", (int) s->info->spec));
	default:
	  llcontbug (message ("sRef_dumpGlobal: bad kind: %q",
			      sRef_unparseFull (s)));
	  return (cstring_makeLiteral ("x"));
	}
    }
     
  BADEXIT;
}

ctype
sRef_deriveType (sRef s, uentryList cl)
{
  if (sRef_isInvalid (s)) return ctype_unknown;

  switch (s->kind)
    {
    case SK_CVAR:
      return (uentry_getType (usymtab_getRefQuiet (s->info->cvar->lexlevel, 
					      s->info->cvar->index)));
    case SK_UNCONSTRAINED:
      return (ctype_unknown);
    case SK_PARAM:
      if (s->info->paramno >= 0) 
	{
	  return uentry_getType (uentryList_getN (cl, s->info->paramno));
	}
      else
	{
	  return ctype_unknown;
	}
    case SK_ARRAYFETCH:
      {
	ctype ca = sRef_deriveType (s->info->arrayfetch->arr, cl);
	
	if (ctype_isArray (ca))
	  {
	    return (ctype_baseArrayPtr (ca));
	  }
	else if (ctype_isUnknown (ca))
	  {
	    return (ca);
	  }
	else
	  {
	    llcontbuglit ("sRef_deriveType: inconsistent array type");
	    return ca;
	  }
      }
    case SK_FIELD:
      {
	ctype ct = sRef_deriveType (s->info->field->rec, cl);
	
	if (ctype_isStructorUnion (ct))
	  {
	    uentry ue = uentryList_lookupField (ctype_getFields (ct), 
					       s->info->field->field);
	    
	    if (uentry_isValid (ue))
	      {
		return (uentry_getType (ue));
	      }
	    else
	      {
		llcontbuglit ("sRef_deriveType: bad field");
		return ctype_unknown;
	      }
	  }
	else if (ctype_isUnknown (ct))
	  {
	    return (ct);
	  }
	else
	  {
	    llcontbuglit ("sRef_deriveType: inconsistent field type");
	    return (ct);
	  }
      }
    case SK_PTR:
      {
	ctype ct = sRef_deriveType (s->info->ref, cl);
	
	if (ctype_isUnknown (ct)) return ct;
	if (ctype_isPointer (ct)) return (ctype_baseArrayPtr (ct));
	else
	  {
	    llcontbuglit ("sRef_deriveType: inconsistent pointer type");
	    return (ct);
	  }
      }
    case SK_ADR:
      {
	ctype ct = sRef_deriveType (s->info->ref, cl);
	
	if (ctype_isUnknown (ct)) return ct;
	return ctype_makePointer (ct);
      }
    case SK_DERIVED:
      {
	return sRef_deriveType (s->info->ref, cl);
      }
    case SK_OBJECT:
      {
	return (s->info->object);
      }
    case SK_CONJ:
      {
	return (ctype_makeConj (sRef_deriveType (s->info->conj->a, cl),
			       sRef_deriveType (s->info->conj->b, cl)));
      }
    case SK_RESULT:
    case SK_CONST:
    case SK_TYPE:
      {
	return (s->type);
      }
    case SK_SPECIAL:
    case SK_UNKNOWN:
    case SK_EXTERNAL:
    case SK_NEW:
      return ctype_unknown;
    }
  BADEXIT;
}

ctype
sRef_getType (sRef s)
{
  if (sRef_isInvalid (s)) return ctype_unknown;
  return s->type;
}


/*@only@*/ cstring
sRef_unparseOpt (sRef s)
{
  sRef rb = sRef_getRootBase (s);

  if (sRef_isMeaningful (rb) && !sRef_isConst (rb))
    {
      cstring ret = sRef_unparse (s);
      
      llassertprint (!cstring_equalLit (ret, "?"), ("print: %s", sRef_unparseDebug (s)));

      if (!cstring_isEmpty (ret))
	{
	  return (cstring_appendChar (ret, ' '));
	}
      else
	{
	  return ret;
	}
    }

  return cstring_undefined;
}

cstring
sRef_unparsePreOpt (sRef s)
{
  sRef rb = sRef_getRootBase (s);

  if (sRef_isMeaningful (rb) && !sRef_isConst (rb))
    {
      cstring ret = sRef_unparse (s);
      
      llassertprint (!cstring_equalLit (ret, "?"), ("print: %s", sRef_unparseDebug (s)));
      return (cstring_prependCharO (' ', ret));
    }

  return cstring_undefined;
}

/*@only@*/ cstring
sRef_unparse (sRef s)
{
  if (sRef_isInvalid (s)) return (cstring_makeLiteral ("?"));

  if (context_inFunctionLike ())
    {
      return (sRef_unparseWithArgs (s, context_getParams ()));
    }
  else
    {
      DPRINTF (("Not in function like: %s", context_unparse ()));
      return (sRef_unparseNoArgs (s));
    }
}

static /*@only@*/ cstring
sRef_unparseWithArgs (sRef s, uentryList args)
{
  if (sRef_isInvalid (s))
    {
      return (cstring_makeLiteral ("?"));
    }

  switch (s->kind)
    {
    case SK_CVAR:
      return (uentry_getName (usymtab_getRefQuiet (s->info->cvar->lexlevel,
						   s->info->cvar->index)));
    case SK_UNCONSTRAINED:
      return (cstring_copy (s->info->fname));
    case SK_PARAM:
      {
	if (s->info->paramno < uentryList_size (args)
	    && s->info->paramno >= 0)
	  {
	    uentry ue = uentryList_getN (args, s->info->paramno);
	    
	    if (uentry_isValid (ue))
	      return uentry_getName (ue);
	  }

	return (message ("parameter %d", s->info->paramno + 1));
      }
    case SK_ARRAYFETCH:
      if (s->info->arrayfetch->indknown)
	{
	  return (message ("%q[%d]", sRef_unparseWithArgs (s->info->arrayfetch->arr, args),
				s->info->arrayfetch->ind));
	}
      else
	{
	  return (message ("%q[]", sRef_unparseWithArgs (s->info->arrayfetch->arr, args)));
	}
    case SK_FIELD:
      if (s->info->field->rec->kind == SK_PTR)
	{
	  sRef ptr = s->info->field->rec;

	  return (message ("%q->%s", sRef_unparseWithArgs (ptr->info->ref, args),
			   s->info->field->field));	  
	}
      return (message ("%q.%s", sRef_unparseWithArgs (s->info->field->rec, args),
		       s->info->field->field));

    case SK_PTR:
      {
	sRef ref = sRef_fixConj (s->info->ref);
	skind sk = ref->kind;
	cstring ret;

	if (sk == SK_NEW)
	  {
	    ret = message ("storage pointed to by %q",
			   sRef_unparseWithArgs (ref, args));
	  }
	else if (skind_isSimple (sk) || sk == SK_PTR)
	  {
	    ret = message ("*%q", sRef_unparseWithArgs (ref, args));
	  }
	else
	  {
	    ret = message ("*(%q)", sRef_unparseWithArgs (ref, args));
	  }

	return ret;
      }
    case SK_ADR:
      return (message ("&%q", sRef_unparseWithArgs (s->info->ref, args)));
    case SK_OBJECT:
      return (cstring_copy (ctype_unparse (s->info->object)));
    case SK_CONJ:
      return (sRef_unparseWithArgs (sRef_getConjA (s), args));
    case SK_NEW:
      if (cstring_isDefined (s->info->fname))
	{
	  return (message ("[result of %s]", s->info->fname));
	}
      else
	{
	  return (cstring_makeLiteral ("<new>"));
	}
    case SK_UNKNOWN:
      return (cstring_makeLiteral ("?"));
    case SK_DERIVED:
      return (message ("<derived %q>", sRef_unparse (s->info->ref)));
    case SK_EXTERNAL:
      return (message ("<external %q>", sRef_unparse (s->info->ref)));
    case SK_TYPE:
      return (message ("<type %s>", ctype_unparse (s->type)));
    case SK_CONST:
      return (message ("<const %s>", ctype_unparse (s->type)));
    case SK_SPECIAL:
      switch (s->info->spec)
	{
	case SR_NOTHING: return cstring_makeLiteral ("nothing");
	case SR_INTERNAL: return cstring_makeLiteral ("internal state");
	case SR_SPECSTATE: return cstring_makeLiteral ("spec state");
	case SR_SYSTEM: return cstring_makeLiteral ("file system state");
	case SR_GLOBALMARKER: return cstring_makeLiteral ("<global marker>");
	}
      BADBRANCH;
    case SK_RESULT:
      return cstring_makeLiteral ("result");
    default:
      {
	llbug (message ("Bad sref, kind = %d", (int) s->kind));
      }
    }

  BADEXIT;
}

/*@only@*/ cstring
sRef_unparseDebug (sRef s)
{
  if (sRef_isInvalid (s)) 
    {
      return (cstring_makeLiteral ("<undef>"));
    }


  switch (s->kind)
    {
    case SK_UNCONSTRAINED:
      return (message ("<unconstrained %s>", s->info->fname));
    case SK_CVAR:
      {
	uentry ce;

	ce = usymtab_getRefQuiet (s->info->cvar->lexlevel, s->info->cvar->index);

	if (uentry_isInvalid (ce))
	  {
	    return (message ("<scope: %d.%d *invalid*>", 
			     s->info->cvar->lexlevel,
			     s->info->cvar->index));
	  }
	else
	  {
	    return (message ("<scope: %d.%d *%q*>", 
			     s->info->cvar->lexlevel,
			     s->info->cvar->index,
			     uentry_getName (ce)));
	  }

      }
    case SK_PARAM:
      {
	return (message ("<parameter %d>", s->info->paramno + 1));
      }
    case SK_ARRAYFETCH:
      if (s->info->arrayfetch->indknown)
	{
	  return (message ("%q[%d]", sRef_unparseDebug (s->info->arrayfetch->arr),
			   s->info->arrayfetch->ind));
	}
      else
	{
	  return (message ("%q[]", sRef_unparseDebug (s->info->arrayfetch->arr)));
	}
    case SK_FIELD:
      return (message ("%q.%s", sRef_unparseDebug (s->info->field->rec),
		       s->info->field->field));
    case SK_PTR:
      if (sRef_isField (s->info->ref)) 
	{
	  sRef fld = s->info->ref;

	  return (message ("%q->%s", sRef_unparseDebug (fld->info->field->rec),
			   fld->info->field->field));
	}
      else
	{
	  return (message ("*(%q)", sRef_unparseDebug (s->info->ref)));
	}
    case SK_ADR:
      return (message ("&%q", sRef_unparseDebug (s->info->ref)));
    case SK_OBJECT:
      return (message ("<object type %s>", ctype_unparse (s->info->object)));
    case SK_CONJ:
      return (message ("%q | %q", sRef_unparseDebug (s->info->conj->a),
		       sRef_unparseDebug (s->info->conj->b)));
    case SK_NEW:
      return message ("<new: %s>", s->info->fname);
    case SK_DERIVED:
      return (message ("<derived %q>", sRef_unparseDebug (s->info->ref)));
    case SK_EXTERNAL:
      return (message ("<external %q>", sRef_unparseDebug (s->info->ref)));
    case SK_TYPE:
      return (message ("<type %s>", ctype_unparse (s->type)));
    case SK_CONST:
      if (sRef_hasValue (s))
	{
	  return (message ("<const %s=%q>", ctype_unparse (s->type), multiVal_unparse (sRef_getValue (s))));
	}
      else
	{
	  return (message ("<const %s>", ctype_unparse (s->type)));
	}
    case SK_RESULT:
      return (message ("<result %s>", ctype_unparse (s->type)));
    case SK_SPECIAL:
      return (message ("<spec %s>",
		       cstring_makeLiteralTemp
		       (s->info->spec == SR_NOTHING ? "nothing"
			: s->info->spec == SR_INTERNAL ? "internalState"
			: s->info->spec == SR_SPECSTATE ? "spec state"
			: s->info->spec == SR_SYSTEM ? "fileSystem"
			: "error")));
    case SK_UNKNOWN:
      return cstring_makeLiteral ("<unknown>");
    }

  BADEXIT;
}

static /*@only@*/ cstring
sRef_unparseNoArgs (sRef s)
{
  if (sRef_isInvalid (s)) return (cstring_makeLiteral ("?"));

  switch (s->kind)
    {
    case SK_UNCONSTRAINED:
      return (cstring_copy (s->info->fname));
    case SK_CVAR:
      {
	uentry ce = usymtab_getRefQuiet (s->info->cvar->lexlevel, 
					 s->info->cvar->index);

	if (uentry_isInvalid (ce))
	  {
	    llcontbug (message ("sRef_unparseNoArgs: bad cvar: %q", 
				sRef_unparseDebug (s)));
	    return (sRef_unparseDebug (s)); 
	  }
	else
	  {
	    return (uentry_getName (ce));
	  }
      }
    case SK_ARRAYFETCH:
      if (s->info->arrayfetch->indknown)
	{
	  return (message ("%q[%d]", sRef_unparseNoArgs (s->info->arrayfetch->arr),
			   s->info->arrayfetch->ind));
	}
      else
	{
	  return (message ("%q[]", sRef_unparseNoArgs (s->info->arrayfetch->arr)));
	}
    case SK_FIELD:
      return (message ("%q.%s", sRef_unparseNoArgs (s->info->field->rec),
		       s->info->field->field));
    case SK_PTR:
      {
	sRef ref = sRef_fixConj (s->info->ref);
	skind sk = ref->kind;
	cstring ret;

	if (skind_isSimple (sk) || sk == SK_PTR)
	  {
	    ret = message ("*%q", sRef_unparseNoArgs (ref));
	  }
	else
	  {
	    ret = message ("*(%q)", sRef_unparseNoArgs (ref));
	  }

	return (ret);
      }
    case SK_ADR:
      return (message ("&%q", sRef_unparseNoArgs (s->info->ref)));
    case SK_OBJECT:
      return (cstring_copy (ctype_unparse (s->info->object)));
    case SK_CONJ:
      return (sRef_unparseNoArgs (s->info->conj->a));
    case SK_NEW:
      return (message ("result of %s", s->info->fname));
    case SK_DERIVED:
      return (message ("<der %q>", sRef_unparseNoArgs (s->info->ref)));
    case SK_EXTERNAL:
      return message ("<ext %q>", sRef_unparseNoArgs (s->info->ref));
    case SK_SPECIAL:
      return (cstring_makeLiteral
	      (s->info->spec == SR_NOTHING ? "nothing"
	       : s->info->spec == SR_INTERNAL ? "internal state"
	       : s->info->spec == SR_SPECSTATE ? "spec state"
	       : s->info->spec == SR_SYSTEM ? "file system state"
	       : "<spec error>"));
    case SK_RESULT:
      return cstring_makeLiteral ("result");
    case SK_CONST:
    case SK_TYPE:
    case SK_UNKNOWN:
      return cstring_makeLiteral ("?");
    case SK_PARAM:
      /* llcontbug (message ("sRef_unparseNoArgs: bad case: %q", sRef_unparseDebug (s))); */
      return (sRef_unparseDebug (s));
    }
  BADEXIT;
}

/*@dependent@*/ sRef sRef_makeUnconstrained (cstring fname)
{
  sRef s = sRef_new ();

  s->kind = SK_UNCONSTRAINED;
  s->info = (sinfo) dmalloc (sizeof (*s->info));
  s->info->fname = cstring_copy (fname); /* evans 2002-07-12: this was exposed, causing memory errors */

  return (s);
}

cstring sRef_unconstrainedName (sRef s)
{
  llassert (sRef_isUnconstrained (s));

  return s->info->fname;
}

bool sRef_isUnconstrained (sRef s) 
{
  return (sRef_isReasonable(s) && s->kind == SK_UNCONSTRAINED);
}

static /*@dependent@*/ /*@notnull@*/ sRef 
  sRef_makeCvarAux (int level, usymId index, ctype ct, /*@only@*/ stateInfo stinfo)
{
  sRef s = sRef_newRef ();
  
  s->kind = SK_CVAR;
  s->info = (sinfo) dmalloc (sizeof (*s->info));

  s->info->cvar = (cref) dmalloc (sizeof (*s->info->cvar));
  s->info->cvar->lexlevel = level;
  s->info->cvar->index = index;

  /* for now, all globals are defined; all locals, aren't */

  if (level <= fileScope)
    {
      s->defstate = SS_UNKNOWN;
    }
  else 
    {
      ctype rct = ctype_realType (ct);

      if (level != paramsScope
	  && (ctype_isStructorUnion (rct) || ctype_isRealArray (rct)))
	{
	  s->defstate = SS_ALLOCATED; 
	  s->oaliaskind = s->aliaskind = AK_STACK;
	}
      else
	{
	  s->defstate = SS_UNDEFINED;
	  s->oaliaskind = s->aliaskind = AK_LOCAL;
	}
    }

  s->type = ct;

  llassert (level >= globScope);
  llassert (usymId_isValid (index));

  DPRINTF (("Made cvar: [%p] %s", s, sRef_unparseDebug (s)));
  llassert (valueTable_isUndefined (s->state));
  s->state = context_createValueTable (s, stinfo); 
  return s;
}

/*@dependent@*/ sRef sRef_makeCvar (int level, usymId index, ctype ct, /*@only@*/ stateInfo stinfo)
{
  return (sRef_makeCvarAux (level, index, ct, stinfo));
}

int sRef_lexLevel (sRef s)
{
  if (sRef_isReasonable (s))
    {
      sRef conj;

      conj = sRef_fixConj (s);
      s = sRef_getRootBase (conj);
      
      if (sRef_isReasonable (s) && s->kind == SK_CVAR)
	{
	  return (s->info->cvar->lexlevel);
	}
    }

  return globScope;
}

sRef
sRef_makeGlobal (usymId l, ctype ct, /*@only@*/ stateInfo stinfo)
{
  return (sRef_makeCvar (globScope, l, ct, stinfo));
}

void
sRef_setParamNo (sRef s, int l)
{
  llassert (sRef_isReasonable (s) && s->kind == SK_PARAM);
  s->info->paramno = l;
  llassert (l >= -1);
}

/*@dependent@*/ sRef
sRef_makeParam (int l, ctype ct, stateInfo stinfo)
{
  sRef s = sRef_new ();

  s->kind = SK_PARAM;
  s->type = ct;

  s->info = (sinfo) dmalloc (sizeof (*s->info));
  s->info->paramno = l; 
  llassert (l >= -1);
  s->defstate = SS_UNKNOWN; 
  /* (probably defined, unless its an out parameter) */

  llassert (valueTable_isUndefined (s->state));
  s->state = context_createValueTable (s, stinfo);
  return s;
}

bool
sRef_isIndexKnown (sRef arr)
{
  bool res;

  llassert (sRef_isReasonable (arr));
  arr = sRef_fixConj (arr);
  
  llassert (arr->kind == SK_ARRAYFETCH);  
  res = arr->info->arrayfetch->indknown;
  return (res);
}

int
sRef_getIndex (sRef arr)
{
  int result;

  llassert (sRef_isReasonable (arr));
  arr = sRef_fixConj (arr);

  llassert (arr->kind == SK_ARRAYFETCH);  

  if (!arr->info->arrayfetch->indknown)
    {
      llcontbug (message ("sRef_getIndex: unknown: %q", sRef_unparse (arr)));
      result = 0; 
    }
  else
    {
      result = arr->info->arrayfetch->ind;
    }

  return result;
}

static bool sRef_isZerothArrayFetch (/*@notnull@*/ sRef s)
{
  return (s->kind == SK_ARRAYFETCH
	  && s->info->arrayfetch->indknown
	  && (s->info->arrayfetch->ind == 0));
}

/*@exposed@*/ sRef sRef_makeAddress (/*@exposed@*/ sRef t)
{
  
  if (sRef_isInvalid (t)) return sRef_undefined;

  if (sRef_isPointer (t))
    {
      return (t->info->ref);
    }
  else if (sRef_isZerothArrayFetch (t))
    {
      return (t->info->arrayfetch->arr);
    }
  else
    {
      sRef s = sRef_newRef ();
      
      s->kind = SK_ADR;
      s->type = ctype_makePointer (t->type);
      s->info = (sinfo) dmalloc (sizeof (*s->info));
      s->info->ref = t; 
      
      if (t->defstate == SS_UNDEFINED) 
	/* no! it is allocated even still: && !ctype_isPointer (t->type)) */
	{
	  s->defstate = SS_ALLOCATED;
	}
      else
	{
	  s->defstate = t->defstate;
	}

      if (t->aliaskind == AK_LOCAL)
	{
	  if (sRef_isLocalVar (t))
	    {
	      s->aliaskind = AK_STACK;
	    }
	}

      llassert (valueTable_isUndefined (s->state));
      s->state = context_createValueTable (s, 
					   stateInfo_makeLoc (g_currentloc, SA_CREATED));
      return s;
    }
}

cstring sRef_getField (sRef s)
{
  cstring res;

  llassert (sRef_isReasonable (s));
  s = sRef_fixConj (s);

  llassertprint (sRef_isReasonable (s) && (s->kind == SK_FIELD),
		 ("s = %s", sRef_unparseDebug (s)));

  res = s->info->field->field;
  return (res);
}

sRef sRef_getBase (sRef s)
{
  sRef res;

  if (sRef_isInvalid (s)) return (sRef_undefined);

  s = sRef_fixConj (s);

  switch (s->kind)
    {
    case SK_ADR:
    case SK_PTR:
    case SK_DERIVED:
    case SK_EXTERNAL:
      res = s->info->ref;
      break;
    case SK_FIELD:
      res = s->info->field->rec;
      break;

    case SK_ARRAYFETCH:
      res = s->info->arrayfetch->arr;
      break;

    default:
      res = sRef_undefined; /* shouldn't need it */
    }

  return (res);
}

/*
** same as getBase, except returns invalid
** (and doesn't use adr's)                   
*/

sRef
sRef_getBaseSafe (sRef s)
{
  sRef res;

  if (sRef_isInvalid (s)) { return sRef_undefined; }

  s = sRef_fixConj (s);

  switch (s->kind)
    {
    case SK_PTR:
            res = s->info->ref; 
      break;
    case SK_FIELD:
            res = s->info->field->rec; break;
    case SK_ARRAYFETCH:
            res = s->info->arrayfetch->arr; 
      break;
    default:
      res = sRef_undefined; break;
    }

  return res;
}

/*@constant int MAXBASEDEPTH;@*/
# define MAXBASEDEPTH 25

static /*@exposed@*/ sRef 
sRef_getRootBaseAux (sRef s, int depth)
{
  if (sRef_isInvalid (s)) return sRef_undefined;

  if (depth > MAXBASEDEPTH)
    {
      llgenmsg (message 
		("Warning: reference base limit exceeded for %q. "
		 "This either means there is a variable with at least "
		 "%d indirections from this reference, or "
		 "there is a bug in Splint.",
		 sRef_unparse (s),
		 MAXBASEDEPTH),
		g_currentloc);

      return sRef_undefined;
    }

  switch (s->kind)
    {
    case SK_ADR:
    case SK_PTR:
      return (sRef_getRootBaseAux (s->info->ref, depth + 1));
    case SK_FIELD:
      return (sRef_getRootBaseAux (s->info->field->rec, depth + 1));
    case SK_ARRAYFETCH:
      return (sRef_getRootBaseAux (s->info->arrayfetch->arr, depth + 1));
    case SK_CONJ:
      return (sRef_getRootBaseAux (sRef_fixConj (s), depth + 1));
    default:
      return s;
    }
}

sRef sRef_getRootBase (sRef s)
{
  return (sRef_getRootBaseAux (s, 0));
}

static bool sRef_isDeep (sRef s)
{
  if (sRef_isInvalid (s)) return FALSE;
  
  switch (s->kind)
    {
    case SK_ADR:
    case SK_PTR:
    case SK_FIELD:
    case SK_ARRAYFETCH:
      return TRUE;
    case SK_CONJ:
      return (sRef_isDeep (sRef_fixConj (s)));
    default:
      return FALSE;
    }
}

static int sRef_depth (sRef s)
{
  if (sRef_isInvalid (s)) return 0;
  
  switch (s->kind)
    {
    case SK_ADR:
    case SK_PTR:
    case SK_DERIVED:
    case SK_EXTERNAL:
      return 1 + sRef_depth (s->info->ref);
    case SK_FIELD:
      return 1 + sRef_depth (s->info->field->rec);
    case SK_ARRAYFETCH:
      return 1 + sRef_depth (s->info->arrayfetch->arr);
    case SK_CONJ:
      return (sRef_depth (sRef_fixConj (s)));
    default:
      return 1;
    }
}

sRef
sRef_makeObject (ctype o)
{
  sRef s = sRef_newRef (); 

  s->kind = SK_OBJECT;
  s->info = (sinfo) dmalloc (sizeof (*s->info));
  s->info->object = o;
  llassert (valueTable_isUndefined (s->state));
  s->state = context_createValueTable (s, stateInfo_makeLoc (g_currentloc, SA_CREATED));
  return s;
}

/*
** This is used to represent storage referenced by a parameter.
*/

sRef sRef_makeExternal (sRef t)
{
  sRef s = sRef_newRef ();

  llassert (sRef_isReasonable (t));

  s->kind = SK_EXTERNAL;
  s->info = (sinfo) dmalloc (sizeof (*s->info));
  s->type = t->type;
  s->info->ref = t;
  llassert (valueTable_isUndefined (s->state));
  s->state = context_createValueTable (s, stateInfo_makeLoc (g_currentloc, SA_DECLARED));
  return s;
}

/*@dependent@*/ sRef sRef_makeDerived (/*@exposed@*/ sRef t)
{
  if (sRef_isReasonable (t))
    {
      sRef s = sRef_newRef ();
      
      s->kind = SK_DERIVED;
      s->info = (sinfo) dmalloc (sizeof (*s->info));
      s->info->ref = t;
      
      s->type = t->type;
      llassert (valueTable_isUndefined (s->state));
      s->state = context_createValueTable (s, stateInfo_makeLoc (g_currentloc, SA_CREATED));
      return s;
    }
  else
    {
      return sRef_undefined;
    }
}

/*
** definitely NOT symmetric:
**
**   res fills in unknown state information from other
*/

void
sRef_mergeStateQuiet (sRef res, sRef other)
{
  llassert (sRef_isReasonable (res));
  llassert (sRef_isReasonable (other));

  res->modified = res->modified || other->modified;
  res->safe = res->safe && other->safe;

  if (res->defstate == SS_UNKNOWN) 
    {
      res->defstate = other->defstate;
      res->definfo = stateInfo_update (res->definfo, other->definfo);
    }

  if (res->aliaskind == AK_UNKNOWN || 
      (res->aliaskind == AK_LOCAL && alkind_isKnown (other->aliaskind)))
    {
      res->aliaskind = other->aliaskind;
      res->oaliaskind = other->oaliaskind;
      res->aliasinfo = stateInfo_update (res->aliasinfo, other->aliasinfo);
    }

  if (res->expkind == XO_UNKNOWN)
    {
      res->expkind = other->expkind;
      res->oexpkind = other->oexpkind;
      res->expinfo = stateInfo_update (res->expinfo, other->expinfo);
    }
  
  /* out takes precedence over implicitly defined */
  if (res->defstate == SS_DEFINED && other->defstate != SS_UNKNOWN) 
    {
      res->defstate = other->defstate;
      res->definfo = stateInfo_update (res->definfo, other->definfo);
    }

  if (sRef_getNullState (other) == NS_ERROR || sRef_getNullState (res) == NS_ERROR) 
    {
      sRef_setNullState (res, NS_ERROR, fileloc_undefined);
    }
  else
    {
      if (sRef_getNullState (other) != NS_UNKNOWN 
	  && (sRef_getNullState (res) == NS_UNKNOWN || sRef_getNullState (res) == NS_NOTNULL 
	      || sRef_getNullState (res) == NS_MNOTNULL))
	{
	  sRef_updateNullState (res, other);
	}
    }
}

/*
** definitely NOT symmetric:
**
**   res fills in known state information from other
*/

void
sRef_mergeStateQuietReverse (/*@dependent@*/ sRef res, /*@dependent@*/ sRef other)
{
  bool changed = FALSE;

  llassert (sRef_isReasonable (res));
  llassert (sRef_isReasonable (other));
  sRef_checkMutable (res);

  if (res->kind != other->kind)
    {
      changed = TRUE;

      sinfo_free (res);

      res->kind = other->kind;
      res->type = other->type;
      res->info = sinfo_fullCopy (other);
    }
  else
    {
      if (!ctype_equal (res->type, other->type))
	{
	  changed = TRUE;
	  res->type = other->type;
	}
      
      sinfo_update (res, other);
    }

  res->modified = res->modified || other->modified;
  res->safe = res->safe && other->safe;

  if (res->aliaskind != other->aliaskind
      && (res->aliaskind == AK_UNKNOWN
	  || ((res->aliaskind == AK_LOCAL 
	       || (res->aliaskind == AK_REFCOUNTED
		   && other->aliaskind != AK_LOCAL))
	      && other->aliaskind != AK_UNKNOWN)))
    {
      changed = TRUE;
      res->aliaskind = other->aliaskind;
      res->oaliaskind = other->oaliaskind;
      res->aliasinfo = stateInfo_update (res->aliasinfo, other->aliasinfo);
    }

  if (other->expkind != XO_UNKNOWN && other->expkind != res->expkind)
    {
      changed = TRUE;
      res->expkind = other->expkind;
      res->expinfo = stateInfo_update (res->expinfo, other->expinfo);
    }

  if (other->oexpkind != XO_UNKNOWN)
    {
      res->oexpkind = other->oexpkind;
    }

  /* out takes precedence over implicitly defined */

  if (res->defstate != other->defstate)
    {
      if (other->defstate != SS_UNKNOWN)
	{
	  res->defstate = other->defstate;
	  res->definfo = stateInfo_update (res->definfo, other->definfo);
	}
    }

  if (sRef_getNullState (other) == NS_ERROR || sRef_getNullState (res) == NS_ERROR)
    {
      if (sRef_getNullState (res) != NS_ERROR)
	{
	  sRef_setNullStateN (res, NS_ERROR);
	  changed = TRUE;
	}
    }
  else
    {
      if (sRef_getNullState (other) != NS_UNKNOWN 
	  && sRef_getNullState (other) != sRef_getNullState (res))
	{
	  changed = TRUE;
	  sRef_updateNullState (res, other);
	}
    }

  if (changed)
    {
      sRef_clearDerived (res); 
    }
}

void 
sRef_mergeState (sRef res, sRef other, clause cl, fileloc loc)
{
  if (sRef_isReasonable (res) && sRef_isReasonable (other))
    {
      sRef_mergeStateAux (res, other, cl, FALSE, loc, TRUE);
    }
  else
    {
      if (sRef_isInvalid (res))
	{
	  llbug (message ("sRef_mergeState: invalid res sRef: %q", 
			  sRef_unparseDebug (other)));
	}
      else 
	{
	  llbug (message ("sRef_mergeState: invalid other sRef: %q", 
			  sRef_unparseDebug (res)));
	}
    }
}

void 
sRef_mergeOptState (sRef res, sRef other, clause cl, fileloc loc)
{
  if (sRef_isReasonable (res) && sRef_isReasonable (other))
    {
      sRef_mergeStateAux (res, other, cl, TRUE, loc, TRUE);
    }
  else
    {
      if (sRef_isInvalid (res))
	{
	  llbug (message ("sRef_mergeOptState: invalid res sRef: %q", 
			  sRef_unparseDebug (other)));
	}
      else 
	{
	  llbug (message ("sRef_mergeOptState: invalid other sRef: %q", 
			  sRef_unparseDebug (res)));
	}
    }
}

static void
sRef_mergeStateAux (/*@notnull@*/ sRef res, /*@notnull@*/ sRef other, 
		    clause cl, bool opt, fileloc loc,
		    bool doDerivs)
   /*@modifies res@*/ 
{
  llassertfatal (sRef_isReasonable (res));
  llassertfatal (sRef_isReasonable (other));
  
  DPRINTF (("Merge aux: %s / %s",
	    sRef_unparseFull (res),
	    sRef_unparseFull (other)));

  sRef_checkMutable (res);
  sRef_checkMutable (other);

  res->modified = res->modified || other->modified;

  if (res->kind == other->kind 
      || (other->kind == SK_UNKNOWN || res->kind == SK_UNKNOWN))
    {
      sstate odef = other->defstate;
      sstate rdef = res->defstate;
      nstate onull = sRef_getNullState (other);
      
      /*
      ** yucky stuff to handle 
      **
      **   if (s) free (s);
      */

      if (other->defstate == SS_DEAD 
	  && ((sRef_isOnly (res) && sRef_definitelyNull (res))
	      || (res->defstate == SS_UNDEFINED
		  || res->defstate == SS_UNUSEABLE)))
	{
	  if (res->defstate == SS_UNDEFINED
	      || res->defstate == SS_UNUSEABLE)
	    {
	      res->defstate = SS_UNUSEABLE;
	    }
	  else
	    {
	      res->defstate = SS_DEAD;
	    }

	  res->definfo = stateInfo_update (res->definfo, other->definfo);
	  sRef_clearDerived (other);
	  sRef_clearDerived (res);
	}
      else if (res->defstate == SS_DEAD 
	       && ((sRef_isOnly (other) && sRef_definitelyNull (other))
		   || (other->defstate == SS_UNDEFINED
		       || other->defstate == SS_UNUSEABLE)))
	{
	  if (other->defstate == SS_UNDEFINED
	      || other->defstate == SS_UNUSEABLE)
	    {
	      res->defstate = SS_UNUSEABLE;
	    }
	  else
	    {
	      res->defstate = SS_DEAD;
	    }
	  
	  res->definfo = stateInfo_update (res->definfo, other->definfo);
	  sRef_clearDerived (other);
	  sRef_clearDerived (res);
	}
      else if (res->defstate == SS_DEFINED 
	       && (other->defstate == SS_ALLOCATED 
		   && sRef_definitelyNull (other)))
	{
	  other->defstate = SS_DEFINED; /* definitely null! */
	}
      else if (other->defstate == SS_DEFINED
	       && (res->defstate == SS_ALLOCATED && sRef_definitelyNull (res)))
	{
	  res->defstate = SS_DEFINED;
	  res->definfo = stateInfo_update (res->definfo, other->definfo);
	}
      else
	{
	  ; /* okay */
	}

      if (res->defstate == SS_DEAD && other->defstate == SS_DEAD)
	{
	  sRef_clearDerived (other);
	  sRef_clearDerived (res);
	}

      /*
      ** only & dead isn't really an only!
      */

      if (alkind_isOnly (other->aliaskind) && other->defstate == SS_DEAD)
	{
	  other->aliaskind = AK_UNKNOWN;
	}

      if (alkind_isOnly (res->aliaskind) && res->defstate == SS_DEAD)
	{
	  res->aliaskind = AK_UNKNOWN;
	}

      /*
      ** Dead and dependent -> dead
      */
      
      if (alkind_isDependent (other->aliaskind) && res->defstate == SS_DEAD)
	{
	  other->aliaskind = AK_UNKNOWN;
	  other->defstate = SS_DEAD;
	  sRef_clearDerived (res);
	  sRef_clearDerived (other);
	}
      
      if (alkind_isDependent (res->aliaskind) && other->defstate == SS_DEAD)
	{
	  res->aliaskind = AK_UNKNOWN;
	  res->defstate = SS_DEAD;
	  sRef_clearDerived (res);
	  sRef_clearDerived (other);
	}

      /*
      ** must do alias combine first, since it depends on 
      ** original values of state and null.
      */

      sRef_combineAliasKinds (res, other, cl, loc);
      sRef_combineDefState (res, other);
      sRef_combineNullState (res, other);

      if (rdef == SS_ALLOCATED || rdef == SS_SPECIAL)
	{
	  if (odef == SS_DEFINED)
	    {
	      if (onull == NS_DEFNULL || onull == NS_CONSTNULL)
		{
		  res->deriv = sRefSet_copyInto (res->deriv, other->deriv);
		  DPRINTF (("Copy derivs: %s", sRef_unparseFull (res)));
		}	      	      	      
	    }
	  else if (odef == SS_ALLOCATED || odef == SS_SPECIAL)
	    {
	      
	      if (doDerivs)
		{
		  if (ctype_isUnion (ctype_realType (sRef_getType (res))))
		    {
		      res->deriv = sRef_mergeUnionDerivs (res->deriv, 
							  other->deriv, 
							  opt, cl, loc);
		      DPRINTF (("Copy derivs: %s", sRef_unparseFull (res)));
		    }
		  else
		    {
		      res->deriv = sRef_mergeDerivs (res->deriv, other->deriv, 
						     opt, cl, loc);
		      DPRINTF (("Copy derivs: %s", sRef_unparseFull (res)));
		    }
		}
	    }
	  else
	    {
	      if (doDerivs)
		{
		  res->deriv = sRef_mergeDerivs (res->deriv, other->deriv, 
						 opt, cl, loc);
		  DPRINTF (("Copy derivs: %s", sRef_unparseFull (res)));
		}
	      else
		{
		  ;
		}
	    }
	}
      else
	{
	  if (rdef == SS_PDEFINED
	      || (rdef == SS_DEFINED && odef == SS_PDEFINED))
	    {
		if (doDerivs)
		    {
		      res->deriv = sRef_mergePdefinedDerivs (res->deriv, other->deriv, 
							     opt, cl, loc);
		      DPRINTF (("Copy derivs: %s", sRef_unparseFull (res)));
		    }
	    }
	  else
	    {
	      if ((rdef == SS_DEFINED  || rdef == SS_UNKNOWN)
		  && res->defstate == SS_ALLOCATED)
		{
		  res->deriv = sRefSet_copyInto (res->deriv, other->deriv);
		}
	      else
		{
		  if (doDerivs)
		    {
		      res->deriv = sRef_mergeDerivs (res->deriv, other->deriv, 
						     opt, cl, loc);
		      DPRINTF (("Copy derivs: %s", sRef_unparseFull (res)));
		    }
		}
	    }
	}
      
      
      sRef_combineExKinds (res, other);
    }
  else
    {
      if (res->kind == SK_ARRAYFETCH && other->kind == SK_PTR)
	{
	  sRef nother = sRef_buildArrayFetchKnown (sRef_getBase (other), 0);

	  sRef_copyState (nother, other);
	  sRef_mergeStateAux (res, nother, cl, opt, loc, doDerivs);
	}
      else if (res->kind == SK_PTR && other->kind == SK_ARRAYFETCH)
	{
	  sRef nother = sRef_buildPointer (sRef_getBase (other));

	  if (sRef_isReasonable (nother))
	    {
	      sRef_copyState (nother, other);
	      sRef_mergeStateAux (res, nother, cl, opt, loc, doDerivs);
	    }
	}
      else
	{
	  llcontbug (message ("merge conj: %q / %q", sRef_unparseFull (res), 
			      sRef_unparseFull (other)));
	  
	}
    }

  /* 
  ** Merge value table states
  */

# if 0
  /*
  ** This doesn't do anything.  And its broken too...
  */

  valueTable_elements (res->state, key, sv) 
    {
      stateValue os = valueTable_lookup (other->state, key);
      /*@unused@*/ int val;
      /*@unused@*/ char *msg;

      llassert (stateValue_isDefined (os));
      
      DPRINTF (("Merge state: %s / %s", 
		cstring_toCharsSafe (stateValue_unparse (sv)), 
		cstring_toCharsSafe (stateValue_unparse (os))));
      /*
	val = valueMatix_lookup (key, 
	stateValue_getValue (os),
	stateValue_getValue (sv), 
	&msg);
	DPRINTF (("Val: %d / %s", val, msg));
      */
  } end_valueTable_elements ; 
# endif

  DPRINTF (("Merge aux: %s / %s",
	    sRef_unparseFull (res),
	    sRef_unparseFull (other)));
}

static sRefSet
sRef_mergeUnionDerivs (/*@only@*/ sRefSet res, 
		       /*@exposed@*/ sRefSet other, bool opt,
		       clause cl, fileloc loc)
{
  if (sRefSet_isEmpty (res))
    {
      return sRefSet_copyInto (res, other);
    }
  else
    {
      sRefSet_allElements (other, el)
	{
	  if (sRef_isReasonable (el))
	    {
	      sRef e2 = sRefSet_lookupMember (other, el);
	      
	      if (sRef_isReasonable (e2))
		{
		  sRef_mergeStateAux (el, e2, cl, opt, loc, FALSE);
		}
	      else
		{
		  res = sRefSet_insert (res, el);
		}
	    }
	} end_sRefSet_allElements ;

      return res;
    }
}

static /*@only@*/ sRefSet
sRef_mergeDerivs (/*@only@*/ sRefSet res, sRefSet other, 
		  bool opt, clause cl, fileloc loc)
{
  sRefSet ret = sRefSet_new ();

  sRefSet_allElements (res, el)
    {
      if (sRef_isReasonable (el))
	{
	  sRef e2 = sRefSet_lookupMember (other, el);

	  if (sRef_isReasonable (e2))
	    {
	      if (el->defstate == SS_ALLOCATED &&
		  e2->defstate == SS_PDEFINED)
		{
		  e2->defstate = SS_ALLOCATED;
		}
	      else if (e2->defstate == SS_ALLOCATED &&
		       el->defstate == SS_PDEFINED)
		{
		  el->defstate = SS_ALLOCATED;
		  el->definfo = stateInfo_update (el->definfo, e2->definfo);
		  sRef_clearDerived (el);
		}
	      else if ((el->defstate == SS_DEAD || sRef_isKept (el)) &&
		       (e2->defstate == SS_DEFINED && !sRef_isKept (e2)))
		{
		  DPRINTF (("Checking dead: %s / %s", sRef_unparseFull (el),
			    sRef_unparseFull (e2)));

		  if (checkDeadState (el, e2, TRUE, loc))
		    {
		      if (sRef_isThroughArrayFetch (el))
			{
			  sRef_maybeKill (el, loc);
			  sRef_maybeKill (e2, loc);
			}
		    }
		}
	      else if ((e2->defstate == SS_DEAD || sRef_isKept (e2)) &&
		       (el->defstate == SS_DEFINED && !sRef_isKept (el)))
		{
		  DPRINTF (("Checking dead: %s / %s", sRef_unparseFull (el),
			    sRef_unparseFull (e2)));

		  if (checkDeadState (e2, el, FALSE, loc))
		    {
		      if (sRef_isThroughArrayFetch (el))
			{
			  sRef_maybeKill (el, loc);
			  sRef_maybeKill (e2, loc);
			}
		    }
		}
	      else if (el->defstate == SS_DEFINED &&
		       e2->defstate == SS_PDEFINED)
		{
		  DPRINTF (("set pdefined: %s", sRef_unparseFull (el)));
		  el->defstate = SS_PDEFINED;
		  el->definfo = stateInfo_update (el->definfo, e2->definfo);
		}
	      else if (e2->defstate == SS_DEFINED &&
		       el->defstate == SS_PDEFINED)
		{
		  DPRINTF (("set pdefined: %s", sRef_unparseFull (e2)));
		  e2->defstate = SS_PDEFINED;
		  e2->definfo = stateInfo_update (e2->definfo, el->definfo);
		}
	      else
		{
		  ; /* okay */
		}

	      if (ctype_isUnion (ctype_realType (sRef_getType (el))))
		{
		  el->deriv = sRef_mergeUnionDerivs (el->deriv, e2->deriv, 
						     opt, cl, loc); 
		}
	      else
		{
		  el->deriv = sRef_mergeDerivs (el->deriv, e2->deriv, opt, cl, loc); 
		}
	      
	      if (sRef_equivalent (el, e2))
		{
		  ret = sRefSet_insert (ret, el);
		}
	      else
		{
		  sRef sr = sRef_leastCommon (el, e2);

		  if (sRef_isReasonable (sr))
		    {
		      ret = sRefSet_insert (ret, sr);
		    }
		  else
		    {
		      ;
		    }
		}
	      
	      (void) sRefSet_delete (other, e2);
	    }
	  else /* not defined */
	    {
	      DPRINTF (("Checking dead: %s", sRef_unparseFull (el)));
	      (void) checkDeadState (el, e2, TRUE, loc);
	    }
	}
    } end_sRefSet_allElements;

  sRefSet_allElements (other, el)
    {
      if (sRef_isReasonable (el))
	{
	  DPRINTF (("Checking dead: %s", sRef_unparseFull (el)));
	  (void) checkDeadState (el, sRef_undefined, FALSE, loc);
	}
    } end_sRefSet_allElements;
    
  sRefSet_free (res); 
  return (ret);
}

/*
** Returns TRUE is there is an error.
*/

static bool checkDeadState (/*@notnull@*/ sRef el, sRef e2, bool tbranch, fileloc loc)
{
  /*
  ** usymtab_isGuarded --- the utab should still be in the
  ** state of the alternate branch.
  **
  ** tbranch TRUE means el is released in the last branch, e.g.
  **     if (x != NULL) { ; } else { sfree (x); }
  ** so, if x is null in the other branch no error is reported.
  **
  ** tbranch FALSE means this is the other branch:
  **     if (x != NULL) { sfree (x); } else { ; }
  ** so, if x is null in this branch there is no error.
  */

  
  if ((sRef_isDead (el) || sRef_isKept (el))
      && !sRef_isDeepUnionField (el) 
      && !sRef_isThroughArrayFetch (el))
    {
       
      if (!tbranch)
	{
	  if (usymtab_isDefinitelyNullDeep (el))
	    {
	      return TRUE;
	    }
	}
      else
	{
	  if (usymtab_isAltDefinitelyNullDeep (el))
	    {
	      return TRUE;
	    }
	}
      
      if (optgenerror
	  (FLG_BRANCHSTATE,
	   message ("Storage %q is %q in one path, but live in another.",
		    sRef_unparse (el),
		    cstring_makeLiteral (sRef_isKept (el) 
					 ? "kept" : "released")),
	   loc))
	{
	  if (sRef_isKept (el))
	    {
	      sRef_showAliasInfo (el);      
	    }
	  else
	    {
	      sRef_showStateInfo (el);
	    }

	  if (sRef_isValid (e2))
	    {
	      if (sRef_isKept (e2))
		{
		  sRef_showAliasInfo (e2);      
		}
	      else
		{
		  sRef_showStateInfo (e2);
		}
	    }

	  /* prevent further errors */
	  el->defstate = SS_UNKNOWN; 
	  sRef_setAliasKind (el, AK_ERROR, fileloc_undefined);	  
	  return FALSE;
	}
    }

  return TRUE;
}

static void 
checkDerivDeadState (/*@notnull@*/ sRef el, bool tbranch, fileloc loc)
{
  if (checkDeadState (el, sRef_undefined, tbranch, loc))
    {
      sRefSet_allElements (el->deriv, t)
	{
	  if (sRef_isReasonable (t))
	    {
	      checkDerivDeadState (t, tbranch, loc);
	    }
	} end_sRefSet_allElements;
    }
}

static sRefSet
  sRef_mergePdefinedDerivs (sRefSet res, sRefSet other, bool opt, 
			    clause cl, fileloc loc)
{
  sRefSet ret = sRefSet_new ();

  sRefSet_allElements (res, el)
    {
      if (sRef_isReasonable (el))
	{
	  sRef e2 = sRefSet_lookupMember (other, el);
	  
	  if (sRef_isReasonable (e2))
	    {
	      if (sRef_isAllocated (el) && !sRef_isAllocated (e2))
		{
		  ;
		}
	      else if (sRef_isAllocated (e2) && !sRef_isAllocated (el))
		{
		  el->deriv = sRefSet_copyInto (el->deriv, e2->deriv); 
		}
	      else
		{
		  el->deriv = sRef_mergePdefinedDerivs (el->deriv, e2->deriv, 
							opt, cl, loc);
		}

	      sRef_mergeStateAux (el, e2, cl, opt, loc, FALSE);
	      
	      ret = sRefSet_insert (ret, el);
	      (void) sRefSet_delete (other, e2);
	    }
	  else
	    {
	      if (!opt)
		{
		  checkDerivDeadState (el, (cl == FALSECLAUSE), loc);
		}

	      ret = sRefSet_insert (ret, el);
	    }
	}
    } end_sRefSet_allElements;
  
  sRefSet_allElements (other, el)
    {
      if (sRef_isReasonable (el))
	{
	  if (!sRefSet_member (ret, el))
	    {
	      	      	      /* was cl == FALSECLAUSE */
	      checkDerivDeadState (el, FALSE, loc);
	      ret = sRefSet_insert (ret, el);
	    }
	  else
	    {
	      /*
	      ** it's okay --- member is a different equality test 
	      */
	    }
	}
    } end_sRefSet_allElements;

  sRefSet_free (res);
  return (ret);
}

sRef sRef_makeConj (/*@exposed@*/ /*@returned@*/ sRef a, /*@exposed@*/ sRef b)
{
  llassert (sRef_isReasonable (a));
  llassert (sRef_isReasonable (b));
      
  if (!sRef_equivalent (a, b))
    {
      sRef s = sRef_newRef ();
      
      s->kind = SK_CONJ;
      s->info = (sinfo) dmalloc (sizeof (*s->info));
      s->info->conj = (cjinfo) dmalloc (sizeof (*s->info->conj));
      s->info->conj->a = a;
      s->info->conj->b = b;
      
      if (ctype_equal (a->type, b->type)) s->type = a->type;
      else s->type = ctype_makeConj (a->type, b->type);
      
      if (a->defstate == b->defstate)
	{
	  s->defstate = a->defstate;
	  s->definfo = stateInfo_update (s->definfo, a->definfo);
	  s->definfo = stateInfo_update (s->definfo, b->definfo);
	}
      else
	{
	  s->defstate = SS_UNKNOWN; 
	}
      
      sRef_setNullStateN (s, NS_UNKNOWN);
      
      s->safe = a->safe && b->safe;
      s->aliaskind = alkind_resolve (a->aliaskind, b->aliaskind);

      llassert (valueTable_isUndefined (s->state));
      s->state = context_createValueTable (s, stateInfo_makeLoc (g_currentloc, SA_MERGED));
      return s;
    }
  else
    {
      /*@-exposetrans@*/ return a; /*@=exposetrans@*/
    }
}

/*@dependent@*/ sRef
sRef_makeUnknown ()
{
  sRef s = sRef_new ();

  s->kind = SK_UNKNOWN;
  return s;
}

static /*@owned@*/ /*@notnull@*/ sRef
sRef_makeSpecial (speckind sk) /*@*/
{
  sRef s = sRef_new ();

  s->kind = SK_SPECIAL;
  s->info = (sinfo) dmalloc (sizeof (*s->info));
  s->info->spec = sk;
  /*@-dependenttrans@*/
  return s;
  /*@=dependenttrans@*/
}

static /*@owned@*/ sRef srnothing = sRef_undefined;
static /*@owned@*/ sRef srinternal = sRef_undefined;
static /*@owned@*/ sRef srsystem = sRef_undefined;
static /*@owned@*/ sRef srspec = sRef_undefined;

/*@dependent@*/ sRef
sRef_makeNothing (void)
{
  if (sRef_isInvalid (srnothing))
    {
      srnothing = sRef_makeSpecial (SR_NOTHING);
    }

  return srnothing;
}

sRef
sRef_makeInternalState (void)
{
  if (sRef_isInvalid (srinternal))
    {
      srinternal = sRef_makeSpecial (SR_INTERNAL);
    }

  return srinternal;
}

sRef
sRef_makeSpecState (void)
{
  if (sRef_isInvalid (srspec))
    {
      srspec = sRef_makeSpecial (SR_SPECSTATE);
    }

  return srspec;
}

sRef
sRef_makeSystemState (void)
{
  if (sRef_isInvalid (srsystem))
    {
      srsystem = sRef_makeSpecial (SR_SYSTEM);
    }

  return srsystem;
}

sRef
sRef_makeGlobalMarker (void)
{
  sRef s = sRef_makeSpecial (SR_GLOBALMARKER);
  llassert (valueTable_isUndefined (s->state));
  s->state = context_createGlobalMarkerValueTable (stateInfo_undefined);
  return s;
}

sRef
sRef_makeResult (ctype c)
{
  sRef s = sRef_newRef ();
  
  s->kind = SK_RESULT;
  s->type = c;
  s->defstate = SS_UNKNOWN; 
  s->aliaskind = AK_UNKNOWN;
  sRef_setNullStateN (s, NS_UNKNOWN);
  llassert (valueTable_isUndefined (s->state));
  s->state = context_createValueTable (s, stateInfo_makeLoc (g_currentloc, SA_DECLARED));

  DPRINTF (("Result: [%p] %s", s, sRef_unparseFull (s)));
  return s;
}


bool
sRef_isNothing (sRef s)
{
  return (sRef_isKindSpecial (s) && s->info->spec == SR_NOTHING);
}

bool
sRef_isInternalState (sRef s)
{
  return (sRef_isKindSpecial (s) && s->info->spec == SR_INTERNAL);
}

bool
sRef_isSpecInternalState (sRef s)
{
  return (sRef_isKindSpecial (s) 
	  && (s->info->spec == SR_INTERNAL || s->info->spec == SR_SPECSTATE));
}

bool
sRef_isSpecState (sRef s)
{
  return (sRef_isKindSpecial (s) && s->info->spec == SR_SPECSTATE);
}

bool
sRef_isResult (sRef s)
{
  return (sRef_isReasonable (s) && s->kind == SK_RESULT);
}

bool
sRef_isSystemState (sRef s)
{
  return (sRef_isKindSpecial (s) && s->info->spec == SR_SYSTEM);
}

bool
sRef_isGlobalMarker (sRef s)
{
  return (sRef_isKindSpecial (s) && s->info->spec == SR_GLOBALMARKER);
}

usymId
sRef_getScopeIndex (sRef s)
{
  llassert (sRef_isReasonable (s));
  llassert (sRef_isCvar (s));

  return (s->info->cvar->index);
}

void
sRef_makeSafe (sRef s)
{
  if (sRef_isReasonable (s)) 
    {
      s->safe = TRUE;
    }
}

void
sRef_makeUnsafe (sRef s)
{
  if (sRef_isReasonable (s)) 
    {
      s->safe = FALSE;
    }
}

/*
** memory state operations
*/

/*@only@*/ cstring sRef_unparseFull (sRef s)
{
  if (sRef_isInvalid (s)) return (cstring_undefined);

  return (message ("[%p] %q - %q { %q } [%s] { %q } < %q >", 
		   s,
		   sRef_unparseDebug (s), 
		   sRef_unparseState (s),
		   stateInfo_unparse (s->definfo),
		   exkind_unparse (s->oexpkind),
		   sRefSet_unparseDebug (s->deriv),
		   valueTable_unparse (s->state)));
}

/*@unused@*/ cstring sRef_unparseDeep (sRef s)
{
  cstring st = cstring_undefined;

  st = message ("%q:", sRef_unparseFull (s));

  if (sRef_isReasonable (s))
    {
      sRefSet_allElements (s->deriv, el)
	{
	  st = message("%q\n%q", st, sRef_unparseDeep (el));
	} end_sRefSet_allElements ;
    }

  return st;
}

/*@only@*/ cstring sRef_unparseState (sRef s)
{
  if (sRef_isConj (s))
    {
      return (message ("%q | %q", 
		       sRef_unparseState (s->info->conj->a),
		       sRef_unparseState (s->info->conj->b)));
    }

  if (sRef_isInvalid (s))
    {
      return (cstring_makeLiteral ("<invalid>"));
    }

  return (message ("%s.%s.%s.%s", 
		   alkind_unparse (s->aliaskind), 
		   nstate_unparse (sRef_getNullState (s)),
		   exkind_unparse (s->expkind),
		   sstate_unparse (s->defstate)));
}

bool sRef_isNotUndefined (sRef s)
{
  return (sRef_isInvalid (s)
	  || (s->defstate != SS_UNDEFINED
	      && s->defstate != SS_UNUSEABLE
	      && s->defstate != SS_DEAD));
}

ynm sRef_isWriteable (sRef s)
{
  if (sRef_isInvalid (s)) return MAYBE;

  if (sRef_isConj (s) && s->defstate == SS_UNKNOWN)
    {
      if (ynm_toBoolStrict (sRef_isWriteable (sRef_getConjA (s))))
	{
	  if (ynm_toBoolStrict (sRef_isWriteable (sRef_getConjB (s))))
	    {
	      return YES;
	    }
	  return MAYBE;
	}
      else
	{
	  if (ynm_toBoolStrict (sRef_isWriteable (sRef_getConjB (s))))
	    {
	      return MAYBE;
	    }
	  return NO;
	}
    }

  return (ynm_fromBool (s->defstate != SS_UNUSEABLE));
}

bool sRef_hasNoStorage (sRef s)
{
  return (!sRef_isAllocatedStorage (s) || sRef_isDefinitelyNull (s));
}

bool sRef_isStrictReadable (sRef s)
{
  return (ynm_toBoolStrict (sRef_isValidLvalue (s)));
}

/*
** Is this what is does?
** Returns YES if s can be used as an rvalue,
**         MAYBE if its not clear
**         NO if s cannot be safely used as an rvalue.
*/

ynm sRef_isValidLvalue (sRef s)
{
  sstate ss;

  if (sRef_isInvalid (s)) return YES;

  ss = s->defstate;
  
  if (sRef_isConj (s) && s->defstate == SS_UNKNOWN)
    {
      if (ynm_toBoolStrict (sRef_isValidLvalue (sRef_getConjA (s))))
	{
	  if (ynm_toBoolStrict (sRef_isValidLvalue (sRef_getConjB (s))))
	    {
	      return YES;
	    }
	  return MAYBE;
	}
      else
	{
	  if (ynm_toBoolStrict (sRef_isValidLvalue (sRef_getConjB (s))))
	    {
	      return MAYBE;
	    }
	  return NO;
	}
    }
  else if (ss == SS_HOFFA)
    {
      if (context_getFlag (FLG_STRICTUSERELEASED))
	{
	  return MAYBE;
	}
      else
	{
	  return YES;
	}
    }
  else
    {
      return (ynm_fromBool (ss == SS_DEFINED 
			    || ss == SS_FIXED 
			    || ss == SS_RELDEF 
			    || ss == SS_PDEFINED 
			    || ss == SS_PARTIAL 
			    || ss == SS_SPECIAL
			    || ss == SS_ALLOCATED 
			    || ss == SS_KILLED /* evans 2001-05-26: added this for killed globals */
			    || ss == SS_UNKNOWN));
    }
}

static /*@exposed@*/ sRef whatUndefined (/*@exposed@*/ sRef fref, int depth)
{
  ctype ct;

  
  if (depth > MAXDEPTH)
    {
      llgenmsg (message 
		("Warning: check definition limit exceeded, checking %q. "
		 "This either means there is a variable with at least "
		 "%d indirections apparent in the program text, or "
		 "there is a bug in Splint.",
		 sRef_unparse (fref),
		 MAXDEPTH),
		g_currentloc);

      return sRef_undefined;
    }

  if (!sRef_isKnown (fref) || sRef_isAnyDefined (fref))
    {
      return sRef_undefined;
    }

  if (sRef_isUnuseable (fref) || sRef_isStateUndefined (fref))
    {
      return fref;
    }

  ct = ctype_realType (sRef_getType (fref));
  
  if (ctype_isUnknown (ct))
    {
      return sRef_undefined;
    }
  else if (ctype_isPointer (ct) || ctype_isArray (ct))
    {
      if (sRef_isStateUnknown (fref))
	{
	  return sRef_undefined;
	}
      else
	{
	  sRef fptr = sRef_constructDeref (fref);

	  return (whatUndefined (fptr, depth + 1));
	}
    }
  else if (ctype_isStruct (ct))
    {
      bool hasOneDefined = FALSE;
      
      if (sRef_isStateUnknown (fref))
	{
	  return fref;
	}
	  
      if (sRef_isPdefined (fref) || sRef_isAnyDefined (fref))
	{
	  sRefSet_realElements (sRef_derivedFields (fref), sr)
	    {
	      hasOneDefined = TRUE;
	      
	      if (sRef_isField (sr))
		{
		  cstring fieldname = sRef_getField (sr);
		  sRef fldref = sRef_makeField (fref, fieldname);
		  bool shouldCheck = !sRef_isRecursiveField (fldref);
		  
		  if (shouldCheck)
		    {
		      sRef wdef = whatUndefined (fldref, depth + 1);

		      if (sRef_isReasonable (wdef))
			{
			  return wdef;
			}
		    }
		}
	    } end_sRefSet_realElements;
	}
      else if (sRef_isAllocated (fref))
	{
	  /*
	  ** for structures, each field must be completely defined
	  */
	  
	  uentryList fields = ctype_getFields (ct);
	      
	  uentryList_elements (fields, ue)
	    {
	      cstring name = uentry_getRealName (ue);
	      sRef ffield = sRef_makeField (fref, name);
	      bool shouldCheck = !sRef_isRecursiveField (ffield);

	      if (sRef_isRelDef (uentry_getSref (ue)))
		{
		  ; /* no error */
		}
	      else
		{
		  if (shouldCheck)
		    {
		      sRef wdef = whatUndefined (ffield, depth + 1);

		      if (sRef_isInvalid (wdef))
			{
			  return wdef;
			}
		    }
		}
	    } end_uentryList_elements;
	}
      else
	{
	  ;
	}
    }
  else if (ctype_isUnion (ct))
    {
      ; 
    }
  else
    {
      ;
    }

  return sRef_undefined;
}

static bool checkDefined (/*@temp@*/ sRef sr)
{
  /*@-temptrans@*/ /* the result from whatUndefined is lost */
  return (sRef_isInvalid (whatUndefined (sr, 0)));
  /*@=temptrans@*/ 
}

bool sRef_isReallyDefined (sRef s)
{
  if (sRef_isReasonable (s))
    {
      if (sRef_isAnyDefined (s))
	{
	  return TRUE;
	}
      else
	{
	  if (sRef_isAllocated (s) || sRef_isPdefined (s))
	    {
	      return checkDefined (s);
	    }
	  else
	    {
	      return FALSE;
	    }
	}
    }
  else
    {
      return TRUE;
    }
}

void sRef_showNotReallyDefined (sRef s)
{
  if (sRef_isReasonable (s))
    {
      if (sRef_isAnyDefined (s))
	{
	  BADBRANCH;
	}
      else
	{
	  if (sRef_isAllocated (s) || sRef_isPdefined (s))
	    {
	      /*@-temptrans@*/ /* the result of whatUndefined is lost */
	      sRef ref = whatUndefined (s, 0);

	      llassert (sRef_isReasonable (ref));

	      if (ref != s)
		{
		  llgenindentmsgnoloc
		    (message ("This sub-reference is %s: %q",
			      sstate_unparse (sRef_getDefState (ref)),
			      sRef_unparse (ref)));
		}
	    }
	  else
	    {
	      ;
	    }
	}
    }
  else
    {
      BADBRANCH;
    }
}

sstate sRef_getDefState (sRef s)
{
  if (sRef_isInvalid (s)) return (SS_UNKNOWN);
  return (s->defstate);
}

void sRef_setDefState (sRef s, sstate defstate, fileloc loc)
{
  sRef_checkMutable (s);  
  sRef_setStateAux (s, defstate, loc);
}

static void sRef_clearAliasStateAux (sRef s, fileloc loc)
{
  sRef_checkMutable (s);  
  sRef_setAliasKind (s, AK_ERROR, loc);
}

void sRef_clearAliasState (sRef s, fileloc loc)
{
  sRef_checkMutable (s);  
  sRef_aliasSetComplete (sRef_clearAliasStateAux, s, loc);
}

void sRef_setAliasKindComplete (sRef s, alkind kind, fileloc loc)
{
  sRef_checkMutable (s);  
  sRef_aliasSetCompleteAlkParam (sRef_setAliasKind, s, kind, loc); 
}

void sRef_setAliasKind (sRef s, alkind kind, fileloc loc)
{
  sRef_checkMutable (s);  

  if (sRef_isReasonable (s))
    {
      sRef_clearDerived (s);

      if ((kind != s->aliaskind && kind != s->oaliaskind)
	  && fileloc_isDefined (loc))
	{
	  s->aliasinfo = stateInfo_updateLoc (s->aliasinfo, stateAction_fromAlkind (kind), loc);
	}
      
      s->aliaskind = kind;
    }
}

void sRef_setOrigAliasKind (sRef s, alkind kind)
{
  sRef_checkMutable (s);  

  if (sRef_isReasonable (s))
    {
      s->oaliaskind = kind;
    }
}

exkind sRef_getExKind (sRef s)
{
  if (sRef_isReasonable (s))
    {
      return (s->expkind);
    }
  else
    {
      return XO_UNKNOWN;
    }
}

exkind sRef_getOrigExKind (sRef s)
{
  if (sRef_isReasonable (s))
    {
      return (s->oexpkind);
    }
  else
    {
      return XO_UNKNOWN;
    }
}

static void sRef_clearExKindAux (sRef s, fileloc loc)
{
  sRef_checkMutable (s);  
  sRef_setExKind (s, XO_UNKNOWN, loc);
}

void sRef_setObserver (sRef s, fileloc loc) 
{
  sRef_checkMutable (s);  
  sRef_setExKind (s, XO_OBSERVER, loc);
}

void sRef_setExposed (sRef s, fileloc loc) 
{
  sRef_checkMutable (s);  
  sRef_setExKind (s, XO_EXPOSED, loc);
}

void sRef_clearExKindComplete (sRef s, fileloc loc)
{
  (void) sRef_aliasSetComplete (sRef_clearExKindAux, s, loc);
}

void sRef_setExKind (sRef s, exkind exp, fileloc loc)
{
  sRef_checkMutable (s);

  if (sRef_isReasonable (s))
    {
      if (s->expkind != exp)
	{
	  s->expinfo = stateInfo_updateLoc (s->expinfo, stateAction_fromExkind (exp), loc);
	}
      
      s->expkind = exp;
    }
}

/*
** s1->derived = s2->derived
*/

static void sRef_copyRealDerived (sRef s1, sRef s2)
{
  DPRINTF (("Copy real: %s / %s", sRef_unparse (s1), sRef_unparse (s2)));
  sRef_checkMutable (s1);

  if (sRef_isReasonable (s1) && sRef_isReasonable (s2))
    {
      sRef sb = sRef_getRootBase (s1);

      sRefSet_clear (s1->deriv);

      sRefSet_allElements (s2->deriv, el)
	{
	  if (sRef_isReasonable (el))
	    {
	      sRef rb = sRef_getRootBase (el);
	      
	      if (!sRef_same (rb, sb))
		{
		  sRef fb = sRef_fixDirectBase (el, s1);
		  
		  if (sRef_isReasonable (fb))
		    {
		      sRef_copyRealDerived (fb, el);
		      sRef_addDeriv (s1, fb);
		    }
		}
	      else
		{
		  sRef_addDeriv (s1, el);
		}
	    }
	} end_sRefSet_allElements ;
    }
  
  }

void sRef_copyRealDerivedComplete (sRef s1, sRef s2)
{
  sRef_innerAliasSetCompleteParam (sRef_copyRealDerived, s1, s2);
}

void sRef_setUndefined (sRef s, fileloc loc)
{
  sRef_checkMutable (s);

  if (sRef_isReasonable (s))
    {
      s->defstate = SS_UNDEFINED;
      s->definfo = stateInfo_updateLoc (s->definfo, SA_UNDEFINED, loc);

      sRef_clearDerived (s);
    }
}

static void sRef_setDefinedAux (sRef s, fileloc loc, bool clear)
{
  sRef_checkMutable (s);
  if (sRef_isInvalid (s)) return;

  DPRINTF (("Set defined: %s", sRef_unparseFull (s)));

  s->definfo = stateInfo_updateLoc (s->definfo, SA_DEFINED, loc);  
  s->defstate = SS_DEFINED;
  
  DPRINTF (("Set defined: %s", sRef_unparseFull (s)));

  /* e.g., if x is allocated, *x = 3 defines x */
  
  if (s->kind == SK_PTR)
    {
      sRef p = s->info->ref;
      sRef arr;

      if (p->defstate == SS_ALLOCATED
	  || p->defstate == SS_SPECIAL) /* evans 2001-07-12: shouldn't need this */
	{
	  sRef_setDefinedAux (p, loc, clear);
	}

      /* 
      ** Defines a[0] also:
      */

      arr = sRef_findDerivedArrayFetch (p, FALSE, 0, FALSE);

      if (sRef_isReasonable (arr))
	{
	  sRef_setDefinedAux (arr, loc, clear);
	}
    }
  else if (s->kind == SK_ARRAYFETCH) 
    {
      if (!s->info->arrayfetch->indknown
	  || (s->info->arrayfetch->ind == 0))
	{
	  sRef p = s->info->arrayfetch->arr;
	  sRef ptr = sRef_constructPointer (p);

	  if (sRef_isReasonable (ptr))
	    {
	      if (ptr->defstate == SS_ALLOCATED 
		  || ptr->defstate == SS_UNDEFINED
		  || ptr->defstate == SS_SPECIAL) /* evans 2001-07-12: shouldn't need this */
		{
		  sRef_setDefinedAux (ptr, loc, clear);
		}
	    }
	  
	  if (p->defstate == SS_RELDEF) 
	    {
	      ;
	    }
	  else if (p->defstate == SS_ALLOCATED || p->defstate == SS_PDEFINED
		   || p->defstate == SS_SPECIAL) /* evans 2001-07-12: shouldn't need this */
	    {
	      p->defstate = SS_DEFINED;
	    }
	  else
	    {
	    }
	}
    }
  else if (s->kind == SK_FIELD)
    {
      sRef parent = s->info->field->rec;
      
      if (sRef_isReasonable (parent))
	{
	  if (ctype_isUnion (ctype_realType (parent->type)))
	    {
	      /*
	      ** Should not clear derived from here.
	      */
	      
	      sRef_setDefinedNoClear (parent, loc);
	    }
	  else
	    {
	      ; /* Nothing to do for structures. */
	    }
	}

          }
  else
    {
      ;
    }

  if (clear)
    {
      sRef_clearDerived (s);
    } 
  else
    {
      /* evans 2001-07-12: need to define the derived references */
      sRefSet_elements (s->deriv, el)
	{
	  llassert (sRef_isReasonable (el));
	  el->defstate = SS_DEFINED;
	} end_sRefSet_elements ;
    }
  
  DPRINTF (("Set defined: %s", sRef_unparseFull (s)));
}

static void sRef_setPartialDefined (sRef s, fileloc loc)
{
  sRef_checkMutable (s);

  if (!sRef_isPartial (s))
    {
      sRef_setDefined (s, loc);
    }
}

void sRef_setPartialDefinedComplete (sRef s, fileloc loc)
{
  sRef_innerAliasSetComplete (sRef_setPartialDefined, s, loc);
}

void sRef_setDefinedComplete (sRef s, fileloc loc)
{
  sRef_innerAliasSetComplete (sRef_setDefined, s, loc);
}

void sRef_setDefinedCompleteDirect (sRef s, fileloc loc)
{
  sRefSet aliases;
  
  aliases = usymtab_allAliases (s);
  DPRINTF (("Set defined complete: %s", sRef_unparseFull (s)));
  DPRINTF (("All aliases: %s", sRefSet_unparseFull (aliases)));
  
  sRef_setDefined (s, loc);

  sRefSet_realElements (aliases, current)
    {
      if (sRef_isReasonable (current))
	{
	  current = sRef_updateSref (current);
	  sRef_setDefined (current, loc);
	}
    } end_sRefSet_realElements;
  
  sRefSet_free (aliases);
  sRef_innerAliasSetComplete (sRef_setDefined, s, loc);
}

void sRef_setDefined (sRef s, fileloc loc)
{
  sRef_checkMutable (s);
  sRef_setDefinedAux (s, loc, TRUE);
}

static void sRef_setDefinedNoClear (sRef s, fileloc loc)
{
  sRef_checkMutable (s);
  DPRINTF (("Defining: %s", sRef_unparseFull (s)));
  sRef_setDefinedAux (s, loc, FALSE);
  DPRINTF (("==> %s", sRef_unparseFull (s)));
}

void sRef_setDefinedNCComplete (sRef s, fileloc loc)
{
  sRef_checkMutable (s);
  DPRINTF (("Set Defined Complete: %s", sRef_unparseFull (s)));
  sRef_innerAliasSetComplete (sRef_setDefinedNoClear, s, loc);
  DPRINTF (("==> %s", sRef_unparseFull (s)));
}

static bool sRef_isDeepUnionField (sRef s)
{
  return (sRef_deepPred (sRef_isUnionField, s));
}

bool sRef_isUnionField (sRef s)
{
  if (sRef_isReasonable (s) && s->kind == SK_FIELD)
    {
      /*
       ** defining one field of a union defines the union
       */
      
      sRef base = s->info->field->rec;

      if (sRef_isReasonable (base))
	{
	  return (ctype_isUnion (ctype_realType (base->type)));
	}
    }

  return FALSE;
}

void sRef_setPdefined (sRef s, fileloc loc)
{
  sRef_checkMutable (s);
  if (sRef_isReasonable (s) && !sRef_isPartial (s))
    {
      sRef base = sRef_getBaseSafe (s);

      if (s->defstate == SS_ALLOCATED)
	{
	  return;
	}
      
      s->definfo = stateInfo_updateLoc (s->definfo, SA_PDEFINED, loc);
      s->defstate = SS_PDEFINED;
      
      /* e.g., if x is allocated, *x = 3 defines x */
      
      while (sRef_isReasonable (base) && sRef_isKnown (base))
	{
	  if (base->defstate == SS_DEFINED)
	    { 
	      sRef nb;
	      
	      DPRINTF (("set pdefined: %s", sRef_unparseFull (base)));
	      base->defstate = SS_PDEFINED; 
	      nb = sRef_getBaseSafe (base); 
	      base = nb;
	    }
	  else 
	    { 
	      break; 
	    }
	}      
    }
}

static void sRef_setStateAux (sRef s, sstate ss, fileloc loc)
{
  sRef_checkMutable (s);

  DPRINTF (("Set state: %s => %s", sRef_unparseFull (s), sstate_unparse (ss)));

  if (sRef_isReasonable (s))
    {
      /* if (s->defstate == SS_RELDEF) return; */

      if (s->defstate != ss && fileloc_isDefined (loc))
	{
	  s->definfo = stateInfo_updateLoc (s->definfo, 
					    stateAction_fromSState (ss), loc);
	}

      s->defstate = ss;
      sRef_clearDerived (s); 

      if (ss == SS_ALLOCATED)
	{
	  sRef base = sRef_getBaseSafe (s);
	  
	  while (sRef_isReasonable (base) && sRef_isKnown (base))
	    {
	      if (base->defstate == SS_DEFINED) 
		{ 
		  sRef nb;

		  DPRINTF (("set pdefined: %s", sRef_unparseFull (s)));		  
		  base->defstate = SS_PDEFINED; 
		  nb = sRef_getBaseSafe (base); 
		  base = nb;
		}
	      else 
		{ 
		  break; 
		}
	    }
	}
    }
}

void sRef_setAllocatedComplete (sRef s, fileloc loc)
{
  sRef_innerAliasSetComplete (sRef_setAllocated, s, loc);
}

static void sRef_setAllocatedShallow (sRef s, fileloc loc)
{
  sRef_checkMutable (s);

  if (sRef_isReasonable (s))
    {
      if (s->defstate == SS_DEAD || s->defstate == SS_UNDEFINED)
	{
	  s->defstate = SS_ALLOCATED;
	  s->definfo = stateInfo_updateLoc (s->definfo, SA_ALLOCATED, loc);
	}
    }
}

void sRef_setAllocatedShallowComplete (sRef s, fileloc loc)
{
  sRef_innerAliasSetComplete (sRef_setAllocatedShallow, s, loc);
}

void sRef_setAllocated (sRef s, fileloc loc)
{
  sRef_checkMutable (s);
  sRef_setStateAux (s, SS_ALLOCATED, loc);
}

void sRef_setPartial (sRef s, fileloc loc)
{
  sRef_checkMutable (s);
  sRef_setStateAux (s, SS_PARTIAL, loc);
}

void sRef_setShared (sRef s, fileloc loc)
{
  sRef_checkMutable (s);

  if (sRef_isReasonable (s))
    {
      if (s->aliaskind != AK_SHARED && fileloc_isDefined (loc))
	{
	  s->aliasinfo = stateInfo_updateLoc (s->aliasinfo, SA_SHARED, loc);
	}

      s->aliaskind = AK_SHARED;
      /* don't! sRef_clearDerived (s); */
    }
}

void sRef_setLastReference (sRef s, /*@exposed@*/ sRef ref, fileloc loc)
{
  sRef_checkMutable (s);

  if (sRef_isReasonable (s))
    {
      s->aliaskind = sRef_getAliasKind (ref);
      s->aliasinfo = stateInfo_updateRefLoc (s->aliasinfo, ref, stateAction_fromAlkind (s->aliaskind), loc);
    }
}

static
void sRef_setNullStateAux (/*@notnull@*/ sRef s, nstate ns, fileloc loc)
{
  sRef_checkMutable (s);
  s->nullstate = ns;
  sRef_resetAliasKind (s);

  if (fileloc_isDefined (loc))
    {
      s->nullinfo = stateInfo_updateLoc (s->nullinfo, stateAction_fromNState (ns), loc);
    }
}

void sRef_setNotNull (sRef s, fileloc loc)
{
  if (sRef_isReasonable (s))
    {
      sRef_setNullStateAux (s, NS_NOTNULL, loc);
    }
}

void sRef_setNullStateN (sRef s, nstate n)
{
  if (sRef_isReasonable (s))
    {
      sRef_checkMutable (s);
      s->nullstate = n;
      DPRINTF (("Set null state ==> %s", sRef_unparseFull (s)));
      sRef_resetAliasKind (s);
    }
}

void sRef_setNullState (sRef s, nstate n, fileloc loc)
{
  if (sRef_isReasonable (s))
    {
      sRef_setNullStateAux (s, n, loc);
    }
}

void sRef_setNullTerminatedStateInnerComplete (sRef s, struct s_bbufinfo b, /*@unused@*/ fileloc loc) 
{
  switch (b.bufstate) {
  case BB_NULLTERMINATED:
    sRef_setNullTerminatedState (s);
    sRef_setLen (s, b.len);
    break;
  case BB_POSSIBLYNULLTERMINATED:
    sRef_setPossiblyNullTerminatedState(s);
    break;
  case BB_NOTNULLTERMINATED:
    sRef_setNotNullTerminatedState (s);
    break;
  }

  sRef_setSize (s, b.size);
  
  /* PL: TO BE DONE : Aliases are not modified right now, have to be similar to
   * setNullStateInnerComplete.
   */
}

void sRef_setNullStateInnerComplete (sRef s, nstate n, fileloc loc)
{
  DPRINTF (("Set null state: %s", nstate_unparse (n)));
  
  sRef_setNullState (s, n, loc);
  
  switch (n)
    {
    case NS_POSNULL:
      sRef_innerAliasSetComplete (sRef_setPosNull, s, loc);
      break;
    case NS_DEFNULL:
      sRef_innerAliasSetComplete (sRef_setDefNull, s, loc);
      break;
    case NS_UNKNOWN:
      sRef_innerAliasSetComplete (sRef_setNullUnknown, s, loc);
      break;
    case NS_NOTNULL:
      sRef_innerAliasSetComplete (sRef_setNotNull, s, loc);
      break;
    case NS_MNOTNULL:
      sRef_innerAliasSetComplete (sRef_setNotNull, s, loc);
      break;
    case NS_RELNULL:
      sRef_innerAliasSetComplete (sRef_setNullUnknown, s, loc);
      break;
    case NS_CONSTNULL:
      sRef_innerAliasSetComplete (sRef_setDefNull, s, loc);
      break;
    case NS_ABSNULL:
      sRef_innerAliasSetComplete (sRef_setNullUnknown, s, loc);
      break;
    case NS_ERROR:
      sRef_innerAliasSetComplete (sRef_setNullErrorLoc, s, loc);
      break;
    }
}

void sRef_setPosNull (sRef s, fileloc loc)
{
  if (sRef_isReasonable (s))
    {
      sRef_setNullStateAux (s, NS_POSNULL, loc);
    }
}
  
void sRef_setDefNull (sRef s, fileloc loc)
{
  if (sRef_isReasonable (s))
    {
      sRef_setNullStateAux (s, NS_DEFNULL, loc);
    }
}

void sRef_setNullUnknown (sRef s, fileloc loc)
{
  if (sRef_isReasonable (s))
    {
      sRef_setNullStateAux (s, NS_UNKNOWN, loc);
    }
}

void sRef_setNullError (sRef s)
{
  if (sRef_isReasonable (s) && !sRef_isConst (s))
    {
      sRef_setNullStateAux (s, NS_UNKNOWN, fileloc_undefined);
    }
}

void sRef_setNullErrorLoc (sRef s, fileloc loc)
{
  if (sRef_isReasonable (s) && !sRef_isConst (s))
    {
      sRef_setNullStateAux (s, NS_UNKNOWN, loc);
    }
}

void sRef_setOnly (sRef s, fileloc loc)
{
  if (sRef_isReasonable (s) && s->aliaskind != AK_ONLY)
    {
      sRef_checkMutable (s);
      s->aliaskind = AK_ONLY;
      s->aliasinfo = stateInfo_updateLoc (s->aliasinfo, SA_ONLY, loc);
    }
}

void sRef_setDependent (sRef s, fileloc loc)
{
  if (sRef_isReasonable (s) && !sRef_isConst (s) && (s->aliaskind != AK_DEPENDENT))
    {
      sRef_checkMutable (s);
      DPRINTF (("Setting dependent: %s", sRef_unparseFull (s)));
      s->aliaskind = AK_DEPENDENT;
      s->aliasinfo = stateInfo_updateLoc (s->aliasinfo, SA_DEPENDENT, loc);
    }
}

void sRef_setOwned (sRef s, fileloc loc)
{
  if (sRef_isReasonable (s) && !sRef_isConst (s) && (s->aliaskind != AK_OWNED))
    {
      sRef_checkMutable (s);
      s->aliaskind = AK_OWNED;
      s->aliasinfo = stateInfo_updateLoc (s->aliasinfo, SA_OWNED, loc);
    }
}

void sRef_setKept (sRef s, fileloc loc)
{
  if (sRef_isReasonable (s) && !sRef_isConst (s) && (s->aliaskind != AK_KEPT))
    {
      sRef base = sRef_getBaseSafe (s);  
 
      while (sRef_isReasonable (base) && sRef_isKnown (base))
	{
	  if (base->defstate == SS_DEFINED) 
	    {
	      sRef_checkMutable (base);
	      base->defstate = SS_PDEFINED; 
	      base = sRef_getBaseSafe (base); 
	    }
	  else 
	    {
	      break; 
	    }
	}

      sRef_checkMutable (s);
      s->aliaskind = AK_KEPT;
      s->aliasinfo = stateInfo_updateLoc (s->aliasinfo, SA_KEPT, loc);
    }
}

static void sRef_setKeptAux (sRef s, fileloc loc)
{
  if (!sRef_isShared (s))
    {
      sRef_setKept (s, loc);
    }
}

static void sRef_setDependentAux (sRef s, fileloc loc)
{
  if (!sRef_isShared (s))
    {
      sRef_setDependent (s, loc);
    }
}

void sRef_setKeptComplete (sRef s, fileloc loc)
{
  sRef_aliasSetComplete (sRef_setKeptAux, s, loc);
}

void sRef_setDependentComplete (sRef s, fileloc loc)
{
  sRef_aliasSetComplete (sRef_setDependentAux, s, loc);
}

void sRef_setFresh (sRef s, fileloc loc)
{
  if (sRef_isReasonable (s) && !sRef_isConst (s))
    {
      sRef_checkMutable (s);
      s->aliaskind = AK_FRESH;
      s->aliasinfo = stateInfo_updateLoc (s->aliasinfo, SA_CREATED, loc);
      DPRINTF (("SetFresh: %s", sRef_unparseFull (s)));
    }
}

void sRef_kill (sRef s, fileloc loc)
{
  DPRINTF (("Kill: %s", sRef_unparseFull (s)));

  if (sRef_isReasonable (s) && !sRef_isShared (s) && !sRef_isConst (s))
    {
      sRef base = sRef_getBaseSafe (s);  
      sRef_checkMutable (s);
	
      while (sRef_isReasonable (base) && sRef_isKnown (base))
	{
	  if (base->defstate == SS_DEFINED) 
	    {
	      sRef_checkMutable (base);
	      base->defstate = SS_PDEFINED; 
	      base = sRef_getBaseSafe (base); 
	    }
	  else 
	    {
	      break; 
	    }
	}
      
      s->aliaskind = s->oaliaskind;
      s->defstate = SS_DEAD;
      s->definfo = stateInfo_updateLoc (s->definfo, SA_KILLED, loc);
      DPRINTF (("State info: %s", stateInfo_unparse (s->definfo)));
      sRef_clearDerived (s);
    }
}

void sRef_maybeKill (sRef s, fileloc loc)
{
  if (sRef_isReasonable (s))
    {
      sRef base = sRef_getBaseSafe (s);  
      sRef_checkMutable (s);
            
      while (sRef_isReasonable (base) && sRef_isKnown (base))
	{
	  if (base->defstate == SS_DEFINED || base->defstate == SS_RELDEF)
	    {
	      sRef_checkMutable (base);
	      base->defstate = SS_PDEFINED; 
	      base = sRef_getBaseSafe (base); 
	    }
	  else 
	    {
	      break; 
	    }
	  
	}
      
      s->aliaskind = s->oaliaskind;
      s->defstate = SS_HOFFA; 
      s->definfo = stateInfo_updateLoc (s->definfo, SA_PKILLED, loc);
      DPRINTF (("State info: %s / %s", sRef_unparse (s), 
		stateInfo_unparse (s->definfo)));
      sRef_clearDerived (s); 
    }

  }

/*
** just for type checking...
*/

static void sRef_killAux (sRef s, fileloc loc)
{
  if (sRef_isReasonable (s) && !sRef_isShared (s))
    {
      if (sRef_isUnknownArrayFetch (s))
	{
	  sRef_maybeKill (s, loc);
	}
      else
	{
	  sRef_kill (s, loc);
	}
    }
}

/*
** kills s and all aliases to s
*/

void sRef_killComplete (sRef s, fileloc loc)
{
  DPRINTF (("Kill complete: %s", sRef_unparseFull (s)));
  sRef_aliasSetComplete (sRef_killAux, s, loc);
}

static bool sRef_equivalent (sRef s1, sRef s2)
{
  return (sRef_compare (s1, s2) == 0);
}

/*
** returns an sRef that will not be free'd on function exit.
*/

/*@only@*/ sRef sRef_saveCopy (sRef s)
{
  sRef ret;

  if (sRef_isReasonable (s))
    {
      bool old = inFunction;

      /*
      ** Exit the function scope, so this sRef is not
      ** stored in the deallocation table.
      */
      
      inFunction = FALSE;
      DPRINTF (("Copying sref: %s", sRef_unparseFull(s)));
      ret = sRef_copy (s);
      DPRINTF (("Copying ===>: %s", sRef_unparseFull(ret)));
      inFunction = old;
    }
  else
    {
      ret = sRef_undefined;
    }

  /*@-dependenttrans@*/ 
  return ret;
  /*@=dependenttrans@*/ 
}

sRef sRef_copy (sRef s)
{
  if (sRef_isKindSpecial (s) && !sRef_isGlobalMarker (s))
    {
      /*@-retalias@*/
      return s; /* don't copy specials (except for global markers) */
      /*@=retalias@*/
    }

  if (sRef_isReasonable (s))
    {
      sRef t = sRef_alloc ();

      DPRINTF (("Copying: [%p] %s", s, sRef_unparse (s)));
      DPRINTF (("Full: %s", sRef_unparseFull (s)));

      t->kind = s->kind;
      t->safe = s->safe;
      t->modified = s->modified;
      t->immut = FALSE; /* Note mutability is not copied. */
      t->type = s->type;
      t->val = multiVal_copy (s->val);

      t->info = sinfo_copy (s);
      t->defstate = s->defstate;
      t->nullstate = s->nullstate;
      DPRINTF (("Set null state==> %s", sRef_unparseFull (t)));

 
      /* start modifications */
      t->bufinfo.bufstate = s->bufinfo.bufstate;
      t->bufinfo.len = s->bufinfo.len;
      t->bufinfo.size = s->bufinfo.size;
      /* end modifications */

      t->aliaskind = s->aliaskind;
      t->oaliaskind = s->oaliaskind;

      t->expkind = s->expkind;
      t->oexpkind = s->oexpkind;

      t->nullinfo = stateInfo_copy (s->nullinfo);
      t->aliasinfo = stateInfo_copy (s->aliasinfo);
      t->definfo = stateInfo_copy (s->definfo);
      t->expinfo = stateInfo_copy (s->expinfo);

      t->deriv = sRefSet_newDeepCopy (s->deriv);
      t->state = valueTable_copy (s->state);
      
      DPRINTF (("Made copy: %s => %s", sRef_unparseFull (s), sRef_unparseFull (t)));
      return t;
    }
  else
    {
      return sRef_undefined;
    }
}

/*@notfunction@*/
# define PREDTEST(func,s) \
   do { if (sRef_isInvalid (s)) { return FALSE; } \
        else { if (sRef_isConj (s)) \
                  { return (func (sRef_getConjA (s)) \
		            || func (sRef_getConjB (s))); }}} while (FALSE);

bool sRef_isAddress (sRef s)
{
  PREDTEST (sRef_isAddress, s);
  return (s->kind == SK_ADR);
}
	  
/*
** pretty weak... maybe a flag should control this.
*/

bool sRef_isThroughArrayFetch (sRef s)
{
  if (sRef_isReasonable (s))
    {
      sRef tref = s;

      do 
	{
	  sRef lt;

	  if (sRef_isArrayFetch (tref)) 
	    {
	      return TRUE;
	    }
	  
	  lt = sRef_getBase (tref);
	  tref = lt;
	} while (sRef_isReasonable (tref));
    } 

  return FALSE;
}

bool sRef_isArrayFetch (sRef s)
{
  PREDTEST (sRef_isArrayFetch, s);
  return (s->kind == SK_ARRAYFETCH);
}

bool sRef_isMacroParamRef (sRef s)
{
  if (context_inMacro () && sRef_isCvar (s))
    {
      uentry ue = sRef_getUentry (s);
      cstring pname = makeParam (uentry_rawName (ue));
      uentry mac = usymtab_lookupSafe (pname);

      cstring_free (pname);
      return (uentry_isValid (mac));
    }

  return FALSE;
}
      
bool sRef_isCvar (sRef s) 
{
  PREDTEST (sRef_isCvar, s);
  return (s->kind == SK_CVAR);
}

bool sRef_isConst (sRef s) 
{
  PREDTEST (sRef_isConst, s);
  return (s->kind == SK_CONST);
}

bool sRef_isObject (sRef s) 
{
  PREDTEST (sRef_isObject, s);
  return (s->kind == SK_OBJECT);
}

bool sRef_isExternal (sRef s) 
{
  PREDTEST (sRef_isExternal, s);
  return (s->kind == SK_EXTERNAL);
}

static bool sRef_isDerived (sRef s) 
{
  PREDTEST (sRef_isDerived, s);
  return (s->kind == SK_DERIVED);
}

bool sRef_isField (sRef s)
{
  PREDTEST (sRef_isField, s);
  return (s->kind == SK_FIELD);
}

static bool sRef_isIndex (sRef s)
{
  PREDTEST (sRef_isIndex, s);
  return (s->kind == SK_ARRAYFETCH);
}

bool sRef_isAnyParam (sRef s)
{
  PREDTEST (sRef_isAnyParam, s);
  return (s->kind == SK_PARAM);  
}

bool sRef_isParam (sRef s)
{
  PREDTEST (sRef_isParam, s);
  return (s->kind == SK_PARAM);
}

bool sRef_isDirectParam (sRef s)
{
  PREDTEST (sRef_isDirectParam, s);

  return ((s->kind == SK_CVAR) &&
	  (s->info->cvar->lexlevel == functionScope) &&
	  (context_inFunction () && 
	   (s->info->cvar->index <= usymId_fromInt (uentryList_size (context_getParams ())))));
}

bool sRef_isPointer (sRef s)
{
  PREDTEST (sRef_isPointer, s);
  return (s->kind == SK_PTR);
}

/*
** returns true if storage referenced by s is visible
*/

bool sRef_isReference (sRef s)
{
  PREDTEST (sRef_isReference, s);

  return (sRef_isPointer (s) || sRef_isIndex (s) || sRef_isFileOrGlobalScope (s)
	  || (sRef_isField (s) && (sRef_isReference (s->info->field->rec))));
}

bool sRef_isIReference (sRef s)
{
  return (sRef_isPointer (s) || sRef_isAddress (s) || sRef_isIndex (s)
	  || sRef_isField (s) || sRef_isArrayFetch (s));
}

bool sRef_isFileOrGlobalScope (sRef s)
{
  return (sRef_isCvar (s) && (s->info->cvar->lexlevel <= fileScope));
}

bool sRef_isRealGlobal (sRef s)
{
  return (sRef_isCvar (s) && (s->info->cvar->lexlevel == globScope));
}

bool sRef_isFileStatic (sRef s)
{
  return (sRef_isCvar (s) && (s->info->cvar->lexlevel == fileScope));
}

bool sRef_isAliasCheckedGlobal (sRef s)
{
  if (sRef_isFileOrGlobalScope (s))
    {
      uentry ue = sRef_getUentry (s);

      return context_checkAliasGlob (ue);
    }
  else
    {
      return FALSE;
    }
}

void sRef_free (/*@only@*/ sRef s)
{
  if (s != sRef_undefined && s->kind != SK_SPECIAL)
    {
      DPRINTF (("Free sref: [%p]", s));

      sRef_checkValid (s);
      
      multiVal_free (s->val); /* evans 2002-07-12 */

      stateInfo_free (s->expinfo);
      stateInfo_free (s->aliasinfo);
      stateInfo_free (s->definfo);
      stateInfo_free (s->nullinfo);

      sRefSet_free (s->deriv);
      s->deriv = sRefSet_undefined;

      valueTable_free (s->state); 
      sinfo_free (s);
      
      
      /* drl added to help locate use after release*/
      s->expinfo = stateInfo_undefined;
      s->aliasinfo = stateInfo_undefined;
      s->definfo = stateInfo_undefined;
      s->nullinfo = stateInfo_undefined;

      sfree (s);
    }
}

void sRef_setType (sRef s, ctype t)
{
  if (sRef_isReasonable (s))
    {
      sRef_checkMutable (s); 
      s->type = t;
    }
}

void sRef_setTypeFull (sRef s, ctype t)
{
  if (sRef_isReasonable (s))
    {
      sRef_checkMutable (s);
      s->type = t;

      sRefSet_allElements (s->deriv, current)
	{
	  sRef_setTypeFull (current, ctype_unknown);
	} end_sRefSet_allElements ;
    }
}

/*@exposed@*/ sRef
  sRef_buildField (/*@exposed@*/ sRef rec, /*@dependent@*/ cstring f)
{
  return (sRef_buildNCField (rec, f)); 
}

static /*@exposed@*/ sRef
sRef_findDerivedField (/*@notnull@*/ sRef rec, cstring f)
{
  sRefSet_allElements (sRef_derivedFields (rec), sr)
    {
      if (sRef_isReasonable (sr))
	{
	  if (sRef_isReasonable (sr))
	    {
	      if (sr->info != NULL) 
		{
		  if (sr->kind == SK_FIELD && cstring_equal (sr->info->field->field, f))
		    {
		      return sr;
		    }
		}
	    }
	  else
	    {
	      llcontbug (message ("Invalid sRef as derived field of %s", sRef_unparse (rec)));
	    }
	}
    } end_sRefSet_allElements;

  return sRef_undefined;
}

/*@dependent@*/ /*@observer@*/ sRefSet sRef_derivedFields (/*@temp@*/ sRef rec)
{
  if (sRef_isReasonable (rec))
    {
      sRefSet ret;
      ret = rec->deriv;
      return (ret);
    }
  else
    {
      return (sRefSet_undefined);
    }
}

static /*@exposed@*/ sRef
  sRef_findDerivedPointer (sRef s)
{
  if (sRef_isReasonable (s))
    {
      sRefSet_realElements (s->deriv, sr)
	{
	  if (sRef_isReasonable (sr) && sr->kind == SK_PTR)
	    {
	      return sr;
	    }
	} end_sRefSet_realElements;
    }

  return sRef_undefined;
}

bool
sRef_isUnknownArrayFetch (sRef s)
{
  return (sRef_isReasonable (s) 
	  && s->kind == SK_ARRAYFETCH
	  && !s->info->arrayfetch->indknown);
}

static /*@exposed@*/ sRef
sRef_findDerivedArrayFetch (/*@notnull@*/ sRef s, bool isknown, int idx, bool dead)
{
  
  if (isknown) 
    {
      sRefSet_realElements (s->deriv, sr)
	{
	  if (sRef_isReasonable (sr)
	      && sr->kind == SK_ARRAYFETCH
	      && sr->info->arrayfetch->indknown
	      && (sr->info->arrayfetch->ind == idx))
	    {
	      return sr;
	    }
	} end_sRefSet_realElements;
    }
  else
    {
      sRefSet_realElements (s->deriv, sr)
	{
	  if (sRef_isReasonable (sr)
	      && sr->kind == SK_ARRAYFETCH
	      && (!sr->info->arrayfetch->indknown
		  || (sr->info->arrayfetch->indknown && 
		      sr->info->arrayfetch->ind == 0)))
	    {
	      if (sRef_isDead (sr) || sRef_isKept (sr))
		{
		  if (dead || context_getFlag (FLG_STRICTUSERELEASED))
		    {
		      return sr;
		    }
		}
	      else
		{
		  return sr;
		}
	    }
	} end_sRefSet_realElements;
    }

  return sRef_undefined;
}

static /*@exposed@*/ sRef 
sRef_buildNCField (/*@exposed@*/ sRef rec, /*@exposed@*/ cstring f)
{
  sRef s;

  DPRINTF (("Build nc field: %s / %s",
	    sRef_unparseFull (rec), f));

  if (sRef_isInvalid (rec))
    {
      return sRef_undefined;
    }
      
  /*
  ** check if the field already has been referenced 
  */

  s = sRef_findDerivedField (rec, f);
  
  if (sRef_isReasonable (s))
    {
      return s;
    }
  else
    {
      ctype ct = ctype_realType (rec->type);
      
      DPRINTF (("Field of: %s", sRef_unparse (rec)));
      
      s = sRef_newRef ();      
      s->kind = SK_FIELD;
      s->info = (sinfo) dmalloc (sizeof (*s->info));
      s->info->field = (fldinfo) dmalloc (sizeof (*s->info->field));
      s->info->field->rec = rec;
      s->info->field->field = f; /* doesn't copy f */
      
      if (ctype_isKnown (ct) && ctype_isSU (ct))
	{
	  uentry ue = uentryList_lookupField (ctype_getFields (ct), f);
	
	  if (!uentry_isUndefined (ue))
	    {
	      DPRINTF (("lookup: %s for %s", uentry_unparseFull (ue),
			ctype_unparse (ct)));
	      
	      s->type = uentry_getType (ue);

	      if (ctype_isMutable (s->type)
		  && rec->aliaskind != AK_STACK 
		  && !alkind_isStatic (rec->aliaskind))
		{
		  s->aliaskind = rec->aliaskind;
		}
	      else
		{
		  s->aliaskind = AK_UNKNOWN;
		}

	      if (sRef_isStateDefined (rec) || sRef_isStateUnknown (rec) 
		  || sRef_isPdefined (rec))
		{
		  sRef_setStateFromUentry (s, ue);
		}
	      else
		{
		  sRef_setPartsFromUentry (s, ue);
		}
	      
	      s->oaliaskind = s->aliaskind;
	      s->oexpkind = s->expkind;

	      DPRINTF (("sref: %s", sRef_unparseFull (s)));
	    }
	  else
	    {
	      /*
		Never report this as an error.  It can happen whenever there
		is casting involved.

	      if (report)
		{
		  llcontbug (message ("buildNCField --- no field %s: %q / %s",
				      f, sRef_unparse (s), ctype_unparse (ct)));
		}
		*/

	      return sRef_undefined;
	    }
	}
      
      if (rec->defstate == SS_DEFINED 
	  && (s->defstate == SS_UNDEFINED || s->defstate == SS_UNKNOWN))
	{
	  s->defstate = SS_DEFINED;
	}
      else if (rec->defstate == SS_PARTIAL)
	{
	  s->defstate = SS_PARTIAL;
	}
      else if (rec->defstate == SS_ALLOCATED) 
	{
	  if (ctype_isStackAllocated (ct) && ctype_isStackAllocated (s->type))
	    {
	      s->defstate = SS_ALLOCATED;
	    }
	  else
	    {
	      s->defstate = SS_UNDEFINED;
	    }
	}
      else if (s->defstate == SS_UNKNOWN)
	{
	  s->defstate = rec->defstate;
	}
      else
	{
	  ; /* no change */
	}

      if (s->defstate == SS_UNDEFINED)
	{
	  ctype rt = ctype_realType (s->type);
	  
	  if (ctype_isArray (rt) || ctype_isSU (rt))
	    {
	      s->defstate = SS_ALLOCATED;
	    }
	}

      sRef_addDeriv (rec, s);
      DPRINTF (("Add deriv: %s", sRef_unparseFull (rec)));

      if (ctype_isInt (s->type) && cstring_equal (f, REFSNAME))
	{
	  s->aliaskind = AK_REFS;
	  s->oaliaskind = AK_REFS;
	}

      DPRINTF (("Build field ==> %s", sRef_unparseFull (s)));
      return s;
    }
}

bool
sRef_isStackAllocated (sRef s)
{
  return (sRef_isReasonable(s) 
	  && s->defstate == SS_ALLOCATED && ctype_isStackAllocated (s->type));
}
	  
static
void sRef_setArrayFetchState (/*@notnull@*/ /*@exposed@*/ sRef s, 
			      /*@notnull@*/ /*@exposed@*/ sRef arr)
{
  sRef_checkMutable (s);

  if (ctype_isRealAP (arr->type))
    {
      s->type = ctype_baseArrayPtr (arr->type);
    }

  /* a hack, methinks... makeArrayFetch (&a[0]) ==> a[] */
  /* evans - 2001-08-27: not sure where this was necessary - it
  ** causes an assertion in in aliasCheckPred to fail.
  */

  if (sRef_isAddress (arr)) 
    {
      sRef t = arr->info->ref;
      
      if (sRef_isArrayFetch (t))
	{
	  s->info->arrayfetch->arr = t->info->arrayfetch->arr;
	}
    }
  else if (ctype_isRealPointer (arr->type))
    {
      sRef sp = sRef_findDerivedPointer (arr);
      
      if (sRef_isReasonable (sp))
	{
	  
	  if (ctype_isMutable (s->type))
	    {
	      s->expkind = sRef_getExKind (sp);
	      s->expinfo = stateInfo_copy (sp->expinfo);
	      
	      s->aliaskind = sp->aliaskind;
	      s->aliasinfo = stateInfo_copy (sp->aliasinfo);
	    }

	  s->defstate = sp->defstate;

	  if (s->defstate == SS_DEFINED) 
	    {
	      if (!context_getFlag (FLG_STRICTDESTROY))
		{
		  s->defstate = SS_PARTIAL;
		}
	    }

	  DPRINTF (("Set null state: %s / %s", sRef_unparseFull (s), sRef_unparseFull (sp)));
	  sRef_setNullStateN (s, sRef_getNullState (sp));
	}
      else
	{
	  if (arr->defstate == SS_UNDEFINED)
	    {
	      s->defstate = SS_UNUSEABLE;
	    }
	  else if ((arr->defstate == SS_ALLOCATED) && !ctype_isSU (s->type))
	    {
	      s->defstate = SS_UNDEFINED;
	    }
	  else
	    {
	      if (!context_getFlag (FLG_STRICTDESTROY))
		{
		  s->defstate = SS_PARTIAL;
		}
	      else
		{
		  s->defstate = SS_DEFINED;
		}

	      /*
	      ** Very weak checking for array elements.
	      ** Was:
	      **     s->defstate = arr->defstate;
	      */
	    }

	  s->expkind = sRef_getExKind (arr);
	  s->expinfo = stateInfo_copy (arr->expinfo);
	  
	  if (arr->aliaskind == AK_LOCAL || arr->aliaskind == AK_FRESH)
	    {
	      s->aliaskind = AK_LOCAL;
	    }
	  else
	    {
	      s->aliaskind = AK_UNKNOWN;
	    }
	  
	  sRef_setTypeState (s);
	}
    }
  else
    {
      if (arr->defstate == SS_DEFINED)
	{
	  /*
	  ** Very weak checking for array elements.
	  ** Was:
	  **     s->defstate = arr->defstate;
	  */

	  if (context_getFlag (FLG_STRICTDESTROY))
	    {
	      s->defstate = SS_DEFINED;
	    }
	  else
	    {
	      s->defstate = SS_PARTIAL;
	    }
	}
      else if (arr->defstate == SS_ALLOCATED)
	{
	  if (ctype_isRealArray (s->type))
	    {
	      s->defstate = SS_ALLOCATED;
	    }
	  else 
	    {
	      if (!s->info->arrayfetch->indknown)
		{
		  /*
		  ** is index is unknown, elements is defined or 
		  ** allocated is any element is!
		  */
		  
		  s->defstate = SS_UNDEFINED;
		  
		  sRefSet_allElements (arr->deriv, sr)
		    {
		      if (sRef_isReasonable (sr))
			{
			  if (sRef_isReasonable (sr))
			    {
			      if (sr->defstate == SS_ALLOCATED)
				{
				  s->defstate = SS_ALLOCATED;
				}
			      else 
				{
				  if (sr->defstate == SS_DEFINED)
				    {
				      if (context_getFlag (FLG_STRICTDESTROY))
					{
					  s->defstate = SS_DEFINED;
					}
				      else
					{
					  s->defstate = SS_PARTIAL;
					}
				      
				      break;
				    }
				}
			    }
			  else
			    {
			      llcontbug (message ("Invalid sRef as derived element of %s", sRef_unparse (arr)));
			    }
			}
		    } end_sRefSet_allElements;
		}
	      else
		{
		  s->defstate = SS_UNDEFINED;
		}
	    }
	}
      else
	{
	  s->defstate = arr->defstate;
	}
      
      
      /*
      ** kludgey way to guess where aliaskind applies
      */
      
      if (ctype_isMutable (s->type) 
	  && !ctype_isPointer (arr->type) 
	  && !alkind_isStatic (arr->aliaskind)
	  && !alkind_isStack (arr->aliaskind)) /* evs - 2000-06-20: don't pass stack allocation to members */
	{
	  s->aliaskind = arr->aliaskind;
	}
      else
	{
	  s->aliaskind = AK_UNKNOWN;
	}
    
      sRef_setTypeState (s);
    }

  if (sRef_isObserver (arr)) 
    {
      s->expkind = XO_OBSERVER;
      s->expinfo = stateInfo_copy (arr->expinfo);
    }
}  

/*@exposed@*/ sRef sRef_buildArrayFetch (/*@exposed@*/ sRef arr)
{
  sRef s;

  if (!sRef_isReasonable (arr)) {
    /*@-nullret@*/ return arr /*@=nullret@*/;
  }

  if (ctype_isRealPointer (arr->type))
    {
      (void) sRef_buildPointer (arr); /* do this to define arr! */
    }
  
  s = sRef_findDerivedArrayFetch (arr, FALSE, 0, FALSE);
  
  if (sRef_isReasonable (s))
    {
      /* evans 2001-07-12: this is bogus, clean-up hack */
      if (s->info->arrayfetch->arr != arr)
	{
	  sRef res;
	  check (sRefSet_delete (arr->deriv, s));
	  res = sRef_buildArrayFetch (arr);
	  sRef_copyState (res, s);
	  llassert (res->info->arrayfetch->arr == arr); 
	  return res;
	}

      s->expkind = sRef_getExKind (arr);
      s->expinfo = stateInfo_copy (arr->expinfo);

      return s;
    }
  else
    {
      s = sRef_newRef ();

      s->kind = SK_ARRAYFETCH;
      s->info = (sinfo) dmalloc (sizeof (*s->info));
      s->info->arrayfetch = (ainfo) dmalloc (sizeof (*s->info->arrayfetch));
      s->info->arrayfetch->indknown = FALSE;
      s->info->arrayfetch->ind = 0;
      s->info->arrayfetch->arr = arr;

      sRef_setArrayFetchState (s, arr);

      s->oaliaskind = s->aliaskind;
      s->oexpkind = s->expkind;

      if (!context_inProtectVars ())
	{
	  sRef_addDeriv (arr, s);
	}
      
      if (valueTable_isUndefined (s->state))
	{
	  s->state = context_createValueTable 
	    (s, stateInfo_makeLoc (g_currentloc, SA_CREATED));
	}
      
      return (s);
    }
}

/*@exposed@*/ sRef
  sRef_buildArrayFetchKnown (/*@exposed@*/ sRef arr, int i)
{
  sRef s;

  if (!sRef_isReasonable (arr)) {
    /*@-nullret@*/ return arr /*@=nullret@*/;
  }

  if (ctype_isRealPointer (arr->type))
    {
      (void) sRef_buildPointer (arr); /* do this to define arr! */
    }

  s = sRef_findDerivedArrayFetch (arr, TRUE, i, FALSE);

  if (sRef_isReasonable (s))
    {
      /* evans 2001-07-12: this is bogus, clean-up hack */
      if (s->info->arrayfetch->arr != arr)
	{
	  sRef res;

	  check (sRefSet_delete (arr->deriv, s));
	  res = sRef_buildArrayFetchKnown (arr, i);

	  llassert (res->info->arrayfetch->arr == arr);
	  sRef_copyState (res, s);
	  llassert (res->info->arrayfetch->arr == arr);
	  return res;
	}

      s->expkind = sRef_getExKind (arr);
      s->expinfo = stateInfo_copy (arr->expinfo);

      llassert (s->info->arrayfetch->arr == arr);
      return s;
    }
  else
    {
      s = sRef_newRef ();
      
      s->kind = SK_ARRAYFETCH;
      s->info = (sinfo) dmalloc (sizeof (*s->info));
      s->info->arrayfetch = (ainfo) dmalloc (sizeof (*s->info->arrayfetch));
      s->info->arrayfetch->arr = arr;
      s->info->arrayfetch->indknown = TRUE;
      s->info->arrayfetch->ind = i;

      sRef_setArrayFetchState (s, arr);
      /* evans 2001-08-27 no: can change this - llassert (s->info->arrayfetch->arr == arr); */

      s->oaliaskind = s->aliaskind;
      s->oexpkind = s->expkind;
      sRef_addDeriv (arr, s);

      llassert (valueTable_isUndefined (s->state));
      s->state = context_createValueTable (s, stateInfo_makeLoc (g_currentloc, SA_CREATED));
      return (s);
    }
}

/*
** sets everything except for defstate
*/

static void
sRef_setPartsFromUentry (sRef s, uentry ue)
{    
  sRef uref = uentry_getSref (ue);

  llassert (sRef_isReasonable (s));

  s->aliaskind = alkind_derive (s->aliaskind, uentry_getAliasKind (ue));
  s->oaliaskind = s->aliaskind;

  if (s->expkind == XO_UNKNOWN)
    {
      s->expkind = uentry_getExpKind (ue);
    }
  
  s->oexpkind = s->expkind;
  
  if (sRef_getNullState (s) == NS_UNKNOWN)
    {
      DPRINTF (("Set null state: %s / %s", sRef_unparseFull (s), uentry_unparseFull (ue)));
      sRef_setNullStateN (s, sRef_getNullState (uentry_getSref (ue)));
    }
  else
    {
      DPRINTF (("Skipping null null state!"));
    }

  if (s->aliaskind == AK_IMPONLY && (sRef_isExposed (s) || sRef_isObserver (s)))
    {
      s->oaliaskind = s->aliaskind = AK_IMPDEPENDENT;
    } 

  if (sRef_isReasonable (uref))
    {
      valueTable utable = uref->state;
      valueTable_free (s->state);
      s->state = valueTable_copy (utable);
    }
}

static void
sRef_setStateFromAbstractUentry (sRef s, uentry ue)
{
  llassert (sRef_isReasonable (s));
  sRef_checkMutable (s);

  sRef_setPartsFromUentry (s, ue);

  s->aliaskind = alkind_derive (s->aliaskind, uentry_getAliasKind (ue));
  s->oaliaskind = s->aliaskind;

  if (s->expkind == XO_UNKNOWN)
    {
      s->expkind = uentry_getExpKind (ue);
    }

  s->oexpkind = s->expkind;
}

void
sRef_setStateFromUentry (sRef s, uentry ue)
{
  sstate defstate;

  sRef_checkMutable (s);
  llassert (sRef_isReasonable (s));
  
  sRef_setPartsFromUentry (s, ue);

  defstate = uentry_getDefState (ue);

  if (sstate_isKnown (defstate))
    {
      s->defstate = defstate;
    }
  else
    {
      ;
    }
}

/*@exposed@*/ sRef
  sRef_buildPointer (/*@exposed@*/ sRef t)
{
  DPRINTF (("build pointer: %s", sRef_unparse (t)));

  if (sRef_isInvalid (t)) return sRef_undefined;

  if (sRef_isAddress (t))
    {
      DPRINTF (("Return ref: %s", sRef_unparse (t->info->ref)));
      return (t->info->ref);
    }
  else
    {
      sRef s = sRef_findDerivedPointer (t);

      DPRINTF (("find derived: %s", sRef_unparse (s)));

      if (sRef_isReasonable (s))
	{
	  s->expkind = sRef_getExKind (t);
	  s->expinfo = stateInfo_copy (t->expinfo);	  

	  s->oaliaskind = s->aliaskind;
	  s->oexpkind = s->expkind;

	  return s;
	}
      else
	{
	  s = sRef_constructPointerAux (t);
	  
	  DPRINTF (("construct: %s", sRef_unparse (s)));

	  if (sRef_isReasonable (s))
	    {
	      sRef_addDeriv (t, s);

	      s->oaliaskind = s->aliaskind;
	      s->oexpkind = s->expkind;
	    }
	  
	  return s;
	}
    }
}

/*@exposed@*/ sRef
sRef_constructPointer (/*@exposed@*/ sRef t)
   /*@modifies t@*/
{
  return sRef_buildPointer (t);
}

static /*@exposed@*/ sRef sRef_constructDerefAux (sRef t, bool isdead)
{
  if (sRef_isReasonable (t))
    {
      sRef s;
      
      /*
      ** if there is a derived t[?], return that.  Otherwise, *t.
      */
      
      s = sRef_findDerivedArrayFetch (t, FALSE, 0, isdead);
      
      if (sRef_isReasonable (s))
	{
	  DPRINTF (("Found array fetch: %s", sRef_unparseFull (s)));
	  return s;
	}
      else
	{
	  sRef ret = sRef_constructPointer (t);

	  DPRINTF (("Constructed pointer: %s", sRef_unparseFull (ret)));

	  return ret;
	}
    }
  else
    {
      return sRef_undefined;
    }
}

sRef sRef_constructDeref (sRef t)
{
  return sRef_constructDerefAux (t, FALSE);
}

sRef sRef_constructDeadDeref (sRef t)
{
  return sRef_constructDerefAux (t, TRUE);
}

static sRef
sRef_constructPointerAux (/*@notnull@*/ /*@exposed@*/ sRef t)
{
  sRef s = sRef_newRef ();
  ctype rt = t->type;
  ctype st;
  
  llassert (valueTable_isUndefined (s->state));

  s->kind = SK_PTR;
  s->info = (sinfo) dmalloc (sizeof (*s->info));
  s->info->ref = t; 
  
  if (ctype_isRealAP (rt))
    {
      s->type = ctype_baseArrayPtr (rt);
    }
  
  st = ctype_realType (s->type);  

  if (t->defstate == SS_UNDEFINED)
    {
      s->defstate = SS_UNUSEABLE;
      s->definfo = stateInfo_copy (t->definfo);
    }
  else if ((t->defstate == SS_ALLOCATED) && !ctype_isSU (st))
    {
      s->defstate = SS_UNDEFINED;
      s->definfo = stateInfo_copy (t->definfo);
    }
  else
    {
      s->defstate = t->defstate;
      s->definfo = stateInfo_copy (t->definfo);
    }
  
  if (t->aliaskind == AK_LOCAL || t->aliaskind == AK_FRESH)
    {
      s->aliaskind = AK_LOCAL;
    }
  else
    {
      s->aliaskind = AK_UNKNOWN;
    }

  s->expkind = sRef_getExKind (t);
  s->expinfo = stateInfo_copy (t->expinfo);

  sRef_setTypeState (s);

  s->oaliaskind = s->aliaskind;
  s->oexpkind = s->expkind;

  if (valueTable_isUndefined (s->state))
    {
      s->state = context_createValueTable (s, stateInfo_makeLoc (g_currentloc, SA_CREATED));
    }

  DPRINTF (("pointer: %s", sRef_unparseFull (s)));
  return s;
}

bool sRef_hasDerived (sRef s)
{
  return (sRef_isReasonable (s) && !sRefSet_isEmpty (s->deriv));
}

void
sRef_clearDerived (sRef s)
{
  if (sRef_isReasonable (s))
    {
      DPRINTF (("Clear derived: [%p] %s", s, sRef_unparseDebug (s)));
      sRefSet_clear (s->deriv); 
    }
}

void
sRef_clearDerivedComplete (sRef s)
{
  if (sRef_isReasonable (s))
    {
      sRef base = sRef_getBaseSafe (s);
      
      while (sRef_isReasonable (base))
	{
	  DPRINTF (("Clear derived: [%p] %s", base, sRef_unparse (base)));
	  sRefSet_clear (base->deriv); 
	  base = sRef_getBaseSafe (base);
	}

      DPRINTF (("Clear derived: [%p] %s", s, sRef_unparse (s)));
      sRefSet_clear (s->deriv); 
    }
}

/*@exposed@*/ sRef sRef_makePointer (/*@exposed@*/ sRef s)
     /*@modifies s@*/
{
  sRef res = sRef_buildPointer (s); 

  DPRINTF (("Res: %s", sRef_unparseFull (res)));
  return res;
}

/*
** &a[] => a (this is for out params)
*/

/*@exposed@*/ sRef
sRef_makeAnyArrayFetch (/*@exposed@*/ sRef arr)
{  
  if (sRef_isAddress (arr))
    {
      return (arr->info->ref);
    }
  else
    {
      return (sRef_buildArrayFetch (arr));
    }
}

/*@exposed@*/ sRef
sRef_makeArrayFetch (/*@exposed@*/ sRef arr)
{
  return (sRef_buildArrayFetch (arr));
}

/*@exposed@*/ sRef
sRef_makeArrayFetchKnown (/*@exposed@*/ sRef arr, int i)
{
  return (sRef_buildArrayFetchKnown (arr, i));
}

/*@exposed@*/ sRef
sRef_makeField (sRef rec, /*@dependent@*/ cstring f)
{
  sRef ret;
  ret = sRef_buildField (rec, f);
  return ret;
}

/*@exposed@*/ sRef
sRef_makeNCField (/*@exposed@*/ sRef rec, /*@dependent@*/ cstring f)
{
  return (sRef_buildNCField (rec, f));
}

/*@only@*/ cstring
sRef_unparseKindName (sRef s)
{
  cstring result;

  if (s == sRef_undefined) return cstring_makeLiteral ("<invalid>");

  s = sRef_fixConj (s);

  switch (s->kind)
    {
    case SK_CVAR: 
      if (sRef_isLocalVar (s)) 
	{
	  result = cstring_makeLiteral ("Variable");
	}
      else
	{
	  result = cstring_makeLiteral ("Undef global");
	}
      break;
    case SK_PARAM:
      result = cstring_makeLiteral ("Out parameter");
      break;
    case SK_ARRAYFETCH:
      if (sRef_isAnyParam (s->info->arrayfetch->arr)) 
	{
	  result = cstring_makeLiteral ("Out parameter");
	}
      else if (sRef_isIndexKnown (s))
	{
	  result = cstring_makeLiteral ("Array element");
	}
      else
	{
	  result = cstring_makeLiteral ("Value");
	}
      break;
    case SK_PTR:
      if (sRef_isAnyParam (s->info->ref)) 
	{
	  result = cstring_makeLiteral ("Out parameter");
	}
      else
	{
	  result = cstring_makeLiteral ("Value");
	}
      break;
    case SK_ADR:
      result = cstring_makeLiteral ("Value");
      break;
    case SK_FIELD:
      result = cstring_makeLiteral ("Field");
      break;
    case SK_OBJECT:
      result = cstring_makeLiteral ("Object");
      break;
    case SK_UNCONSTRAINED:
      result = cstring_makeLiteral ("<anything>");
      break;
    case SK_RESULT:
    case SK_SPECIAL:
    case SK_UNKNOWN:
    case SK_EXTERNAL:
    case SK_DERIVED:
    case SK_CONST:
    case SK_TYPE:
      result = cstring_makeLiteral ("<unknown>");
      break;
    case SK_CONJ:
      result = cstring_makeLiteral ("<conj>");
      break;
    case SK_NEW:
      result = cstring_makeLiteral ("Storage");
      break;
    }
  
  return result;
}

/*@only@*/ cstring
sRef_unparseKindNamePlain (sRef s)
{
  cstring result;

  if (s == sRef_undefined) return cstring_makeLiteral ("<invalid>");

  s = sRef_fixConj (s);

  switch (s->kind)
    {
    case SK_CVAR: 
      if (sRef_isLocalVar (s)) 
	{
	  result = cstring_makeLiteral ("Variable");
	}
      else 
	{
	  result = cstring_makeLiteral ("Global");
	}
      break;
    case SK_PARAM:
      result = cstring_makeLiteral ("Parameter");
      break;
    case SK_ARRAYFETCH:
      if (sRef_isAnyParam (s->info->arrayfetch->arr)) 
	{
	  result = cstring_makeLiteral ("Parameter");
	}
      else if (sRef_isIndexKnown (s))
	{
	  result = cstring_makeLiteral ("Array element");
	}
      else 
	{
	  result = cstring_makeLiteral ("Value");
	}
      break;
    case SK_PTR:
      if (sRef_isAnyParam (s->info->ref))
	{
	  result = cstring_makeLiteral ("Parameter");
	}
      else
	{
	  result = cstring_makeLiteral ("Value");
	}
      break;
    case SK_ADR:
      result = cstring_makeLiteral ("Value");
      break;
    case SK_FIELD:
      result = cstring_makeLiteral ("Field");
      break;
    case SK_OBJECT:
      result = cstring_makeLiteral ("Object");
      break;
    case SK_NEW:
      result = cstring_makeLiteral ("Storage");
      break;
    case SK_UNCONSTRAINED:
      result = cstring_makeLiteral ("<anything>");
      break;
    case SK_RESULT:
    case SK_TYPE:
    case SK_CONST:
    case SK_EXTERNAL:
    case SK_DERIVED:
    case SK_UNKNOWN:
    case SK_SPECIAL:
      result = cstring_makeLiteral ("<unknown>");
      break;
    case SK_CONJ:
      result = cstring_makeLiteral ("<conj>");
      break;
    }
  
  return result;
}

/*
** s1 <- s2
*/

void
sRef_copyState (sRef s1, sRef s2)
{
  if (sRef_isReasonable (s1) && sRef_isReasonable (s2))
    {
      s1->defstate = s2->defstate;
      
      /* start modifications */
      s1->bufinfo.bufstate = s2->bufinfo.bufstate;
      s1->bufinfo.len = s2->bufinfo.len;
      s1->bufinfo.size = s2->bufinfo.size;
      /* end modifications */

      s1->aliaskind = s2->aliaskind;
      s1->aliasinfo = stateInfo_update (s1->aliasinfo, s2->aliasinfo);

      s1->expkind = s2->expkind;
      s1->expinfo = stateInfo_update (s1->expinfo, s2->expinfo);

      s1->nullstate = s2->nullstate;
      DPRINTF (("Set null state==> %s", sRef_unparseFull (s1)));
      s1->nullinfo = stateInfo_update (s1->nullinfo, s2->nullinfo);

      valueTable_free (s1->state);  
      s1->state = valueTable_copy (s2->state);
      s1->safe = s2->safe;
    }
}

sRef
sRef_makeNew (ctype ct, sRef t, cstring name)
{
  sRef s = sRef_newRef ();

  s->kind = SK_NEW;
  s->type = ct;

  llassert (sRef_isReasonable (t));
  s->defstate = t->defstate;

  s->aliaskind = t->aliaskind;
  s->oaliaskind = s->aliaskind;
  s->nullstate = t->nullstate;
  
  s->expkind = t->expkind;
  s->oexpkind = s->expkind;
  
  s->info = (sinfo) dmalloc (sizeof (*s->info));
  s->info->fname = name;

  /* start modifications */
  s->bufinfo.bufstate = t->bufinfo.bufstate;
  /* end modifications */
  
  llassert (valueTable_isUndefined (s->state));
  s->state = valueTable_copy (t->state);

  DPRINTF (("==> Copying state: %s", valueTable_unparse (s->state)));
  DPRINTF (("==> new: %s", sRef_unparseFull (s)));
  return s;
}

sRef
sRef_makeType (ctype ct)
{
  sRef s = sRef_newRef ();

  sRef_checkMutable (s);

  s->kind = SK_TYPE;
  s->type = ct;

  s->defstate = SS_UNKNOWN; 
  s->aliaskind = AK_UNKNOWN;
  sRef_setNullStateN (s, NS_UNKNOWN);

  /* start modification */
  s->bufinfo.bufstate = BB_NOTNULLTERMINATED;
  /* end modification */

    
  if (ctype_isUA (ct))
    {
      typeId uid = ctype_typeId (ct);
      uentry ue = usymtab_getTypeEntrySafe (uid);

      if (uentry_isValid (ue))
	{
	  sRef_mergeStateQuiet (s, uentry_getSref (ue));
	}
    }
  
  s->oaliaskind = s->aliaskind;
  s->oexpkind = s->expkind;
  llassert (valueTable_isUndefined (s->state));
  s->state = context_createValueTable (s, stateInfo_makeLoc (g_currentloc, SA_CREATED));

  DPRINTF (("Create: %s", sRef_unparseFull (s)));
  return s;
}

sRef
sRef_makeConst (ctype ct)
{
  sRef s = sRef_newRef ();
  
  s->kind = SK_CONST;
  s->type = ct;

  s->defstate = SS_UNKNOWN;
  s->aliaskind = AK_UNKNOWN;
  sRef_setNullStateN (s, NS_UNKNOWN);

  /* start modification */
  s->bufinfo.bufstate = BB_NULLTERMINATED;
  /* end modification */

  /* evans 2002-04-22: added isManifestBool to avoid errors for -boolfalse initializations */
  if (!ctype_isManifestBool (ct) && ctype_isUA (ct)) 
    {
      typeId uid = ctype_typeId (ct);
      uentry te = usymtab_getTypeEntrySafe (uid);
      
      if (uentry_isValid (te))
	{
	  sRef_mergeStateQuiet (s, uentry_getSref (te));
	}
    }
  
  s->oaliaskind = s->aliaskind;
  s->oexpkind = s->expkind;

  llassert (valueTable_isUndefined (s->state));
  s->state = context_createValueTable (s, stateInfo_makeLoc (g_currentloc, SA_CREATED));

  return s;
}

bool sRef_hasName (sRef s)
{
  if (sRef_isInvalid (s))
    {
      return (FALSE);
    }

  switch (s->kind)
    {
    case SK_CVAR:
      {
	uentry u = usymtab_getRefQuiet (s->info->cvar->lexlevel,
					 s->info->cvar->index);
	return (uentry_hasName (u));
      }
    case SK_PARAM:
      {
	if (s->info->paramno >= 0)
	  {
	    uentry u = uentryList_getN (context_getParams (), 
					s->info->paramno);
	    
	    return (uentry_hasName (u));
	  }
	else
	  {
	    llassert (s->info->paramno == PARAMUNKNOWN);
	    return FALSE;
	  }
      }
    default:
      return TRUE;
    }
}

bool
sRef_sameName (sRef s1, sRef s2)
{
  if (sRef_isInvalid (s1))
    {
      return sRef_isInvalid (s2);
    }

  if (sRef_isInvalid (s2))
    {
      return (FALSE);
    }

  switch (s1->kind)
    {
    case SK_CVAR:
      if (s2->kind == SK_CVAR)
	{
	  return (s1->info->cvar->lexlevel == s2->info->cvar->lexlevel
		  && s1->info->cvar->index == s2->info->cvar->index);
	}
      else if (s2->kind == SK_PARAM)
	{
	  if (context_inFunctionLike ())
	    {
	      if (s2->info->paramno != PARAMUNKNOWN)
		{
		  uentry u1 = usymtab_getRefQuiet (s1->info->cvar->lexlevel,
						   s1->info->cvar->index);
		  uentry u2 = uentryList_getN (context_getParams (), 
					       s2->info->paramno);
		  
		  return (cstring_equalFree (uentry_getName (u1),
					     uentry_getName (u2)));
		}
	      else
		{
		  return s1->info->paramno == PARAMUNKNOWN;
		}
	    }
	  else 
	    {
	      return FALSE;
	    }
	}
      else
	{
	  return FALSE;
	}
    case SK_PARAM:
      {
	if (s2->kind == SK_PARAM)
	  {
	    return (s1->info->paramno == s2->info->paramno);
	  }
	else if (s2->kind == SK_CVAR)
	  {
	    if (context_inFunctionLike ())
	      {
		if (s1->info->paramno == PARAMUNKNOWN)
		  {
		    return FALSE;
		  }
		else
		  {
		    uentry u1 = uentryList_getN (context_getParams (), 
						 s1->info->paramno);
		    uentry u2 = usymtab_getRefQuiet (s2->info->cvar->lexlevel,
						     s2->info->cvar->index);
		    
		    
		    return (cstring_equalFree (uentry_getName (u1),
					       uentry_getName (u2)));
		  }
	      }
	    else 
	      {
		return FALSE;
	      }
	  }
	else
	  {
	    return FALSE;
	  }
      }

    case SK_UNCONSTRAINED:
      return FALSE;

    case SK_ARRAYFETCH:
      if (s2->kind == SK_ARRAYFETCH)
	{
	  if (bool_equal (s1->info->arrayfetch->indknown,
			  s2->info->arrayfetch->indknown))
	    {
	      if (!s1->info->arrayfetch->indknown 
		  || (s1->info->arrayfetch->ind == s2->info->arrayfetch->ind))
		{
		  return sRef_sameName (s1->info->arrayfetch->arr,
					s2->info->arrayfetch->arr);
		}
	    }
	}

      return FALSE;
    case SK_FIELD:
      if (s2->kind == SK_FIELD)
	{
	  if (cstring_equal (s1->info->field->field,
			     s2->info->field->field))
	    {
	      return sRef_sameName (s1->info->field->rec,
				    s2->info->field->rec);
	    }

	}
      return FALSE;
    case SK_PTR:
    case SK_ADR:
    case SK_DERIVED:
    case SK_EXTERNAL:
      if (s2->kind == s1->kind)
	{
	  return sRef_sameName (s1->info->ref,
				s2->info->ref);
	}

      return FALSE;
    case SK_OBJECT:
      return FALSE;
    case SK_CONJ:
      return sRef_sameName (sRef_getConjA (s1), s2);
    case SK_NEW:
      return FALSE;
    case SK_UNKNOWN:
      return (s2->kind == SK_UNKNOWN);
    case SK_TYPE:
    case SK_CONST:
      if (s2->kind == s1->kind)
	{
	  return (ctype_equal (s1->type, s2->type));
	}
      
      return FALSE;
    case SK_SPECIAL:
      if (s2->kind == SK_SPECIAL)
	{
	  return (s1->info->spec == s2->info->spec);
	}
      return FALSE;
    case SK_RESULT:
      return (s2->kind == SK_RESULT);
    default:
      return FALSE;
    }
  BADEXIT;
}
		
sRef
sRef_fixOuterRef (/*@returned@*/ sRef s)
{
  sRef root = sRef_getRootBase (s);

  if (sRef_isCvar (root))
    {
      uentry ue = usymtab_getRefQuiet (root->info->cvar->lexlevel, 
				       root->info->cvar->index);

      if (uentry_isValid (ue))
	{
	  sRef uref = uentry_getSref (ue);
	  sRef sr = sRef_fixBase (s, uref);

	  return (sr);
	}
      else
	{
	  llcontbug (message ("sRef_fixOuterRef: undefined: %q", sRef_unparseDebug (s)));
	  return (s);
	}
    }

  return (s);
}

void
sRef_storeState (sRef s)
{
  if (sRef_isInvalid (s)) return;

  sRef_checkMutable (s);
  s->oaliaskind = s->aliaskind;
  s->oexpkind = s->expkind;
}
  
static void sRef_resetStateAux (sRef s, /*@unused@*/ fileloc loc)
{
  sRef_resetState (s);
}

void
sRef_resetState (sRef s)
{
  bool changed = FALSE;
  if (sRef_isInvalid (s)) return;

  
  if (s->oaliaskind == AK_KILLREF && !sRef_isParam (s))
    {
      /*
      ** killref is used in a kludgey way, to save having to add
      ** another alias kind (see usymtab_handleParams)
      */
 
      if (s->expkind != s->oexpkind)
	{
	  changed = TRUE;
	  s->expkind = s->oexpkind;
	}
    }
  else
    {
      if (s->expkind != s->oexpkind)
	{
	  changed = TRUE;
	  s->expkind = s->oexpkind;	  
	}

      if (s->aliaskind != s->oaliaskind
	  && s->aliaskind != AK_REFCOUNTED
	  && s->aliaskind != AK_REFS)
	{
	  changed = TRUE;
	  s->aliaskind = s->oaliaskind;
	}
    }

  if (changed)
    {
      sRef_clearDerived (s);
    }
  
  }

void
sRef_resetStateComplete (sRef s)
{
  sRef_innerAliasSetComplete (sRef_resetStateAux, s, fileloc_undefined);
}

/*@exposed@*/ sRef
sRef_fixBase (/*@returned@*/ sRef s, /*@returned@*/ sRef base)
{
  sRef tmp = sRef_undefined;
  sRef ret;

  if (sRef_isInvalid (s)) return s;
  if (sRef_isInvalid (base)) return base;

  switch (s->kind)
    {
    case SK_RESULT:
    case SK_PARAM:
    case SK_CVAR:
      ret = base;
      break;
    case SK_ARRAYFETCH:
      tmp = sRef_fixBase (s->info->arrayfetch->arr, base);

      if (s->info->arrayfetch->indknown)
	{
	  ret = sRef_makeArrayFetchKnown (tmp, s->info->arrayfetch->ind);
	}
      else
	{
	  ret = sRef_makeArrayFetch (tmp);
	}
      break;
    case SK_FIELD:
      tmp = sRef_fixBase (s->info->field->rec, base);
      ret = sRef_buildNCField (tmp, s->info->field->field);
      break;
    case SK_PTR:
      tmp = sRef_fixBase (s->info->ref, base);
      ret = sRef_makePointer (tmp);
      break;
    case SK_ADR:
      tmp = sRef_fixBase (s->info->ref, base);
      ret = sRef_makeAddress (tmp);
      break;
    case SK_CONJ:
      {
	sRef tmpb;

	tmp = sRef_fixBase (s->info->conj->a, base);
	tmpb = sRef_fixBase (s->info->conj->b, base);

	ret = sRef_makeConj (tmp, tmpb);
	break;
      }
      BADDEFAULT;
    }

  return ret;
}

static /*@exposed@*/ sRef 
sRef_fixDirectBase (sRef s, sRef base)
{
  sRef ret;

  
  if (sRef_isInvalid (s))
    {
      return sRef_undefined;
    }
  
  switch (s->kind)
    {
    case SK_ARRAYFETCH:
      if (s->info->arrayfetch->indknown)
	{
	  ret = sRef_makeArrayFetchKnown (base, s->info->arrayfetch->ind);
	}
      else
	{
	  ret = sRef_makeArrayFetch (base);
	}
      break;
    case SK_FIELD:
      ret = sRef_buildNCField (base, s->info->field->field);
      break;
    case SK_PTR:
            ret = sRef_makePointer (base);
            break;
    case SK_ADR:
      ret = sRef_makeAddress (base);
      break;
    case SK_CONJ:
      {
	sRef tmpa, tmpb;

	tmpa = sRef_fixDirectBase (s->info->conj->a, base);
	tmpb = sRef_fixDirectBase (s->info->conj->b, base);

	ret = sRef_makeConj (tmpa, tmpb);
	break;
      }
      BADDEFAULT;
    }

    sRef_copyState (ret, s);
    return ret;
}

bool
sRef_isAllocIndexRef (sRef s)
{
  return (sRef_isArrayFetch (s) && !(s->info->arrayfetch->indknown) 
	  && sRef_isAllocated (s->info->arrayfetch->arr));
}

void
sRef_showRefLost (sRef s)
{
  if (sRef_hasAliasInfoLoc (s))
    {
      llgenindentmsg (cstring_makeLiteral ("Original reference lost"),
		      sRef_getAliasInfoLoc (s));
    }
}

void
sRef_showRefKilled (sRef s)
{
  if (sRef_isValid (s)) 
    {
      DPRINTF (("Killed: %s", sRef_unparseFull (s)));
      if (context_getLocIndentSpaces () == 0) {
	stateInfo_display (s->definfo, message ("  Storage %q", sRef_unparseOpt (s)));
      } else {
	stateInfo_display (s->definfo, message ("Storage %q", sRef_unparseOpt (s)));
      }
    }
}

void
sRef_showStateInconsistent (sRef s)
{
  if (sRef_hasStateInfoLoc (s))
    {
      llgenindentmsg
	(message ("Storage %qbecomes inconsistent (released on one branch)",
		  sRef_unparseOpt (s)), 
	 sRef_getStateInfoLoc (s));
    }
}

void
sRef_showStateInfo (sRef s)
{
  if (sRef_isValid (s)) {
    if (context_getLocIndentSpaces () == 0) {
      stateInfo_display (s->definfo, message ("   Storage %q", sRef_unparseOpt (s)));
    } else {
      stateInfo_display (s->definfo, message ("Storage %q", sRef_unparseOpt (s)));
    }
  }
}

void
sRef_showExpInfo (sRef s)
{
  if (sRef_isValid (s)) {
    if (context_getLocIndentSpaces () == 0) {
      stateInfo_display (s->expinfo, message ("   Storage %q", sRef_unparseOpt (s)));
    } else {
      stateInfo_display (s->expinfo, message ("Storage %q", sRef_unparseOpt (s)));
    }
  }
}

void
sRef_showMetaStateInfo (sRef s, cstring key)
{
  stateValue val;
  metaStateInfo minfo = context_lookupMetaStateInfo (key);

  llassert (sRef_isReasonable (s));
  llassert (valueTable_isDefined (s->state));
  llassert (metaStateInfo_isDefined (minfo));

  val = valueTable_lookup (s->state, key);
  
  if (stateValue_hasLoc (val))
    {
      llgenindentmsg 
	(message ("%qbecomes %q", sRef_unparseOpt (s), 
		  stateValue_unparseValue (val, minfo)),
	 stateValue_getLoc (val));
    }
}

void
sRef_showNullInfo (sRef s)
{
  DPRINTF (("Show null info: %s", sRef_unparseFull (s)));

  if (sRef_hasNullInfoLoc (s) && sRef_isKnown (s))
    {
      DPRINTF (("has null info: %s",
		fileloc_unparse (sRef_getNullInfoLoc (s))));

      switch (sRef_getNullState (s))
	{
	case NS_CONSTNULL:
	  {
	    fileloc loc = sRef_getNullInfoLoc (s);
	    
	    if (fileloc_isDefined (loc) && !fileloc_isLib (loc))
	      {
		llgenindentmsg 
		  (message ("Storage %qbecomes null", sRef_unparseOpt (s)),
		   loc);
	      }
	    break;
	  }
	case NS_DEFNULL:
	  {
	    fileloc loc = sRef_getNullInfoLoc (s);
	    
	    if (fileloc_isDefined (loc) && !fileloc_isLib (loc))
	      {
		llgenindentmsg (message ("Storage %qbecomes null", sRef_unparseOpt (s)),
				loc);
	      }
	    break;
	  }
	case NS_ABSNULL:
	case NS_POSNULL:
	  llgenindentmsg
	    (message ("Storage %qmay become null", sRef_unparseOpt (s)),
	     sRef_getNullInfoLoc (s));
	  break;
	case NS_NOTNULL:
	case NS_MNOTNULL:
	  llgenindentmsg
	    (message ("Storage %qbecomes not null", sRef_unparseOpt (s)),
	     sRef_getNullInfoLoc (s));
	  break;
	case NS_UNKNOWN:
	  llgenindentmsg
	    (message ("Storage %qnull state becomes unknown",
		      sRef_unparseOpt (s)),
	     sRef_getNullInfoLoc (s));
	  break;

	case NS_ERROR:
	  BADBRANCHCONT;
	  break;

	default:
	  llgenindentmsg
	    (message ("<error case> Storage %q becomes %s",
		      sRef_unparse (s), 
		      nstate_unparse (sRef_getNullState (s))),
	     sRef_getNullInfoLoc (s));
	  
	  break;
	}
    }
}

void
sRef_showAliasInfo (sRef s)
{
  if (sRef_isValid (s)) 
    {
      if (sRef_isFresh (s))
	{
	  if (context_getLocIndentSpaces () == 0) {
	    stateInfo_display (s->aliasinfo, message ("   Fresh storage %q", sRef_unparseOpt (s)));
	  } else {
	    stateInfo_display (s->aliasinfo, message ("Fresh storage %q", sRef_unparseOpt (s)));
	  }
	}
      else
	{
	  if (context_getLocIndentSpaces () == 0) {
	    stateInfo_display (s->aliasinfo, message ("   Storage %q", sRef_unparseOpt (s))); 
	  } else {
	    stateInfo_display (s->aliasinfo, message ("Storage %q", sRef_unparseOpt (s)));
	  }
	}
    }
}

void
sRef_mergeNullState (sRef s, nstate n)
{
  if (sRef_isReasonable (s))
    {
      nstate old;
      
      old = sRef_getNullState (s);
      
      if (n != old && n != NS_UNKNOWN)
	{	  	  
	  sRef_setNullState (s, n, g_currentloc);
	}
    }
  else
    {
      llbuglit ("sRef_mergeNullState: invalid");
    }
}

bool
sRef_possiblyNull (sRef s)
{
  if (sRef_isReasonable (s))
    {
      if (sRef_getNullState (s) == NS_ABSNULL)
	{
	  ctype rct = ctype_realType (s->type);
	  
	  if (ctype_isAbstract (rct))
	    {
	      return FALSE;
	    }
	  else
	    {
	      if (ctype_isUser (rct))
		{
		  uentry ue = usymtab_getTypeEntry (ctype_typeId (rct));
		  
		  return (nstate_possiblyNull
			  (sRef_getNullState (uentry_getSref (ue))));
		}
	      else
		{
		  return FALSE;
		}
	    }
	}
      else
	{
	  return nstate_possiblyNull (sRef_getNullState (s));
	}
    }
  
  return FALSE;
}

cstring
sRef_getScopeName (sRef s)
{
  sRef base = sRef_getRootBase (s);

  if (sRef_isRealGlobal (base))
    {
      return (cstring_makeLiteralTemp ("Global"));
    }
  else if (sRef_isFileStatic (base))
    {
      return (cstring_makeLiteralTemp ("Static"));
    }
  else
    {
      return (cstring_makeLiteralTemp ("Local"));
    }
}

cstring
sRef_unparseScope (sRef s)
{
  sRef base = sRef_getRootBase (s);

  if (sRef_isRealGlobal (base))
    {
      return (cstring_makeLiteralTemp ("global"));
    }
  else if (sRef_isFileStatic (base))
    {
      return (cstring_makeLiteralTemp ("file static"));
    }
  else
    {
      BADEXIT;
    }
}

int
sRef_getScope (sRef s)
{
  llassert (sRef_isReasonable (s));

  if (sRef_isCvar (s))
    {
      return s->info->cvar->lexlevel;
    }
  else if (sRef_isParam (s))
    {
      return paramsScope;
    }
  else
    {
      return fileScope;
    }
}

bool
sRef_isDead (sRef s)
{
  return (sRef_isReasonable (s) && (s)->defstate == SS_DEAD);
}

bool
sRef_isDeadStorage (sRef s)
{
  if (sRef_isReasonable (s))
    {
      if (s->defstate == SS_DEAD
	  || s->defstate == SS_UNUSEABLE
	  || s->defstate == SS_UNDEFINED
	  || s->defstate == SS_UNKNOWN)
	{
	  return TRUE;
	}
      else 
	{
	  return (sRef_isDefinitelyNull (s));
	}
    }
  else
    {
      return FALSE;
    }
}

bool
sRef_isPossiblyDead (sRef s)
{
  return (sRef_isReasonable (s) && s->defstate == SS_HOFFA);
}

bool sRef_isStateLive (sRef s)
{
  if (sRef_isReasonable (s))
    {
      sstate ds = s->defstate;

      return (!(ds == SS_UNDEFINED 
		|| ds == SS_DEAD
		|| ds == SS_UNUSEABLE
		|| ds == SS_HOFFA));
    }
  else
    {
      return FALSE;
    }
}


bool sRef_isStateUndefined (sRef s)
{
  return ((sRef_isReasonable(s)) && ((s)->defstate == SS_UNDEFINED));
}

bool sRef_isJustAllocated (sRef s)
{
  if (sRef_isAllocated (s))
    {
      sRefSet_allElements (s->deriv, el)
	{
	  if (!(sRef_isStateUndefined (el) || sRef_isUnuseable (el)))
	    {
	      return FALSE;
	    }
	} end_sRefSet_allElements ;

      return TRUE;
    }

  return FALSE;
}

static bool
sRef_isAllocatedStorage (sRef s)
{
  if (sRef_isReasonable (s) && ynm_toBoolStrict (sRef_isValidLvalue (s)))
    {
      return (ctype_isVisiblySharable (sRef_getType (s)));
    }
  else
    {
      return FALSE;
    }
}

bool
sRef_isUnuseable (sRef s)
{
  return ((sRef_isValid(s)) && ((s)->defstate == SS_UNUSEABLE));
}

bool
sRef_perhapsNull (sRef s)
{
  if (sRef_isReasonable (s))
    {
      if (sRef_getNullState (s) == NS_ABSNULL)
	{
	  ctype rct = ctype_realType (s->type);

	  if (ctype_isAbstract (rct))
	    {
	      return FALSE;
	    }
	  else
	    {
	      if (ctype_isUser (rct))
		{
		  uentry ue = usymtab_getTypeEntry (ctype_typeId (rct));

		  return (nstate_perhapsNull 
			  (sRef_getNullState (uentry_getSref (ue))));
		}
	      else
		{
		  return FALSE;
		}
	    }
	}
      else
	{
	  return nstate_perhapsNull (sRef_getNullState (s));
	}
    }

  return FALSE;
}

/*
** definitelyNull --- called when TRUE is good
*/

bool 
sRef_definitelyNull (sRef s)
{
  return (sRef_isReasonable (s)
	  && (sRef_getNullState (s) == NS_DEFNULL || sRef_getNullState (s) == NS_CONSTNULL));
}

/*
** based on sRef_similar
*/

void
sRef_setDerivNullState (sRef set, sRef guide, nstate ns)
{
  if (sRef_isReasonable (set))
    {
      sRef deriv = sRef_getDeriv (set, guide);
      
      if (sRef_isReasonable (deriv))
	{
	  sRef_setNullStateN (deriv, ns);
	}
    }
}

static /*@exposed@*/ sRef
sRef_getDeriv (/*@returned@*/ /*@notnull@*/ sRef set, sRef guide)
{
  llassert (sRef_isReasonable (set));
  llassert (sRef_isReasonable (guide));

  switch (guide->kind)
    {
    case SK_CVAR:
      llassert (set->kind == SK_CVAR);      
      return set;

    case SK_PARAM:
      llassert (set->kind == guide->kind);
      llassert (set->info->paramno == guide->info->paramno);
      return set;

    case SK_ARRAYFETCH:

      if (set->kind == SK_ARRAYFETCH
	  && (sRef_similar (set->info->arrayfetch->arr,
			    guide->info->arrayfetch->arr)))
	{
	  return set;
	}
      else
	{
	  return (sRef_makeAnyArrayFetch 
		  (sRef_getDeriv (set, guide->info->arrayfetch->arr)));
	}

    case SK_PTR:
      
      if (set->kind == SK_PTR && sRef_similar (set->info->ref, guide->info->ref))
	{
	  return set;
	}
      else
	{
	  return (sRef_makePointer (sRef_getDeriv (set, guide->info->ref)));
	}
      
    case SK_FIELD:
      
      if ((set->kind == SK_FIELD &&
	   (sRef_similar (set->info->field->rec, guide->info->field->rec) &&
	    cstring_equal (set->info->field->field, guide->info->field->field))))
	{
	  return set;
	}
      else
	{
	  return (sRef_makeField (sRef_getDeriv (set, guide->info->field->rec),
				  guide->info->field->field));
	}
    case SK_ADR:
      
      if ((set->kind == SK_ADR) && sRef_similar (set->info->ref, guide->info->ref))
	{
	  return set;
	}
      else
	{
	  return (sRef_makeAddress (sRef_getDeriv (set, guide->info->ref)));
	}

    case SK_CONJ:
      
            return sRef_undefined;

    case SK_RESULT:
    case SK_SPECIAL:
    case SK_UNCONSTRAINED:
    case SK_TYPE:
    case SK_CONST:
    case SK_NEW:
    case SK_UNKNOWN:
    case SK_OBJECT:
    case SK_DERIVED:
    case SK_EXTERNAL:
      return sRef_undefined;
    }

  BADEXIT;
}
      
/*
** sRef_aliasCheckPred
**
** A confusing but spiffy function:
**
**    Calls predf (s, e, text, <alias>) on s and all of s's aliases
**    (unless checkAliases (s) is FALSE).
**
**    For alias calls, calls as
**          predf (alias, e, text, s)
*/

void
sRef_aliasCheckPred (bool (predf) (sRef, exprNode, sRef, exprNode),
		     /*@null@*/ bool (checkAliases) (sRef),
		     sRef s, exprNode e, exprNode err)
{
  bool error = (*predf)(s, e, sRef_undefined, err);
  
  
  if (checkAliases != NULL && !(checkAliases (s)))
    {
      /* don't check aliases */
    }
  else
    {
      sRefSet aliases = usymtab_allAliases (s);
      
      sRefSet_realElements (aliases, current)
	{
	  if (sRef_isReasonable (current))
	    {
	      if (sRef_isReasonable (current))
		{
		  if (!sRef_similar (current, s)
		      || (error && sRef_sameName (current, s)))
		    {
		      (void) (*predf)(current, e, s, err);
		    }
		}
	      else
		{
		  llcontbug (message ("Invalid sRef as alias field of %s", sRef_unparse (s)));
		}
	    }
	} end_sRefSet_realElements;

      sRefSet_free (aliases);
    }
}

/*
** return TRUE iff predf (s) is true for s or any alias of s
*/

bool
sRef_aliasCheckSimplePred (sRefTest predf, sRef s)
{
    
  if ((*predf)(s))
    {
      return TRUE;
    }
  else
    {
      sRefSet aliases;

      aliases = usymtab_allAliases (s);
      
      sRefSet_realElements (aliases, current)
	{
	  if (sRef_isReasonable (current))
	    {
	      sRef cref = sRef_updateSref (current);
	      
	      /* Whoa! a very kludgey way to make sure the right sref is used
	      ** where there is a conditional symbol table.  I am beginning
	      ** to think that having a conditional symbol table wasn't such
	      ** a great idea.  ;(
	      */
	      
	      if ((*predf)(cref))
		{
		  DPRINTF (("Checking alias: %s", sRef_unparseFull (cref)));
		  sRefSet_free (aliases);
		  return TRUE;
		}
	    }
	} end_sRefSet_realElements;

      sRefSet_free (aliases);
    }
  return FALSE;
}

bool
sRef_aliasCompleteSimplePred (bool (predf) (sRef), sRef s)
{
  sRefSet aliases;
  bool result = FALSE;
  
  
  aliases = usymtab_allAliases (s);
  
  if ((*predf)(s)) result = TRUE;

  
  sRefSet_realElements (aliases, current)
    {
      if (sRef_isReasonable (current))
	{
	  current = sRef_updateSref (current);
	  if ((*predf)(current)) result = TRUE;
	}
    } end_sRefSet_realElements;
  
  sRefSet_free (aliases);
  return result;
}

void
sRef_aliasSetComplete (void (predf) (sRef, fileloc), sRef s, fileloc loc)
{
  sRefSet aliases;
  
  aliases = usymtab_allAliases (s);

  DPRINTF (("All aliases: %s --> %s", sRef_unparseFull (s), sRefSet_unparseFull (aliases)));

  (*predf)(s, loc);

  sRefSet_realElements (aliases, current)
    {
      if (sRef_isReasonable (current))
	{
	  DPRINTF (("Update: %s", sRef_unparseFull (current)));
	  current = sRef_updateSref (current);
	  DPRINTF (("Updated ==> %s", sRef_unparseFull (current)));
	  ((*predf)(current, loc));
	  DPRINTF (("Killed: %s", sRef_unparseFull (current)));
	}
    } end_sRefSet_realElements;

  sRefSet_free (aliases);
}

void
sRef_aliasSetCompleteParam (void (predf) (sRef, int, fileloc), sRef s, 
			    int kind, fileloc loc)
{
  sRefSet aliases;
  
  if (sRef_isDeep (s))
    {
      aliases = usymtab_allAliases (s);
    }
  else
    {
      aliases = usymtab_aliasedBy (s);
    }

  (*predf)(s, kind, loc);

  sRefSet_realElements (aliases, current)
    {
      if (sRef_isReasonable (current))
	{
	  current = sRef_updateSref (current);
	  ((*predf)(current, kind, loc));
	}
    } end_sRefSet_realElements;

  sRefSet_free (aliases);
}

/*
** Version of aliasSetCompleteParam for alkind parameters
*/

void
sRef_aliasSetCompleteAlkParam (void (predf) (sRef, alkind, fileloc), sRef s, 
			       alkind kind, fileloc loc)
{
  sRefSet aliases;
  
  if (sRef_isDeep (s))
    {
      aliases = usymtab_allAliases (s);
    }
  else
    {
      aliases = usymtab_aliasedBy (s);
    }

  (*predf)(s, kind, loc);

  sRefSet_realElements (aliases, current)
    {
      if (sRef_isReasonable (current))
	{
	  current = sRef_updateSref (current);
	  ((*predf)(current, kind, loc));
	}
    } end_sRefSet_realElements;

  sRefSet_free (aliases);
}

static void
sRef_innerAliasSetComplete (void (predf) (sRef, fileloc), sRef s, fileloc loc)
{
  sRef inner;
  sRefSet aliases;
  ctype ct;

  if (!sRef_isReasonable (s)) return;

  /*
  ** Type equivalence checking is necessary --- there might be casting.
  */

  (*predf)(s, loc);

  switch (s->kind)
    {
    case SK_UNCONSTRAINED:
    case SK_CVAR:
    case SK_PARAM:
      break;
    case SK_PTR:
      inner = s->info->ref;
      aliases = usymtab_allAliases (inner);
      ct = sRef_getType (inner);
      
      sRefSet_realElements (aliases, current)
	{
	  if (sRef_isReasonable (current))
	    {
	      current = sRef_updateSref (current);
	      
	      if (ctype_equal (ct, sRef_getType (current)))
		{
		  sRef ptr = sRef_makePointer (current);
		  ((*predf)(ptr, loc));
		}
	    }
	} end_sRefSet_realElements;

      sRefSet_free (aliases);
      break;
    case SK_ARRAYFETCH:
      inner = s->info->arrayfetch->arr;
      aliases = usymtab_allAliases (inner);
      ct = sRef_getType (inner);

      DPRINTF (("Array fetch: %s", sRefSet_unparse (aliases)));

      sRefSet_realElements (aliases, current)
	{
	  if (sRef_isReasonable (current))
	    {
	      current = sRef_updateSref (current);
	      DPRINTF (("Current: %s", sRef_unparseFull (current)));

	      if (ctype_equal (ct, sRef_getType (current)))
		{
		  if (s->info->arrayfetch->indknown)
		    {
		      sRef af = sRef_makeArrayFetchKnown (current, s->info->arrayfetch->ind);
		      DPRINTF (("Defining: %s", sRef_unparseFull (af)));
		      /* evans 2001-08-27 This isn't true:
			   llassert (af->info->arrayfetch->arr == current);
			 see comments in buildArrayFetchKnown
		      */
		      ((*predf)(af, loc));
		    }
		  else
		    {
		      sRef af = sRef_makeArrayFetch (current);
		      /* evans 2001-08-27 This isn't true:
			 llassert (af->info->arrayfetch->arr == current);
			 see comments in buildArrayFetch
		      */ 
		      DPRINTF (("Defining: %s", sRef_unparseFull (af)));
		      ((*predf)(af, loc));
		    }
		}
	      else
		{
		  DPRINTF (("Type mismatch: %s / %s",
			    ctype_unparse (ct),
			    ctype_unparse (sRef_getType (current))));
		}
	    }
	} end_sRefSet_realElements;

      sRefSet_free (aliases);
      break;
    case SK_FIELD:
      inner = s->info->field->rec;
      aliases = usymtab_allAliases (inner);
      ct = sRef_getType (inner);
      
      sRefSet_realElements (aliases, current)
	{
	  if (sRef_isReasonable (current))
	    {
	      current = sRef_updateSref (current);
	      
	      if (ctype_equal (ct, sRef_getType (current)))
		{
		  sRef f = sRef_makeField (current, s->info->field->field);
		  
		  ((*predf)(f, loc));
		}
	    }
	} end_sRefSet_realElements;
      
      sRefSet_free (aliases);
      break;
    case SK_CONJ:
      sRef_innerAliasSetComplete (predf, s->info->conj->a, loc);
      sRef_innerAliasSetComplete (predf, s->info->conj->b, loc);
      break;
    case SK_SPECIAL:
    case SK_ADR:
    case SK_TYPE:
    case SK_CONST:
    case SK_NEW:
    case SK_UNKNOWN:
    case SK_OBJECT:
    case SK_DERIVED:
    case SK_EXTERNAL:
    case SK_RESULT:
      break;
    }
}

static void
sRef_innerAliasSetCompleteParam (void (predf) (sRef, sRef), sRef s, sRef t)
{
  sRef inner;
  sRefSet aliases;
  ctype ct;

  if (!sRef_isReasonable (s)) return;

  /*
  ** Type equivalence checking is necessary --- there might be casting.
  */

  (*predf)(s, t);

  switch (s->kind)
    {
    case SK_UNCONSTRAINED:
    case SK_CVAR:
    case SK_PARAM:
      break;
    case SK_PTR:
      inner = s->info->ref;
      aliases = usymtab_allAliases (inner);
      ct = sRef_getType (inner);
            
      sRefSet_realElements (aliases, current)
	{
	  if (sRef_isReasonable (current))
	    {
	      current = sRef_updateSref (current);
	      
	      if (ctype_equal (ct, sRef_getType (current)))
		{
		  sRef ptr = sRef_makePointer (current);
		  
		  ((*predf)(ptr, t));
		}
	    }
	} end_sRefSet_realElements;

      sRefSet_free (aliases);
      break;
    case SK_ARRAYFETCH:
      inner = s->info->arrayfetch->arr;
      aliases = usymtab_allAliases (inner);
      ct = sRef_getType (inner);

      sRefSet_realElements (aliases, current)
	{
	  if (sRef_isReasonable (current))
	    {
	      current = sRef_updateSref (current);
	      
	      if (ctype_equal (ct, sRef_getType (current)))
		{
		  		  
		  if (s->info->arrayfetch->indknown)
		    {
		      sRef af = sRef_makeArrayFetchKnown (current, s->info->arrayfetch->ind);
		      
		      ((*predf)(af, t));
		    }
		  else
		    {
		      sRef af = sRef_makeArrayFetch (current);
		      
		      ((*predf)(af, t));
		    }
		}
	    }
	} end_sRefSet_realElements;

      sRefSet_free (aliases);
      break;
    case SK_FIELD:
      inner = s->info->field->rec;
      aliases = usymtab_allAliases (inner);
      ct = sRef_getType (inner);

      
      sRefSet_realElements (aliases, current)
	{
	  if (sRef_isReasonable (current))
	    {
	      current = sRef_updateSref (current);
	      
	      if (ctype_equal (ct, sRef_getType (current)))
		{
		  sRef f = sRef_makeField (current, s->info->field->field);
		  
		  ((*predf)(f, t));
		}
	    }
	} end_sRefSet_realElements;
      
      sRefSet_free (aliases);
      break;
    case SK_CONJ:
      sRef_innerAliasSetCompleteParam (predf, s->info->conj->a, t);
      sRef_innerAliasSetCompleteParam (predf, s->info->conj->b, t);
      break;
    case SK_SPECIAL:
    case SK_ADR:
    case SK_TYPE:
    case SK_CONST:
    case SK_NEW:
    case SK_UNKNOWN:
    case SK_OBJECT:
    case SK_DERIVED:
    case SK_EXTERNAL:
    case SK_RESULT:
      break;
    }
}

static void sRef_combineExKinds (/*@notnull@*/ sRef res, /*@notnull@*/ sRef other)
{
  exkind a1 = sRef_getExKind (res);
  exkind a2 = sRef_getExKind (other);

  if (a1 == a2 || a2 == XO_UNKNOWN) 
    {
      ;
    }
  else if (a1 == XO_UNKNOWN) 
    { 
      res->expinfo = stateInfo_update (res->expinfo, other->expinfo);
      res->expkind = a2;
    }
  else
    {
      res->expkind = XO_OBSERVER;
    }
}

/*
** Currently, this is a very ad hoc implementation, with lots of fixes to
** make real code work okay.  I need to come up with some more general
** rules or principles here.
*/

static void 
  sRef_combineAliasKindsError (/*@notnull@*/ sRef res, 
			       /*@notnull@*/ sRef other, 
			       clause cl, fileloc loc)
{
  bool hasError = FALSE;
  alkind ares = sRef_getAliasKind (res);
  alkind aother = sRef_getAliasKind (other);

  sRef_checkMutable (res);

  if (alkind_isDependent (ares))
    {
      if (aother == AK_KEPT)
	{
	  res->aliasinfo = stateInfo_update (res->aliasinfo, other->aliasinfo);
	  res->aliaskind = AK_KEPT;      
	}
      else 
	{
	  if (aother == AK_LOCAL || aother == AK_STATIC 
	      || alkind_isTemp (aother))
	    {
	      DPRINTF (("Setting dependent: %s", sRef_unparseFull (res)));
	      res->aliaskind = AK_DEPENDENT;
	    }
	}
    }
  else if (alkind_isDependent (aother))
    {
      if (ares == AK_KEPT)
	{
	  res->aliaskind = AK_KEPT;      
	}
      else 
	{
	  if (ares == AK_LOCAL || ares == AK_STATIC || alkind_isTemp (ares))
	    {
	      DPRINTF (("Setting dependent: %s", sRef_unparseFull (res)));
	      res->aliasinfo = stateInfo_update (res->aliasinfo, other->aliasinfo);
	      res->aliaskind = AK_DEPENDENT;
	    }
	}
    }
  else if ((ares == AK_LOCAL || ares == AK_UNIQUE
	    || ares == AK_STATIC || alkind_isTemp (ares))
	   && sRef_isFresh (other))
    {
      /*
      ** cases like: if (s == NULL) s = malloc...;
      **    don't generate errors
      */
      
      if (usymtab_isAltDefinitelyNullDeep (res))
	{
	  res->aliaskind = ares;
	}
      else
	{
	  hasError = TRUE; 
	}
    }
  else if ((aother == AK_LOCAL || aother == AK_UNIQUE
	    || aother == AK_STATIC || alkind_isTemp (aother))
	   && sRef_isFresh (res))
    {
      /*
      ** cases like: if (s == NULL) s = malloc...;
      **    don't generate errors
      */
      
      if (usymtab_isDefinitelyNullDeep (other))
	{
	  res->aliasinfo = stateInfo_update (res->aliasinfo, other->aliasinfo);
	  res->aliaskind = aother;
	}
      else
	{
	  hasError = TRUE;
	}
    }
  else if (ares == AK_NEWREF && aother == AK_REFCOUNTED 
	   && sRef_isConst (other))
    {
      res->aliaskind = AK_NEWREF;
    }
  else if (aother == AK_NEWREF && ares == AK_REFCOUNTED
	   && sRef_isConst (res))
    {
      res->aliasinfo = stateInfo_update (res->aliasinfo, other->aliasinfo);
      res->aliaskind = AK_NEWREF;
    }
  else if (sRef_isLocalVar (res)
	   && ((ares == AK_KEPT && aother == AK_LOCAL)
	       || (aother == AK_KEPT && ares == AK_LOCAL)))
    {
      res->aliaskind = AK_KEPT;
    }
  else if ((ares == AK_OWNED && aother == AK_ONLY)
	   || (aother == AK_OWNED && ares == AK_ONLY))
    {
      res->aliaskind = AK_OWNED;

      if (aother == AK_OWNED)
	{
	  res->aliasinfo = stateInfo_update (res->aliasinfo, other->aliasinfo);
	}
    }
  else
    {
      hasError = TRUE;
    }

  if (hasError)
    {
      if (sRef_isThroughArrayFetch (res))
	{
	  if (optgenerror2 
	      (FLG_BRANCHSTATE, FLG_STRICTBRANCHSTATE,
	       message
	       ("Clauses exit with %q possibly referencing %s storage %s, "
		"%s storage %s", 
		sRef_unparse (res),
		alkind_unparse (aother),
		clause_nameTaken (cl),
		alkind_unparse (ares),
		clause_nameAlternate (cl)),
	       loc))
	    {
	      sRef_showAliasInfo (res);
	      sRef_showAliasInfo (other);
	      res->aliaskind = AK_ERROR;
	    }
	  else
	    {
	      if (ares == AK_KEPT || aother == AK_KEPT)
		{
		  sRef_maybeKill (res, loc);
		}
	    }
	}
      else 
	{
	  if (optgenerror 
	      (FLG_BRANCHSTATE,
	       message ("Clauses exit with %q referencing %s storage %s, "
			"%s storage %s", 
			sRef_unparse (res),
			alkind_unparse (aother),
			clause_nameTaken (cl),
			alkind_unparse (ares),
			clause_nameAlternate (cl)),
	       loc))
	    {
	      sRef_showAliasInfo (res);
	      sRef_showAliasInfo (other);
	      
	      res->aliaskind = AK_ERROR;
	    }
	}
      
      res->aliaskind = (sRef_isLocalVar (res) ? AK_LOCAL : AK_UNKNOWN);
    }
}

static void 
  sRef_combineAliasKinds (/*@notnull@*/ sRef res, /*@notnull@*/ sRef other, 
			  clause cl, fileloc loc)
{
  alkind ares = sRef_getAliasKind (res);
  alkind aother = sRef_getAliasKind (other);

  sRef_checkMutable (res);

  DPRINTF (("Combine alias kinds: \n\t%s / \n\t%s",
	    sRef_unparseFull (res), sRef_unparseFull (other)));
  if (alkind_equal (ares, aother)
      || aother == AK_UNKNOWN
      || aother == AK_ERROR)
    {
      ; /* keep current state */
    }
  else if (sRef_isDead (res) || sRef_isDead (other))
    {
      /* dead error reported (or storage is dead) */
      res ->aliaskind = AK_ERROR; 
    }
  else if (ares == AK_UNKNOWN || ares == AK_ERROR
	   || sRef_isStateUndefined (res)
	   || sRef_isDefinitelyNull (res))
    { 
      res->aliasinfo = stateInfo_update (res->aliasinfo, other->aliasinfo);
      res->aliaskind = aother;  
    }
  else if (sRef_isStateUndefined (other)
	   || sRef_isDefinitelyNull (other))
    {
      ;
    }
  else if (((ares == AK_UNIQUE || alkind_isTemp (ares))
	    && aother == AK_LOCAL) 
	   || ((aother == AK_UNIQUE || alkind_isTemp (aother))
	       && ares == AK_LOCAL))
    {
      if (ares != AK_LOCAL)
	{
	  res->aliasinfo = stateInfo_update (res->aliasinfo, other->aliasinfo);
	}

      res->aliaskind = AK_LOCAL;
    }
  else if ((ares == AK_OWNED && aother == AK_FRESH) 
	   || (aother == AK_OWNED && ares == AK_FRESH))
    {
      if (ares != AK_FRESH)
	{
	  res->aliasinfo = stateInfo_update (res->aliasinfo, other->aliasinfo);
	}
      
      res->aliaskind = AK_FRESH;
    }
  else if ((ares == AK_KEEP && aother == AK_FRESH) ||
	   (aother == AK_KEEP && ares == AK_FRESH))
    {
      if (ares != AK_KEEP)
	{
	  res->aliasinfo = stateInfo_update (res->aliasinfo, other->aliasinfo);
	}
      
      res->aliaskind = AK_KEEP;
    }
  else if ((ares == AK_LOCAL && aother == AK_STACK) ||
	   (aother == AK_LOCAL && ares == AK_STACK))
    {
      if (ares != AK_STACK)
	{
	  res->aliasinfo = stateInfo_update (res->aliasinfo, other->aliasinfo);
	}

      res->aliaskind = AK_STACK;
    }
  else if ((ares == AK_LOCAL
	    && (aother == AK_OWNED && sRef_isLocalVar (other)))
	   || (aother == AK_LOCAL 
	       && (ares == AK_OWNED && sRef_isLocalVar (res))))
    {
      if (ares != AK_LOCAL)
	{
	  res->aliasinfo = stateInfo_update (res->aliasinfo, other->aliasinfo);
	}

      res->aliaskind = AK_LOCAL;
    }
  else if ((ares == AK_FRESH && alkind_isOnly (aother))
	   || (aother == AK_FRESH && alkind_isOnly (ares)))
    {
      res->aliaskind = AK_FRESH;
    }
  else if ((aother == AK_FRESH && sRef_definitelyNull (res))
	   || (ares == AK_FRESH && sRef_definitelyNull (other)))
    {
      if (ares != AK_FRESH)
	{
	  res->aliasinfo = stateInfo_update (res->aliasinfo, other->aliasinfo);
	  res->aliaskind = AK_FRESH;
	}
    }
  else if ((sRef_isFresh (res) && sRef_isConst (other))
	   || (sRef_isFresh (other) && sRef_isConst (res)))
    {
      /*
      ** for NULL constantants
      ** this is bogus!
      */

      if (!sRef_isFresh (res))
	{
	  res->aliasinfo = stateInfo_update (res->aliasinfo, other->aliasinfo);
	}

      res->aliaskind = AK_FRESH;
    }
  else if ((alkind_isStatic (aother) && sRef_isConst (res))
	   || (alkind_isStatic (ares) && sRef_isConst (other)))
    {
      if (!alkind_isStatic (ares))
	{
	  res->aliasinfo = stateInfo_update (res->aliasinfo, other->aliasinfo);
	  res->aliaskind = AK_STATIC;
	}
    }
  else
    {
      sRef_combineAliasKindsError (res, other, cl, loc);
    }
}

static void sRef_combineDefState (/*@notnull@*/ sRef res, 
				  /*@notnull@*/ sRef other)
{
  sstate s1 = res->defstate;
  sstate s2 = other->defstate;
  bool flip = FALSE;

  sRef_checkMutable (res);

  if (s1 == s2 || s2 == SS_UNKNOWN)
    {
      ;
    }
  else if (s1 == SS_UNKNOWN)
    {
      flip = TRUE;
    }
  else
    {
      switch (s1)
	{
	case SS_FIXED:   
	  if (s2 == SS_DEFINED) 
	    {
	      break;
	    }
	  else
	    {
	      llcontbuglit ("ssfixed: not implemented");
	      flip = TRUE;
	    }
	  break;
	case SS_DEFINED: 
	  flip = TRUE;
	  break;
	case SS_PDEFINED:
	case SS_ALLOCATED: 
	  flip = (s2 != SS_DEFINED);
	  break;
	case SS_HOFFA:
	case SS_RELDEF:
	case SS_UNUSEABLE: 
	case SS_UNDEFINED: 
	case SS_PARTIAL:
	case SS_UNDEFGLOB:
	case SS_KILLED:
	case SS_DEAD:      
	case SS_SPECIAL: 
	  break;
	BADDEFAULT;
	}
    }

  if (flip)
    {
      res->definfo = stateInfo_update (res->definfo, other->definfo);
      res->defstate = s2;
    }
}

extern /*@notnull@*/ sRef sRef_getConjA (sRef s)
{
  sRef ret;
  llassert (sRef_isConj (s));

  ret = s->info->conj->a;
  llassert (ret != NULL);
  return ret;
}

extern /*@notnull@*/ sRef sRef_getConjB (sRef s)
{
  sRef ret;
  llassert (sRef_isConj (s));

  ret = s->info->conj->b;
  llassert (ret != NULL);
  return ret;
}
  
extern /*@exposed@*/ sRef sRef_makeArrow (sRef s, /*@dependent@*/ cstring f)
{
  sRef p;
  sRef ret;
  
  p = sRef_makePointer (s);
  ret = sRef_makeField (p, f);
  DPRINTF (("Arrow: %s => %s",
	    sRef_unparseFull (s), sRef_unparseFull (ret)));
  return ret;
}

extern /*@exposed@*/ sRef sRef_buildArrow (sRef s, cstring f)
{
  sRef p;
  sRef ret;

  p = sRef_buildPointer (s);
  ret = sRef_buildField (p, f);
  
  return ret;
}

static /*@null@*/ sinfo sinfo_copy (/*@notnull@*/ sRef s)
{
  sinfo ret;

  switch (s->kind)
    {
    case SK_CVAR:
      ret = (sinfo) dmalloc (sizeof (*ret));
      ret->cvar = (cref) dmalloc (sizeof (*ret->cvar));
      ret->cvar->lexlevel = s->info->cvar->lexlevel; 
      ret->cvar->index = s->info->cvar->index; 
      break;

    case SK_PARAM:
      ret = (sinfo) dmalloc (sizeof (*ret));
      ret->paramno = s->info->paramno; 
      llassert (ret->paramno >= -1);
      break;

    case SK_ARRAYFETCH:
      ret = (sinfo) dmalloc (sizeof (*ret));
      ret->arrayfetch = (ainfo) dmalloc (sizeof (*ret->arrayfetch));
      ret->arrayfetch->indknown = s->info->arrayfetch->indknown;
      ret->arrayfetch->ind = s->info->arrayfetch->ind;
      ret->arrayfetch->arr = s->info->arrayfetch->arr; 
      break;

    case SK_FIELD:
      ret = (sinfo) dmalloc (sizeof (*ret));
      ret->field = (fldinfo) dmalloc (sizeof (*ret->field));
      ret->field->rec = s->info->field->rec; 
      ret->field->field = s->info->field->field; 
      break;

    case SK_OBJECT:
      ret = (sinfo) dmalloc (sizeof (*ret));
      ret->object = s->info->object;
      break;

    case SK_PTR:
    case SK_ADR:
    case SK_DERIVED:
    case SK_EXTERNAL:
      ret = (sinfo) dmalloc (sizeof (*ret));
      ret->ref = s->info->ref; /* Ref_copy (s->info->ref); */
      break;

    case SK_CONJ:
      ret = (sinfo) dmalloc (sizeof (*ret));
      ret->conj = (cjinfo) dmalloc (sizeof (*ret->conj));
      ret->conj->a = s->info->conj->a; /* sRef_copy (s->info->conj->a); */
      ret->conj->b = s->info->conj->b; /* sRef_copy (s->info->conj->b);*/
      break;
    case SK_SPECIAL:
      ret = (sinfo) dmalloc (sizeof (*ret));
      ret->spec = s->info->spec;
      break;

    case SK_UNCONSTRAINED:
    case SK_NEW:
      ret = (sinfo) dmalloc (sizeof (*ret));
      ret->fname = s->info->fname;
      break;

    case SK_RESULT:
    case SK_CONST:
    case SK_TYPE:
    case SK_UNKNOWN:
      llassertprint (s->info == NULL, ("s = %s", sRef_unparse (s)));
      ret = NULL;
      break;
    }

  return ret;
}

static /*@null@*/ sinfo sinfo_fullCopy (/*@notnull@*/ sRef s)
{
  sinfo ret;

  /*
  ** Since its a full copy, only storage is assigned
  ** to dependent fields.
  */
  /*@-onlytrans@*/

  switch (s->kind)
    {
    case SK_CVAR:
      ret = (sinfo) dmalloc (sizeof (*ret));
      ret->cvar = (cref) dmalloc (sizeof (*ret->cvar));
      ret->cvar->lexlevel = s->info->cvar->lexlevel; 
      ret->cvar->index = s->info->cvar->index; 
      break;

    case SK_PARAM:
      ret = (sinfo) dmalloc (sizeof (*ret));
      ret->paramno = s->info->paramno; 
      llassert (ret->paramno >= -1);
      break;

    case SK_ARRAYFETCH:
      ret = (sinfo) dmalloc (sizeof (*ret));
      ret->arrayfetch = (ainfo) dmalloc (sizeof (*ret->arrayfetch));
      ret->arrayfetch->indknown = s->info->arrayfetch->indknown;
      ret->arrayfetch->ind = s->info->arrayfetch->ind;
      ret->arrayfetch->arr = sRef_saveCopy (s->info->arrayfetch->arr);
      break;

    case SK_FIELD:
      ret = (sinfo) dmalloc (sizeof (*ret));
      ret->field = (fldinfo) dmalloc (sizeof (*ret->field));
      ret->field->rec = sRef_saveCopy (s->info->field->rec);
      ret->field->field = s->info->field->field; 
      break;

    case SK_OBJECT:
      ret = (sinfo) dmalloc (sizeof (*ret));
      ret->object = s->info->object;
      break;

    case SK_PTR:
    case SK_ADR:
    case SK_DERIVED:
    case SK_EXTERNAL:
      ret = (sinfo) dmalloc (sizeof (*ret));
      ret->ref = sRef_saveCopy (s->info->ref);	 
      break;

    case SK_CONJ:
      ret = (sinfo) dmalloc (sizeof (*ret));
      ret->conj = (cjinfo) dmalloc (sizeof (*ret->conj));
      ret->conj->a = sRef_saveCopy (s->info->conj->a);
      ret->conj->b = sRef_saveCopy (s->info->conj->b);
      break;
    case SK_SPECIAL:
      ret = (sinfo) dmalloc (sizeof (*ret));
      ret->spec = s->info->spec;
      break;
    case SK_NEW:
    case SK_UNCONSTRAINED:
      ret = (sinfo) dmalloc (sizeof (*ret));
      ret->fname = s->info->fname;
      break;
    case SK_CONST:
    case SK_TYPE:
    case SK_RESULT:
    case SK_UNKNOWN:
      llassert (s->info == NULL);
      ret = NULL;
      break;
    }
  /*@=onlytrans@*/ 
  return ret;
}


static void 
  sinfo_update (/*@notnull@*/ /*@exposed@*/ sRef res, 
		/*@notnull@*/ /*@exposed@*/ sRef other)
{
  llassert (res->kind == other->kind);

  switch (res->kind)
    {
    case SK_CVAR:
      res->info->cvar->lexlevel = other->info->cvar->lexlevel; 
      res->info->cvar->index = other->info->cvar->index; 
      break;

    case SK_PARAM:
      res->info->paramno = other->info->paramno; 
      llassert (res->info->paramno >= -1);
      break;

    case SK_ARRAYFETCH:
      res->info->arrayfetch->indknown = other->info->arrayfetch->indknown;
      res->info->arrayfetch->ind = other->info->arrayfetch->ind;
      res->info->arrayfetch->arr = other->info->arrayfetch->arr;
      break;

    case SK_FIELD:
      res->info->field->rec = other->info->field->rec;
      res->info->field->field = other->info->field->field; 
      break;

    case SK_OBJECT:
      res->info->object = other->info->object;
      break;

    case SK_PTR:
    case SK_ADR:
    case SK_DERIVED:
    case SK_EXTERNAL:
      res->info->ref = other->info->ref;	 
      break;

    case SK_CONJ:
      res->info->conj->a = other->info->conj->a;
      res->info->conj->b = other->info->conj->b;
      break;

    case SK_SPECIAL:
      res->info->spec = other->info->spec;
      break;

    case SK_NEW:
    case SK_UNCONSTRAINED:
      res->info->fname = other->info->fname;
      break;

    case SK_CONST:
    case SK_TYPE:
    case SK_UNKNOWN:
    case SK_RESULT:
      llassert (res->info == NULL);
      break;
    }
}

static void sinfo_free (/*@special@*/ /*@temp@*/ /*@notnull@*/ sRef s)
   /*@uses s->kind, s->info@*/
   /*@releases s->info@*/ 
{
  switch (s->kind)
    {
    case SK_CVAR:
      DPRINTF (("Free sinfo: [%p]", s->info->cvar));
      sfree (s->info->cvar);
      break;

    case SK_PARAM:
      break;

    case SK_ARRAYFETCH:
      DPRINTF (("Free sinfo: [%p]", s->info->arrayfetch));
      sfree (s->info->arrayfetch);
      break;

    case SK_FIELD:
      DPRINTF (("Free sinfo: [%p]", s->info->field));
      sfree (s->info->field); 
      break;

    case SK_OBJECT:
      break;

    case SK_PTR:
    case SK_ADR:
    case SK_DERIVED:
    case SK_EXTERNAL: /* is copy now! */
      break;

    case SK_CONJ:
      DPRINTF (("Free sinfo: [%p]", s->info->conj));
      sfree (s->info->conj);
      break;

    case SK_UNCONSTRAINED:
    case SK_SPECIAL:
    case SK_CONST:
    case SK_NEW:
    case SK_TYPE:
    case SK_UNKNOWN:
    case SK_RESULT:
      break;
    }

  if (s->info != NULL) {
      DPRINTF (("Free sinfo: [%p]", s->info));
  }

  sfree (s->info);
}

bool sRef_isNSLocalVar (sRef s)  
{
  if (sRef_isLocalVar (s))
    {
      uentry ue = sRef_getUentry (s);

      return (!uentry_isStatic (ue));
    }
  else
    {
      return FALSE;
    }
}

bool sRef_isLocalVar (sRef s)  
{
  if (sRef_isValid(s))
    {
      return (s->kind == SK_CVAR 
	      && (s->info->cvar->lexlevel > fileScope));
    }
  
  return FALSE;
}

bool sRef_isRealLocalVar (sRef s)  
{
  if (sRef_isValid(s))
    {
      if (s->kind == SK_CVAR)
	{
	  if (s->info->cvar->lexlevel == functionScope)
	    {
	      uentry ue = sRef_getUentry (s);

	      if (uentry_isAnyParam (ue)
		  || uentry_isRefParam (ue))
		{
		  return FALSE;
		}
	      else
		{
		  return TRUE;
		}
	    }
	  else
	    {
	      return (s->info->cvar->lexlevel > functionScope);
	    }
	}
    }
  
  return FALSE;
}

bool sRef_isLocalParamVar (sRef s)  
{
  if (sRef_isValid(s))
    {
      return (s->kind == SK_PARAM
	      || (s->kind == SK_CVAR 
		  && (s->info->cvar->lexlevel > fileScope)));
    }
  
  return FALSE;
}

static speckind speckind_fromInt (int i)
{
  /*@+enumint@*/ 
  llassert (i >= SR_NOTHING && i <= SR_SYSTEM); 
  /*@=enumint@*/

  return ((speckind) i);
}


static void sRef_updateNullState (/*@notnull@*/ sRef res, /*@notnull@*/ sRef other)
     /*@modifies res@*/
{
  res->nullstate = other->nullstate;
  DPRINTF (("update null state==> %s", sRef_unparseFull (res)));
  res->nullinfo = stateInfo_update (res->nullinfo, other->nullinfo);
  sRef_resetAliasKind (res);
}

void sRef_combineNullState (/*@notnull@*/ sRef res, /*@notnull@*/ sRef other)
{
  nstate n1 = sRef_getNullState (res);
  nstate n2 = sRef_getNullState (other);
  bool flip = FALSE;
  nstate nn = n1;

  if (n1 == n2 || n2 == NS_UNKNOWN)
    {
      ;
    }
  else
    {
      /* note: n2 is not unknown or defnull */

      switch (n1)
	{
	case NS_ERROR:   nn = NS_ERROR; break;
	case NS_UNKNOWN: flip = TRUE; nn = n2; break; 
	case NS_POSNULL: break;
	case NS_DEFNULL: nn = NS_POSNULL; break;
	case NS_RELNULL: break;
	case NS_NOTNULL:  
	  if (n2 == NS_MNOTNULL)
	    {
	      ;
	    }
	  else 
	    { 
	      flip = TRUE;
	      nn = NS_POSNULL; 
	    }
	  break;
	case NS_MNOTNULL: 
	  if (n2 == NS_NOTNULL) 
	    {
	      nn = NS_NOTNULL; 
	    }
	  else 
	    {
	      flip = TRUE;
	      nn = NS_POSNULL; 
	    }
	  break;
	case NS_CONSTNULL:
	case NS_ABSNULL:
	  flip = TRUE;
	  nn = n2;
	}
    }
  
  if (flip)
    {
      res->nullinfo = stateInfo_update (res->nullinfo, other->nullinfo);      
    }

  res->nullstate = nn;
  DPRINTF (("update null state==> %s", sRef_unparseFull (res)));
  sRef_resetAliasKind (res);
}

cstring sRef_nullMessage (sRef s)
{
  llassert (sRef_isReasonable (s));

  switch (sRef_getNullState (s))
    {
    case NS_DEFNULL:
    case NS_CONSTNULL:
      return (cstring_makeLiteralTemp ("null"));
    default:
      return (cstring_makeLiteralTemp ("possibly null"));
    }
  BADEXIT;
}

/*@observer@*/ cstring sRef_ntMessage (sRef s)
{
  llassert (sRef_isReasonable (s));

  switch (s->nullstate)
    {
    case NS_DEFNULL:
    case NS_CONSTNULL:
      return (cstring_makeLiteralTemp ("not nullterminated"));
    default:
      return (cstring_makeLiteralTemp ("possibly non-nullterminated"));
    }
  BADEXIT;
}



sRef sRef_fixResultType (/*@returned@*/ sRef s, ctype typ, uentry ue)
{
  sRef tmp = sRef_undefined;
  sRef ret;

  llassert (sRef_isReasonable (s));

  switch (s->kind)
    {
    case SK_RESULT:
      s->type = typ;
      ret = s;
      break;
    case SK_ARRAYFETCH:
      {
	ctype ct;
	tmp = sRef_fixResultType (s->info->arrayfetch->arr, typ, ue);

	ct = ctype_realType (sRef_getType (tmp));

	
	if (ctype_isKnown (ct))
	  {
	    if (ctype_isAP (ct))
	      {
		;
	      }
	    else
	      {
		voptgenerror 
		  (FLG_TYPE,
		   message
		   ("Special clause indexes non-array (%t): *%q",
		    ct, sRef_unparse (s->info->arrayfetch->arr)),
		   uentry_whereLast (ue));
	      }
	  }

	tmp = sRef_fixResultType (s->info->arrayfetch->arr, typ, ue);

	if (s->info->arrayfetch->indknown)
	  {
	    ret = sRef_makeArrayFetchKnown (tmp, s->info->arrayfetch->ind);
	  }
	else
	  {
	    ret = sRef_makeArrayFetch (tmp);
	  }
      }
      break;
    case SK_FIELD:
      {
	sRef rec = sRef_fixResultType (s->info->field->rec, typ, ue);
	ctype ct = ctype_realType (sRef_getType (rec));

	if (ctype_isKnown (ct))
	  {
	    if (ctype_isSU (ct))
	      {
		if (uentry_isValid (uentryList_lookupField (ctype_getFields (ct), 
							    s->info->field->field)))
		  {
		    ;
		  }
		else
		  {
		    voptgenerror 
		      (FLG_TYPE,
		       message
		       ("Special clause accesses non-existent field of result: %q.%s",
			sRef_unparse (rec), s->info->field->field),
		       uentry_whereLast (ue));
		  }
	      }
	    else
	      {
		voptgenerror 
		  (FLG_TYPE,
		   message
		   ("Special clause accesses field of non-struct or union result (%t): %q.%s",
		    ct, sRef_unparse (rec), s->info->field->field),
		   uentry_whereLast (ue));
	      }
	  }
	
	ret = sRef_makeField (tmp, s->info->field->field);
	break;
      }
    case SK_PTR:
      {
	ctype ct;
	tmp = sRef_fixResultType (s->info->ref, typ, ue);

	ct = ctype_realType (sRef_getType (tmp));

	if (ctype_isKnown (ct))
	  {
	    if (ctype_isAP (ct))
	      {
		;
	      }
	    else
	      {
		voptgenerror 
		  (FLG_TYPE,
		   message
		   ("Special clause dereferences non-pointer (%t): *%q",
		    ct, sRef_unparse (s->info->ref)),
		   uentry_whereLast (ue));
	      }
	  }
	
	ret = sRef_makePointer (tmp);
	break;
      }
    case SK_ADR:
      voptgenerror 
	(FLG_TYPE,
	 message
	 ("Special clause uses & operator (not allowed): &%q", sRef_unparse (s->info->ref)),
	 uentry_whereLast (ue));
      ret = s;
      break;
    BADDEFAULT;
    }

  return ret;
}

bool sRef_isOnly (sRef s)
{
  return (sRef_isValid(s) && alkind_isOnly (s->aliaskind));
}

bool sRef_isDependent (sRef s) 
{
  return (sRef_isValid(s) && alkind_isDependent (s->aliaskind));
}

bool sRef_isOwned (sRef s)
{
  return (sRef_isReasonable (s) && (s->aliaskind == AK_OWNED));
}

bool sRef_isKeep (sRef s) 
{
  return (sRef_isReasonable (s) && (s->aliaskind == AK_KEEP));
}

bool sRef_isTemp (sRef s)
{
  return (sRef_isReasonable (s) && alkind_isTemp (s->aliaskind));
}

bool sRef_isLocalState (sRef s) 
{
  return (sRef_isReasonable (s) && (s->aliaskind == AK_LOCAL));
}

bool sRef_isUnique (sRef s)
{
  return (sRef_isReasonable (s) && (s->aliaskind == AK_UNIQUE));
}

bool sRef_isShared (sRef s) 
{
  return (sRef_isReasonable (s) && (s->aliaskind == AK_SHARED));
}

bool sRef_isExposed (sRef s) 
{
  return (sRef_isReasonable (s) && (s->expkind == XO_EXPOSED));
}

bool sRef_isObserver (sRef s) 
{
  return (sRef_isReasonable (s) && (s->expkind == XO_OBSERVER));
}

bool sRef_isFresh (sRef s) 
{
  return (sRef_isReasonable (s) && (s->aliaskind == AK_FRESH));
}

bool sRef_isDefinitelyNull (sRef s) 
{
  return (sRef_isReasonable (s) && (sRef_getNullState (s) == NS_DEFNULL 
			       || sRef_getNullState (s) == NS_CONSTNULL));
}

bool sRef_isAllocated (sRef s)
{
  return (sRef_isReasonable (s) && (s->defstate == SS_ALLOCATED));
}

bool sRef_isStack (sRef s)
{
  return (sRef_isReasonable (s) && (s->aliaskind == AK_STACK));
}

bool sRef_isNotNull (sRef s)
{
  return (sRef_isValid(s) && (sRef_getNullState (s) == NS_MNOTNULL 
			      || sRef_getNullState (s) == NS_NOTNULL));
}

alkind sRef_getAliasKind (sRef s)
{
  if (sRef_isValid(s)) {
    llassert (alkind_isValid (s->aliaskind));
    return s->aliaskind;
  }

  return AK_ERROR;
}

nstate sRef_getNullState (sRef s)
{
  if (sRef_isReasonable (s)) {
    llassert (nstate_isValid (s->nullstate));
    return s->nullstate;
  }
  
  return NS_UNKNOWN;
}

void sRef_reflectAnnotation (sRef s, annotationInfo a, fileloc loc)
{
  if (sRef_isReasonable (s))
    {
      if (!valueTable_isDefined (s->state))
	{
	  s->state = valueTable_create (1);
	  valueTable_insert
	    (s->state, 
	     cstring_copy (metaStateInfo_getName (annotationInfo_getState (a))),
	     stateValue_create (annotationInfo_getValue (a), 
				stateInfo_makeLoc (loc, SA_DECLARED)));
	}
      else
	{
	  DPRINTF (("reflect loc: %s", fileloc_unparse (loc)));
	  valueTable_update 
	    (s->state,
	     metaStateInfo_getName (annotationInfo_getState (a)),
	     stateValue_create (annotationInfo_getValue (a),
				stateInfo_makeLoc (loc, SA_DECLARED)));
	  
	  DPRINTF (("sref: %s", sRef_unparse (s)));
	  DPRINTF (("sref: %s", sRef_unparseFull (s)));
	}
    }
}

void sRef_setMetaStateValueComplete (sRef s, cstring key, int value, fileloc loc)
{
  sRefSet aliases = usymtab_allAliases (s);

  sRef_setMetaStateValue (s, key, value, loc);

  sRefSet_realElements (aliases, current)
    {
      if (sRef_isReasonable (current))
	{
	  current = sRef_updateSref (current);
	  sRef_setMetaStateValue (current, key, value, loc);
	}
    } end_sRefSet_realElements ;

  sRefSet_free (aliases);
}

void sRef_setMetaStateValue (sRef s, cstring key, int value, fileloc loc)
{
  sRef_checkMutable (s);

  if (sRef_isReasonable (s))
    {
      if (!valueTable_isDefined (s->state))
	{
	  DPRINTF (("inserting state: %s: %s %d", sRef_unparse (s), key, value));
	  s->state = valueTable_create (1);
	  valueTable_insert (s->state, cstring_copy (key),
			     stateValue_create (value, 
						stateInfo_makeLoc (loc, SA_CHANGED)));
	}
      else
	{
	  DPRINTF (("Updating state: %s: %s %d / %s", sRef_unparse (s), key, value,
		    fileloc_unparse (loc)));
	  if (valueTable_contains (s->state, key))
	    {
	      valueTable_update 
		(s->state, key, stateValue_create (value,
						   stateInfo_makeLoc (loc, SA_CHANGED)));
	    }
	  else
	    {
	      valueTable_insert 
		(s->state, cstring_copy (key),
		 stateValue_create (value, stateInfo_makeLoc (loc, SA_CHANGED)));
	    }

	  DPRINTF (("After: %s", sRef_unparseFull (s)));
	}
    }
}

bool sRef_checkMetaStateValue (sRef s, cstring key, int value)
{
  if (sRef_isReasonable (s))
    {
      if (valueTable_isDefined (s->state))
	{
	  stateValue val;
	  
	  DPRINTF (("check state: %s: %s %d", sRef_unparse (s), key, value));
	  
	  val = valueTable_lookup (s->state, key);
	  llassert (stateValue_isDefined (val));
	  return (stateValue_isError (val)
		  || stateValue_getValue (val) == value);
	}
      else
	{
	  return TRUE;
	}
    }
  else
    {
      return TRUE;
    }
}

/*@observer@*/ stateValue sRef_getMetaStateValue (sRef s, cstring key)
{
  if (sRef_isReasonable (s))
    {
      if (valueTable_isDefined (s->state))
	{
	  stateValue val;
	  
	  val = valueTable_lookup (s->state, key);
	  /* Okay if its not defined, just returns stateValue_undefined */
	  return val;
	}
      else
	{
	  return stateValue_undefined;
	}
    }
  else
    {
      return stateValue_undefined;
    }
}

/*@observer@*/ valueTable sRef_getValueTable (sRef s) 
{
  DPRINTF (("Get value table: %s", sRef_unparse (s)));

  if (sRef_isReasonable (s)) 
    {
      llassert (sRef_isReasonable (s));
      DPRINTF (("Value table: %s", valueTable_unparse (s->state)));
      return s->state;
    }  
  else 
    {
      DPRINTF (("No value table!"));
      return valueTable_undefined;
    }
}

bool sRef_makeStateSpecial (sRef s)
{
  /*
  ** Default defined state can be made special.
  */

  llassert (sRef_isReasonable (s));

  if (s->defstate == SS_UNKNOWN || s->defstate == SS_DEFINED || s->defstate == SS_SPECIAL)
    {
      /* s->aliaskind = AK_IMPTEMP; */ /* evans 2001-07-23 shouldn't effect alias state */
      s->defstate = SS_SPECIAL;
      DPRINTF (("Made special: %s", sRef_unparseFull (s)));
      return TRUE;
    }
  else
    {
      /* s->aliaskind = AK_IMPTEMP; */
      s->defstate = SS_SPECIAL;
      return FALSE;
    }
}

void sRef_markImmutable (sRef s)
{
  if (sRef_isReasonable (s))
    {
      DPRINTF (("Mark immutable: %s", sRef_unparseFull (s)));
      s->immut = TRUE;
    }
}

bool sRef_definitelyNullContext (sRef s)
{
  return (sRef_definitelyNull (s)
	  || usymtab_isDefinitelyNullDeep (s));
}

bool sRef_definitelyNullAltContext (sRef s)
{
  return (sRef_definitelyNull (s)
	  || usymtab_isAltDefinitelyNullDeep (s));
}


/* start modifications */
struct s_bbufinfo sRef_getNullTerminatedState (sRef p_s) {
   struct s_bbufinfo BUFSTATE_UNKNOWN;
   BUFSTATE_UNKNOWN.bufstate = BB_NOTNULLTERMINATED;
   BUFSTATE_UNKNOWN.size = 0;
   BUFSTATE_UNKNOWN.len = 0;
   
   if (sRef_isValid(p_s))
      return p_s->bufinfo;
   return BUFSTATE_UNKNOWN; 
}

void sRef_setNullTerminatedState(sRef p_s) {
   if(sRef_isReasonable (p_s)) {
      p_s->bufinfo.bufstate = BB_NULLTERMINATED;
   } else {
      llfatalbug( message("sRef_setNT passed a invalid sRef\n"));
   }
}


void sRef_setPossiblyNullTerminatedState(sRef p_s) {
   if( sRef_isReasonable (p_s)) {
      p_s->bufinfo.bufstate = BB_POSSIBLYNULLTERMINATED;
   } else {
      llfatalbug( message("sRef_setPossNT passed a invalid sRef\n"));
   }
}

void sRef_setNotNullTerminatedState(sRef p_s) {
   if( sRef_isReasonable (p_s)) {
      p_s->bufinfo.bufstate = BB_NOTNULLTERMINATED;
   } else {
      llfatalbug( message("sRef_unsetNT passed a invalid sRef\n"));
   }
}

void sRef_setLen(sRef p_s, int len) {
   if (sRef_isReasonable (p_s) && sRef_isNullTerminated(p_s)) 
     {
       p_s->bufinfo.len = len;
     } 
   else 
     {
       llfatalbug( message("sRef_setLen passed an invalid sRef\n"));
     }
}
    

void sRef_setSize(sRef p_s, int size) {
   if( sRef_isValid(p_s)) 
     {
       p_s->bufinfo.size = size;
     } 
   else
     {
       llfatalbug( message("sRef_setSize passed a invalid sRef\n"));
     }
}

void sRef_resetLen(sRef p_s) {
  if (sRef_isReasonable (p_s)) 
    {
      p_s->bufinfo.len = 0;
    }
  else
    {
      llfatalbug (message ("sRef_setLen passed an invalid sRef\n"));
    }
}

/*drl7x 11/28/2000 */

bool sRef_isFixedArray (sRef p_s) /*@*/ {
  ctype c;
  c = sRef_getType (p_s);
  return ( ctype_isFixedArray (c) );
}

size_t sRef_getArraySize (sRef p_s) /*@*/ 
{
  ctype c;
  llassert (sRef_isFixedArray(p_s) );
  DPRINTF (( message ("sRef_getArraySize getting array size for %s", sRef_unparse(p_s) )  ));
  
  c = sRef_getType (p_s);
  return (ctype_getArraySize (c));
}

void sRef_setValue (sRef s, multiVal val)
{
  if (!sRef_isReasonable (s))
    {
      llcontbuglit ("Unreasonable sRef");
      llcontbug (message ("sRef: %s", sRef_unparse (s)));
      return;
    }

  multiVal_free (s->val);
  s->val = val;
}

bool sRef_hasValue (sRef s)
{
  return (sRef_isReasonable (s)
	  && multiVal_isDefined (s->val));
}

multiVal sRef_getValue (sRef s)
{
  if (sRef_isReasonable (s))
    {
      return s->val;
    }

  return multiVal_undefined;
}
