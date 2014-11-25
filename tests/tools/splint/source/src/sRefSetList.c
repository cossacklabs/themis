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
** sRefSetList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

static /*@notnull@*/ /*@only@*/ sRefSetList
sRefSetList_newEmpty (void)
{
  sRefSetList s = (sRefSetList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = sRefSetListBASESIZE; 
  s->elements = (sRefSet *) dmalloc (sizeof (*s->elements) * sRefSetListBASESIZE);

  return (s);
}

static void
sRefSetList_grow (/*@notnull@*/ sRefSetList s)
{
  int i;
  sRefSet *newelements;
  
  s->nspace += sRefSetListBASESIZE; 

  newelements = (sRefSet *) dmalloc (sizeof (*newelements) 
				     * (s->nelements + s->nspace));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }
  
  sfree (s->elements); 
  s->elements = newelements;
}

sRefSetList sRefSetList_add (sRefSetList s, /*@dependent@*/ sRefSet el)
{
  if (sRefSetList_isUndefined (s))
    {
      s = sRefSetList_newEmpty ();
    }

  if (s->nspace <= 0)
    {
      sRefSetList_grow (s);
    }
  
  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

void
sRefSetList_clear (sRefSetList s)
{
  if (sRefSetList_isDefined (s))
    {
      s->nspace += s->nelements;
      s->nelements = 0;
    }
}

void
sRefSetList_free (sRefSetList s)
{
  if (sRefSetList_isDefined (s))
    {
      sfree (s->elements);
      sfree (s);
    }
}
