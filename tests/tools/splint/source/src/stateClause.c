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
** stateClause.c
*/

# include "splintMacros.nf"
# include "basic.h"
# include "cgrammar.h"
# include "cgrammar_tokens.h"

static stateClause 
stateClause_createRaw (stateConstraint st, stateClauseKind sk, /*@only@*/ sRefSet s) 
{
  stateClause ret = (stateClause) dmalloc (sizeof (*ret));

  ret->state = st;
  ret->kind = sk;
  ret->squal = qual_createUnknown ();
  ret->refs = s;
  ret->loc = fileloc_undefined;
  return ret;
}

/*drl added 3/7/2003*/
bool stateClause_hasEmptyReferences (stateClause s)
{
  if (sRefSet_isUndefined(s->refs) )
    return TRUE;
    else
    return FALSE;
}

bool stateClause_isMetaState (stateClause s)
{

  if (qual_isMetaState (s->squal) )
    return TRUE;
  else
    return FALSE;
}
/*end drl added*/

stateClause 
stateClause_create (lltok tok, qual q, sRefSet s) 
{
  stateClause ret = (stateClause) dmalloc (sizeof (*ret));

  if (lltok_getTok (tok) == QPRECLAUSE) 
    {
      ret->state = TK_BEFORE;
    }
  else if (lltok_getTok (tok) == QPOSTCLAUSE)
    {
      ret->state = TK_AFTER;
    }
  else
    {
      BADBRANCH;
    }

  ret->loc = fileloc_copy (lltok_getLoc (tok));

  ret->squal = q;
  ret->refs = s;

  if (sRefSet_isDefined (s))
    {
      ret->kind = SP_QUAL;
    }
  else
    {
      ret->kind = SP_GLOBAL;
    }

  return ret;
}

bool stateClause_isBeforeOnly (stateClause cl)
{
  return (cl->state == TK_BEFORE);
}

bool stateClause_isBefore (stateClause cl)
{
  return (cl->state == TK_BEFORE || cl->state == TK_BOTH);
}

bool stateClause_isAfter (stateClause cl)
{
  return (cl->state == TK_AFTER || cl->state == TK_BOTH);
}

bool stateClause_isEnsures (stateClause cl)
{
  return (cl->state == TK_AFTER);
}

bool stateClause_isQual (stateClause cl)
{
  return (cl->kind == SP_QUAL);
}

bool stateClause_isMemoryAllocation (stateClause cl)
{
  switch (cl->kind)
    {
    case SP_ALLOCATES:
    case SP_RELEASES:
      return TRUE;
    case SP_USES:
    case SP_DEFINES:
    case SP_SETS:
      return FALSE;
    case SP_GLOBAL:
      return FALSE;
    case SP_QUAL:
      return (qual_isMemoryAllocation (cl->squal)
	      || qual_isSharing (cl->squal));
    }
  
  BADEXIT;
}

/*
** An error is reported if the test is NOT true.
*/

# ifdef WIN32
/* Microsoft doesn't believe in higher order functions... */
# pragma warning( disable : 4550 )
# endif  

sRefTest stateClause_getPreTestFunction (stateClause cl)
{
  switch (cl->kind)
    {
    case SP_USES:
      return sRef_isStrictReadable;
    case SP_ALLOCATES:
      return sRef_hasNoStorage; 
    case SP_DEFINES:
      return sRef_hasNoStorage;
    case SP_SETS:
      return sRef_isNotUndefined;
    case SP_RELEASES:
      return sRef_isNotUndefined;
    case SP_GLOBAL:
      BADBRANCH;
    case SP_QUAL:
      {
	if (qual_isOnly (cl->squal)) {
	  return sRef_isOnly;
	} else if (qual_isShared (cl->squal)) {
	  return sRef_isShared;
	} else if (qual_isDependent (cl->squal)) {
	  return sRef_isDependent;
	} else if (qual_isOwned (cl->squal)) {
	  return sRef_isOwned;
	} else if (qual_isObserver (cl->squal)) {
	  return sRef_isObserver;
	} else if (qual_isExposed (cl->squal)) {
	  return sRef_isExposed;
	} else if (qual_isNotNull (cl->squal)) {
	  return sRef_isNotNull;
	} else if (qual_isIsNull (cl->squal)) {
	  return sRef_isDefinitelyNull;
	} else {
	  BADBRANCH;
	}
      }
  }

  BADEXIT;
}

