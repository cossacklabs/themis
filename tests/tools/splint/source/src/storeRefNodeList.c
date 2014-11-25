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
** storeRefNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ storeRefNodeList
storeRefNodeList_new ()
{
  storeRefNodeList s = (storeRefNodeList) dmalloc (sizeof (*s));

  s->nelements = 0;
  s->nspace = storeRefNodeListBASESIZE;
  s->elements = (storeRefNode *) 
    dmalloc (sizeof (*s->elements) * storeRefNodeListBASESIZE);

  return (s);
}

static void
storeRefNodeList_grow (storeRefNodeList s)
{
  int i;
  storeRefNode *newelements; 

  s->nspace += storeRefNodeListBASESIZE;

  newelements = (storeRefNode *) dmalloc (sizeof (*newelements)
					  * (s->nelements + s->nspace));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i]; 
    }

  sfree (s->elements); 
  s->elements = newelements;
}

storeRefNodeList
storeRefNodeList_add (storeRefNodeList s, storeRefNode el)
{
  if (s->nspace <= 0)
    storeRefNodeList_grow (s);

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

/*@only@*/ storeRefNodeList 
storeRefNodeList_copy (storeRefNodeList s)
{
  storeRefNodeList r = storeRefNodeList_new ();

  storeRefNodeList_elements (s, x)
    {
      r = storeRefNodeList_add (r, storeRefNode_copy (x));
    } end_storeRefNodeList_elements;

  return r;
}

/*@only@*/ cstring
storeRefNodeList_unparse (storeRefNodeList s)
{
  bool first = TRUE;
  cstring st = cstring_undefined;

  storeRefNodeList_elements (s, current)
  {
    if (first)
      {
	first = FALSE;
      }
    else
      {
	st = message ("%q, ", st);
      }

    switch (current->kind)
      {
      case SRN_OBJ:
	st = message ("%qobj", st);
	/*@switchbreak@*/ break;
      case SRN_TERM:
	st = message ("%q%q", st, termNode_unparse (current->content.term));
	/*@switchbreak@*/ break;
      case SRN_TYPE:
	st = message ("%q%q", st, lclTypeSpecNode_unparse (current->content.type));
	/*@switchbreak@*/ break;
      case SRN_SPECIAL:
	st = message ("%q%q", st, sRef_unparse (current->content.ref));
	/*@switchbreak@*/ break;
      }
  } end_storeRefNodeList_elements;

  return st;
}

void
storeRefNodeList_free (storeRefNodeList s)
{
  int i;
  for (i = 0; i < s->nelements; i++)
    {
      storeRefNode_free (s->elements[i]);
    }

  sfree (s->elements);
  sfree (s);
}
