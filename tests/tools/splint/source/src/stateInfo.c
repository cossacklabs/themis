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

# include "splintMacros.nf"
# include "basic.h"

# ifdef WIN32
/*
** Make Microsoft VC++ happy: its control checking produces too
** many spurious warnings.
*/

# pragma warning (disable:4715) 
# endif

static /*@observer@*/ cstring stateAction_unparse (stateAction p_sa) /*@*/ ;

void stateInfo_free (/*@only@*/ stateInfo a)
{
  if (a != NULL)
    {
      fileloc_free (a->loc);
      sfree (a);
    }
}

/*@only@*/ stateInfo stateInfo_update (/*@only@*/ stateInfo old, stateInfo newinfo)
     /*
     ** returns an stateInfo with the same value as new.  May reuse the
     ** storage of old.  (i.e., same effect as copy, but more
     ** efficient.)
     */
{
  if (old == NULL) 
    {
      DPRINTF (("Update state ==> %s", stateInfo_unparse (newinfo)));
      return stateInfo_copy (newinfo);
    }
  else if (newinfo == NULL)
    {
      stateInfo_free (old);
      return NULL;
    }
  else
    {
      if (fileloc_equal (old->loc, newinfo->loc)
	  && old->action == newinfo->action
	  /*@-abstractcompare@*/ && old->ref == newinfo->ref /*@=abstractcompare@*/)
	{
	  /*
	  ** Duplicate (change through alias most likely)
	  ** don't add this info
	  */
	  
	  return old;
	}
      else
	{
	  stateInfo snew = stateInfo_makeRefLoc (newinfo->ref, 
						 newinfo->loc, newinfo->action);
	  llassert (snew->previous == NULL);
	  snew->previous = old;
	  DPRINTF (("Update state ==> %s", stateInfo_unparse (snew)));
	  return snew;
	}
    }
}

static /*@observer@*/ stateInfo stateInfo_sort (/*@temp@*/ stateInfo stinfo)
     /* Sorts in reverse location order */
{
  DPRINTF (("Sorting: %s", stateInfo_unparse (stinfo)));

  if (stinfo == NULL || stinfo->previous == NULL) 
    {
      return stinfo;
    }
  else
    {
      stateInfo snext = stateInfo_sort (stinfo->previous);
      stateInfo sfirst = snext;

      DPRINTF (("stinfo/sext: %s // %s", stateInfo_unparse (stinfo), stateInfo_unparse (snext)));
      llassert (snext != NULL);

      if (!fileloc_lessthan (stinfo->loc, snext->loc))
	{
	  /*@i2@*/ stinfo->previous = sfirst; /* spurious? */
	  DPRINTF (("Sorted ==> %s", stateInfo_unparse (stinfo)));
	  /*@i2@*/ return stinfo; /* spurious? */
	}
      else
	{
	  while (snext != NULL && fileloc_lessthan (stinfo->loc, snext->loc))
	    {
	      /*
	      ** swap the order
	      */
	      fileloc tloc = snext->loc;
	      stateAction taction = snext->action;
	      sRef tref = snext->ref;
	      
	      DPRINTF (("in while: stinfo/sext: %s // %s", stateInfo_unparse (stinfo), stateInfo_unparse (snext)));
      
	      snext->loc = stinfo->loc;
	      snext->action = stinfo->action;
	      /*@-modobserver@*/
	      snext->ref = stinfo->ref; /* Doesn't actually modifie sfirst */ 
	      /*@=modobserver@*/
	      
	      stinfo->loc = tloc;
	      stinfo->action = taction;
	      stinfo->ref = tref;
	      /*@-mustfreeonly@*/
	      stinfo->previous = snext->previous;
	      /*@=mustfreeonly@*/
	      snext = snext->previous;
	      DPRINTF (("in while: stinfo/sext: %s // %s", stateInfo_unparse (stinfo), stateInfo_unparse (snext)));
	    }
	  
	  DPRINTF (("Sorted ==> %s", stateInfo_unparse (sfirst)));
	  /*@-compmempass@*/
	  return sfirst;
	  /*@=compmempass@*/
	}
    }
}