sRefTest stateClause_getPostTestFunction (stateClause cl)
{
  llassert (stateClause_isAfter (cl));

  switch (cl->kind)
    {
    case SP_USES:
      return NULL;
    case SP_ALLOCATES:
      return sRef_isAllocated;
    case SP_DEFINES:
      return sRef_isReallyDefined;
    case SP_SETS:
      return sRef_isReallyDefined;
    case SP_RELEASES:
      return sRef_isDeadStorage;
    case SP_GLOBAL:
      BADBRANCH;
    case SP_QUAL:
      if (qual_isOnly (cl->squal)) {
	return sRef_isOnly;
      } else if (qual_isShared (cl->squal)) {
	return sRef_isShared;
      } else if (qual_isDependent (cl->squal)) {
	return sRef_isDependent;
      } else if (qual_isOwned (cl->squal)) {
	return sRef_isOwned;
      } else if (qual_isObserver (cl->squal)) {
	return sRef_isObserver;
      } else if (qual_isExposed (cl->squal)) {
	return sRef_isExposed;
      } else if (qual_isNotNull (cl->squal)) {
	return sRef_isNotNull;
      } else if (qual_isIsNull (cl->squal)) {
	return sRef_isDefinitelyNull;
      } else {
	BADBRANCH;
      }
    }
  
  BADEXIT;
}

sRefShower stateClause_getPostTestShower (stateClause cl)
{
  switch (cl->kind)
    {
    case SP_USES:
    case SP_ALLOCATES:
      return NULL;
    case SP_DEFINES:
    case SP_SETS:
      return sRef_showNotReallyDefined;
    case SP_RELEASES:
      return NULL;
    case SP_GLOBAL:
      BADBRANCH;
    case SP_QUAL:
      if (qual_isMemoryAllocation (cl->squal)) {
	return sRef_showAliasInfo;
      } else if (qual_isSharing (cl->squal)) {
	return sRef_showExpInfo;
      } else if (qual_isIsNull (cl->squal) || qual_isNotNull (cl->squal)) {
	return sRef_showNullInfo;
      } else {
	BADBRANCH;
      }
    }

  BADEXIT;
}

sRefMod stateClause_getEntryFunction (stateClause cl)
{
  if (cl->state == TK_BEFORE || cl->state == TK_BOTH)
    {
      switch (cl->kind)
	{
	case SP_USES:
	  return sRef_setDefinedComplete;
	case SP_ALLOCATES:
	  return sRef_setUndefined; /* evans 2002-01-01 */
	case SP_DEFINES:
	  return sRef_setUndefined; /* evans 2002-01-01 */
	case SP_SETS:
	  return sRef_setAllocatedComplete;
	case SP_RELEASES:
	  return sRef_setDefinedComplete;
	case SP_GLOBAL:
	  BADBRANCH;
	case SP_QUAL:
	  if (qual_isOnly (cl->squal)) {
	    return sRef_setOnly;
	  } else if (qual_isShared (cl->squal)) {
	    return sRef_setShared;
	  } else if (qual_isDependent (cl->squal)) {
	    return sRef_setDependent;
	  } else if (qual_isOwned (cl->squal)) {
	    return sRef_setOwned;
	  } else if (qual_isObserver (cl->squal)) {
	    return sRef_setObserver;
	  } else if (qual_isExposed (cl->squal)) {
	    return sRef_setExposed;
	  } else if (qual_isNotNull (cl->squal)) {
	    return sRef_setNotNull;
	  } else if (qual_isIsNull (cl->squal)) {
	    return sRef_setDefNull;
	  } else {
	    DPRINTF (("Here we are: %s", 
		      qual_unparse (cl->squal)));
	    BADBRANCH;
	  }
	}
      
      BADBRANCH;
    }
  else
    {
      return NULL;
    }

  BADBRANCHNULL;
}

