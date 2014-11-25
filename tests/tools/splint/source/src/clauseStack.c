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
** clauseStack.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

clauseStack
clauseStack_new ()
{
  clauseStack s = (clauseStack) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = clauseStackBASESIZE; 
  s->elements = (clause *) dmalloc (sizeof (*s->elements) * clauseStackBASESIZE);
  s->current = 0;
  
  return (s);
}

static void
clauseStack_grow (clauseStack s)
{
  int i;
  clause *newelements; 
  
  s->nspace += clauseStackBASESIZE; 

  newelements = (clause *) dmalloc (sizeof (*newelements)
				    * (s->nelements + s->nspace));
  
  if (newelements == (clause *) 0)
    {
      llfatalerror (cstring_makeLiteral ("clauseStack_grow: out of memory!"));
    }
  
  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }
  
  sfree (s->elements);
  s->elements = newelements;
}

void clauseStack_push (clauseStack s, clause el)
{
  if (s->nspace <= 0)
    clauseStack_grow (s);
  
  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;
  }

void clauseStack_pop (clauseStack s)
{
  s->nelements--;
  s->nspace++;
  }

clause clauseStack_top (clauseStack s)
{
  return (s->elements[s->nelements - 1]);
}

void clauseStack_switchTop (clauseStack s, clause x)
{
  llassert (s->nelements > 0);
  
  s->elements[s->nelements - 1] = x;
}

void
clauseStack_removeFirst (clauseStack s, clause key)
{
  if (clauseStack_top (s) == key) 
    {
      clauseStack_pop (s);
    }
  else
    {
      int i;
      
      for (i = s->nelements - 2; i >= 0; i--)
	{
	  clause el = s->elements[i];
	  
	  if (el == key) 
	    {
	      int j;

	      for (j = i; j < s->nelements - 1; j++)
		{
		  s->elements[j] = s->elements[j + 1];
		}

	      s->nelements--;
	      s->nspace++;
	      return;
	    }
	}
      
      llbuglit ("clauseStack_removeFirst: not found");
    }
}

int
clauseStack_controlDepth (clauseStack s)
{
  int depth = 0;
  int i;

  for (i = 0; i < s->nelements; i++)
    {
      clause current = s->elements[i];

      if (clause_isConditional (current))
	{
	  depth++;
	}
    }

  return depth;
}

cstring
clauseStack_unparse (clauseStack s)
{
  int i;
  cstring st = cstring_makeLiteral ("[");
  
  for (i = 0; i < s->nelements; i++)
    {
      if (i == 0)
	{
	  st = message ("%q %s", st, clause_unparse (s->elements[i]));
	}
      else
	st = message ("%q, %s", st, clause_unparse (s->elements[i]));
    }
  
  st = message ("%q ]", st);
  return st;
}

void
clauseStack_clear (clauseStack s)
{
  s->nspace += s->nelements;
  s->nelements = 0;
}

void
clauseStack_free (clauseStack s)
{
  sfree (s->elements); 
  sfree (s);
}
