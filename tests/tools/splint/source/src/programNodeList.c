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
** programNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ programNodeList
programNodeList_new ()
{
  programNodeList s = (programNodeList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = programNodeListBASESIZE;
  s->elements = (programNode *) 
    dmalloc (sizeof (*s->elements) * programNodeListBASESIZE);

  return (s);
}

static void
programNodeList_grow (programNodeList s)
{
  int i;
  programNode *newelements;

  s->nspace += programNodeListBASESIZE;
  newelements = (programNode *) dmalloc (sizeof (*newelements)
					 * (s->nelements + s->nspace));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }

  sfree (s->elements); 
  s->elements = newelements;
}

void 
programNodeList_addh (programNodeList s, /*@keep@*/ programNode el)
{
  if (s->nspace <= 0)
    programNodeList_grow (s);

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;
}

/*@only@*/ cstring
programNodeList_unparse (programNodeList s)
{
  bool first = TRUE;
  cstring st = cstring_undefined;

  programNodeList_elements (s, current)
  {
    if (first)
      {
	st = programNode_unparse (current);
	first = FALSE;
      }
    else
      {
	st = message ("%q; %q", st, programNode_unparse (current));
      }
  } end_programNodeList_elements;

  return st;
}

void
programNodeList_free (programNodeList s)
{
  int i;
  for (i = 0; i < s->nelements; i++)
    {
      programNode_free (s->elements[i]); 
    }

  sfree (s->elements); 
  sfree (s);
}
