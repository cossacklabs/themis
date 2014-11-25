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
** sRefList.c (from slist_template.c)
*/

# include "splintMacros.nf"
# include "basic.h"

sRefList
sRefList_new ()
{
  return sRefList_undefined;
}

static /*@only@*/ /*@notnull@*/ sRefList
sRefList_newEmpty (void)
{
  sRefList s = (sRefList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = sRefListBASESIZE;
  s->elements = (sRef *) dmalloc (sizeof (*s->elements) * sRefListBASESIZE);

  return (s);
}

/*@only@*/ sRefList sRefList_single (sRef el)
{
  sRefList res = sRefList_newEmpty ();
  res = sRefList_add (res, el);
  return res;
}

static void
sRefList_grow (/*@notnull@*/ sRefList s)
{
  int i;
  sRef *oldelements = s->elements;
  
  s->nspace += sRefListBASESIZE; 

  s->elements = (sRef *) dmalloc (sizeof (*s->elements) * (s->nelements + s->nspace));
    
  for (i = 0; i < s->nelements; i++)
    {
      s->elements[i] = oldelements[i];
    }
  
  sfree (oldelements);
}

/*@notnull@*/ sRefList sRefList_add (sRefList s, sRef el)
{
  if (sRefList_isUndefined (s))
    {
      s = sRefList_newEmpty ();
    }
  
  if (s->nspace <= 0)
    sRefList_grow (s);
  
  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return (s);
}

sRefList sRefList_copy (sRefList s)
{
  sRefList t = sRefList_new ();

  sRefList_elements (s, current)
    {
      t = sRefList_add (t, sRef_copy (current));
    } end_sRefList_elements;

  return t;
}

/*@only@*/ cstring
sRefList_unparse (sRefList s)
{
   int i;
   cstring st = cstring_undefined;

   if (sRefList_isDefined (s))
     {
       for (i = 0; i < sRefList_size (s); i++)
	 {
	   if (i == 0)
	     {
	       st = message ("%q%q ", st, sRef_unparse (s->elements[i]));
	     }
	   else
	     st = message ("%q%q ", st, sRef_unparse (s->elements[i]));
	 }
     }

   return st;
}

int sRefList_size (sRefList s)
{
  if (sRefList_isUndefined (s)) return 0;
  return s->nelements;
}

void
sRefList_free (/*@only@*/ sRefList s)
{
  if (sRefList_isDefined (s))
    {
      sfree (s->elements); 
      sfree (s);
    }
}




