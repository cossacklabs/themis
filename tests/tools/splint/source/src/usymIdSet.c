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
** usymIdSet.c
**
** based on set_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

usymIdSet
usymIdSet_new ()
{
  return usymIdSet_undefined;
}

static /*@notnull@*/ /*@only@*/ usymIdSet
usymIdSet_newEmpty (void)
{
  usymIdSet s = (usymIdSet) dmalloc (sizeof (*s));
  
  s->entries = 0;
  s->nspace = usymIdSetBASESIZE;
  s->elements = (usymId *) dmalloc (sizeof (*s->elements) * usymIdSetBASESIZE);

  return (s);
}

static void
usymIdSet_grow (/*@notnull@*/ usymIdSet s)
{
  int i;
  usymId *newelements;

  s->nspace = usymIdSetBASESIZE;
  newelements = (usymId *) dmalloc (sizeof (*newelements) * (s->entries + s->nspace));

  for (i = 0; i < s->entries; i++)
    {
      newelements[i] = s->elements[i];
    }

  sfree (s->elements);
  s->elements = newelements;
}

/*@only@*/ usymIdSet
usymIdSet_single (usymId t)
{
  usymIdSet s = (usymIdSet) dmalloc (sizeof (*s));
   
  s->entries = 1;
  s->nspace = usymIdSetBASESIZE - 1;
  s->elements = (usymId *) dmalloc (sizeof (*s->elements) * usymIdSetBASESIZE);
  s->elements[0] = t;

  return (s);
}

static usymIdSet 
  usymIdSet_insert (/*@returned@*/ usymIdSet s, usymId el)
{
  if (usymIdSet_isUndefined (s))
    {
      s = usymIdSet_newEmpty ();
    }

  if (usymIdSet_member (s, el))
    {
      return s;
    }
  else
    {
      if (s->nspace <= 0)
	usymIdSet_grow (s);
      s->nspace--;
      s->elements[s->entries] = el;
      s->entries++;
      return s;
    }
}

static usymIdSet
usymIdSet_copy (/*@notnull@*/ usymIdSet s)
{
  int size = s->entries + 1;
  usymIdSet t = (usymIdSet) dmalloc (sizeof (*t));
  int i;
  
  t->entries = s->entries;
  t->nspace = 1;
  t->elements = (usymId *) dmalloc (sizeof (*t->elements) * size);
  
  for (i = 0; i < s->entries; i++)
    {
      t->elements[i] = s->elements[i];
    }
  
  return t;
}

usymIdSet
usymIdSet_add (usymIdSet s, usymId el)
{
  if (usymIdSet_isDefined (s))
    {
      llassert (!usymIdSet_member (s, el));
      
      return (usymIdSet_insert (usymIdSet_copy (s), el));
    }
  else
    {
      return (usymIdSet_single (el));
    }
}

usymIdSet
usymIdSet_removeFresh (/*@temp@*/ usymIdSet s, usymId el)
{
  if (usymIdSet_isDefined (s))
    {
      usymIdSet t = usymIdSet_newEmpty ();
      int i;
      
      for (i = 0; i < s->entries; i++)
	{
	  if (!usymId_equal (el, s->elements[i]))
	    {
	      t = usymIdSet_insert (t, s->elements[i]);
	    }
	}
      
      return t;
    }
  else
    {
      return usymIdSet_undefined;
    }
}

usymIdSet
usymIdSet_newUnion (usymIdSet s1, usymIdSet s2)
{
  usymIdSet t = usymIdSet_new ();

  usymIdSet_elements (s1, current)
    {
      t = usymIdSet_insert (t, current);
    } end_usymIdSet_elements;

  usymIdSet_elements (s2, current)
    {
      t = usymIdSet_insert (t, current);
    } end_usymIdSet_elements;

  return t;
}

/*
** returns a new usymIdSet comprised of all elements
** in s which are not in t.
*/

