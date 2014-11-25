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
** sortSetList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ sortSetList
sortSetList_new ()
{
  sortSetList s = (sortSetList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->free = sortSetListBASESIZE;
  s->elements = (sortSet *) dmalloc (sizeof (*s->elements) * sortSetListBASESIZE);
  s->current = 0;
  
  return (s);
}

static void
sortSetList_grow (sortSetList s)
{
  int i;
  sortSet *newelements;

  s->free += sortSetListBASESIZE;
  newelements = (sortSet *) dmalloc (sizeof (*newelements)
				     * (s->nelements + s->free));
  
  if (newelements == (sortSet *) 0)
    {
      llfatalerror (cstring_makeLiteral ("sortSetList_grow: out of memory!"));
    }

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }

  sfree (s->elements);
  s->elements = newelements;
}

void 
sortSetList_addh (sortSetList s, sortSet el)
{
  llassert (sortSetListBASESIZE > 0);

  if (s->free <= 0)
    sortSetList_grow (s);

  s->free--;
  s->elements[s->nelements] = el;
  s->nelements++;
}

void 
sortSetList_reset (sortSetList s)
{
  s->current = 0;
}

void 
sortSetList_advance (sortSetList s)
{
  s->current++;
}

/*@observer@*/ sortSet 
sortSetList_head (sortSetList s)
{
  llassert (s->nelements > 0);
  return (s->elements[0]);
}

/*@observer@*/ sortSet 
sortSetList_current (sortSetList s)
{
  if (s->current < 0 || s->current >= s->nelements)
    {
      llbug (message ("sortSetList_current: current out of range: %d (size: %d)",
		      s->current, s->nelements));
    }
  return (s->elements[s->current]);
}

/*@only@*/ cstring
sortSetList_unparse (sortSetList s)
{
  int i;
  cstring st = cstring_makeLiteral ("[ ");

  for (i = 0; i < s->nelements; i++)
    {
      if (i != 0)
	st = message ("%q, %q", st, sortSet_unparse (s->elements[i]));
      else
	st = message ("%q%q", st, sortSet_unparse (s->elements[i]));
    }
  
  st = message ("%q]", st);
  return st;
}

void
sortSetList_free (sortSetList s)
{
  /* note: elements are dependent */

  sfree (s->elements);	     
  sfree (s);
}
