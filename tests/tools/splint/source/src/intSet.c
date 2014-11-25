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
** intSet.c
**
** based on set_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"
# include "intSet.h"

/*@only@*/ intSet
intSet_new ()
{
  intSet s = (intSet) dmalloc (sizeof (*s));
  
  s->entries = 0;
  s->nspace = intSetBASESIZE;
  s->elements = (int *) dmalloc (sizeof (*s->elements) * intSetBASESIZE);

  return (s);
}

static void
intSet_grow (intSet s)
{
  int i;
  int *newelements;

  s->nspace = intSetBASESIZE;
  newelements = (int *) dmalloc (sizeof (*newelements) * (s->entries + s->nspace));

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
intSet_insert (intSet s, int el)
{
  int i;

  for (i = 0; i < s->entries; i++)
    {
      if (s->elements[i] >= el)
	break;
    }

  if (s->entries > 0 && s->elements[i] == el)
    {
      return FALSE;
    }
  else
    {
      if (s->nspace <= 0)
	intSet_grow (s);

      s->nspace--;

      if (i == (s->entries - 1))
	{
	  s->elements[s->entries] = el;
	}
      else
	{
	  int j;
	  
	  for (j = s->entries; j > i; j--)
	    {
	      s->elements[j] = s->elements[j-1];
	    }
	  s->elements[i] = el;
	}
      
      s->entries++;      
      return TRUE;
    }
}

bool
intSet_member (intSet s, int el)
{
  int i;

  for (i = 0; i < s->entries; i++)
    {
      if (el == s->elements[i])
	{
	  return TRUE;
	}
      if (el > s->elements[i]) 
	{
	  return FALSE;
	}
    }
  return FALSE;
}

/*@only@*/ cstring
intSet_unparseText (intSet s)
{
  int i;
  cstring st = cstring_undefined;
  int lastentry = s->entries - 1;

  for (i = 0; i < s->entries; i++)
    {
      if (i == 0)
	st = message ("%d", s->elements[i]);
      else if (i == lastentry)
	st = message ("%q or %d", st, s->elements[i]);
      else
	st = message ("%q, %d", st, s->elements[i]);
    }

  return st;
}

/*@only@*/ cstring
intSet_unparse (intSet s)
{
  int i;
  cstring st = cstring_makeLiteral ("{");

  for (i = 0; i < s->entries; i++)
    {
      st = message ("%q %d", st, s->elements[i]);
    }

  st = message ("%q}", st);
  return st;
}

void
intSet_free (intSet s)
{
  sfree (s->elements); 
  sfree (s);
}
