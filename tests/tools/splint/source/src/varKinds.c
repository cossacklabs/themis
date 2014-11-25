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
** varKinds.c
*/

# include "splintMacros.nf"
# include "basic.h"

alkind alkind_fromInt (int n)
{
  /*@+enumint@*/
  if (n < AK_UNKNOWN || n > AK_LOCAL)
    {
      llbug (message ("Alias kind out of range: %d", n));
    }
  /*@=enumint@*/

  return ((alkind)n);
}

nstate nstate_fromInt (int n)
{
  /*@+enumint@*/
  llassertprint (n >= NS_ERROR && n <= NS_ABSNULL,
		 ("Bad null state: %d", n));
  /*@=enumint@*/

  return ((nstate)n);
}

sstate sstate_fromInt (int n)
{
  /*@+enumint@*/
  llassert (n >= SS_UNKNOWN && n < SS_LAST);
  /*@=enumint@*/

  return ((sstate)n);
}

exkind exkind_fromInt (int n)
{
  /*@+enumint@*/
  llassert (n >= XO_UNKNOWN && n <= XO_OBSERVER);
  /*@=enumint@*/

  return ((exkind) n);
}

cstring sstate_unparse (sstate s)
{
  switch (s)
    {
    case SS_UNKNOWN:   return cstring_makeLiteralTemp ("unknown");
    case SS_UNUSEABLE: return cstring_makeLiteralTemp ("unuseable");
    case SS_UNDEFINED: return cstring_makeLiteralTemp ("undefined");
    case SS_MUNDEFINED:return cstring_makeLiteralTemp ("possibly undefined");
    case SS_ALLOCATED: return cstring_makeLiteralTemp ("allocated");
    case SS_PDEFINED:  return cstring_makeLiteralTemp ("partially defined");
    case SS_DEFINED:   return cstring_makeLiteralTemp ("defined");
    case SS_PARTIAL:   return cstring_makeLiteralTemp ("partial");
    case SS_SPECIAL:   return cstring_makeLiteralTemp ("special");
    case SS_DEAD:      return cstring_makeLiteralTemp ("dead");
    case SS_HOFFA:     return cstring_makeLiteralTemp ("probably dead");
    case SS_FIXED:     return cstring_makeLiteralTemp ("unmodifiable");
    case SS_RELDEF:    return cstring_makeLiteralTemp ("reldef");
    case SS_LAST:      llcontbuglit ("sstate_unparse: last");
                       return cstring_makeLiteralTemp ("<error>");
    case SS_UNDEFGLOB:     return cstring_makeLiteralTemp ("undefglob");
    case SS_KILLED:    return cstring_makeLiteralTemp ("killed");
    case SS_UNDEFKILLED:
      return cstring_makeLiteralTemp ("undefkilled");
    }

  BADEXIT;
}

bool nstate_possiblyNull (nstate n)
{
  /*
  ** note: not NS_UNKNOWN or NS_ERROR 
  */

  return ((n >= NS_CONSTNULL) && (n <= NS_ABSNULL));
}

bool nstate_perhapsNull (nstate n)
{
  /*
  ** note: not NS_UNKNOWN or NS_ERROR 
  */

  return ((n >= NS_RELNULL) && (n <= NS_ABSNULL));
}

cstring nstate_unparse (nstate n)
{
  switch (n)
    {
    case NS_ERROR:     return cstring_makeLiteralTemp ("<null error>");
    case NS_UNKNOWN:   return cstring_makeLiteralTemp ("implicitly non-null");
    case NS_POSNULL:   return cstring_makeLiteralTemp ("null");
    case NS_DEFNULL:   return cstring_makeLiteralTemp ("null");
    case NS_NOTNULL:   return cstring_makeLiteralTemp ("notnull");
    case NS_MNOTNULL:  return cstring_makeLiteralTemp ("notnull");
    case NS_ABSNULL:   return cstring_makeLiteralTemp ("null");
    case NS_RELNULL:   return cstring_makeLiteralTemp ("relnull");
    case NS_CONSTNULL: return cstring_makeLiteralTemp ("null");
    }

  /*@notreached@*/ llcontbuglit ("bad null state!");
  /*@notreached@*/ return cstring_makeLiteralTemp ("!!! bad null state !!!");
  BADEXIT;
}

/*
** ??? (used to do something different for guarded)
*/

int nstate_compare (nstate n1, nstate n2)
{
  return (generic_compare (n1, n2));
}

/*
** This occurs when we select a field with alkind inner, 
** from a structure with alkind outer.  It is probably
** unnecessary.
*/

