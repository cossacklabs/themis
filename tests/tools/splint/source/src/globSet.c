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
** globSet.c
*/

# include "splintMacros.nf"
# include "basic.h"

globSet
globSet_new ()
{
  return (globSet_undefined);
}

void
globSet_clear (globSet g)
{
  sRefSet_clear (g);
}

globSet
globSet_insert (/*@returned@*/ globSet s, /*@exposed@*/ sRef el)
{
  if (sRef_isKnown (el) && !sRef_isConst (el) && !sRef_isType (el))
    {
      llassertprint (sRef_isFileOrGlobalScope (el) || sRef_isKindSpecial (el),
		     ("el: %s", sRef_unparse (el)));
      
      return (sRefSet_insert (s, el));
    }
  else
    {
      return s;
    }
}

globSet
globSet_single (/*@exposed@*/ sRef el)
{
  globSet res = globSet_new ();
  return globSet_insert (res, el);
}

void 
globSet_markImmutable (globSet g)
{
  sRefSet_markImmutable (g);
}
 
globSet
globSet_copyInto (/*@returned@*/ globSet s1, /*@exposed@*/ globSet s2)
{
  return (sRefSet_copyInto (s1, s2));
}

/*@only@*/ globSet
globSet_newCopy (globSet s)
{
  return (sRefSet_newCopy (s));
}

bool
globSet_member (globSet s, sRef el)
{
  return (sRefSet_member (s, el));
}

/*@exposed@*/ sRef globSet_lookup (globSet s, sRef el)
{
  sRefSet_allElements (s, e)
    {
      if (sRef_similar (e, el))
	{
	  return e;
	}
    } end_sRefSet_allElements;

  return sRef_undefined;
}

bool
globSet_hasStatic (globSet s)
{
  sRefSet_allElements (s, el)
    {
      if (sRef_isFileStatic (el))
	{
	  return TRUE;
	}
    } end_sRefSet_allElements;

  return FALSE;
}

void
globSet_free (/*@only@*/ globSet s)
{
  sRefSet_free (s);
}

/*@only@*/ cstring
globSet_dump (globSet lset)
{
  cstring st = cstring_undefined;
  bool first = TRUE;

  
  sRefSet_allElements (lset, el)
    {
      if (!first)
	{
	  st = cstring_appendChar (st, ',');
	}
      else
	{
	  first = FALSE;
	}

      st = cstring_concatFree (st, sRef_dumpGlobal (el));
    } end_sRefSet_allElements;

  return st;
}

globSet
globSet_undump (char **s)
{
  char c;
  sRefSet sl = sRefSet_new ();

  while ((c = **s) != '#' && c != '@' && c != '$' && c != '&')
    {
      sl = sRefSet_insert (sl, sRef_undumpGlobal (s));

      
      if (**s == ',')
	{
	  (*s)++;
	}
    }

    return sl;
}

/*@only@*/ cstring
globSet_unparse (globSet ll)
{
  /* return (sRefSet_unparseFull (ll)); */
  return (sRefSet_unparsePlain (ll)); 
}

int 
globSet_compare (globSet l1, globSet l2)
{
  return (sRefSet_compare (l1, l2));
}

