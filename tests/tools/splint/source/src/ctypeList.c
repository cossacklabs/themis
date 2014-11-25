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
** ctypeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ ctypeList
ctypeList_new ()
{
  ctypeList s = (ctypeList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = ctypeListBASESIZE; 
  s->elements = (ctype *) dmalloc (sizeof (*s->elements) * ctypeListBASESIZE);

  return (s);
}

static void
ctypeList_grow (/*@notnull@*/ ctypeList s)
{
  int i;
  ctype *newelements;
  
  s->nspace += ctypeListBASESIZE; 
  newelements = (ctype *) dmalloc (sizeof (*newelements) * (s->nelements + s->nspace));

  if (newelements == (ctype *) 0)
    {
      llfatalerror (cstring_makeLiteral ("ctypeList_grow: out of memory!"));
    }
    
  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }
  
  sfree (s->elements);
  s->elements = newelements;
}

void ctypeList_addh (ctypeList s, ctype el)
{
  llassert (ctypeList_isDefined (s));
  llassert (ctypeListBASESIZE > 0);

  if (s->nspace <= 0) ctypeList_grow (s);
  
  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;
}

ctypeList ctypeList_add (ctypeList s, ctype el)
{
  llassert (ctypeListBASESIZE > 0);

  if (ctypeList_isUndefined (s))
    {
      s = ctypeList_new ();
    }

  ctypeList_addh (s, el);
  return s;
}

ctypeList ctypeList_append (ctypeList s1, ctypeList s2)
{
  ctypeList res = s1;

  ctypeList_elements (s2, el)
    {
      res = ctypeList_add (res, el);
    } end_ctypeList_elements;

  return res;
}
      
/*@only@*/ cstring
ctypeList_unparse (ctypeList ct)
{
  cstring s = cstring_undefined;
  int i;
  bool first = TRUE;

  if (ctypeList_isUndefined (ct) || ctypeList_size (ct) == 0)
    {
      return (cstring_makeLiteral ("void"));
    }

  for (i = 0; i < ct->nelements; i++)
    {
      if (first)
	{
	  s = cstring_copy (ctype_unparse (ct->elements[i]));
	  first = FALSE;
	}
      else
	{
	  s = message ("%q, %s", s, ctype_unparse (ct->elements[i]));
	}
    }

  return s;
}

void
ctypeList_free (/*@only@*/ ctypeList s)
{
  if (ctypeList_isDefined (s))
    {
      int i;
      for (i = 0; i < s->nelements; i++)
	{
	  /*      ctype_free (s->elements[i]); */
	}
      
      sfree (s->elements); /* not quite!!! */
      sfree (s);
    }
}