usymIdSet
usymIdSet_subtract (usymIdSet s, usymIdSet t)
{
  usymIdSet r = usymIdSet_new ();

  usymIdSet_elements (s, current)
    {
      if (!usymIdSet_member (t, current))
	{
	  r = usymIdSet_insert (r, current);
	}
    } end_usymIdSet_elements;

  return r;
}

bool
usymIdSet_member (usymIdSet s, usymId el)
{
  if (usymIdSet_isUndefined (s))
    {
      return FALSE;
    }
  else
    {
      int i;
      
      for (i = 0; i < s->entries; i++)
	{
	  if (usymId_equal (el, s->elements[i]))
	    return TRUE;
	}
      return FALSE;
    }
}

void
usymIdSet_free (/*@only@*/ usymIdSet s)
{
  if (!usymIdSet_isUndefined (s))
    {
      int i;
      for (i = 0; i < s->entries; i++)
	{
	  /*      usymId_free (s->elements[i]); */
	}
      
      sfree (s->elements); 
      sfree (s);
    }
}

cstring usymIdSet_dump (usymIdSet lset)
{
  cstring st = cstring_undefined;
  
  if (!usymIdSet_isUndefined (lset))
    {
      bool first = TRUE;
      int i;
      
      for (i = 0; i < lset->entries; i++)
	{
	  usymId current = lset->elements[i];
	  
	  if (!usymId_isInvalid (current))
	    {
	      current = usymtab_convertId (current);

	      if (first)
		{
		  st = message ("%d", current);
		  first = FALSE;
		}
	      else
		{
		  st = message ("%q,%d", st, current);
		}
	    }
	}
    }
  return (st);
}

/*
** end of list is '@' or '\0'
*/

usymIdSet
usymIdSet_undump (char **s)
{
  usymIdSet t = usymIdSet_new ();
  char *olds = *s;
  char c;

  
  while ((c = **s) != '\0' && c != '@' && c != '#' && c != '\n')
    {
      int tid = 0;

      while (c != '@' && c != '#' && c != ',' && c != '\0' && c != '\n')
	{
	  while (c >= '0' && c <= '9')
	    {
	      tid *= 10;
	      tid += (int) (c - '0');
	      (*s)++;
	      c = **s;
	    }

	  if (*s == olds)
	    {
	      llcontbug (message ("usymIdSet_undump: loop: %s", 
				  cstring_fromChars (*s)));

	      while (**s != '\0') 
		{
		  (*s)++;
		}

	      /*@innerbreak@*/ break;
	    }

	  olds = *s;
	  
	  t = usymIdSet_insert (t, usymId_fromInt (tid));
	}

      if (c == ',')
	{
	  (*s)++;
	}
    }

  return t;
}

/*@only@*/ cstring
usymIdSet_unparse (usymIdSet ll)
{
  cstring s = cstring_undefined;

  if (!usymIdSet_isUndefined (ll))
    {
      int i;
      
      for (i = 0; i < ll->entries; i++)
	{
	  usymId current = ll->elements[i];

	  if (i == 0)
	    s = uentry_getName (usymtab_getGlobalEntry (current));
	  else
	    s = message ("%q, %q", s, uentry_getName (usymtab_getGlobalEntry (current)));
	}
    }
  
  return s;
}

int 
usymIdSet_compare (usymIdSet l1, usymIdSet l2)
{
  if (usymIdSet_isUndefined (l1))
    {
      return (usymIdSet_size (l2) == 0 ? 0 : 1);
    }
  
  if (usymIdSet_isUndefined (l2))
    {
      return (usymIdSet_size (l1) == 0 ? 0 : 1);
    }
  
  {
    int li1 = l1->entries;
    int li2 = l2->entries;
    int leastelements = (li1 < li2) ? li1 : li2;
    int i = 0;
    
    while (i < leastelements)
      {
	if (usymId_equal (l1->elements[i], l2->elements[i]))
	  {
	    i++;
	  }
	else
	  {
	    if (l1->elements[i] > l2->elements[i]) 
	      {
		return 1;
	      }
	    else
	      {
		return -1;
	      }
	  }
      }

    return (int_compare (li1, li2));
  }
}
