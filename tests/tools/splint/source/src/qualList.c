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
** qualList.c (from slist_template.c)
*/

# include "splintMacros.nf"
# include "basic.h"

qualList
qualList_new ()
{
  return qualList_undefined;
}

static /*@only@*/ /*@notnull@*/ qualList
qualList_newEmpty (void)
{
  qualList s = (qualList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->free = qualListBASESIZE;
  s->elements = (qual *) dmalloc (sizeof (*s->elements) * qualListBASESIZE);

  return (s);
}

void
qualList_clear (qualList q)
{
  if (qualList_isDefined (q))
    {
      q->free += q->nelements;
      q->nelements = 0;
    }
}

static void
qualList_grow (/*@notnull@*/ qualList s)
{
  int i;
  qual *oldelements = s->elements;
  
  s->free += qualListBASESIZE; 

  s->elements = (qual *) dmalloc (sizeof (*s->elements) * (s->nelements + s->free));
    
  for (i = 0; i < s->nelements; i++)
    {
      s->elements[i] = oldelements[i];
    }
  
  sfree (oldelements);
}

qualList qualList_single (qual el)
{
  /*@-unqualifiedtrans@*/ /* must be only */
  return (qualList_add (qualList_undefined, el));
  /*@=unqualifiedtrans@*/
}

qualList qualList_add (qualList s, qual el)
{
  if (qualList_isUndefined (s))
    {
      s = qualList_newEmpty ();
    }
  
  if (s->free <= 0)
    qualList_grow (s);
  
  s->free--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return (s);
}

qualList qualList_appendList (qualList s, qualList t)
{
  qualList_elements (t, current)
    {
      s = qualList_add (s, current);
    } end_qualList_elements;

  return s;
}

qualList qualList_copy (qualList s)
{
  qualList t = qualList_new ();

  qualList_elements (s, current)
    {
      t = qualList_add (t, current);
    } end_qualList_elements;

  return t;
}

/*@only@*/ cstring
qualList_unparse (qualList s)
{
   int i;
   cstring st = cstring_undefined;

   if (qualList_isDefined (s))
     {
       for (i = 0; i < qualList_size (s); i++)
	 {
	   if (i == 0)
	     {
	       st = message ("%q%s ", st, qual_unparse (s->elements[i]));
	     }
	   else
	     st = message ("%q%s ", st, qual_unparse (s->elements[i]));
	 }
     }

   return st;
}

/*@only@*/ cstring
qualList_toCComments (qualList s)
{
   int i;
   cstring st = cstring_undefined;

   if (qualList_isDefined (s))
     {
       for (i = 0; i < qualList_size (s); i++)
	 {
	   if (i == 0)
	     {
	       st = message ("%q/*@%s@*/ ", st, qual_unparse (s->elements[i]));
	     }
	   else
	     st = message ("%q/*@%s@*/ ", st, qual_unparse (s->elements[i]));
	 }
     }

   return st;
}

bool
qualList_hasAliasQualifier (qualList s)
{
  if (qualList_isDefined (s))
    {
      qualList_elements (s, q)
	{
	  if (qual_isAliasQual (q)) return TRUE;
	} end_qualList_elements;
    }

  return FALSE;
}

bool
qualList_hasExposureQualifier (qualList s)
{
  if (qualList_isDefined (s))
    {
      qualList_elements (s, q)
	{
	  if (qual_isExQual (q)) return TRUE;
	} end_qualList_elements;
    }

  return FALSE;
}

void
qualList_free (/*@only@*/ qualList s)
{
  if (qualList_isDefined (s))
    {
      sfree (s->elements); 
      sfree (s);
    }
}

/* start modifications */
/*
requires: p is defined
returns: true if qual is present in qualList
modifies: none
*/
bool qualList_hasNullTerminatedQualifier(qualList s) {
    qualList_elements(s, qu) {
      if( qual_isNullTerminated(qu) ) return TRUE;
    } end_qualList_elements ;
   
  return FALSE;
}

/* end modification/s */        


