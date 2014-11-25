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
** traitRefNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ traitRefNodeList
traitRefNodeList_new ()
{
  traitRefNodeList s = (traitRefNodeList) dmalloc (sizeof (*s));

  s->nelements = 0;
  s->nspace = traitRefNodeListBASESIZE;
  s->elements = (traitRefNode *)
    dmalloc (sizeof (*s->elements) * traitRefNodeListBASESIZE);

  return (s);
}

static void
traitRefNodeList_grow (traitRefNodeList s)
{
  int i;
  traitRefNode *newelements; 

  s->nspace += traitRefNodeListBASESIZE;
  newelements = (traitRefNode *) dmalloc (sizeof (*newelements)
					  * (s->nelements + s->nspace));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i]; 
    }

  sfree (s->elements);
  s->elements = newelements;
}

traitRefNodeList
traitRefNodeList_add (traitRefNodeList s, /*@only@*/ traitRefNode el)
{
  if (s->nspace <= 0)
    traitRefNodeList_grow (s);

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

/*@only@*/ cstring
traitRefNodeList_unparse (traitRefNodeList s)
{
  cstring st = cstring_undefined;

  traitRefNodeList_elements (s, current)
  {
    st = message ("%quses (%q)", st, printLeaves2 (current->traitid));

    if (current->rename != 0)
      {
	st = message ("%q (%q)", st, renamingNode_unparse (current->rename));
      }

    st = message ("%q\n", st);
  } end_traitRefNodeList_elements;

  return (st);
}

void
traitRefNodeList_free (traitRefNodeList s)
{
  int i;
  for (i = 0; i < s->nelements; i++)
    {
      traitRefNode_free (s->elements[i]); 
    }

  sfree (s->elements); 
  sfree (s);
}
