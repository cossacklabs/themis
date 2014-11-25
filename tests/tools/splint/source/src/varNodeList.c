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
** varNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

varNodeList varNodeList_new ()
{
  varNodeList s = (varNodeList) dmalloc (sizeof (*s));

  s->nelements = 0;
  s->nspace = varNodeListBASESIZE;
  s->elements = (varNode *)
    dmalloc (sizeof (*s->elements) * varNodeListBASESIZE);

  return (s);
}

static void
varNodeList_grow (varNodeList s)
{
  int i;
  varNode *newelements; 

  s->nspace += varNodeListBASESIZE;
  newelements = (varNode *) dmalloc (sizeof (*newelements)
				     * (s->nelements + s->nspace));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i]; 
    }

  sfree (s->elements); 
  s->elements = newelements;
}

varNodeList 
varNodeList_add (varNodeList s, varNode el)
{
  if (s->nspace <= 0)
    varNodeList_grow (s);

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;
  return s;
}

cstring
varNodeList_unparse (varNodeList s)
{
  int i;
  cstring st = cstring_undefined;
  bool first = TRUE;

  for (i = 0; i < s->nelements; i++)
    {
      cstring type = cstring_undefined;
      varNode current = s->elements[i];

      if (current->isObj)
	{
	  type = cstring_makeLiteral ("obj ");
	}

      if (current->type != NULL)
	{
	  type = message (": %q%q", type, lclTypeSpecNode_unparse (current->type));
	}

      if (first)
	{
	  st = type;
	  first = FALSE;
	}
      else
	{
	  st = message ("%q, %q", st, type);
	}
    }

  return st;
}

void
varNodeList_free (varNodeList s)
{
  int i;
  for (i = 0; i < s->nelements; i++)
    {
      varNode_free (s->elements[i]); 
    }

  sfree (s->elements);
  sfree (s);
}

varNodeList
varNodeList_copy (varNodeList s)
{
  varNodeList ret = varNodeList_new ();

  varNodeList_elements (s, el)
    {
      ret = varNodeList_add (ret, varNode_copy (el));
    } end_varNodeList_elements;

  return ret;
}
