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
** paramNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ paramNodeList
paramNodeList_new ()
{
  paramNodeList s = (paramNodeList) dmalloc (sizeof (*s));

  s->nelements = 0;
  s->nspace = paramNodeListBASESIZE;
  s->elements = (paramNode *) 
    dmalloc (sizeof (*s->elements) * paramNodeListBASESIZE);

  return (s);
}

/*@only@*/ paramNodeList
paramNodeList_single (/*@keep@*/ paramNode p)
{
  paramNodeList s = (paramNodeList) dmalloc (sizeof (*s));
  
  s->nelements = 1;
  s->nspace = paramNodeListBASESIZE - 1;
  s->elements = (paramNode *) dmalloc (sizeof (*s->elements) * paramNodeListBASESIZE);
  s->elements[0] = p;

  return (s);
}

static void
paramNodeList_grow (/*@notnull@*/ paramNodeList s)
{
  int i;
  paramNode *newelements;

  s->nspace += paramNodeListBASESIZE;

  newelements = (paramNode *) dmalloc (sizeof (*newelements)
				       * (s->nelements + s->nspace));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }

  sfree (s->elements);
  s->elements = newelements;
}

paramNodeList
paramNodeList_add (paramNodeList s, paramNode el)
{
  llassert (paramNodeList_isDefined (s));

  if (s->nspace <= 0)
    paramNodeList_grow (s);

  s->nspace--;
  s->elements[s->nelements] = el;
  
  s->nelements++;
  return s;
}

/*@only@*/ paramNodeList 
paramNodeList_copy (paramNodeList s)
{
  paramNodeList r = paramNodeList_new ();

  paramNodeList_elements (s, x)
  {
    r = paramNodeList_add (r, paramNode_copy (x));
  } end_paramNodeList_elements;

  return r;
}

/*@only@*/ cstring
paramNodeList_unparse (paramNodeList s)
{
  bool first = TRUE;
  cstring st = cstring_undefined;

  paramNodeList_elements (s, current)
  {
    if (first)
      {
	st = paramNode_unparse (current);
	first = FALSE;
      }
    else
      {
	st = message ("%q, %q", st, paramNode_unparse (current));
      }
  } end_paramNodeList_elements;

  return st;
}

/*@only@*/ cstring
paramNodeList_unparseComments (paramNodeList s)
{
  bool first = TRUE;
  cstring st = cstring_undefined;

  paramNodeList_elements (s, current)
  {
    if (first)
      {
	st = paramNode_unparseComments (current);
	first = FALSE;
      }
    else
      {
	st = message ("%q, %q", st, paramNode_unparseComments (current));
      }
  } end_paramNodeList_elements;

  return st;
}

void
paramNodeList_free (/*@only@*/ paramNodeList s)
{
  if (paramNodeList_isDefined (s))
    {
      int i;
      for (i = 0; i < s->nelements; i++)
	{
	  paramNode_free (s->elements[i]); 
	}
      
      sfree (s->elements); 
      sfree (s);
    }
}
