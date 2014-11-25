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
** structDeclNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ stDeclNodeList
stDeclNodeList_new ()
{
  stDeclNodeList s = (stDeclNodeList) dmalloc (sizeof (*s));

  s->nelements = 0;
  s->nspace = stDeclNodeListBASESIZE;
  s->elements = (stDeclNode *)
    dmalloc (sizeof (*s->elements) * stDeclNodeListBASESIZE);

  return (s);
}

static void
stDeclNodeList_grow (stDeclNodeList s)
{
  int i;
  stDeclNode *newelements; 

  s->nspace += stDeclNodeListBASESIZE;

  newelements = (stDeclNode *) dmalloc (sizeof (*newelements)
					* (s->nelements + s->nspace));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i]; 
    }

  sfree (s->elements); 
  s->elements = newelements;
}

stDeclNodeList  
stDeclNodeList_add (stDeclNodeList s, /*@only@*/ stDeclNode el)
{
  if (s->nspace <= 0)
    stDeclNodeList_grow (s);

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

/*@only@*/ stDeclNodeList 
stDeclNodeList_copy (stDeclNodeList s)
{
  stDeclNodeList r = stDeclNodeList_new ();

  stDeclNodeList_elements (s, x)
    {
      r = stDeclNodeList_add (r, stDeclNode_copy (x));
    } end_stDeclNodeList_elements;
  
  return r;
}

/*@only@*/ cstring
stDeclNodeList_unparse (stDeclNodeList s)
{
  bool first = TRUE;
  cstring st = cstring_undefined;

  stDeclNodeList_elements (s, current)
  {
    if (first)
      {
	st = message ("%q %q;", lclTypeSpecNode_unparse (current->lcltypespec),
		      declaratorNodeList_unparse (current->declarators));
	first = FALSE;
      }
    else
      {
	st = message ("%q %q %q;", st, lclTypeSpecNode_unparse (current->lcltypespec),
		      declaratorNodeList_unparse (current->declarators));
      }
  } end_stDeclNodeList_elements;

  return st;
}

void
stDeclNodeList_free (stDeclNodeList s)
{
  int i;
  for (i = 0; i < s->nelements; i++)
    {
      stDeclNode_free (s->elements[i]); 
    }

  sfree (s->elements); 
  sfree (s);
}