sRefMod stateClause_getEffectFunction (stateClause cl)
{
  if (cl->state == TK_AFTER || cl->state == TK_BOTH)
    {
      switch (cl->kind)
	{
	case SP_USES:
	  return NULL;
	case SP_ALLOCATES:
	  return sRef_setAllocatedComplete;
	case SP_DEFINES:
	  return sRef_setDefinedNCComplete;
	case SP_SETS:
	  return sRef_setDefinedNCComplete;
	case SP_RELEASES:
	  return sRef_killComplete;
	case SP_GLOBAL:
	  BADBRANCH;
	case SP_QUAL:
	  if (qual_isOnly (cl->squal)) {
	    return sRef_setOnly;
	  } else if (qual_isShared (cl->squal)) {
	    return sRef_setShared;
	  } else if (qual_isDependent (cl->squal)) {
	    return sRef_setDependent;
	  } else if (qual_isOwned (cl->squal)) {
	    return sRef_setOwned;
	  } else if (qual_isObserver (cl->squal)) {
	    return sRef_setObserver;
	  } else if (qual_isExposed (cl->squal)) {
	    return sRef_setExposed;
	  } else if (qual_isNotNull (cl->squal)) {
	    return sRef_setNotNull;
	  } else if (qual_isIsNull (cl->squal)) {
	    return sRef_setDefNull;
	  } else {
	    BADBRANCH;
	  }
	}

      BADBRANCH;
    }
  else
    {
      return NULL;
    }

  BADBRANCHNULL;
}

sRefMod stateClause_getReturnEffectFunction (stateClause cl)
{
  if (cl->state == TK_AFTER || cl->state == TK_BOTH)
    {
      switch (cl->kind)
	{
	case SP_USES:
	case SP_ALLOCATES:
	case SP_DEFINES:
	case SP_SETS:
	case SP_RELEASES:
	  return NULL;
	case SP_GLOBAL:
	  BADBRANCH;
	case SP_QUAL:
	  if (qual_isOnly (cl->squal)) {
	    return sRef_killComplete;
	  } else {
	    return NULL;
	  }
	}

      BADBRANCH;
    }
  else
    {
      return NULL;
    }

  BADBRANCHNULL;
}

static flagcode stateClause_qualErrorCode (stateClause cl)
{
  if (qual_isOnly (cl->squal)) {
    return FLG_ONLYTRANS;
  } else if (qual_isShared (cl->squal)) {
    return FLG_SHAREDTRANS;
  } else if (qual_isDependent (cl->squal)) {
    return FLG_DEPENDENTTRANS;
  } else if (qual_isOwned (cl->squal)) {
    return FLG_OWNEDTRANS;
  } else if (qual_isObserver (cl->squal)) {
    return FLG_OBSERVERTRANS;
  } else if (qual_isExposed (cl->squal)) {
    return FLG_EXPOSETRANS;
  } else if (qual_isIsNull (cl->squal)
	     || qual_isNotNull (cl->squal)) {
    return FLG_NULLSTATE;
  } else {
    BADBRANCH;
  }

  BADBRANCHRET (INVALID_FLAG);
}

flagcode stateClause_preErrorCode (stateClause cl)
{
  llassert (cl->state == TK_BOTH || cl->state == TK_BEFORE);

  switch (cl->kind)
    {
    case SP_USES:
      return FLG_USEDEF;
    case SP_ALLOCATES: /*@fallthrough@*/ 
    case SP_DEFINES:
    case SP_SETS:
      return FLG_MUSTFREEONLY;
    case SP_RELEASES:
      return FLG_USEDEF;
    case SP_GLOBAL:
    case SP_QUAL:
      return stateClause_qualErrorCode (cl);
    }

  BADBRANCHRET (INVALID_FLAG);
}

