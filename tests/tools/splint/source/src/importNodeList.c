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
** importNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ importNodeList
importNodeList_new ()
{
  importNodeList s = (importNodeList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = importNodeListBASESIZE;
  s->elements = (importNode *) 
    dmalloc (sizeof (*s->elements) * importNodeListBASESIZE);
  
  return (s);
}

static void
importNodeList_grow (importNodeList s)
{
  int i;
  importNode *newelements;

  s->nspace += importNodeListBASESIZE;

  newelements = (importNode *) dmalloc (sizeof (*newelements) 
					* (s->nelements + s->nspace));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }

  sfree (s->elements);
  s->elements = newelements;
}

importNodeList
importNodeList_add (importNodeList s, importNode el)
{
  if (s->nspace <= 0)
    importNodeList_grow (s);

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;
  return s;
}

/*@only@*/ cstring
importNodeList_unparse (importNodeList s)
{
  bool first = TRUE;
  cstring st = cstring_undefined;

  importNodeList_elements (s, current)
  {
    if (first)
      {
	st = cstring_copy (ltoken_unparse (current->val));
	first = FALSE;
      }
    else
      {
	st = message ("%q, %s", st, ltoken_unparse (current->val));
      }
  } end_importNodeList_elements;

  return st;
}

void
importNodeList_free (importNodeList s)
{
  int i;
  
  for (i = 0; i < s->nelements; i++)
    {
      importNode_free (s->elements[i]); 
    }
  
  sfree (s->elements);
  sfree (s);
}