/*@only@*/ stateInfo 
stateInfo_updateLoc (/*@only@*/ stateInfo old, stateAction action, fileloc loc)
{
  if (fileloc_isUndefined (loc)) {
    loc = fileloc_copy (g_currentloc);
  }

  if (old != NULL && fileloc_equal (old->loc, loc) && old->action == action)
    {
      /*
      ** Duplicate (change through alias most likely)
      ** don't add this info
      */
      
      return old;
    }
  else
    {
      stateInfo snew = stateInfo_makeLoc (loc, action);
      llassert (snew->previous == NULL);
      snew->previous = old;
      DPRINTF (("Update state ==> %s", stateInfo_unparse (snew)));
      return snew;
    }
}

/*@only@*/ stateInfo 
stateInfo_updateRefLoc (/*@only@*/ stateInfo old, /*@exposed@*/ sRef ref, 
			stateAction action, fileloc loc)
{
  if (fileloc_isUndefined (loc)) {
    loc = fileloc_copy (g_currentloc);
  }

  if (old != NULL && fileloc_equal (old->loc, loc)
      && old->action == action
      /*@-abstractcompare*/ && old->ref == ref /*@=abstractcompare@*/)
    {
      /*
      ** Duplicate (change through alias most likely)
      ** don't add this info
      */
      
      return old;
    }
  else
    {
      stateInfo snew = stateInfo_makeRefLoc (ref, loc, action);
      llassert (snew->previous == NULL);
      snew->previous = old;
      DPRINTF (("Update state ==> %s", stateInfo_unparse (snew)));
      return snew;
    }
}

/*@only@*/ stateInfo stateInfo_copy (stateInfo a)
{
  if (a == NULL)
    {
      return NULL;
    }
  else
    {
      stateInfo ret = (stateInfo) dmalloc (sizeof (*ret));
      
      ret->loc = fileloc_copy (a->loc); /*< should report bug without copy! >*/
      ret->ref = a->ref;
      ret->action = a->action;
      ret->previous = stateInfo_copy (a->previous); 

      return ret;
    }
}

/*@only@*/ /*@notnull@*/ stateInfo
stateInfo_currentLoc (void)
{
  return stateInfo_makeLoc (g_currentloc, SA_DECLARED);
}

/*@only@*/ /*@notnull@*/ stateInfo
stateInfo_makeLoc (fileloc loc, stateAction action)
{
  stateInfo ret = (stateInfo) dmalloc (sizeof (*ret));

  if (fileloc_isUndefined (loc)) {
    ret->loc = fileloc_copy (g_currentloc);
  } else {
    ret->loc = fileloc_copy (loc);
  }

  ret->ref = sRef_undefined;
  ret->action = action;
  ret->previous = stateInfo_undefined;

  DPRINTF (("Make loc ==> %s", stateInfo_unparse (ret)));
  return ret;
}

/*@only@*/ /*@notnull@*/ stateInfo
stateInfo_makeRefLoc (/*@exposed@*/ sRef ref, fileloc loc, stateAction action)
     /*@post:isnull result->previous@*/
{
  stateInfo ret = (stateInfo) dmalloc (sizeof (*ret));

  if (fileloc_isUndefined (loc)) {
    ret->loc = fileloc_copy (g_currentloc);
  } else {
    ret->loc = fileloc_copy (loc);
  }

  ret->ref = ref;
  ret->action = action;
  ret->previous = stateInfo_undefined;

  return ret;
}

/*@only@*/ cstring
stateInfo_unparse (stateInfo s)
{
  cstring res = cstring_makeLiteral ("");

  while (stateInfo_isDefined (s)) {
    res = message ("%q%q: ", res, fileloc_unparse (s->loc));
    if (sRef_isValid (s->ref)) {
      res = message ("%q through alias %q ", res, sRef_unparse (s->ref));
    }

    res = message ("%q%s; ", res, stateAction_unparse (s->action)); 
    s = s->previous;
  }

  return res;
}