static /*@observer@*/ cstring stateClause_qualErrorString (stateClause cl, sRef sr)
{
  if (qual_isMemoryAllocation (cl->squal)) {
    return alkind_capName (sRef_getAliasKind (sr));
  } else if (qual_isObserver (cl->squal)) {
    return cstring_makeLiteralTemp ("Non-observer");
  } else if (qual_isExposed (cl->squal)) {
    if (sRef_isObserver (sr))
      {
	return cstring_makeLiteralTemp ("Observer");
      }
    else
      {
	return cstring_makeLiteralTemp ("Non-exposed");
      }
  } else if (qual_isNotNull (cl->squal)) {
    if (sRef_isDefinitelyNull (sr))
      {
	return cstring_makeLiteralTemp ("Null");
      }
    else
      {
	return cstring_makeLiteralTemp ("Possibly null");
      }
  } else if (qual_isIsNull (cl->squal)) {
    return cstring_makeLiteralTemp ("Non-null");
  } else {
    BADBRANCH;
  }
  
  BADBRANCHRET (cstring_undefined);
}

cstring stateClause_preErrorString (stateClause cl, sRef sr)
{
  llassert (cl->state == TK_BOTH || cl->state == TK_BEFORE);

  switch (cl->kind)
    {
    case SP_USES:
      if (sRef_isDead (sr)) 
	return cstring_makeLiteralTemp ("Dead");
      else
	return cstring_makeLiteralTemp ("Undefined");
    case SP_ALLOCATES: /*@fallthrough@*/ 
    case SP_DEFINES:
    case SP_SETS:
      return cstring_makeLiteralTemp ("Allocated");
    case SP_RELEASES:
      if (sRef_isDead (sr)) 
	{
	  return cstring_makeLiteralTemp ("Dead");
	}
      else if (sRef_isDependent (sr) 
	       || sRef_isShared (sr))
	{
	  return alkind_unparse (sRef_getAliasKind (sr));
	}
      else if (sRef_isObserver (sr) || sRef_isExposed (sr))
	{
	  return exkind_unparse (sRef_getExKind (sr));
	}
      else
	{
	  return cstring_makeLiteralTemp ("Undefined");
	}
    case SP_GLOBAL:
      BADBRANCH;
    case SP_QUAL:
      return stateClause_qualErrorString (cl, sr);
    }
  
  BADEXIT;
}

flagcode stateClause_postErrorCode (stateClause cl)
{
  llassert (cl->state == TK_BOTH || cl->state == TK_AFTER);

  switch (cl->kind)
    {
    case SP_USES:
      BADBRANCHCONT;
      return INVALID_FLAG;
    case SP_ALLOCATES: 
    case SP_DEFINES:  
    case SP_SETS:     
      return FLG_COMPDEF;
    case SP_RELEASES:
      return FLG_MUSTFREEONLY;
    case SP_GLOBAL:
      BADBRANCH;
    case SP_QUAL:
      return stateClause_qualErrorCode (cl);
    }

  BADBRANCHRET (INVALID_FLAG);
}

cstring stateClause_postErrorString (stateClause cl, sRef sr)
{
  llassert (cl->state == TK_BOTH || cl->state == TK_AFTER);
  
  switch (cl->kind)
    {
    case SP_USES:
      BADBRANCHCONT;
      return cstring_makeLiteralTemp ("<ERROR>");
    case SP_ALLOCATES: 
      return cstring_makeLiteralTemp ("Unallocated");
    case SP_DEFINES:
    case SP_SETS:
      return cstring_makeLiteralTemp ("Undefined");
    case SP_RELEASES:
      return cstring_makeLiteralTemp ("Unreleased");
    case SP_GLOBAL:
      BADBRANCH;
    case SP_QUAL:
      return stateClause_qualErrorString (cl, sr);
    }

  BADEXIT;
}

cstring stateClause_dump (stateClause s)
{
  return (message ("%d.%d.%q.%q",
		   (int) s->state,
		   (int) s->kind,
		   qual_dump (s->squal),
		   sRefSet_dump (s->refs)));
}

