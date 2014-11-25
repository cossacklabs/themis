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
** quantifierNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ quantifierNodeList
quantifierNodeList_new ()
{
  quantifierNodeList s = (quantifierNodeList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = quantifierNodeListBASESIZE;
  s->elements = (quantifierNode *) 
    dmalloc (sizeof (*s->elements) * quantifierNodeListBASESIZE);

  return (s);
}

static void
quantifierNodeList_grow (quantifierNodeList s)
{
  int i;
  quantifierNode *newelements; 

  s->nspace += quantifierNodeListBASESIZE;

  newelements = (quantifierNode *) dmalloc (sizeof (*newelements)
					    * (s->nelements + s->nspace));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i]; 
    }

  sfree (s->elements);
  s->elements = newelements;
}

quantifierNodeList 
quantifierNodeList_add (quantifierNodeList s, quantifierNode el)
{
  if (s->nspace <= 0)
    quantifierNodeList_grow (s);

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

/*@only@*/ quantifierNodeList 
quantifierNodeList_copy (quantifierNodeList s)
{
  quantifierNodeList r = quantifierNodeList_new ();

  quantifierNodeList_elements (s, x)
    {
      r = quantifierNodeList_add (r, quantifierNode_copy (x));
    } end_quantifierNodeList_elements;
  
  return r;
}

/*@only@*/ cstring
quantifierNodeList_unparse (quantifierNodeList s)
{
  cstring st = cstring_undefined;

  quantifierNodeList_elements (s, current)
  {
    st = message ("%q%s %q", 
		  st, ltoken_getRawString (current->quant),
		  varNodeList_unparse (current->vars));
  } end_quantifierNodeList_elements;

  return st;
}

void
quantifierNodeList_free (quantifierNodeList s)
{
  int i;
  for (i = 0; i < s->nelements; i++)
    {
      quantifierNode_free (s->elements[i]); 
    }

  sfree (s->elements); 
  sfree (s);
}
