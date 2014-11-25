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
** initDeclNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ initDeclNodeList
initDeclNodeList_new ()
{
  initDeclNodeList s = (initDeclNodeList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = initDeclNodeListBASESIZE;
  s->elements = (initDeclNode *)
    dmalloc (sizeof (*s->elements) * initDeclNodeListBASESIZE);

  return (s);
}

static void
initDeclNodeList_grow (initDeclNodeList s)
{
  int i;
  initDeclNode *newelements;

  s->nspace += initDeclNodeListBASESIZE;
  newelements = (initDeclNode *) dmalloc (sizeof (*newelements)
					  * (s->nelements + s->nspace));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }

  sfree (s->elements);
  s->elements = newelements;
}

initDeclNodeList
initDeclNodeList_add (initDeclNodeList s, initDeclNode el)
{
  if (s->nspace <= 0)
    initDeclNodeList_grow (s);

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

/*@only@*/ cstring
initDeclNodeList_unparse (initDeclNodeList s)
{
  cstring st = cstring_undefined;
  bool first = TRUE;

  initDeclNodeList_elements (s, current)
  {
    if (first)
      {
	first = FALSE;
	st = declaratorNode_unparse (current->declarator);
      }
    else
      {
	st = message ("%q, %q", st, declaratorNode_unparse (current->declarator));
      }

    if (current->value != (termNode) 0)
      {
	st = message ("%q = %q", st, termNode_unparse (current->value));
      }
  } end_initDeclNodeList_elements;

  return st;
}

void
initDeclNodeList_free (initDeclNodeList s)
{
  int i;
  for (i = 0; i < s->nelements; i++)
    {
      initDeclNode_free (s->elements[i]);
    }

  sfree (s->elements);
  sfree (s);
}