alkind alkind_derive (alkind outer, alkind inner)
{
  switch (outer)
    {
    case AK_ERROR:
    case AK_UNKNOWN: return inner;
    case AK_KEPT:
    case AK_KEEP:
    case AK_ONLY: 
    case AK_IMPONLY:
    case AK_OWNED:
    case AK_IMPDEPENDENT:
    case AK_DEPENDENT:
      if (inner == AK_SHARED) return AK_SHARED;
      else if (outer == AK_DEPENDENT) return AK_IMPDEPENDENT;
      else if (outer == AK_ONLY) return AK_IMPONLY;
      else return outer;
      /* not so sure about these? */
    case AK_REFCOUNTED:
    case AK_NEWREF:
    case AK_KILLREF:
    case AK_REFS:
    case AK_STACK:
    case AK_STATIC:
      return outer;
    case AK_TEMP: 
    case AK_IMPTEMP:
    case AK_SHARED:
    case AK_UNIQUE:
    case AK_LOCAL: 
    case AK_FRESH:
    case AK_RETURNED:
      if (alkind_isKnown (inner)) return inner; 
      else return outer;
    }
  BADEXIT;
}

cstring alkind_unparse (alkind a)
{
  switch (a)
    {
    case AK_ERROR:           return cstring_makeLiteralTemp ("<error>");
    case AK_UNKNOWN:         return cstring_makeLiteralTemp ("unqualified");
    case AK_ONLY:            return cstring_makeLiteralTemp ("only");
    case AK_IMPONLY:         return cstring_makeLiteralTemp ("implicitly only");
    case AK_OWNED:           return cstring_makeLiteralTemp ("owned");
    case AK_IMPDEPENDENT:    return cstring_makeLiteralTemp ("implicitly dependent");
    case AK_DEPENDENT:       return cstring_makeLiteralTemp ("dependent");
    case AK_KEEP:            return cstring_makeLiteralTemp ("keep");
    case AK_KEPT:            return cstring_makeLiteralTemp ("kept");
    case AK_IMPTEMP:         return cstring_makeLiteralTemp ("implicitly temp");
    case AK_TEMP:            return cstring_makeLiteralTemp ("temp");
    case AK_SHARED:          return cstring_makeLiteralTemp ("shared");
    case AK_UNIQUE:          return cstring_makeLiteralTemp ("unique");
    case AK_RETURNED:        return cstring_makeLiteralTemp ("returned");
    case AK_FRESH:           return cstring_makeLiteralTemp ("fresh");
    case AK_STACK:           return cstring_makeLiteralTemp ("stack");
    case AK_REFCOUNTED:      return cstring_makeLiteralTemp ("refcounted");
    case AK_REFS:            return cstring_makeLiteralTemp ("refs");
    case AK_KILLREF:         return cstring_makeLiteralTemp ("killref");
    case AK_NEWREF:          return cstring_makeLiteralTemp ("newref");
    case AK_LOCAL:           return cstring_makeLiteralTemp ("local");
    case AK_STATIC:          return cstring_makeLiteralTemp ("unqualified static");
    }
    BADEXIT;
}

cstring exkind_unparse (exkind a)
{
  switch (a)
    {
    case XO_UNKNOWN:         return cstring_makeLiteralTemp ("unknown");
    case XO_NORMAL:          return cstring_makeLiteralTemp ("unexposed");
    case XO_EXPOSED:         return cstring_makeLiteralTemp ("exposed");
    case XO_OBSERVER:        return cstring_makeLiteralTemp ("observer");
    }
  BADEXIT;
}

cstring exkind_capName (exkind a)
{
  switch (a)
    {
    case XO_UNKNOWN:         return cstring_makeLiteralTemp ("Unknown");
    case XO_NORMAL:          return cstring_makeLiteralTemp ("Unexposed");
    case XO_EXPOSED:         return cstring_makeLiteralTemp ("Exposed");
    case XO_OBSERVER:        return cstring_makeLiteralTemp ("Observer");
    }
  BADEXIT;
}

cstring exkind_unparseError (exkind a)
{
  switch (a)
    {
    case XO_UNKNOWN:         return cstring_makeLiteralTemp ("unqualified");
    case XO_NORMAL:          return cstring_makeLiteralTemp ("unqualifier");
    case XO_EXPOSED:         return cstring_makeLiteralTemp ("exposed");
    case XO_OBSERVER:        return cstring_makeLiteralTemp ("observer");
    }
  BADEXIT;
}

