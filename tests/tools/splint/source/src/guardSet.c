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
** guardSet.c
**
** if x is in true guards, then x is not null on this branch,
**                          and x is "probably" null on the other branch.
** if x is in false guards, then x is "probably" null on this branch,
**                          and x is not null on the other branch.
**
** Either guards is obsolete and should be removed soon.
*/

# include "splintMacros.nf"
# include "basic.h"

guardSet guardSet_new ()
{
  return guardSet_undefined;
}

static /*@notnull@*/ /*@special@*/ guardSet guardSet_newEmpty (void)
   /*@defines result@*/
   /*@post:isnull result->tguard, result->fguard@*/
{
  guardSet g = (guardSet) dmalloc (sizeof (*g));

  g->tguard = sRefSet_undefined;
  g->fguard = sRefSet_undefined;

  return g;
}

sRefSet guardSet_getTrueGuards (guardSet g)
{
  
  if (guardSet_isDefined (g))
    {
      return (g->tguard);
    }
  else
    {
      return (sRefSet_undefined);
    }
}

void guardSet_delete (guardSet g, sRef s) 
{
  bool res;
  llassert (guardSet_isDefined (g));
  res = sRefSet_delete (g->tguard, s);
  
  /*
  ** This assertion is no longer always true:
  ** llassert (res);
  */
}

/*@dependent@*/ /*@exposed@*/ sRefSet guardSet_getFalseGuards (guardSet g)
{
  if (guardSet_isDefined (g))
    { 
      return (g->fguard);
    }
  else
    {
      return (sRefSet_undefined);
    }
}

guardSet guardSet_or (/*@returned@*/ /*@unique@*/ guardSet s, guardSet t)
{
  
  llassert (NOALIAS (s, t));

  if (s == guardSet_undefined)
    {
      if (t == guardSet_undefined)
	{
	  return s;
	}

      s = guardSet_newEmpty ();
    }

  if (t == guardSet_undefined)
    {
      sRefSet_free (s->tguard);
      s->tguard = sRefSet_new ();
    }
  else
    {
      sRefSet last;

      s->tguard = sRefSet_intersect (last = s->tguard, t->tguard);
      sRefSet_free (last);
      s->fguard = sRefSet_union (s->fguard, t->fguard);
    }
  
  
  return s;
}

guardSet guardSet_and (/*@returned@*/ /*@unique@*/ guardSet s, guardSet t)
{
  llassert (NOALIAS (s, t));

  if (s == guardSet_undefined)
    {
      if (t == guardSet_undefined)
	{
	  return s;
	}

      s = guardSet_newEmpty ();
    }

  if (t == guardSet_undefined)
    {
      sRefSet_free (s->fguard);
      s->fguard = sRefSet_new ();
    }
  else
    {
      sRefSet last;

      s->tguard = sRefSet_union (s->tguard, t->tguard);

      s->fguard = sRefSet_intersect (last = s->fguard, t->fguard);
      sRefSet_free (last);
    }
  
  return s;
}

/*@only@*/ guardSet guardSet_union (/*@only@*/ guardSet s, guardSet t)
{
  if (t == guardSet_undefined) return s; 

  llassert (NOALIAS (s, t));

  if (guardSet_isDefined (s)) 
    {
      s->tguard = sRefSet_union (s->tguard, t->tguard);
      s->fguard = sRefSet_union (s->fguard, t->fguard);
    }
  else
    {
      s = guardSet_newEmpty ();

      s->tguard = sRefSet_newCopy (t->tguard);
      s->fguard = sRefSet_newCopy (t->fguard);
    }

  return s;
}

guardSet guardSet_levelUnion (/*@only@*/ guardSet s, guardSet t, int lexlevel)
{
  if (t == guardSet_undefined) return s;

  llassert (NOALIAS (s, t));

  if (guardSet_isDefined (s))
    {
      s->tguard = sRefSet_levelUnion (s->tguard, t->tguard, lexlevel);
      s->fguard = sRefSet_levelUnion (s->fguard, t->fguard, lexlevel);
    }
  else
    {
      s = guardSet_newEmpty ();

      /* should be necessary! */

      sRefSet_free (s->tguard);
      sRefSet_free (s->fguard);

      s->tguard = sRefSet_levelCopy (t->tguard, lexlevel);
      s->fguard = sRefSet_levelCopy (t->fguard, lexlevel);
    }

  return s;
}

