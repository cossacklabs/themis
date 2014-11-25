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
** termNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

termNodeList termNodeList_new ()
{
  termNodeList s = (termNodeList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspacelow = termNodeListGROWLOW;
  s->nspacehigh = termNodeListGROWHI;
  s->elementsroot = (termNode *) dmalloc (sizeof (*s->elements) * (s->nspacelow + s->nspacehigh));
  s->elements = s->elementsroot + termNodeListGROWLOW;
  s->current = 0;

  return (s);
}

static void
termNodeList_grow (termNodeList s)
{
  int i;
  termNode *newelements = (termNode *) dmalloc (sizeof (*newelements)
						* (s->nelements + termNodeListBASESIZE));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i + termNodeListGROWLOW] = s->elements[i];
    }
  
  sfree (s->elementsroot);

  s->nspacelow = termNodeListGROWLOW;
  s->nspacehigh = termNodeListGROWHI; 

  s->elementsroot = newelements;
  s->elements = s->elementsroot + s->nspacelow;
}

void 
termNodeList_addh (termNodeList s, termNode el)
{
  llassert (termNodeListGROWHI > 0);

  if (s->nspacehigh <= 0)
    termNodeList_grow (s);

  s->nspacehigh--;
  s->elements[s->nelements] = el;
  s->nelements++;
}

termNodeList 
termNodeList_push (termNodeList s, termNode el)
{
  termNodeList_addh (s, el);
  return s;
}

void 
termNodeList_addl (termNodeList s, termNode el)
{
  llassert (termNodeListGROWLOW > 0);

  if (s->nspacelow <= 0)
    termNodeList_grow (s);

  s->nspacelow--;
  s->elements--;
  s->elements[0] = el;
  s->current++;
  s->nelements++;
}

void 
termNodeList_reset (termNodeList s)
{
  s->current = 0;
}

void 
termNodeList_finish (termNodeList s)
{
  s->current = s->nelements - 1;
}

void 
termNodeList_advance (termNodeList s)
{
  s->current++;
  llassert (s->current < s->nelements);
}

/*@exposed@*/ termNode 
termNodeList_head (termNodeList s)
{
  llassert (s->nelements > 0);
  return (s->elements[0]);
}

/*@only@*/ termNodeList 
termNodeList_copy (termNodeList s)
{
  termNodeList r = termNodeList_new ();

  termNodeList_elements (s, x)
  {
    termNodeList_addh (r, termNode_copySafe (x));
  } end_termNodeList_elements;

  return r;
}

/*@exposed@*/ termNode 
termNodeList_current (termNodeList s)
{
  llassert (!(s->current >= s->nelements));
  return (s->elements[s->current]);
}

termNode 
termNodeList_getN (termNodeList s, int n)
{
  llassert (n >= 0 && n < s->nelements);

  return (s->elements[n]);
}

/*@only@*/ cstring
termNodeList_unparse (termNodeList s)
{
  bool first = TRUE;
  cstring st = cstring_undefined;

  termNodeList_elements (s, current)
  {
    if (first)
      {
	st = termNode_unparse (current);
	first = FALSE;
      }
    else
      st = message ("%q, %q", st, termNode_unparse (current));
  } end_termNodeList_elements;

  return st;
}

/*@only@*/ cstring
termNodeList_unparseTail (termNodeList s)
{
  bool head = TRUE;
  bool first = TRUE;
  cstring st = cstring_undefined;

  termNodeList_elements (s, current)
  {
    if (head)
      {
	head = FALSE;
      }
    else
      {
	if (first)
	  {
	    st = termNode_unparse (current);
	    first = FALSE;
	  }
	else
	  st = message ("%q, %q", st, termNode_unparse (current));
      }
  } end_termNodeList_elements;

  return st;
}

/*@only@*/ cstring
termNodeList_unparseToCurrent (termNodeList s)
{
  int i;
  cstring st = cstring_undefined;

  for (i = 0; i < s->current; i++)
    {
      termNode current = s->elements[i];

      if (i == 0)
	st = termNode_unparse (current);
      else
	st = message ("%q, %q", st, termNode_unparse (current));
    }

  return st;
}

/*@only@*/ cstring
termNodeList_unparseSecondToCurrent (termNodeList s)
{
  int i;
  cstring st = cstring_undefined;

  for (i = 1; i < s->current; i++)
    {
      termNode current = s->elements[i];

      if (i == 1)
	{
	  st = termNode_unparse (current);
	}
      else
	{
	  st = message ("%q, %q", st, termNode_unparse (current));
	}
    }

  return st;
}

void
termNodeList_free (termNodeList s)
{
  int i;
  for (i = 0; i < s->nelements; i++)
    {
      termNode_free (s->elements[i]); 
    }

  sfree (s->elementsroot);
  sfree (s);
}