cstring alkind_capName (alkind a)
{
  switch (a)
    {
    case AK_ERROR:    
      return cstring_makeLiteralTemp ("<Error>");
    case AK_UNKNOWN:     
      return cstring_makeLiteralTemp ("Unqualified");
    case AK_ONLY:  
      return cstring_makeLiteralTemp ("Only");
    case AK_IMPONLY:
      return cstring_makeLiteralTemp ("Implicitly only");
    case AK_OWNED:
      return cstring_makeLiteralTemp ("Owned");
    case AK_IMPDEPENDENT:  
      return cstring_makeLiteralTemp ("Implicitly dependent");
    case AK_DEPENDENT:  
      return cstring_makeLiteralTemp ("Dependent");
    case AK_KEEP:     
      return cstring_makeLiteralTemp ("Keep");
    case AK_KEPT:    
      return cstring_makeLiteralTemp ("Kept");
    case AK_IMPTEMP:   
      return cstring_makeLiteralTemp ("Implicitly temp");
    case AK_TEMP:    
      return cstring_makeLiteralTemp ("Temp");
    case AK_SHARED:
      return cstring_makeLiteralTemp ("Shared");
    case AK_UNIQUE:    
      return cstring_makeLiteralTemp ("Unique");
    case AK_RETURNED:
      return cstring_makeLiteralTemp ("Returned");
    case AK_FRESH:   
      return cstring_makeLiteralTemp ("Fresh");
    case AK_STACK:      
      return cstring_makeLiteralTemp ("Stack");
    case AK_REFCOUNTED: 
      return cstring_makeLiteralTemp ("Refcounted");
    case AK_REFS:
      return cstring_makeLiteralTemp ("Refs");
    case AK_KILLREF: 
      return cstring_makeLiteralTemp ("Killref");
    case AK_NEWREF:    
      return cstring_makeLiteralTemp ("Newref");
    case AK_LOCAL:    
      return cstring_makeLiteralTemp ("Local");
    case AK_STATIC: 
      return cstring_makeLiteralTemp ("Unqualified static");
    }
  BADEXIT;
}

exkind
exkind_fromQual (qual q)
{
  if (qual_isExposed (q)) {
    return XO_EXPOSED;
  } else if (qual_isObserver (q)) {
    return XO_OBSERVER;
  } else
    {
      llcontbug (message ("exkind_fromQual: not exp qualifier: %s" , 
			  qual_unparse (q)));
      return XO_UNKNOWN;
    }
}

sstate
sstate_fromQual (qual q)
{
  if (qual_isOut (q))          return SS_ALLOCATED;
  if (qual_isIn (q))           return SS_DEFINED;
  else if (qual_isPartial (q)) return SS_PARTIAL;
  else if (qual_isRelDef (q))  return SS_RELDEF;
  else if (qual_isUndef (q))   return SS_UNDEFGLOB;
  else if (qual_isKilled (q))  return SS_KILLED;
  else if (qual_isSpecial (q)) return SS_SPECIAL;
  else
    {
      llcontbug (message ("sstate_fromQual: not alias qualifier: %s", 
			  qual_unparse (q)));
      return SS_UNKNOWN;
    }
}

exitkind
exitkind_fromQual (qual q)
{
  if (qual_isExits (q))     return XK_MUSTEXIT;
  if (qual_isMayExit (q))   return XK_MAYEXIT;
  if (qual_isTrueExit (q))  return XK_TRUEEXIT;
  if (qual_isFalseExit (q)) return XK_FALSEEXIT;
  if (qual_isNeverExit (q)) return XK_NEVERESCAPE;
  else
    {
      llcontbug (message ("exitkind_fromQual: not exit qualifier: %s",
			  qual_unparse (q)));
      return XK_UNKNOWN;
    }
}

alkind
alkind_fromQual (qual q)
{
  if (qual_isOnly (q))       return AK_ONLY;
  if (qual_isImpOnly (q))    return AK_IMPONLY;
  if (qual_isKeep (q))       return AK_KEEP;
  if (qual_isKept (q))       return AK_KEPT;
  if (qual_isTemp (q))       return AK_TEMP;
  if (qual_isShared (q))     return AK_SHARED;
  if (qual_isUnique (q))     return AK_UNIQUE;
  if (qual_isRefCounted (q)) return AK_REFCOUNTED;
  if (qual_isRefs (q))       return AK_REFS;
  if (qual_isNewRef (q))     return AK_NEWREF;
  if (qual_isKillRef (q))    return AK_KILLREF;
  if (qual_isTempRef (q))    return AK_KILLREF; /* kludge? use kill ref for this */
  if (qual_isOwned (q))      return AK_OWNED;
  if (qual_isDependent (q))  return AK_DEPENDENT;

  llcontbug (message ("alkind_fromQual: not alias qualifier: %s", qual_unparse (q)));
  return AK_ERROR;
}    

