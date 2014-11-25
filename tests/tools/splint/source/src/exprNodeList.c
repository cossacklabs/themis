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
** exprNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ exprNodeList
exprNodeList_new ()
{
  exprNodeList s = (exprNodeList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = exprNodeListBASESIZE; 
  s->elements = (exprNode *)
    dmalloc (sizeof (*s->elements) * exprNodeListBASESIZE);
  s->current = 0;

  return (s);
}

static void
exprNodeList_grow (exprNodeList s)
{
  int i;
  exprNode *newelements; 
  int numnew;

  if (s->nelements < exprNodeListBASESIZE)
    {
      numnew = exprNodeListBASESIZE;
    }
  else
    {
      numnew = s->nelements;
    }

  s->nspace = numnew + s->nspace; 

  newelements = (exprNode *) dmalloc (sizeof (*newelements) * (s->nelements + numnew));

  if (newelements == (exprNode *) 0)
    {
      llfatalerror (cstring_makeLiteral ("exprNodeList_grow: out of memory!"));
    }

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }
  
  sfree (s->elements);
  s->elements = newelements;
}

void exprNodeList_addh (exprNodeList s, /*@only@*/ exprNode el)
{
  llassert (exprNodeListBASESIZE > 0);

  if (s->nspace <= 0)
    exprNodeList_grow (s);
  
  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;
}

void exprNodeList_reset (exprNodeList s)
{
  s->current = 0;
}

void exprNodeList_advance (exprNodeList s)
{
  s->current++;
  llassert (s->current <= s->nelements);
}

/*@observer@*/ exprNode exprNodeList_head (exprNodeList s)
{
  llassert (s->nelements > 0);
  return (s->elements[0]);
}

/*@observer@*/ exprNode exprNodeList_current (exprNodeList s)
{
  llassert (s->current >= 0 && s->current < s->nelements);
  return (s->elements[s->current]);
}

exprNode exprNodeList_getN (exprNodeList s, int n)
{
  llassert (n >= 0 && n < s->nelements);
  return (s->elements[n]);
}

/*@only@*/ exprNodeList exprNodeList_singleton (/*@only@*/ exprNode e)
{
  exprNodeList s = (exprNodeList) dmalloc (sizeof (*s));
  
  s->nelements = 1;
  s->nspace = exprNodeListBASESIZE - 1; 
  s->elements = (exprNode *) dmalloc (sizeof (*s->elements) * exprNodeListBASESIZE);
  s->elements[0] = e;
  s->current = 0;

  DPRINTF (("List: %s", exprNode_unparse (e)));
  return (s);
}

exprNodeList exprNodeList_push (/*@returned@*/ exprNodeList args, /*@only@*/ exprNode e)
{
  exprNodeList_addh (args, e);
  return (args);
}

/*@exposed@*/ exprNode
exprNodeList_nth (exprNodeList args, int n)
{
  if (n >= exprNodeList_size (args) || n < 0)
    {
      llcontbug (message ("exprNodeList_nth: out of range: %q arg %d\n", 
			  exprNodeList_unparse (args), n));
      return exprNode_undefined;
    }

  return args->elements[n]; 
}

/*@only@*/ cstring
exprNodeList_unparse (exprNodeList s)
{
   int i;
   cstring st = cstring_undefined;

      for (i = 0; i < s->nelements; i++)
     {
       if (i == 0)
	 {
	   st = cstring_copy (exprNode_unparse (s->elements[i]));
	 }
       else
	 st = message ("%q, %s", st, exprNode_unparse (s->elements[i]));
     }
   
   return st;
}

void
exprNodeList_free (exprNodeList s)
{
  int i;

  for (i = 0; i < s->nelements; i++)
    {
      exprNode_free (s->elements[i]); 
    }
  
  sfree (s->elements); 
  sfree (s);
}

void
exprNodeList_freeShallow (/*@only@*/ exprNodeList s)
{
  int i;

  for (i = 0; i < s->nelements; i++)
    {
      exprNode_freeShallow (s->elements[i]); 
    }
  
  sfree (s->elements); 
  sfree (s);
}

