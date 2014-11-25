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
** sigNodeSet.c
**
** based on set_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"
# include "intSet.h"

static bool sigNodeSet_member (sigNodeSet p_s, sigNode p_el);

/*@only@*/ sigNodeSet
sigNodeSet_new ()
{
  sigNodeSet s = (sigNodeSet) dmalloc (sizeof (*s));

  s->entries = 0;
  s->nspace = sigNodeSetBASESIZE;
  s->elements = (sigNode *) dmalloc (sizeof (*s->elements) * sigNodeSetBASESIZE);

  return (s);
}

/*@only@*/ sigNodeSet
sigNodeSet_singleton (sigNode el)
{
  sigNodeSet s = (sigNodeSet) dmalloc (sizeof (*s));

  s->entries = 1;
  s->nspace = sigNodeSetBASESIZE - 1;
  s->elements = (sigNode *) dmalloc (sizeof (*s->elements) * sigNodeSetBASESIZE);
  s->elements[0] = el;

  return (s);
}

static void
sigNodeSet_grow (/*@notnull@*/ sigNodeSet s)
{
  int i;
  sigNode *newelements; 

  s->nspace = sigNodeSetBASESIZE;
  newelements = (sigNode *) dmalloc (sizeof (*newelements) 
					   * (s->entries + s->nspace));
  
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
sigNodeSet_insert (sigNodeSet s, /*@owned@*/ sigNode el)
{
  llassert (sigNodeSet_isDefined (s));

  if (sigNodeSet_member (s, el))
    {
      sigNode_free (el);
      return FALSE;
    }
  else
    {
      if (s->nspace <= 0)
	{
	  sigNodeSet_grow (s);
	}

      s->nspace--;
      s->elements[s->entries] = el;
      s->entries++;
      return TRUE;
    }
}

static bool
sigNodeSet_member (sigNodeSet s, sigNode el)
{
  if (sigNodeSet_isUndefined (s)) 
    {
      return FALSE;
    }
  else
    {
      int i;
      
      for (i = 0; i < s->entries; i++)
	{
	  if (sigNode_equal (el, s->elements[i]))
	    return TRUE;
	}
      return FALSE;
    }
}

/*@only@*/ cstring
sigNodeSet_unparse (sigNodeSet s)
{
  int i;
  cstring st = cstring_undefined;

  if (sigNodeSet_isDefined (s))
    {
      for (i = 0; i < s->entries; i++)
	{
	  if (i == 0)
	    {
	      st = sigNode_unparse (s->elements[i]);
	    }
	  else
	    st = message ("%q, %q", st, sigNode_unparse (s->elements[i]));
	}
    }
     
  return st;
}

/*@only@*/ cstring
sigNodeSet_unparseSomeSigs (sigNodeSet s)
{
  int i;
  cstring st = cstring_undefined;

  if (sigNodeSet_isDefined (s))
    {
      for (i = 0; i < s->entries; i++)
	{
	  cstring t = sigNode_unparseText (s->elements[i]);
	  
	  if (i == 0)
	    {
	      st = cstring_copy (t);
	      cstring_free (t);
	    }
	  else if (i > 5 && (s->entries > 8))
	    {
	      return (message ("%q; %q; ... (%d more signatures)",
			       st, t, (s->entries - i - 1)));
	    }
	  else
	    {
	      st = message ("%q; %q", st, t);
	    }
	}
    }
     
  return st;
}

/*@only@*/ cstring
sigNodeSet_unparsePossibleAritys (sigNodeSet s)
{
  int i;
  intSet is = intSet_new ();
  cstring st;

  if (sigNodeSet_isDefined (s))
    {
      for (i = 0; i < s->entries; i++)
	{
	  int arity = ltokenList_size ((s->elements[i])->domain);
	  (void) intSet_insert (is, arity);
	}
    }

  st = intSet_unparseText (is);
  intSet_free (is);
  return (st);
}

void
sigNodeSet_free (sigNodeSet s)
{
  if (sigNodeSet_isDefined (s))
    {
      int i;
      for (i = 0; i < s->entries; i++)
	{
	  sigNode_free (s->elements[i]); 
	}
      
      sfree (s->elements); 
      sfree (s);
    }
}
