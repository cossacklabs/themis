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
** cstringSList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

cstringSList
cstringSList_new ()
{
  return cstringSList_undefined;
}

static /*@notnull@*/ cstringSList
cstringSList_newEmpty (void)
{
  cstringSList s = (cstringSList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = cstringSListBASESIZE; 
  s->elements = (cstring *) dmalloc (sizeof (*s->elements) * cstringSListBASESIZE);

  return (s);
}

static void
cstringSList_grow (/*@notnull@*/ cstringSList s)
{
  int i;
  cstring *newelements;
  
  s->nspace += cstringSListBASESIZE;

  newelements = (cstring *) dmalloc (sizeof (*newelements) 
				     * (s->nelements + s->nspace));


  if (newelements == (cstring *) 0)
    {
      llfatalerror (cstring_makeLiteral ("cstringSList_grow: out of memory!"));
    }

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }
  
  sfree (s->elements); 
  s->elements = newelements;
}

cstringSList cstringSList_single (/*@exposed@*/ cstring el) 
{
  cstringSList s = cstringSList_new ();
  s = cstringSList_add (s, el);
  return s;
}

cstringSList cstringSList_add (cstringSList s, /*@exposed@*/ cstring el)
{
  if (!cstringSList_isDefined (s))
    {
      s = cstringSList_newEmpty ();
    }

  if (s->nspace <= 0)
    {
      cstringSList_grow (s);
    }
  
  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

cstring
cstringSList_get (cstringSList s, int index)
{
  llassert (s != NULL);
  llassert (index < s->nelements);
  return s->elements[index];
}

cstring
cstringSList_unparse (cstringSList s)
{
  return cstringSList_unparseSep (s, cstring_makeLiteralTemp (", "));
}

cstring
cstringSList_unparseSep (cstringSList s, cstring sep)
{
   cstring st = cstring_undefined;

   if (cstringSList_isDefined (s))
     {
       int i;

       for (i = 0; i < s->nelements; i++)
	 {
	   if (i == 0)
	     {
	       st = cstring_copy (s->elements[i]);
	     }
	   else
	     st = message ("%q%s%s", st, sep, s->elements[i]);
	 }
     }

   return st;
}

void
cstringSList_printSpaced (cstringSList s, size_t indent, size_t gap, int linelen)
{
  if (cstringSList_isDefined (s))
    {
      cstring line = cstring_undefined;
      cstring istring = cstring_fill (cstring_undefined, indent);
      cstring gstring = cstring_fill (cstring_undefined, gap);
      int numcol;
      size_t longest = 0;
      int i;
 
      /*
      ** find the longest string
      */

      for (i = 0; i < s->nelements; i++)
	{
	  size_t len = cstring_length (s->elements[i]);

	  if (len > longest)
	    {
	      longest = len;
	    }
	}

      numcol = size_toInt ((linelen - indent) / (longest + gap));
      
      if (numcol <= 1) 
	{
	  numcol = 1;
	}

      for (i = 0; i < s->nelements; i++)
	{
	  if (i % numcol == 0)
	    {
	      if (i != 0)
		{
		  llmsg (line);
		}
	      
	      line = message ("%s%q", istring,
			      cstring_fill (s->elements[i], longest));
	    }
	  else
	    {
	      line = message ("%q%s%q", line, gstring, 
			      cstring_fill (s->elements[i], longest));
	    }
	}

      cstring_free (line);
      cstring_free (istring);
      cstring_free (gstring);
    }
}

/*@only@*/ cstring
cstringSList_unparseAbbrev (cstringSList s)
{
   cstring st = cstring_undefined;

   if (cstringSList_isDefined (s))
     {
       int i;
       
       for (i = 0; i < s->nelements; i++)
	 {
	   if (i == 0)
	     {
	       st = cstring_copy (s->elements[i]);
	     }
	   else if (i > 3 && s->nelements > 5)
	     {
	       st = message ("%q, ...", st);
	       break;
	     }
	   else
	     {
	       st = message ("%q, %s", st, s->elements[i]);
	     }
	 }
     }

   return st;
}

void
cstringSList_free (cstringSList s)
{
  if (cstringSList_isDefined (s))
    {
      /*
      ** A modification of observer message is reported here, since
      ** *s->elements is an observer.  But sfree doesn't REALLY modify
      ** the value of this object.
      */

      /*@-modobserver@*/ 
      sfree (s->elements);
      /*@=modobserver@*/

      sfree (s);
    }
}

void
cstringSList_alphabetize (cstringSList s)
{
  if (cstringSList_isDefined (s))
    {
      /*@-modobserver@*/
      qsort (s->elements, (size_t) s->nelements, 
	     sizeof (*s->elements), (int (*)(const void *, const void *)) cstring_xcompare);
      /*@=modobserver@*/
    }
}