stateClause stateClause_undump (char **s)
{
  stateClause ret = (stateClause) dmalloc (sizeof (*ret));

  ret->loc = fileloc_undefined;
  ret->state = (stateConstraint) reader_getInt (s);
  reader_checkChar (s, '.');
  ret->kind = (stateClauseKind) reader_getInt (s);
  reader_checkChar (s, '.');
  ret->squal = qual_undump (s);
  reader_checkChar (s, '.');
  ret->refs = sRefSet_undump (s);

  return ret;
}

stateClause stateClause_copy (stateClause s) 
{
  stateClause ret = (stateClause) dmalloc (sizeof (*ret));
  
  ret->state = s->state;
  ret->kind = s->kind;
  ret->squal = s->squal;
  ret->refs = sRefSet_newCopy (s->refs);
  ret->loc = fileloc_copy (s->loc);

  return ret;
}

bool stateClause_sameKind (stateClause s1, stateClause s2)
{
  return (s1->state == s2->state 
	  && s1->kind == s2->kind
	  && qual_match (s1->squal, s2->squal));
}

void stateClause_free (stateClause s)
{
  sRefSet_free (s->refs);
  fileloc_free (s->loc);
  sfree (s);
}

static /*@observer@*/ cstring 
  stateClauseKind_unparse (stateClause s) 
{
  switch (s->kind)
    {
    case SP_USES: 
      return cstring_makeLiteralTemp ("uses");
    case SP_DEFINES:
      return cstring_makeLiteralTemp ("defines");
    case SP_ALLOCATES:
      return cstring_makeLiteralTemp ("allocates");
    case SP_RELEASES:
      return cstring_makeLiteralTemp ("releases");
    case SP_SETS:
      return cstring_makeLiteralTemp ("sets");
    case SP_GLOBAL:
      return qual_unparse (s->squal);
    case SP_QUAL:
      return qual_unparse (s->squal);
    }

  BADEXIT;
}

cstring stateClause_unparseKind (stateClause s)
{
  return 
    (message ("%s%s",
	      cstring_makeLiteralTemp (s->state == TK_BEFORE 
				       ? "requires "
				       : (s->state == TK_AFTER
					  ? "ensures " : "")),
	      stateClauseKind_unparse (s)));
}

cstring stateClause_unparse (stateClause s)
{
  return (message ("%q %q", 
		   stateClause_unparseKind (s), sRefSet_unparsePlain (s->refs)));
}

stateClause stateClause_createDefines (sRefSet s)
{
  return (stateClause_createRaw (TK_BOTH, SP_DEFINES, s));
}

stateClause stateClause_createUses (sRefSet s)
{
  return (stateClause_createRaw (TK_BOTH, SP_USES, s));
}

stateClause stateClause_createSets (sRefSet s)
{
  return (stateClause_createRaw (TK_BOTH, SP_SETS, s));
}

stateClause stateClause_createReleases (sRefSet s)
{
  return (stateClause_createRaw (TK_BOTH, SP_RELEASES, s));
}

stateClause stateClause_createPlain (lltok tok, sRefSet s)
{
  switch (lltok_getTok (tok))
    {
    case QUSES:
      return stateClause_createUses (s);
    case QDEFINES:
      return stateClause_createDefines (s);
    case QALLOCATES:
      return stateClause_createAllocates (s);
    case QSETS:
      return stateClause_createSets (s);
    case QRELEASES:
      return stateClause_createReleases (s);
    default:
      sRefSet_free (s);
      BADBRANCH;
    }

  BADBRANCHRET (stateClause_createUses (sRefSet_undefined));
}

stateClause stateClause_createAllocates (sRefSet s)
{
  return (stateClause_createRaw (TK_BOTH, SP_ALLOCATES, s));
}

bool stateClause_matchKind (stateClause s1, stateClause s2)
{
  return (s1->state == s2->state && s1->kind == s2->kind
	  && qual_match (s1->squal, s2->squal));
}