fileloc stateInfo_getLoc (stateInfo info)
{
    if (stateInfo_isDefined (info)) 
      {
	return info->loc;
      }
    
    return fileloc_undefined;
}

stateAction stateAction_fromNState (nstate ns)
{
  switch (ns) 
    {
    case NS_ERROR:
    case NS_UNKNOWN:
      return SA_UNKNOWN;
    case NS_NOTNULL:
    case NS_MNOTNULL:
      return SA_BECOMESNONNULL;
    case NS_RELNULL:
    case NS_CONSTNULL:
      return SA_DECLARED;
    case NS_POSNULL:
      return SA_BECOMESPOSSIBLYNULL;
    case NS_DEFNULL:
      return SA_BECOMESNULL;
    case NS_ABSNULL:
      return SA_BECOMESPOSSIBLYNULL;
    }
}

stateAction stateAction_fromExkind (exkind ex)
{
  switch (ex) 
    {
    case XO_UNKNOWN:
    case XO_NORMAL:
      return SA_UNKNOWN;
    case XO_EXPOSED:
      return SA_EXPOSED;
    case XO_OBSERVER:
      return SA_OBSERVER;
    }

  BADBRANCH;
  /*@notreached@*/ return SA_UNKNOWN;
}

stateAction stateAction_fromAlkind (alkind ak)
{
  switch (ak)
    {
    case AK_UNKNOWN:
    case AK_ERROR:
      return SA_UNKNOWN;
    case AK_ONLY:
      return SA_ONLY;
    case AK_IMPONLY:
      return SA_IMPONLY;
    case AK_KEEP:
      return SA_KEEP;
    case AK_KEPT:
      return SA_KEPT;
    case AK_TEMP:
      return SA_TEMP;
    case AK_IMPTEMP:
      return SA_IMPTEMP;
    case AK_SHARED:
      return SA_SHARED;
    case AK_UNIQUE:
    case AK_RETURNED:
      return SA_DECLARED;
    case AK_FRESH:
      return SA_FRESH;
    case AK_STACK:
      return SA_XSTACK;
    case AK_REFCOUNTED:
      return SA_REFCOUNTED;
    case AK_REFS:
      return SA_REFS;
    case AK_KILLREF:
      return SA_KILLREF;
    case AK_NEWREF:
      return SA_NEWREF;
    case AK_OWNED:
      return SA_OWNED;
    case AK_DEPENDENT:
      return SA_DEPENDENT;
    case AK_IMPDEPENDENT:
      return SA_IMPDEPENDENT;
    case AK_STATIC:
      return SA_STATIC;
    case AK_LOCAL:
      return SA_LOCAL;
    }

  BADBRANCH;
  /*@notreached@*/ return SA_UNKNOWN;
}

stateAction stateAction_fromSState (sstate ss)
{
  switch (ss) 
    {
    case SS_UNKNOWN: return SA_DECLARED;
    case SS_UNUSEABLE: return SA_KILLED;
    case SS_UNDEFINED: return SA_UNDEFINED;
    case SS_MUNDEFINED: return SA_MUNDEFINED;
    case SS_ALLOCATED: return SA_ALLOCATED;
    case SS_PDEFINED: return SA_PDEFINED;
    case SS_DEFINED: return SA_DEFINED;
    case SS_PARTIAL: return SA_PDEFINED;
    case SS_DEAD: return SA_RELEASED;
    case SS_HOFFA: return SA_PKILLED;
    case SS_SPECIAL: return SA_DECLARED;
    case SS_RELDEF: return SA_DECLARED;
    case SS_FIXED:
    case SS_UNDEFGLOB: 
    case SS_KILLED:     
    case SS_UNDEFKILLED:
    case SS_LAST:
      llbug (message ("Unexpected sstate: %s", sstate_unparse (ss)));
      /*@notreached@*/ return SA_UNKNOWN;
    }
}