static bool alkind_isMeaningless (alkind a1)
{
  return (a1 == AK_ERROR || a1 == AK_UNKNOWN || a1 == AK_RETURNED
	  || a1 == AK_STACK || a1 == AK_REFCOUNTED
	  || a1 == AK_REFS || a1 == AK_KILLREF || a1 == AK_NEWREF
	  || a1 == AK_LOCAL);
}

bool alkind_compatible (alkind a1, alkind a2)
{
  if (a1 == a2) return TRUE;
  if (a2 == AK_ERROR) return TRUE;
  if (a2 == AK_UNKNOWN)
    {
      return (alkind_isMeaningless (a1) || (a1 == AK_IMPTEMP));
    }

  switch (a1)
    {
    case AK_ERROR:               return TRUE;
    case AK_UNKNOWN:             return (alkind_isMeaningless (a2)
					 || (a2 == AK_IMPTEMP));
    case AK_IMPONLY:             return (a2 == AK_KEEP || a2 == AK_FRESH 
					 || a2 == AK_ONLY);
    case AK_ONLY:                return (a2 == AK_KEEP || a2 == AK_FRESH
					 || a2 == AK_IMPONLY);
    case AK_OWNED:               return FALSE;
    case AK_IMPDEPENDENT:        return (a2 == AK_DEPENDENT);
    case AK_DEPENDENT:           return (a2 == AK_IMPDEPENDENT);
    case AK_KEEP:                return (a2 == AK_ONLY || a2 == AK_FRESH
					 || a2 == AK_IMPONLY);
    case AK_KEPT:                return FALSE;
    case AK_IMPTEMP:             return (a2 == AK_TEMP);
    case AK_TEMP:                return (a2 == AK_IMPTEMP);
    case AK_SHARED:              return FALSE;
    case AK_UNIQUE:              return (a2 == AK_TEMP);
    case AK_RETURNED:            return (alkind_isMeaningless (a2));
    case AK_FRESH:               return (alkind_isOnly (a2));
    case AK_STACK:               return (alkind_isMeaningless (a2));
    case AK_REFCOUNTED:          return (alkind_isMeaningless (a2));
    case AK_REFS:                return (alkind_isMeaningless (a2));
    case AK_KILLREF:             return (alkind_isMeaningless (a2));
    case AK_NEWREF:              return (alkind_isMeaningless (a2));
    case AK_LOCAL:               return (alkind_isMeaningless (a2));
    case AK_STATIC:              return (alkind_isMeaningless (a2));
    }
  BADEXIT;
}

bool alkind_equal (alkind a1, alkind a2)
{
  if (a1 == a2) return TRUE;
  if (a2 == AK_ERROR) return TRUE;

  switch (a1)
    {
    case AK_ERROR:               return TRUE;
    case AK_IMPONLY:             return (a2 == AK_ONLY);
    case AK_ONLY:                return (a2 == AK_IMPONLY);
    case AK_IMPDEPENDENT:        return (a2 == AK_DEPENDENT);
    case AK_DEPENDENT:           return (a2 == AK_IMPDEPENDENT);
    case AK_IMPTEMP:             return (a2 == AK_TEMP);
    case AK_TEMP:                return (a2 == AK_IMPTEMP);
    default:                     return FALSE;
    }

  BADEXIT;
}

alkind
alkind_fixImplicit (alkind a)
{
  if (a == AK_IMPTEMP) return AK_TEMP;
  if (a == AK_IMPONLY) return AK_IMPONLY;
  if (a == AK_IMPDEPENDENT) return AK_DEPENDENT;

  return a;
}

cstring exitkind_unparse (exitkind k)
{
  switch (k)
    {
    case XK_ERROR:       return (cstring_makeLiteralTemp ("<error>"));
    case XK_UNKNOWN:     return (cstring_makeLiteralTemp ("?"));
    case XK_NEVERESCAPE: return (cstring_makeLiteralTemp ("never escape"));
    case XK_MAYEXIT:     return (cstring_makeLiteralTemp ("mayexit"));
    case XK_MUSTEXIT:    return (cstring_makeLiteralTemp ("exits"));
    case XK_TRUEEXIT:    return (cstring_makeLiteralTemp ("trueexit"));
    case XK_FALSEEXIT:   return (cstring_makeLiteralTemp ("falseexit"));
    case XK_MUSTRETURN:  return (cstring_makeLiteralTemp ("mustreturn"));
    case XK_MAYRETURN:   return (cstring_makeLiteralTemp ("mayreturn"));
    case XK_MUSTRETURNEXIT: return (cstring_makeLiteralTemp ("mustreturnexit"));
    case XK_MAYRETURNEXIT: return (cstring_makeLiteralTemp ("mayreturnexit"));
    case XK_GOTO:        return (cstring_makeLiteralTemp ("goto"));
    case XK_MAYGOTO:     return (cstring_makeLiteralTemp ("maygoto"));
    }
  
 BADEXIT;
}

