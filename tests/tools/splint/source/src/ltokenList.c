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
** ltokenList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@notnull@*/ /*@only@*/ ltokenList
ltokenList_new ()
{
  ltokenList s = (ltokenList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = ltokenListBASESIZE;
  s->elements = (ltoken *) 
    dmalloc (sizeof (*s->elements) * ltokenListBASESIZE);
  s->current = 0;

  return (s);
}

/*@notnull@*/ /*@only@*/ ltokenList
ltokenList_singleton (ltoken l)
{
  ltokenList s = (ltokenList) dmalloc (sizeof (*s));

  s->nelements = 1;
  s->nspace = ltokenListBASESIZE - 1;
  s->elements = (ltoken *) dmalloc (sizeof (*s->elements) * ltokenListBASESIZE);
  s->elements[0] = l;
  s->current = 0;

  return (s);
}

static void
ltokenList_grow (/*@notnull@*/ ltokenList s)
{
  int i;
  ltoken *newelements;

  s->nspace += ltokenListBASESIZE;

  newelements = (ltoken *) dmalloc (sizeof (*newelements)
				    * (s->nelements + s->nspace));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] =  s->elements[i]; 
    }

  sfree (s->elements); 
  s->elements = newelements;
}

ltokenList 
ltokenList_push (/*@returned@*/ ltokenList s, ltoken el)
{
  ltokenList_addh (s, el);
  return s;
}

void 
ltokenList_addh (ltokenList s, ltoken el)
{
  llassert (ltokenList_isDefined (s));

  if (s->nspace <= 0)
    ltokenList_grow (s);

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;
}

void 
ltokenList_reset (ltokenList s)
{
  if (ltokenList_isDefined (s))
    {
      s->current = 0;
    }
}

bool
ltokenList_isFinished (ltokenList s)
{
  return (ltokenList_isUndefined(s) || (s->current == s->nelements));
}

void 
ltokenList_advance (ltokenList s)
{
  if (ltokenList_isDefined (s))
    {
      s->current++;
      llassert (s->current <= s->nelements);
    }
}

ltoken 
ltokenList_head (ltokenList s)
{
  llassert (ltokenList_isDefined (s) && s->nelements > 0);
  return (s->elements[0]);
}

bool 
ltokenList_equal (ltokenList s1, ltokenList s2)
{
  if (ltokenList_isUndefined (s1))
    {
      return (ltokenList_isEmpty (s2));
    }
  else
    {
      if (ltokenList_isUndefined (s2))
	{
	  return ltokenList_isEmpty (s1);
	}
      else
	{
	  int i;
	  int size = s1->nelements;
	  
	  if (s2->nelements != size)
	    return FALSE;
	  
	  for (i = 0; i < size; i++)
	    {
	      if (!ltoken_similar (s1->elements[i], s2->elements[i]))
		return FALSE;
	    }
	  return TRUE;
	}
    }
}

/*@only@*/ ltokenList 
ltokenList_copy (ltokenList s)
{
  ltokenList r = ltokenList_new ();

  ltokenList_elements (s, x)
  {
    ltokenList_addh (r, ltoken_copy (x));
  } end_ltokenList_elements;

  return r;
}

void
ltokenList_removeCurrent (ltokenList s)
{
  int i;
  llassert (ltokenList_isDefined (s) && s->current >= 0 && s->current < s->nelements);

  for (i = s->current; i < s->nelements - 1; i++)
    {
      s->elements[i] = s->elements[i+1];
    }

  s->nelements--;
  s->nspace++;
}

ltoken 
ltokenList_current (ltokenList s)
{
  llassert (ltokenList_isDefined (s) && s->current >= 0 && s->current < s->nelements);
  return (s->elements[s->current]);
}

/*@only@*/ cstring
ltokenList_unparse (ltokenList s)
{
  int i;
  cstring st = cstring_undefined;

  if (ltokenList_isDefined (s))
    {
      for (i = 0; i < s->nelements; i++)
	{
	  if (i == 0)
	    {
	      st = cstring_copy (ltoken_unparse (s->elements[i]));
	    }
	  else
	    st = message ("%q, %s", st, ltoken_unparse (s->elements[i]));
	}
    }

  return st;
}

void
ltokenList_free (ltokenList s)
{
  if (ltokenList_isDefined (s))
    {
      sfree (s->elements);
      sfree (s);
    }
}
