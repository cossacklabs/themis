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
** lsymbolSet.c
**
** based on set_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"
 
lsymbolSet lsymbolSet_new ()
{
  lsymbolSet s = (lsymbolSet) dmalloc (sizeof (*s));

  s->entries = 0;
  s->nspace = lsymbolSetBASESIZE;
  s->elements = (lsymbol *) dmalloc (sizeof (*s->elements) * lsymbolSetBASESIZE);

  return (s);
}

static void
lsymbolSet_grow (lsymbolSet s)
{
  int i;
  lsymbol *newelements; 

  llassert (lsymbolSet_isDefined (s));

  s->nspace = lsymbolSetBASESIZE;
  newelements = (lsymbol *) dmalloc (sizeof (*newelements) 
				       * (s->entries + s->nspace));

  if (newelements == (lsymbol *) 0)
    {
      llfatalerror (cstring_makeLiteral ("lsymbolSet_grow: out of memory!"));
    }

  for (i = 0; i < s->entries; i++)
    {
      newelements[i] = s->elements[i]; 
    }

  sfree (s->elements); 
  s->elements = newelements;
}

/*
** Ensures: if *e \in *s
**          then unchanged (*s) & result = false
**          else *s' = insert (*s, *e) & result = true
** Modifies: *s
*/

bool
lsymbolSet_insert (lsymbolSet s, lsymbol el)
{
  llassert (lsymbolSet_isDefined (s));

  if (lsymbolSet_member (s, el))
    {
      return FALSE;
    }
  else
    {
      if (s->nspace <= 0)
	lsymbolSet_grow (s);
      s->nspace--;
      s->elements[s->entries] = el;
      s->entries++;
      return TRUE;
    }
}

bool
lsymbolSet_member (lsymbolSet s, lsymbol el)
{
  if (lsymbolSet_isDefined (s))
    {
      int i;
      
      for (i = 0; i < s->entries; i++)
	{
	  /* was: &el == &s->elements[i] ! */

	  if (lsymbol_equal (el, s->elements[i]))
	    {
	      return TRUE;
	    }
	}
    }

  return FALSE;
}

/*@only@*/ cstring
lsymbolSet_unparse (lsymbolSet s)
{
  if (lsymbolSet_isDefined (s))
    {
      int i;
      cstring st = cstring_makeLiteral ("{");
      
      for (i = 0; i < s->entries; i++)
	{
	  if (i == 0)
	    {
	      st = message ("%q %s", st, 
			    cstring_fromChars (lsymbol_toChars (s->elements[i])));
	    }
	  else
	    st = message ("%q, %s", st, 
			  cstring_fromChars (lsymbol_toChars (s->elements[i])));
	}
      
      st = message ("%q }", st);
      return st;
    }
  else
    {
      return (cstring_makeLiteral ("{ }"));
    }
}

void
lsymbolSet_free (/*@null@*/ lsymbolSet s)
{
  if (lsymbolSet_isDefined (s))
    {
      sfree (s->elements); 
      sfree (s);
    }
}
