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
** genericTable.c
**
** A genericTable is a mapping from string keys to void * objects.
** We sacrific type checking here for code reuse.
*/

# include "splintMacros.nf"
# include "basic.h"
# include "randomNumbers.h"

/*@constant null ghbucket ghbucket_undefined; @*/
# define ghbucket_undefined 0

static /*@nullwhentrue@*/ bool ghbucket_isNull (/*@null@*/ ghbucket h) 
{ 
  return (h == ghbucket_undefined); 
}

static ghentry
ghentry_create (/*@keep@*/ cstring key, /*@keep@*/ void *val)
{
  ghentry h = (ghentry) dmalloc (sizeof (*h));

  h->key = key;

  llassert (val != NULL);
  h->val = val;

  return (h);
}

static void
ghentry_free (/*@only@*/ ghentry ghe)
{
  cstring_free (ghe->key);
  /* can't free val contents */
  sfree (ghe->val);
  sfree (ghe);
}

static bool
ghbucket_isEmpty (ghbucket h)
{
  return (h == ghbucket_undefined || h->size == 0);
}

int genericTable_size (genericTable h)
{
  if (genericTable_isDefined (h)) {
    return h->nentries;
  } else {
    return 0;
  }
}

# if 0
static /*@unused@*/ cstring
ghbucket_unparse (ghbucket h)
{
  cstring s = cstring_undefined;

  if (!ghbucket_isNull (h))
    {
      int i;
      
      for (i = 0; i < h->size; i++)
	{
	  s = message ("%q %s", s, h->entries[i]->key);
	}
    }

  return s;
}
# endif

static ghbucket ghbucket_single (/*@keep@*/ ghentry e)
{
  ghbucket h = (ghbucket) dmalloc (sizeof (*h));
  
  h->size = 1;
  h->nspace = GHBUCKET_BASESIZE - 1;
  h->entries = (ghentry *) dmalloc (GHBUCKET_BASESIZE * sizeof (*h->entries));
  h->entries[0] = e;
  
  return (h);
}

static void
ghbucket_grow (/*@notnull@*/ ghbucket h)
{
  int i;
  ghentry *newentries; 
  
  h->nspace += GHBUCKET_BASESIZE;

  newentries = (ghentry *) 
    dmalloc ((h->size + GHBUCKET_BASESIZE) * sizeof (*newentries));
  
  for (i = 0; i < h->size; i++) 
    {
      newentries[i] = h->entries[i]; 
    }
  
  sfree (h->entries);
  h->entries = newentries; 
  /*@-compmempass@*/
} /*@=compmempass*/ /* Spurious warnings reported for h->entries */ 

static /*@null@*/ /*@exposed@*/ void *ghbucket_lookup (ghbucket p_h, cstring p_key);

/*
** bizarre duplicate behaviour
** required for duplicate entries
*/

static void
ghbucket_add (/*@notnull@*/ ghbucket h, /*@only@*/ ghentry e)
{
  void *exloc = ghbucket_lookup (h, e->key);
  
  if (exloc != NULL) {
    llcontbug (message ("ghbucket_add: adding duplicate entry: %s",
			e->key));
    ghentry_free (e);
    return;
  }
  
  if (h->nspace == 0) {
    ghbucket_grow (h);
  }
  
  h->entries[h->size] = e;
  h->size++;
  h->nspace--;
}

static int
ghbucket_ncollisions (ghbucket h)
{
  if (!ghbucket_isNull (h) && (h->size > 1))
    return (h->size - 1);
  else
    return 0;
}

/*@exposed@*/ /*@null@*/ void *
ghbucket_lookup (ghbucket h, cstring key)
{
  if (!ghbucket_isNull (h))
    {
      int i;
      
      for (i = 0; i < h->size; i++)
	{
	  if (cstring_equal (h->entries[i]->key, key))
	    {
		return h->entries[i]->val;
	    }
	}
    }

  return NULL;
}

static
void ghbucket_free (/*@only@*/ ghbucket h)
{
  if (!ghbucket_isNull (h))
    {
      int i;

      for (i = 0; i < h->size; i++)  
	{
	  ghentry_free (h->entries[i]);
	}

      sfree (h->entries);
      sfree (h);
    }
}

void 
genericTable_free (/*@only@*/ genericTable h)
{
  if (genericTable_isDefined (h))
    {
      int i;
      
      for (i = 0; i < h->size; i++)
	{
	  ghbucket_free (h->buckets[i]);
	}
      
      sfree (h->buckets);
      sfree (h);
    }
}

static int
genericTable_countCollisions (genericTable h)
{
  int nc = 0;
  int i;

  llassert (genericTable_isDefined (h)); 

  for (i = 0; i < h->size; i++)
    {
      nc += ghbucket_ncollisions (h->buckets[i]);
    }

  return (nc);
}


static int
genericTable_countEmpty (genericTable h)
{
  int nc = 0;
  int i;

  llassert (genericTable_isDefined (h)); 

  for (i = 0; i < h->size; i++)
    {
      if (ghbucket_isEmpty (h->buckets[i]))
	{
	  nc++;
	}
    }

  return (nc);
}

/*
** hash function snarfed from quake/hash.c Hash_String
** by Stephen Harrison
*/

static unsigned int 
genericTable_hashValue (/*@notnull@*/ genericTable h, cstring key)
{
  char *p;
  unsigned int hash_value = 0;

  llassert (h->size != 0);

  for (p = cstring_toCharsSafe (key); *p != '\0'; p++)
    {
      hash_value = (hash_value << 1) ^ g_randomNumbers[*p % 256];
    }

  return (hash_value % h->size);
}

static /*@exposed@*/ ghbucket
genericTable_hash (/*@notnull@*/ genericTable h, cstring key)
{
  return h->buckets[genericTable_hashValue (h, key)];
}


