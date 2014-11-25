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
** mtMergeClauseList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

mtMergeClauseList
mtMergeClauseList_new ()
{
  return mtMergeClauseList_undefined;
}

static /*@notnull@*/ mtMergeClauseList
mtMergeClauseList_newEmpty (void)
{
  mtMergeClauseList s = (mtMergeClauseList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = mtMergeClauseListBASESIZE; 
  s->elements = (mtMergeClause *) dmalloc (sizeof (*s->elements) * mtMergeClauseListBASESIZE);

  return (s);
}

static void
mtMergeClauseList_grow (/*@notnull@*/ mtMergeClauseList s)
{
  int i;
  mtMergeClause *newelements;
  
  s->nspace += mtMergeClauseListBASESIZE;

  newelements = (mtMergeClause *) dmalloc (sizeof (*newelements) * (s->nelements + s->nspace));

  if (newelements == (mtMergeClause *) 0)
    {
      llfatalerror (cstring_makeLiteral ("mtMergeClauseList_grow: out of memory!"));
    }

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }
  
  sfree (s->elements); 
  s->elements = newelements;
}

mtMergeClauseList mtMergeClauseList_single (/*@keep@*/ mtMergeClause el) 
{
  mtMergeClauseList s = mtMergeClauseList_new ();
  s = mtMergeClauseList_add (s, el);
  return s;
}

mtMergeClauseList mtMergeClauseList_add (mtMergeClauseList s, /*@keep@*/ mtMergeClause el)
{
  if (!mtMergeClauseList_isDefined (s))
    {
      s = mtMergeClauseList_newEmpty ();
    }

  if (s->nspace <= 0)
    {
      mtMergeClauseList_grow (s);
    }
  
  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

mtMergeClauseList mtMergeClauseList_prepend (mtMergeClauseList s, /*@keep@*/ mtMergeClause el)
{
  int i;

  if (!mtMergeClauseList_isDefined (s))
    {
      return mtMergeClauseList_single (el);
    }

  if (s->nspace <= 0)
    {
      mtMergeClauseList_grow (s);
    }
  
  s->nspace--;

  for (i = s->nelements; i > 0; i--) 
    {
      s->elements[i] = s->elements [i - 1];
    }

  s->elements[0] = el;
  s->nelements++;

  return s;
}

cstring
mtMergeClauseList_unparse (mtMergeClauseList s)
{
  return mtMergeClauseList_unparseSep (s, cstring_makeLiteralTemp (" "));
}

cstring
mtMergeClauseList_unparseSep (mtMergeClauseList s, cstring sep)
{
   cstring st = cstring_undefined;

   if (mtMergeClauseList_isDefined (s))
     {
       int i;

       for (i = 0; i < s->nelements; i++)
	 {
	   if (i == 0)
	     {
	       st = mtMergeClause_unparse (s->elements[i]);
	     }
	   else
	     st = message ("%q%s%q", st, sep, mtMergeClause_unparse (s->elements[i]));
	 }
     }

   return st;
}

void
mtMergeClauseList_free (mtMergeClauseList s)
{
  if (mtMergeClauseList_isDefined (s))
    {
      int i;

      for (i = 0; i < s->nelements; i++) {
	mtMergeClause_free (s->elements[i]);
      }

      sfree (s->elements);
      sfree (s);
    }
}

