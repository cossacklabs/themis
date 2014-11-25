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
** mapping.c
**
** Module for lsymbol maps.
**
**  AUTHOR:
**	Yang Meng Tan,
**         Massachusetts Institute of Technology
*/

# include "splintMacros.nf"
# include "basic.h"

/*@constant int MAPPING_SIZE; @*/
# define MAPPING_SIZE 127

/* use lower-order bits by masking out higher order bits */

/*@-macrofcndecl@*/
# define MMASH(key)  ((unsigned int) ((key) & MAPPING_SIZE))
/*@=macrofcndecl@*/

static void mappair_free (/*@null@*/ /*@only@*/ mappair *p)
{
  if (p == NULL) 
    {
      return;
    }
  else
    {
      mappair_free (p->next);
      sfree (p);
    }
}

void mapping_free (/*@only@*/ mapping m)
{
  int i;

  for (i = 0; i <= MAPPING_SIZE; i++)
    {
      mappair_free (m->buckets[i]);
    }
  
  sfree (m->buckets);
  sfree (m);
}

/*@only@*/ mapping
mapping_create (void)
{
  int i;
  mapping t = (mapping) dmalloc (sizeof (*t));

  t->buckets = (mappair **) dmalloc ((MAPPING_SIZE + 1) * sizeof (*t->buckets));
  t->count = 0;

  for (i = 0; i <= MAPPING_SIZE; i++)
    {
      t->buckets[i] = (mappair *) 0;
    }

  return t;
}

lsymbol
mapping_find (mapping t, lsymbol domain)
{
  mappair *entry;
  unsigned int key;

  key = MMASH (domain);
  entry = t->buckets[key];
  for (; entry != NULL; entry = entry->next)
    {
      if (entry->domain == domain)
	return entry->range;
    }

  return lsymbol_undefined;
}

void
mapping_bind (mapping t, lsymbol domain, lsymbol range)
{
  /* add the binding (domain -> range) to t */
  /* can assume that the binding is a new one in m, so no need
     to check. */
  mappair *entry;
  mappair *newentry = (mappair *) dmalloc (sizeof (*newentry));
  unsigned int key;

  key = MMASH (domain);
  /*@-deparrays@*/ entry = t->buckets[key]; /*@=deparrays@*/
  newentry->domain = domain;
  newentry->range = range;
  newentry->next = entry;

  t->buckets[key] = newentry; 
  t->count++;
}
