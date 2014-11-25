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
** exprNodeSList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"
# include "exprNodeSList.h"

exprNodeSList
  exprNodeSList_new ()
{
  exprNodeSList s = (exprNodeSList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = exprNodeSListBASESIZE; 
  s->elements = (exprNode *) 
    dmalloc (sizeof (*s->elements) * exprNodeSListBASESIZE);

  return (s);
}

static void
exprNodeSList_grow (exprNodeSList s)
{
  int i;
  exprNode *newelements; 
  
  s->nspace += exprNodeSListBASESIZE; 

  newelements = (exprNode *) dmalloc (sizeof (*newelements)
				      * (s->nelements + s->nspace));

  if (newelements == (exprNode *) 0)
    {
      llfatalerror (cstring_makeLiteral ("exprNodeSList_grow: out of memory!"));
    }

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }
  
  sfree (s->elements);
  s->elements = newelements;
}

void exprNodeSList_addh (exprNodeSList s, /*@exposed@*/ /*@dependent@*/ exprNode el)
{
  llassert (exprNodeSListBASESIZE > 0);

  if (s->nspace <= 0)
    exprNodeSList_grow (s);
  
  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;
}

/*
** appends s2 to s1 
*/

exprNodeSList exprNodeSList_append (/*@returned@*/ exprNodeSList s1, /*@only@*/ exprNodeSList s2)
{
  exprNodeSList_elements (s2, x)
    {
      exprNodeSList_addh (s1, x);
    } end_exprNodeSList_elements;

  exprNodeSList_free (s2);
  return s1;
}

/*@only@*/ exprNodeSList exprNodeSList_singleton (/*@exposed@*/ /*@dependent@*/ exprNode e)
{
  exprNodeSList s = (exprNodeSList) dmalloc (sizeof (*s));
  
  s->nelements = 1;
  s->nspace = exprNodeSListBASESIZE - 1; 
  s->elements = (exprNode *) dmalloc (sizeof (*s->elements) * exprNodeSListBASESIZE);
  s->elements[0] = e;

  return (s);
}

/*@only@*/ cstring
exprNodeSList_unparse (exprNodeSList s)
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
exprNodeSList_free (exprNodeSList s)
{
  sfree (s->elements); 
  sfree (s);
}