guardSet 
  guardSet_levelUnionFree (/*@returned@*/ /*@unique@*/ guardSet s,
			   /*@only@*/ guardSet t, int lexlevel)
{
  if (t == guardSet_undefined) return s;

  if (guardSet_isDefined (s))
    {
      s->tguard = sRefSet_levelUnion (s->tguard, t->tguard, lexlevel);
      s->fguard = sRefSet_levelUnion (s->fguard, t->fguard, lexlevel);
    }
  else
    {
      s = guardSet_newEmpty ();

      /* should be necessary! */

      sRefSet_free (s->tguard);
      sRefSet_free (s->fguard);

      s->tguard = sRefSet_levelCopy (t->tguard, lexlevel);
      s->fguard = sRefSet_levelCopy (t->fguard, lexlevel);
    }

  guardSet_free (t);
  return s;
}

void guardSet_flip (guardSet g)
{
  if (g != guardSet_undefined)
    {
      sRefSet tmp = g->tguard;

      g->tguard = g->fguard;      
      g->fguard = tmp;
    }
}

/*@only@*/ guardSet guardSet_invert (/*@temp@*/ guardSet g)
{
  if (g != guardSet_undefined)
    {
      guardSet ret = guardSet_newEmpty ();

      ret->tguard = sRefSet_newCopy (g->fguard);      
      ret->fguard = sRefSet_newCopy (g->tguard);

      return ret;
    }
  return guardSet_undefined;
}

/*@only@*/ guardSet guardSet_copy (/*@temp@*/ guardSet g)
{
  if (g != guardSet_undefined)
    {
      guardSet ret = guardSet_newEmpty ();

      ret->tguard = sRefSet_newCopy (g->tguard);      
      ret->fguard = sRefSet_newCopy (g->fguard);

      return ret;
    }
  return guardSet_undefined;
}
  
guardSet guardSet_addTrueGuard (/*@returned@*/ guardSet g, /*@exposed@*/ sRef s)
{
  if (sRef_isMeaningful (s))
    {
      if (g == guardSet_undefined)
	{
	  g = guardSet_newEmpty ();
	}
      
      g->tguard = sRefSet_insert (g->tguard, s);
    }

  return g;
}

guardSet guardSet_addFalseGuard (/*@returned@*/ guardSet g, /*@exposed@*/ sRef s)
{
  if (sRef_isMeaningful (s))
    {
      if (g == guardSet_undefined)
	{
	  g = guardSet_newEmpty ();
	}
      
      g->fguard = sRefSet_insert (g->fguard, s);
    }

  return g;
}

/*@only@*/ cstring guardSet_unparse (guardSet g)
{
  if (g == guardSet_undefined)
    {
      return (cstring_makeLiteral ("<no guards>"));
    }
  else
    {
      return (message ("not null: %q / prob null: %q",
		       sRefSet_unparseDebug (g->tguard),
		       sRefSet_unparseDebug (g->fguard)));
    }
}

void guardSet_free (/*@only@*/ guardSet g)
{
  if (g == guardSet_undefined) return;

  sRefSet_free (g->tguard);
  sRefSet_free (g->fguard);
  
  sfree (g);
}

bool
guardSet_isGuarded (guardSet g, sRef s)
{
  if (g == guardSet_undefined) return FALSE;
  
  return (sRefSet_member (g->tguard, s));
}

bool
guardSet_mustBeNull (guardSet g, sRef s)
{
  bool ret;

  if (g == guardSet_undefined) return FALSE;

  ret = sRefSet_member (g->fguard, s);
  return ret;
}

bool guardSet_isEmpty (guardSet g)
{
  if (guardSet_isDefined (g))
    {
      return (sRefSet_isEmpty (g->tguard) && sRefSet_isEmpty (g->fguard));
    }
  else
    {
      return TRUE;
    }
}