bool stateClause_hasEnsures (stateClause cl)
{
  return (cl->state == TK_AFTER && (cl->kind == SP_QUAL || cl->kind == SP_GLOBAL));
}

bool stateClause_hasRequires (stateClause cl)
{
  return (cl->state == TK_BEFORE && (cl->kind == SP_QUAL || cl->kind == SP_GLOBAL));
}

bool stateClause_setsMetaState (stateClause cl)
{
  return ((cl->kind == SP_QUAL || cl->kind == SP_GLOBAL)
	  && qual_isMetaState (cl->squal));
}

qual stateClause_getMetaQual (stateClause cl)
{
  llassert (cl->kind == SP_QUAL || cl->kind == SP_GLOBAL);
  return cl->squal;
}

static sRefModVal stateClause_getStateFunction (stateClause cl)
{
  qual sq;

  llassert (cl->kind == SP_QUAL || cl->kind == SP_GLOBAL);

  sq = cl->squal;

  /*@+enumint@*/

  if (qual_isNullStateQual (sq))
    {
      return (sRefModVal) sRef_setNullState;
    }
  else if (qual_isExQual (sq))
    {
      return (sRefModVal) sRef_setExKind;
    }
  else if (qual_isAliasQual (sq))
    {
      return (sRefModVal) sRef_setAliasKind; 
    }
  else
    {
      DPRINTF (("Unhandled ensures qual: %s", qual_unparse (sq)));
      BADBRANCH;
    }
  /*@=enumint@*/
  BADBRANCHRET (NULL);
}

int stateClause_getStateParameter (stateClause cl)
{
  qual sq;

  llassert (cl->kind == SP_QUAL || cl->kind == SP_GLOBAL);

  sq = cl->squal;
  
  /*@+enumint@*/ 
  /*
  ** Since this can be many different types of state kinds, we need to allow all
  ** enum's to be returned as int.
  */

  if (qual_isNotNull (sq))
    {
      return NS_MNOTNULL;
    }
  else if (qual_isIsNull (sq))
    {
      return NS_DEFNULL;
    }
  else if (qual_isNull (sq))
    {
      return NS_POSNULL;
    }
  else if (qual_isRelNull (sq))
    {
      return NS_RELNULL;
    }
  else if (qual_isExposed (sq))
    {
      return XO_EXPOSED;
    }
  else if (qual_isObserver (sq))
    {
      return XO_OBSERVER;
    }
  else if (qual_isAliasQual (sq))
    {
      if (qual_isOnly (sq)) return AK_ONLY;
      if (qual_isImpOnly (sq)) return AK_IMPONLY;
      if (qual_isTemp (sq)) return AK_TEMP;
      if (qual_isOwned (sq)) return AK_OWNED;
      if (qual_isShared (sq)) return AK_SHARED;
      if (qual_isUnique (sq)) return AK_UNIQUE;
      if (qual_isDependent (sq)) return AK_DEPENDENT;
      if (qual_isKeep (sq)) return AK_KEEP;
      if (qual_isKept (sq)) return AK_KEPT;
      BADBRANCH;
    }
  else
    {
      DPRINTF (("Unhandled ensures qual: %s", qual_unparse (sq)));
      BADBRANCH;
    }

  /*@=enumint@*/
  /*@=relaxtypes@*/
  BADBRANCHRET (0);
}

sRefModVal stateClause_getEnsuresFunction (stateClause cl)
{
  llassertprint (cl->state == TK_AFTER, ("Not after: %s", stateClause_unparse (cl)));
  llassert (cl->kind == SP_QUAL || cl->kind == SP_GLOBAL);
  return stateClause_getStateFunction (cl);
}

sRefModVal stateClause_getRequiresBodyFunction (stateClause cl)
{
  llassertprint (cl->state == TK_BEFORE, ("Not before: %s", stateClause_unparse (cl)));
  llassert (cl->kind == SP_QUAL || cl->kind == SP_GLOBAL);
  return stateClause_getStateFunction (cl);
}

/*@observer@*/ fileloc stateClause_loc (stateClause s)
{
  return s->loc;
}

