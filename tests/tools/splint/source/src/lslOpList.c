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
** lslOpList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ lslOpList
lslOpList_new ()
{
  lslOpList s = (lslOpList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = lslOpListBASESIZE;
  s->elements = (lslOp *)
    dmalloc (sizeof (*s->elements) * lslOpListBASESIZE);

  return (s);
}

static void
lslOpList_grow (lslOpList s)
{
  int i;
  lslOp *newelements; 

  s->nspace += lslOpListBASESIZE;
  
  newelements = (lslOp *) dmalloc (sizeof (*newelements)
					 * (s->nelements + s->nspace));
  
  if (newelements == (lslOp *) 0)
    {
      llfatalerror (cstring_makeLiteral ("lslOpList_grow: out of memory!"));
    }

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }

  sfree (s->elements);
  s->elements = newelements;
}

void 
lslOpList_add (lslOpList s, lslOp el)
{
  llassert (lslOpListBASESIZE != 0);

  if (s->nspace <= 0)
    {
      lslOpList_grow (s);
    }

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;
}

/*@only@*/ cstring
lslOpList_unparse (lslOpList s)
{
  int i;
  cstring st = cstring_undefined;

  for (i = 0; i < s->nelements; i++)
    {
      st = message ("%q%q\n", st, lslOp_unparse (s->elements[i]));
    }

  return st;
}

void
lslOpList_free (lslOpList s)
{
  sfree (s->elements);	
  sfree (s);
}
