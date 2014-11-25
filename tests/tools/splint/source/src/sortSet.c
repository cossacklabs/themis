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
** sortSet.c
**
** based on set_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

sortSet sortSet_new ()
{
  sortSet s = (sortSet) dmalloc (sizeof (*s));
  
  s->entries = 0;
  s->nspace = sortSetBASESIZE;
  s->elements = (sort *) dmalloc (sizeof (*s->elements) * sortSetBASESIZE);
  
  return (s);
}

static /*@notnull@*/ sortSet
sortSet_predict (int size)
{
  sortSet s = (sortSet) dmalloc (sizeof (*s));
  
  s->entries = 0;

  if (size > 0)
    {
      s->nspace = size;
      s->elements = (sort *) dmalloc (sizeof (*s->elements) * size);
    }
  else
    {
      s->nspace = 0;
      s->elements = NULL;
    }
  
  return (s);
}

static void
sortSet_grow (/*@notnull@*/ sortSet s)
{
  int i;
  sort *newelements; 

  s->nspace = sortSetBASESIZE;
  newelements = (sort *) dmalloc (sizeof (*newelements) * (s->entries + s->nspace));

  if (newelements == (sort *) 0)
    {
      llfatalerror (cstring_makeLiteral ("sortSet_grow: out of memory!"));
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
sortSet_insert (sortSet s, sort el)
{
  llassert (sortSet_isDefined (s));

  if (sortSet_member (s, el))
    {
      return FALSE;
    }
  else
    {
      if (s->nspace <= 0)
	sortSet_grow (s);
      s->nspace--;
      s->elements[s->entries] = el;
      s->entries++;
      return TRUE;
    }
}

sort
sortSet_choose (sortSet s)
{
  llassert (sortSet_isDefined (s) && s->entries > 0);
  return (s->elements[0]);
}

bool
sortSet_member (sortSet s, sort el)
{
  if (sortSet_isDefined (s))
    {
      int i;

      for (i = 0; i < s->entries; i++)
	{
	  if (sort_equal (el, s->elements[i]))
	    {
	      return TRUE;
	    }
	}
    }

  return FALSE;
}

/*@only@*/ cstring
sortSet_unparse (sortSet s)
{
  return (message ("{ %q }", sortSet_unparseClean (s)));
}

/*@only@*/ cstring
sortSet_unparseClean (sortSet s)
{
  cstring st = cstring_undefined;

  if (sortSet_isDefined (s))
    {
      int i;

      for (i = 0; i < s->entries; i++)
	{
	  if (i == 0)
	    {
	      st = message ("%q%s", st, sort_unparseName (s->elements[i]));
	    }
	  else
	    {
	      st = message ("%q, %s", st, sort_unparseName (s->elements[i]));
	    }
	}
    }

  return st;
}

/*@only@*/ cstring
sortSet_unparseOr (sortSet s)
{
  cstring st = cstring_undefined;

  if (sortSet_isDefined (s))
    {
      int i;
      int last = s->entries - 1;
      
      for (i = 0; i < s->entries; i++)
	{
	  if (i == 0)
	    {
	      st = cstring_concatFree (st, sort_unparse (s->elements[i]));
	    }
	  else
	    {
	      if (i == last)
		{
		  /* was sort_unparse ??? */
		  st = message ("%q or %q", st, sort_unparse (s->elements[i]));
		}
	      else
		{
		  st = message ("%q, %q", st, sort_unparse (s->elements[i]));
		}
	    }
	}
    }
  
  return st;
}

void
sortSet_free (sortSet s)
{
  if (sortSet_isDefined (s))
    {
      sfree (s->elements); 
      sfree (s);
    }
}

/*@only@*/ sortSet
sortSet_copy (sortSet s)
{
  sortSet t = sortSet_predict (sortSet_size (s));
  int i;

  if (sortSet_isDefined (s))
    {
      for (i = 0; i < sortSet_size (s); i++)
	{
	  (void) sortSet_insert (t, s->elements[i]); 
	}
    }

  return t;
}



