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
** mtLoseReferenceList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

mtLoseReferenceList
mtLoseReferenceList_new ()
{
  return mtLoseReferenceList_undefined;
}

static /*@notnull@*/ mtLoseReferenceList
mtLoseReferenceList_newEmpty (void)
{
  mtLoseReferenceList s = (mtLoseReferenceList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = mtLoseReferenceListBASESIZE; 
  s->elements = (mtLoseReference *) dmalloc (sizeof (*s->elements) * mtLoseReferenceListBASESIZE);

  return (s);
}

static void
mtLoseReferenceList_grow (/*@notnull@*/ mtLoseReferenceList s)
{
  int i;
  mtLoseReference *newelements;
  
  s->nspace += mtLoseReferenceListBASESIZE;

  newelements = (mtLoseReference *) dmalloc (sizeof (*newelements) * (s->nelements + s->nspace));

  if (newelements == (mtLoseReference *) 0)
    {
      llfatalerror (cstring_makeLiteral ("mtLoseReferenceList_grow: out of memory!"));
    }

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }
  
  sfree (s->elements); 
  s->elements = newelements;
}

mtLoseReferenceList mtLoseReferenceList_single (mtLoseReference el) 
{
  mtLoseReferenceList s = mtLoseReferenceList_new ();
  s = mtLoseReferenceList_add (s, el);
  return s;
}

mtLoseReferenceList mtLoseReferenceList_add (mtLoseReferenceList s, /*@keep@*/ mtLoseReference el)
{
  if (!mtLoseReferenceList_isDefined (s))
    {
      s = mtLoseReferenceList_newEmpty ();
    }

  if (s->nspace <= 0)
    {
      mtLoseReferenceList_grow (s);
    }
  
  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

mtLoseReferenceList mtLoseReferenceList_prepend (mtLoseReferenceList s, mtLoseReference el)
{
  int i;

  if (!mtLoseReferenceList_isDefined (s))
    {
      return mtLoseReferenceList_single (el);
    }

  if (s->nspace <= 0)
    {
      mtLoseReferenceList_grow (s);
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
mtLoseReferenceList_unparse (mtLoseReferenceList s)
{
  return mtLoseReferenceList_unparseSep (s, cstring_makeLiteralTemp (" "));
}

cstring
mtLoseReferenceList_unparseSep (mtLoseReferenceList s, cstring sep)
{
   cstring st = cstring_undefined;

   if (mtLoseReferenceList_isDefined (s))
     {
       int i;

       for (i = 0; i < s->nelements; i++)
	 {
	   if (i == 0)
	     {
	       st = mtLoseReference_unparse (s->elements[i]);
	     }
	   else
	     st = message ("%q%s%q", st, sep, mtLoseReference_unparse (s->elements[i]));
	 }
     }

   return st;
}

void
mtLoseReferenceList_free (mtLoseReferenceList s)
{
  if (mtLoseReferenceList_isDefined (s))
    {
      int i;

      for (i = 0; i < s->nelements; i++) {
	mtLoseReference_free (s->elements[i]);
      }

      sfree (s->elements);
      sfree (s);
    }
}