static /*@observer@*/ cstring stateAction_unparse (stateAction sa)
{
  switch (sa) 
    {
    case SA_UNKNOWN: return cstring_makeLiteralTemp ("changed <unknown modification>");
    case SA_CHANGED: return cstring_makeLiteralTemp ("changed");

    case SA_CREATED: return cstring_makeLiteralTemp ("created");
    case SA_DECLARED: return cstring_makeLiteralTemp ("declared");
    case SA_DEFINED: return cstring_makeLiteralTemp ("defined");
    case SA_PDEFINED: return cstring_makeLiteralTemp ("partially defined");
    case SA_RELEASED: return cstring_makeLiteralTemp ("released");
    case SA_ALLOCATED: return cstring_makeLiteralTemp ("allocated");
    case SA_KILLED: return cstring_makeLiteralTemp ("released");
    case SA_PKILLED: return cstring_makeLiteralTemp ("possibly released");
    case SA_MERGED: return cstring_makeLiteralTemp ("merged");
    case SA_UNDEFINED: return cstring_makeLiteralTemp ("becomes undefined");
    case SA_MUNDEFINED: return cstring_makeLiteralTemp ("possibly undefined");

    case SA_SHARED: return cstring_makeLiteralTemp ("becomes shared");
    case SA_ONLY: return cstring_makeLiteralTemp ("becomes only");
    case SA_IMPONLY: return cstring_makeLiteralTemp ("becomes implicitly only");
    case SA_OWNED: return cstring_makeLiteralTemp ("becomes owned");
    case SA_DEPENDENT: return cstring_makeLiteralTemp ("becomes dependent");
    case SA_IMPDEPENDENT: return cstring_makeLiteralTemp ("becomes implicitly dependent");
    case SA_KEPT: return cstring_makeLiteralTemp ("becomes kept");
    case SA_KEEP: return cstring_makeLiteralTemp ("becomes keep");
    case SA_FRESH: return cstring_makeLiteralTemp ("becomes fresh");
    case SA_TEMP: return cstring_makeLiteralTemp ("becomes temp");
    case SA_IMPTEMP: return cstring_makeLiteralTemp ("becomes implicitly temp");
    case SA_XSTACK: return cstring_makeLiteralTemp ("becomes stack-allocated storage");
    case SA_STATIC: return cstring_makeLiteralTemp ("becomes static");
    case SA_LOCAL: return cstring_makeLiteralTemp ("becomes local");

    case SA_REFCOUNTED: return cstring_makeLiteralTemp ("becomes refcounted");
    case SA_REFS: return cstring_makeLiteralTemp ("becomes refs");
    case SA_NEWREF: return cstring_makeLiteralTemp ("becomes newref");
    case SA_KILLREF: return cstring_makeLiteralTemp ("becomes killref");

    case SA_OBSERVER: return cstring_makeLiteralTemp ("becomes observer");
    case SA_EXPOSED: return cstring_makeLiteralTemp ("becomes exposed");

    case SA_BECOMESNULL: return cstring_makeLiteralTemp ("becomes null");
    case SA_BECOMESNONNULL: return cstring_makeLiteralTemp ("becomes non-null");
    case SA_BECOMESPOSSIBLYNULL: return cstring_makeLiteralTemp ("becomes possibly null");
    } 
 
  DPRINTF (("Bad state action: %d", sa));
  BADBRANCH;
}

void stateInfo_display (stateInfo s, cstring sname)
{
  bool showdeep = context_flagOn (FLG_SHOWDEEPHISTORY, g_currentloc);

  s = stateInfo_sort (s);
  
  while (stateInfo_isDefined (s))
    {
      cstring msg = message ("%s%s", sname, stateAction_unparse (s->action)); 
      
      if (sRef_isValid (s->ref)) {
	msg = message ("%q (through alias %q)", msg, sRef_unparse (s->ref));
      }
      
      llgenindentmsg (msg, s->loc);

      if (!showdeep) {
	break;
      }

      s = s->previous;
    }

  cstring_free (sname);
}


