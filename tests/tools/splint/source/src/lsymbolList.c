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
** lsymbolList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ lsymbolList
lsymbolList_new ()
{
  lsymbolList s = (lsymbolList) dmalloc (sizeof (*s));
  
  s->nelements = 0;

  s->nspace = lsymbolListBASESIZE;
  s->elements = (lsymbol *) dmalloc (sizeof (*s->elements) * lsymbolListBASESIZE);

  return (s);
}

static void
lsymbolList_grow (lsymbolList s)
{
  int i;
  lsymbol *newelements;

  s->nspace += lsymbolListBASESIZE;
  
   newelements = (lsymbol *) dmalloc (sizeof (*newelements) * (s->nelements + s->nspace));

  if (newelements == (lsymbol *) 0)
    {
      llfatalerror (cstring_makeLiteral ("lsymbolList_grow: out of memory!"));
    }

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }

  sfree (s->elements);
  s->elements = newelements;
}

void 
lsymbolList_addh (lsymbolList s, lsymbol el)
{
  if (s->nspace <= 0)
    lsymbolList_grow (s);

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;
}

void
lsymbolList_free (lsymbolList s)
{
  int i;
  for (i = 0; i < s->nelements; i++)
    {
     /*      lsymbol_free (s->elements[i]); */
    }

  sfree (s->elements);	     
  sfree (s);
}

