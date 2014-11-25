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
** cstringList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

cstringList
cstringList_new ()
{
  return cstringList_undefined;
}

static /*@notnull@*/ cstringList
cstringList_newEmpty (void)
{
  cstringList s = (cstringList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = cstringListBASESIZE; 
  s->elements = (cstring *) dmalloc (sizeof (*s->elements) * cstringListBASESIZE);

  return (s);
}

static /*@notnull@*/ cstringList
cstringList_newPredict (int size)
{
  cstringList s = (cstringList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = size; 
  s->elements = (cstring *) dmalloc (sizeof (*s->elements) * size);

  return (s);
}

static void
cstringList_grow (/*@notnull@*/ cstringList s)
{
  int i;
  cstring *newelements;
  
  s->nspace += cstringListBASESIZE;
  
  newelements = (cstring *) dmalloc (sizeof (*newelements) 
				     * (s->nelements + s->nspace));
  

  if (newelements == (cstring *) 0)
    {
      llfatalerror (cstring_makeLiteral ("cstringList_grow: out of memory!"));
    }

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }
  
  sfree (s->elements); 
  s->elements = newelements;
}

cstringList cstringList_single (/*@keep@*/ cstring el) 
{
  cstringList s = cstringList_new ();
  s = cstringList_add (s, el);
  return s;
}

cstringList cstringList_add (cstringList s, /*@keep@*/ cstring el)
{
  if (!cstringList_isDefined (s))
    {
      s = cstringList_newEmpty ();
    }

  if (s->nspace <= 0)
    {
      cstringList_grow (s);
    }
  
  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

cstringList cstringList_prepend (cstringList s, /*@keep@*/ cstring el)
{
  int i;

  DPRINTF (("Prepend:  %s + %s",
	    cstringList_unparse (s), cstring_toCharsSafe (el)));

  if (!cstringList_isDefined (s))
    {
      return cstringList_single (el);
    }

  if (s->nspace <= 0)
    {
      cstringList_grow (s);
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
cstringList_unparse (cstringList s)
{
  return cstringList_unparseSep (s, cstring_makeLiteralTemp (", "));
}

cstring
cstringList_unparseSep (cstringList s, cstring sep)
{
   cstring st = cstring_undefined;

   if (cstringList_isDefined (s))
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
cstringList_printSpaced (cstringList s, size_t indent, size_t gap, int linelen)
{
  if (cstringList_isDefined (s))
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
cstringList_unparseAbbrev (cstringList s)
{
   cstring st = cstring_undefined;

   if (cstringList_isDefined (s))
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
cstringList_free (cstringList s)
{
  if (cstringList_isDefined (s))
    {
      int i;

      DPRINTF (("cstringList free: [%p] %s",
		s, cstringList_unparse (s)));

      /* evans 2002-07-12: this was missing, not detected because of reldef */
      for (i = 0; i < s->nelements; i++)
	{
	  cstring_free (s->elements[i]);
	}

      sfree (s->elements);
      sfree (s);
    }
}

void
cstringList_alphabetize (cstringList s)
{
  if (cstringList_isDefined (s))
    {
      /*@-modobserver@*/
      qsort (s->elements, (size_t) s->nelements, 
	     sizeof (*s->elements), (int (*)(const void *, const void *)) cstring_xcompare);
      /*@=modobserver@*/
    }
}

int cstringList_getIndex (cstringList s, cstring key)
{
  int index = 0;

  cstringList_elements (s, el) 
    {
      if (cstring_equal (el, key))
	{
	  return index;
	}

      index++;
    } end_cstringList_elements ;

  BADBRANCHRET (0);
}

bool cstringList_contains (cstringList s, cstring key)
{
  int index = 0;

  cstringList_elements (s, el) 
    {
      if (cstring_equal (el, key))
	{
	  return TRUE;
	}

      index++;
    } end_cstringList_elements ;

  return FALSE;
}

cstringList cstringList_copy (cstringList s)
{
  cstringList res = cstringList_newPredict (cstringList_size (s));

  cstringList_elements (s, el)
    {
      res = cstringList_add (res, cstring_copy (el));
    } end_cstringList_elements ;

  return res;
}

cstring
cstringList_get (cstringList s, int index)
{
  llassertretnull (s != NULL);
  llassertretnull (index >= 0);
  llassertretnull (index < s->nelements);
  return s->elements[index];
}

ob_cstring *
cstringList_getElements (cstringList s)
{
  if (cstringList_isDefined (s))
    {
      /*@-compmempass@*/
      return s->elements;
      /*@=compmempass@*/ /* This is exposed */
    }
  else
    {
      return NULL;
    }
}