exitkind exitkind_makeConditional (exitkind k)
{
  switch (k)
    {
    case XK_TRUEEXIT:
    case XK_FALSEEXIT: 
    case XK_MUSTEXIT:       return XK_MAYEXIT;
    case XK_MUSTRETURN:     return XK_MAYRETURN;
    case XK_MUSTRETURNEXIT: return XK_MAYRETURNEXIT;
    case XK_GOTO:           return XK_MAYGOTO;
    default:                return k;
    }
}

exitkind exitkind_combine (exitkind k1, exitkind k2)
{
  if (k1 == k2)
    {
      return k1;
    }

  if (k2 == XK_ERROR)
    {
      return XK_ERROR;
    }

  switch (k1)
    {
    case XK_ERROR: return XK_ERROR;
    case XK_UNKNOWN:     
    case XK_NEVERESCAPE: return (exitkind_makeConditional (k2));
    case XK_MUSTEXIT:    
      switch (k2)
	{
	case XK_MUSTRETURNEXIT:
	case XK_MUSTRETURN: return XK_MUSTRETURNEXIT;
	case XK_MAYRETURNEXIT:
	case XK_MAYRETURN:  return XK_MAYRETURNEXIT;
	default:             return XK_MAYEXIT;
	}
      BADEXIT;

    case XK_MAYEXIT:     
    case XK_TRUEEXIT:    
    case XK_FALSEEXIT:   
      switch (k2)
	{
	case XK_MUSTRETURNEXIT:
	case XK_MAYRETURNEXIT:
	case XK_MAYRETURN:
	case XK_MUSTRETURN: return XK_MAYRETURNEXIT;
	default:             return XK_MAYEXIT;
	}
      BADEXIT;

    case XK_MUSTRETURN:
      switch (k2)
	{
	case XK_MUSTRETURNEXIT:
	case XK_MUSTEXIT:    return XK_MUSTRETURNEXIT;
	case XK_MAYRETURNEXIT:
	case XK_TRUEEXIT:
	case XK_FALSEEXIT:
	case XK_MAYEXIT:     return XK_MAYRETURNEXIT;
	default:              return XK_MAYRETURN;
	}
      BADEXIT;

    case XK_MAYRETURN:
      if (exitkind_couldExit (k2))
	{
	  return XK_MAYRETURNEXIT;
	}
      else
	{
	  return XK_MAYRETURN;
	}

    case XK_MUSTRETURNEXIT: 
      switch (k2)
	{
	case XK_MUSTRETURN:
	case XK_MUSTEXIT:    return XK_MUSTRETURNEXIT;
	default:              return XK_MAYRETURNEXIT;
	}
      BADEXIT;

    case XK_MAYRETURNEXIT:   return XK_MAYRETURNEXIT;
    case XK_GOTO:
    case XK_MAYGOTO:         
      if (exitkind_couldExit (k2))
	{
	  return XK_MAYRETURNEXIT;
	}
      return XK_MAYGOTO;
    }
  
 BADEXIT;
}

bool exitkind_couldExit (exitkind e)
{
  switch (e)
    {
    case XK_MAYEXIT:
    case XK_MUSTEXIT:
    case XK_TRUEEXIT:
    case XK_FALSEEXIT:
    case XK_MAYRETURNEXIT:
    case XK_MUSTRETURNEXIT: 
    case XK_GOTO:
    case XK_MAYGOTO: return TRUE;
    default: return FALSE;
    }
}

static bool exitkind_couldReturn (exitkind e) /*@*/ 
{
  switch (e)
    {
    case XK_MUSTRETURN:
    case XK_MAYRETURN:
    case XK_MAYRETURNEXIT:
    case XK_MUSTRETURNEXIT:  return TRUE;
    default: return FALSE;
    }
}

static bool exitkind_couldGoto (exitkind e) /*@*/
{
  return (e == XK_GOTO || e == XK_MAYGOTO);
}

bool exitkind_couldEscape (exitkind e)
{
  return exitkind_couldReturn (e) || exitkind_couldExit (e)
    || exitkind_couldGoto (e);
}

exitkind exitkind_fromInt (int x)
{
  /*@+enumint@*/
  llassert (x >= XK_ERROR && x <= XK_LAST);
  /*@=enumint@*/

  return (exitkind) x;
}







