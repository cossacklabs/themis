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
** metaStateConstraintList.c (from slist_template.c)
*/

# include "splintMacros.nf"
# include "basic.h"
# include "metaStateConstraintList.h"

/*
** Invariant:  If any member of the list is metaStateConstraint_undefined, then
**             the 0th member is metaStateConstraint_undefined.
*/

metaStateConstraintList
metaStateConstraintList_new ()
{
  return (metaStateConstraintList_undefined);
}

static /*@notnull@*/ /*@only@*/ metaStateConstraintList
metaStateConstraintList_newEmpty (void)
{
  metaStateConstraintList s = (metaStateConstraintList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->free = metaStateConstraintListBASESIZE;
  s->elements = (metaStateConstraint *) dmalloc (sizeof (*s->elements) * metaStateConstraintListBASESIZE);

  return (s);
}

static void
metaStateConstraintList_grow (/*@notnull@*/ metaStateConstraintList s)
{
  int i;
  metaStateConstraint *oldelements = s->elements;
  
  s->free += metaStateConstraintListBASESIZE; 
  s->elements = (metaStateConstraint *) dmalloc (sizeof (*s->elements) 
						 * (s->nelements + s->free));
  
  for (i = 0; i < s->nelements; i++)
    {
      s->elements[i] = oldelements[i];
    }
  
  sfree (oldelements);
}

metaStateConstraintList 
metaStateConstraintList_append (/*@returned@*/ metaStateConstraintList s, /*@only@*/ metaStateConstraintList t)
{
  llassert (NOALIAS (s, t));

  if (metaStateConstraintList_isUndefined (t) || metaStateConstraintList_isEmpty (t)) return s;

  if (metaStateConstraintList_isUndefined (s)) 
    {
      s = metaStateConstraintList_newEmpty ();
    }

  metaStateConstraintList_elements (t, fl)
    {
      /* Okay to use exposed storage here, t is begin eaten. */
      
      /*@-exposetrans@*/ /*@-dependenttrans@*/
      s = metaStateConstraintList_add (s, fl);
      /*@=exposetrans@*/ /*@=dependenttrans@*/
    } end_metaStateConstraintList_elements;

  sfree (t->elements);
  sfree (t);

  return s;
}

metaStateConstraintList 
metaStateConstraintList_add (/*@returned@*/ metaStateConstraintList s, /*@observer@*/ metaStateConstraint el)
{
  if (metaStateConstraintList_isUndefined (s))
    {
      s = metaStateConstraintList_newEmpty ();
    }

  if (s->free <= 0)
    {
      metaStateConstraintList_grow (s);
    }
  
  s->free--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

metaStateConstraintList 
metaStateConstraintList_single (metaStateConstraint el)
{
  metaStateConstraintList res = metaStateConstraintList_newEmpty ();
  return metaStateConstraintList_add (res, el);
}

metaStateConstraint
metaStateConstraintList_getFirst (metaStateConstraintList s)
{
  llassert (metaStateConstraintList_isDefined (s)
	    && metaStateConstraintList_size (s) >= 1);
  return s->elements[0];
}

/*@only@*/ cstring
metaStateConstraintList_unparse (metaStateConstraintList s)
{
   int i;
   cstring st = cstring_makeLiteral ("[");

   if (metaStateConstraintList_isDefined (s))
     {
       for (i = 0; i < metaStateConstraintList_size (s); i++)
	 {
	   if (i == 0)
	     {
	       st = message ("%q %q", st, metaStateConstraint_unparse (s->elements[i]));
	     }
	   else
	     st = message ("%q, %q", st, metaStateConstraint_unparse (s->elements[i]));
	 }
     }
   
   st = message ("%q ]", st);
   return st;
}

void
metaStateConstraintList_free (/*@only@*/ metaStateConstraintList s)
{
  if (metaStateConstraintList_isDefined (s))
    {
      sfree (s->elements); 
      sfree (s);
    }
}






