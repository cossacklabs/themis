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
** sortList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ sortList
sortList_new ()
{
  sortList s = (sortList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = sortListBASESIZE;
  s->elements = (sort *) dmalloc (sizeof (*s->elements) * sortListBASESIZE);
  s->current = 0;

  return (s);
}

static void
sortList_grow (sortList s)
{
  int i;
  sort *newelements;

  s->nspace += sortListBASESIZE;

  newelements = (sort *) dmalloc (sizeof (*newelements) * (s->nelements + s->nspace));

  if (newelements == (sort *) 0)
    {
      llfatalerror (cstring_makeLiteral ("sortList_grow: out of memory!"));
    }

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }

  sfree (s->elements);
  s->elements = newelements;
}

void 
sortList_addh (sortList s, sort el)
{
  if (s->nspace <= 0)
    sortList_grow (s);

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;
}

void 
sortList_reset (sortList s)
{
  s->current = 0;
}

void 
sortList_advance (sortList s)
{
  s->current++;
  llassert (s->current < s->nelements);
}

sort 
sortList_current (sortList s)
{
  if (s->current < 0 || s->current >= s->nelements)
    {
      llbug (message ("sortList_current: current out of range: %d (size: %d)",
		      s->current, s->nelements));
    }
  return (s->elements[s->current]);
}

/*@only@*/ cstring
sortList_unparse (sortList s)
{
  int i;
  cstring st = cstring_undefined;

  for (i = 0; i < s->nelements; i++)
    {
      if (i == 0)
	{
	  st = cstring_copy (sort_unparseName (s->elements[i])); /* !!! NEED COPY HERE !!! */
	}
      else
	{
	  st = message ("%q, %s", st, sort_unparseName (s->elements[i]));
	}
    }

  return st;
}

void
sortList_free (sortList s)
{
  int i;
  for (i = 0; i < s->nelements; i++)
    {
      /*      sort_free (s->elements[i]); */
    }
  
  sfree (s->elements);		/* not quite!!! */
  sfree (s);
}
