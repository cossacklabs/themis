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
** lslOpSet.c
**
** based on set_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"
# include "checking.h"		/* for lslOp_equal */

static bool lslOpSet_member (lslOpSet p_s, lslOp p_el);

lslOpSet lslOpSet_new ()
{
  lslOpSet s = (lslOpSet) dmalloc (sizeof (*s));

  s->entries = 0;
  s->nspace = lslOpSetBASESIZE;
  s->elements = (lslOp *)
    dmalloc (sizeof (*s->elements) * lslOpSetBASESIZE);
  
  return (s);
}

static /*@only@*/ lslOpSet
lslOpSet_predict (int size)
{
  lslOpSet s = (lslOpSet) dmalloc (sizeof (*s));
  
  s->entries = 0;

  if (size > 0)
    {
      s->nspace = size;
      s->elements = (lslOp *) dmalloc (sizeof (*s->elements) * size);
    }
  else
    {
      s->nspace = 0;
      s->elements = NULL;
    }

  return (s);
}

static void
lslOpSet_grow (/*@notnull@*/ lslOpSet s)
{
  int i;
  lslOp *newelements;

  s->nspace = lslOpSetBASESIZE;
  newelements = (lslOp *) dmalloc (sizeof (*newelements)
					 * (s->entries + s->nspace));

  if (newelements == (lslOp *) 0)
    {
      llfatalerror (cstring_makeLiteral ("lslOpSet_grow: out of memory!"));
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
lslOpSet_insert (lslOpSet s, /*@only@*/ lslOp el)
{
  llassert (lslOpSet_isDefined (s));

  if (lslOpSet_member (s, el))
    {
      lslOp_free (el);
      return FALSE;
    }
  else
    {
      if (s->nspace <= 0)
	lslOpSet_grow (s);
      s->nspace--;
      s->elements[s->entries] = el;
      s->entries++;
      return TRUE;
    }
}

static bool
lslOpSet_member (lslOpSet s, lslOp el)
{
  if (lslOpSet_isDefined (s))
    {
      int i;
      
      for (i = 0; i < lslOpSet_size (s); i++)
	{
	  if (lslOp_equal (el, s->elements[i]))
	    return TRUE;
	}
    }

  return FALSE;
}

/*@only@*/ cstring
lslOpSet_unparse (lslOpSet s)
{
  if (lslOpSet_isDefined (s))
    {
      int i;
      cstring st = cstring_makeLiteral ("{");
      
      for (i = 0; i < lslOpSet_size (s); i++)
	{
	  st = message ("%q   %q", st, lslOp_unparse (s->elements[i]));
	}
      
      st = message ("%q}", st);
      return st;
    }
  else
    {
      return (cstring_makeLiteral ("{ }"));
    }
}

/*@only@*/ lslOpSet
lslOpSet_copy (lslOpSet s)
{
  if (lslOpSet_isDefined (s))
    {
      lslOpSet t = lslOpSet_predict (lslOpSet_size (s));
      int i;
      
      for (i = 0; i < lslOpSet_size (s); i++)
	{
	  (void) lslOpSet_insert (t, lslOp_copy (s->elements[i])); 
	}
      
      return t;
    }
  else
    {
      return lslOpSet_undefined;
    }
}

void
lslOpSet_free (lslOpSet s)
{
  if (lslOpSet_isDefined (s))
    {
      int i;
      for (i = 0; i < s->entries; i++)
	{
	  lslOp_free (s->elements[i]); 
	}
      
      sfree (s->elements); 
      sfree (s);
    }
}
