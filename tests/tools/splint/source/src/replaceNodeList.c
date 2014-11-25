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
** replaceNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ replaceNodeList
replaceNodeList_new ()
{
  replaceNodeList s = (replaceNodeList) dmalloc (sizeof (*s));

  s->nelements = 0;
  s->nspace = replaceNodeListBASESIZE;
  s->elements = (replaceNode *)
    dmalloc (sizeof (*s->elements) * replaceNodeListBASESIZE);

  return (s);
}

static void
replaceNodeList_grow (replaceNodeList s)
{
  int i;
  replaceNode *newelements; 

  s->nspace += replaceNodeListBASESIZE;

  newelements = (replaceNode *) dmalloc (sizeof (*newelements)
					 * (s->nelements + s->nspace));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }

  sfree (s->elements); 
  s->elements  = newelements;
}

replaceNodeList
  replaceNodeList_add (replaceNodeList s, replaceNode el)
{
  if (s->nspace <= 0)
    replaceNodeList_grow (s);

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

/*@only@*/ cstring
replaceNodeList_unparse (replaceNodeList s)
{
  bool first = TRUE;
  cstring st = cstring_undefined;

  replaceNodeList_elements (s, current)
  {
    if (first)
      {
	st = replaceNode_unparse (current);
	first = FALSE;
      }
    else
      {
	st = message ("%q, %q", st, replaceNode_unparse (current));
      }
  } end_replaceNodeList_elements;

  return st;
}

void
replaceNodeList_free (replaceNodeList s)
{
  int i;
  for (i = 0; i < s->nelements; i++)
    {
      replaceNode_free (s->elements[i]); 
    }

  sfree (s->elements); 
  sfree (s);
}