/*@only@*/ genericTable
genericTable_create (int size)
{
  int i;
  genericTable h = (genericTable) dmalloc (sizeof (*h));

  llassert (size > 0);
  h->size = size;
  h->nentries = 0;
  h->buckets = (ghbucket *) dmalloc (sizeof (*h->buckets) * size);
  
  /*@+loopexec@*/
  for (i = 0; i < size; i++)
    {
      h->buckets[i] = ghbucket_undefined;
    }
  /*@-loopexec@*/
  return h;
}

# if 0
/*@-mustfree@*/
static /*@unused@*/ void
genericTable_print (genericTable h)
{
  int i;

  if (genericTable_isDefined (h)) {
    for (i = 0; i < h->size; i++)
      {
	ghbucket hb = h->buckets[i];
	
	if (hb != NULL)
	  {
	    llmsg (message ("%d. %s\n", i, ghbucket_unparse (hb)));
	  }
      }
    
    llmsg (message ("size: %d, collisions: %d, empty: %d", 
		    h->size, 
		    genericTable_countCollisions (h),
		    genericTable_countEmpty (h)));
  } else {
    llmsglit ("Empty hash table.");
  }
}
/*@=mustfree@*/
# endif

/*@only@*/ cstring
genericTable_stats (genericTable h)
{
  llassert (genericTable_isDefined (h)); 
  return (message ("size: %d, collisions: %d, empty: %d\n", 
		   h->size, genericTable_countCollisions (h),
		   genericTable_countEmpty (h)));
}

static void
genericTable_addEntry (/*@notnull@*/ genericTable h, /*@only@*/ ghentry e)
{
  unsigned int hindex = genericTable_hashValue (h, e->key);
  /*
  ** using
  **   ghbucket hb = h->buckets[hindex];  
  ** instead reveals a bug I don't want to deal with right now!
  */

  h->nentries++;
  
  if (ghbucket_isNull (h->buckets[hindex]))
    {
      h->buckets[hindex] = ghbucket_single (e); 
    }
  else
    {
      ghbucket_add (h->buckets[hindex], e);
    }
}

void
genericTable_insert (genericTable h, cstring key, void *value)
{
  unsigned int hindex;
  ghbucket hb;
  ghentry e;  

  llassert (genericTable_isDefined (h)); 

  /*
  ** rehashing based (loosely) on code by Steve Harrison
  */

  if (h->nentries * 162 > h->size * 100) 
    {
      int i;
      int oldsize = h->size;
      int newsize = 1 + ((oldsize * 26244) / 10000); /* 26244 = 162^2 */
      ghbucket *oldbuckets = h->buckets;

      DPRINTF (("Rehashing..."));
      h->size = newsize;  
      h->nentries = 0;
      h->buckets = (ghbucket *) dmalloc (sizeof (*h->buckets) * newsize);

      /*@+loopexec@*/
      for (i = 0; i < newsize; i++)
	{
	  h->buckets[i] = ghbucket_undefined;
	}
      /*@=loopexec@*/
      
      for (i = 0; i < oldsize; i++)
	{
	  ghbucket bucket = oldbuckets[i];

	  oldbuckets[i] = NULL;

	  if (!ghbucket_isNull (bucket))
	    {
	      int j;
	      
	      for (j = 0; j < bucket->size; j++)
		{
		  genericTable_addEntry (h, bucket->entries[j]);
		}
	      
	      sfree (bucket->entries); /* evans 2001-03-24: Splint caught this */
	      sfree (bucket);
	    }
	}

      sfree (oldbuckets);
    }

  /* evans 2000-12-22: this was before the rehash!  Lost an entry size! */
  h->nentries++;
  e = ghentry_create (key, value);
  hindex = genericTable_hashValue (h, key);
  hb = h->buckets[hindex];
  
  if (ghbucket_isNull (hb))
      {
	h->buckets[hindex] = ghbucket_single (e);
      }
  else
      {
	ghbucket_add (hb, e);
      }
}

/*@null@*/ /*@exposed@*/ void *
genericTable_lookup (genericTable h, cstring key)
{
  ghbucket hb;
  void *res;
  llassert (genericTable_isDefined (h));

  hb = genericTable_hash (h, key);
  res = ghbucket_lookup (hb, key);

  /* if (res == NULL) { DPRINTF (("Lookup not found: %s", key)); } */
  return res;
}

void
genericTable_update (genericTable h, cstring key, /*@only@*/ void *newval)
{
  ghbucket hb;

  llassert (genericTable_isDefined (h));

  hb = genericTable_hash (h, key);

  if (!ghbucket_isNull (hb))
    {
      int i;
      
      for (i = 0; i < hb->size; i++)
	{
	  if (cstring_equal (hb->entries[i]->key, key))
	    {
	      llassert (newval != NULL);
	      hb->entries[i]->val = newval;
	      return;
	    }
	}
    }

  llbug (message ("genericTable_update: %s not found", key));
}

void
genericTable_remove (genericTable h, cstring key)
{
  ghbucket hb;

  llassert (genericTable_isDefined (h));
  hb = genericTable_hash (h, key);

  if (!ghbucket_isNull (hb))
    {
      int i;
      
      for (i = 0; i < hb->size; i++)
	{
	  if (cstring_equal (hb->entries[i]->key, key))
	    {
	      if (i < hb->size - 1)
		{
		  hb->entries[i] = hb->entries[hb->size - 1];
		}
	      
	      hb->size--;
	      return;
	    }
	}
    }

  llbug (message ("genericTable_removeKey: %s not found", key));
}

bool genericTable_contains (genericTable h, cstring key) 
{
  return (genericTable_lookup (h, key) != NULL);
}
