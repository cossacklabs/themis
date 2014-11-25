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
** letDeclNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ letDeclNodeList
letDeclNodeList_new ()
{
  letDeclNodeList s = (letDeclNodeList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = letDeclNodeListBASESIZE;
  s->elements = (letDeclNode *)
    dmalloc (sizeof (*s->elements) * letDeclNodeListBASESIZE);

  return (s);
}

static void
letDeclNodeList_grow (letDeclNodeList s)
{
  int i;
  letDeclNode *newelements;

  s->nspace += letDeclNodeListBASESIZE;

  newelements = (letDeclNode *) dmalloc (sizeof (*newelements)
					 * (s->nelements + s->nspace));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }

  sfree (s->elements); 
  s->elements = newelements;
}

letDeclNodeList
letDeclNodeList_add (letDeclNodeList s, letDeclNode el)
{
  llassert (letDeclNodeListBASESIZE > 0);

  if (s->nspace <= 0)
    letDeclNodeList_grow (s);

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

/*@only@*/ cstring
letDeclNodeList_unparse (letDeclNodeList s)
{
  cstring st = cstring_undefined;

  letDeclNodeList_elements (s, current)
  {
    st = message ("%qlet %s", st, ltoken_getRawString (current->varid));

    if (current->sortspec != NULL)
      {
	st = message ("%q: %q", st, lclTypeSpecNode_unparse (current->sortspec));
      }
    else
      {
	st = message ("%q ", st);
      }
    st = message ("%q = %q\n", st, termNode_unparse (current->term));
  } end_letDeclNodeList_elements;

  return st;
}

void
letDeclNodeList_free (letDeclNodeList s)
{
  int i;
  for (i = 0; i < s->nelements; i++)
    {
      letDeclNode_free (s->elements[i]); 
    }

  sfree (s->elements);
  sfree (s);
}
