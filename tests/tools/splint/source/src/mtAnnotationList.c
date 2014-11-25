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
** mtAnnotationList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

mtAnnotationList
mtAnnotationList_new ()
{
  return mtAnnotationList_undefined;
}

static /*@notnull@*/ mtAnnotationList
mtAnnotationList_newEmpty (void)
{
  mtAnnotationList s = (mtAnnotationList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = mtAnnotationListBASESIZE; 
  s->elements = (mtAnnotationDecl *) dmalloc (sizeof (*s->elements) * mtAnnotationListBASESIZE);

  return (s);
}

static void
mtAnnotationList_grow (/*@notnull@*/ mtAnnotationList s)
{
  int i;
  mtAnnotationDecl *newelements;
  
  s->nspace += mtAnnotationListBASESIZE;

  newelements = (mtAnnotationDecl *) dmalloc (sizeof (*newelements) * (s->nelements + s->nspace));

  if (newelements == (mtAnnotationDecl *) 0)
    {
      llfatalerror (cstring_makeLiteral ("mtAnnotationList_grow: out of memory!"));
    }

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }
  
  sfree (s->elements); 
  s->elements = newelements;
}

mtAnnotationList mtAnnotationList_single (/*@only@*/ mtAnnotationDecl el) 
{
  mtAnnotationList s = mtAnnotationList_new ();
  s = mtAnnotationList_add (s, el);
  return s;
}

mtAnnotationList mtAnnotationList_add (mtAnnotationList s, mtAnnotationDecl el)
{
  if (!mtAnnotationList_isDefined (s))
    {
      s = mtAnnotationList_newEmpty ();
    }

  if (s->nspace <= 0)
    {
      mtAnnotationList_grow (s);
    }
  
  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

mtAnnotationList mtAnnotationList_prepend (mtAnnotationList s, mtAnnotationDecl el)
{
  int i;

  if (!mtAnnotationList_isDefined (s))
    {
      return mtAnnotationList_single (el);
    }

  if (s->nspace <= 0)
    {
      mtAnnotationList_grow (s);
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
mtAnnotationList_unparse (mtAnnotationList s)
{
  return mtAnnotationList_unparseSep (s, cstring_makeLiteralTemp (" "));
}

cstring
mtAnnotationList_unparseSep (mtAnnotationList s, cstring sep)
{
   cstring st = cstring_undefined;

   if (mtAnnotationList_isDefined (s))
     {
       int i;

       for (i = 0; i < s->nelements; i++)
	 {
	   if (i == 0)
	     {
	       st = mtAnnotationDecl_unparse (s->elements[i]);
	     }
	   else
	     st = message ("%q%s%q", st, sep, 
			   mtAnnotationDecl_unparse (s->elements[i]));
	 }
     }

   return st;
}

void
mtAnnotationList_free (mtAnnotationList s)
{
  if (mtAnnotationList_isDefined (s))
    {
      sfree (s->elements);
      sfree (s);
    }
}

