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
** sRefTable.c
**
** based on table_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"
# include "sRefTable.h"

static /*@notnull@*/ /*@only@*/ sRefTable
sRefTable_new (void)
{
  sRefTable s = (sRefTable) dmalloc (sizeof (*s));

  s->entries = 0;
  s->nspace = sRefTableBASESIZE;
  s->elements = (sRef *) dmalloc (sizeof (*s->elements) * sRefTableBASESIZE);

  return (s);
}

static void
sRefTable_grow (/*@notnull@*/ sRefTable s)
{
  int i;
  sRef *newelements; 

  s->nspace = sRefTableBASESIZE;
  newelements = (sRef *) dmalloc (sizeof (*newelements) * (s->entries + s->nspace));

  for (i = 0; i < s->entries; i++)
    {
      newelements[i] = s->elements[i];
    }

  sfree (s->elements);
  s->elements = newelements;
}

sRefTable
sRefTable_add (sRefTable s, /*@owned@*/ sRef el)
{
  if (sRefTable_isNull (s))
    {
      s = sRefTable_new ();
    }

  if (s->nspace <= 0)
    {
      sRefTable_grow (s);
    }

  s->nspace--;
  
  llassert (s->elements != NULL);
  s->elements[s->entries] = el;
  DPRINTF (("Adding to sRef table: [%p]", el));
  
  s->entries++;
  return s;
}

void
sRefTable_clear (sRefTable s)
{
# ifdef DEBUGSPLINT
  usymtab_checkAllValid ();
# endif

  if (sRefTable_isDefined (s))
    {
      int i;
      
      for (i = 0; i < s->entries; i++)
	{
	  DPRINTF (("Table clear: [%p] %s", s->elements[i], sRef_unparseDebug (s->elements[i])));
	  /* sRef_checkValid (s->elements[i]); */
	  sRef_free (s->elements[i]); 
	}
      
      s->nspace += s->entries;
      s->entries = 0;
    }

# ifdef DEBUGSPLINT
  usymtab_checkAllValid ();
# endif
}

static int sRefTable_size (sRefTable s)
{
  if (sRefTable_isNull (s)) return 0;
  return s->entries;
}

/*@only@*/ cstring
sRefTable_unparse (sRefTable s)
{
  int i;
  cstring st = cstring_undefined;

  if (sRefTable_isDefined (s))
    {
      for (i = 0; i < sRefTable_size (s); i++)
	{
	  if (i == 0)
	    st = message ("%4d. %q\n", i, sRef_unparse (s->elements[i]));
	  else
	    st = message ("%q%4d. %q\n", st, i, sRef_unparse (s->elements[i]));
	}
    }
  return st;
}

void
sRefTable_free (/*@only@*/ sRefTable s)
{
  if (sRefTable_isDefined (s))
    {
      int i;

      for (i = 0; i < s->entries; i++)
	{
	  DPRINTF (("Table free: [%p] %s", s->elements[i], sRef_unparse (s->elements[i])));
	  sRef_free (s->elements[i]);
	}

      sfree (s->elements); 
      sfree (s);
    }
}

