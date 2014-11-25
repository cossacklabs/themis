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
** cstringTable.c
**
** Since hsearch defined in <search.h> only allows one hash table,
** I'll implement my own.
**
** Try to find a decent hash function, etc. later...
**
** cstringTable is from string -> unsigned
**
*/

# include "splintMacros.nf"
# include "basic.h"
# include "randomNumbers.h"

/*@constant null hbucket hbucket_undefined; @*/
# define hbucket_undefined 0

static void
cstringTable_addEntry (/*@notnull@*/ cstringTable p_h, /*@only@*/ hentry p_e) 
     /*@modifies p_h@*/ ;

static /*@nullwhentrue@*/ bool hbucket_isNull (/*@null@*/ hbucket h) 
{ 
  return (h == hbucket_undefined); 
}

static hentry hentry_create (/*@only@*/ cstring key, int val)
{
  hentry h = (hentry) dmalloc (sizeof (*h));

  h->key = key;
  h->val = val;
  llassert (val != HBUCKET_DNE); 
  return (h);
}

static void hentry_free (/*@only@*/ hentry h)
{
  cstring_free (h->key);
  sfree (h);
}

static bool
hbucket_isEmpty (hbucket h)
{
  return (h == hbucket_undefined || h->size == 0);
}

static cstring
hbucket_unparse (hbucket h)
{
  cstring s = cstring_undefined;

  if (!hbucket_isNull (h))
    {
      int i;
      
      for (i = 0; i < h->size; i++)
	{
	 s = message ("%q %s:%d", s, h->entries[i]->key, h->entries[i]->val);
	}
    }

  return s;
}

static hbucket
hbucket_single (/*@only@*/ hentry e)
{
  hbucket h = (hbucket) dmalloc (sizeof (*h));
  
  h->size = 1;
  h->nspace = HBUCKET_BASESIZE - 1;
  h->entries = (hentry *) dmalloc (HBUCKET_BASESIZE * sizeof (*h->entries));
 h->entries[0] = e;
  
  return (h);
}

static void
hbucket_grow (/*@notnull@*/ hbucket h)
{
  int i;
  hentry *newentries; 
  
  h->nspace += HBUCKET_BASESIZE;

  newentries = (hentry *) 
    dmalloc ((h->size + HBUCKET_BASESIZE) * sizeof (*newentries));
  
  for (i = 0; i < h->size; i++) 
    {
      newentries[i] = h->entries[i]; 
    }
 
  sfree (h->entries);
  h->entries = newentries; 
  /*@-compmempass@*/
} /*@=compmempass@*/ /* Spurious warnings reported - shouldn't need this */

static int hbucket_lookup (hbucket p_h, cstring p_key);

static bool hbucket_contains (hbucket p_h, cstring p_key) /*@*/ {
  return (hbucket_lookup (p_h, p_key) != HBUCKET_DNE);
}

/*
** bizarre duplicate behaviour
** required for duplicate entries
*/

static void
hbucket_add (/*@notnull@*/ hbucket h, /*@only@*/ hentry e)
{
  int exloc = hbucket_lookup (h, e->key);

  llassert (exloc == HBUCKET_DNE);
  
  if (h->nspace == 0)
    {
      hbucket_grow (h);
    }
  
  llassert (e->val != HBUCKET_DNE);
  h->entries[h->size] = e;
  h->size++;
  h->nspace--;
}

static int
hbucket_ncollisions (hbucket h)
{
  if (!hbucket_isNull (h) && (h->size > 1))
    return (h->size - 1);
  else
    return 0;
}

int
hbucket_lookup (hbucket h, cstring key)
{
  if (!hbucket_isNull (h))
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

  return HBUCKET_DNE;
}

static
void hbucket_free (/*@only@*/ hbucket h)
{
  if (!hbucket_isNull (h))
    {
      int i;
      /* evans 2002-07-12: added this to free entries (splint warning had been suppressed) */
      for (i = 0; i < h->size; i++)
	{
	  hentry_free (h->entries[i]);
	}

      sfree (h->entries);
      sfree (h);
    }
}

void 
cstringTable_free (/*@only@*/ cstringTable h)
{
  unsigned long i;

  llassert (cstringTable_isDefined (h)); 

  for (i = 0; i < h->size; i++)
    {
      hbucket_free (h->buckets[i]);
    }

  sfree (h->buckets);
  sfree (h);
}
  
static int
cstringTable_countCollisions (cstringTable h)
{
  int nc = 0;
  unsigned long i;

  llassert (cstringTable_isDefined (h)); 

  for (i = 0; i < h->size; i++)
    {
     nc += hbucket_ncollisions (h->buckets[i]);
    }

  return (nc);
}


static int
cstringTable_countEmpty (cstringTable h)
{
  int nc = 0;
  unsigned long i;

  llassert (cstringTable_isDefined (h)); 

  for (i = 0; i < h->size; i++)
    {
       if (hbucket_isEmpty (h->buckets[i]))
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
cstringTable_hashValue (/*@notnull@*/ cstringTable h, cstring key)
{
  char *p;
  unsigned int hash_value = 0;

  for (p = cstring_toCharsSafe (key); *p != '\0'; p++)
    {
      hash_value = (hash_value << 1) ^ g_randomNumbers[*p % 256];
    }

  return (hash_value % h->size);
}

static /*@exposed@*/ hbucket
cstringTable_hash (/*@notnull@*/ cstringTable h, cstring key)
{
  return h->buckets[cstringTable_hashValue (h, key)];
}


/*@only@*/ cstringTable
cstringTable_create (unsigned long size)
{
  unsigned long i;
  cstringTable h = (cstringTable) dmalloc (sizeof (*h));
  
  h->size = size;
  h->nentries = 0;
  h->buckets = (hbucket *) dmalloc (sizeof (*h->buckets) * size);
  
  /*@+loopexec@*/
  for (i = 0; i < size; i++)
    {
       h->buckets[i] = hbucket_undefined;
    }
  /*@-loopexec@*/
  return h;
}

cstring cstringTable_unparse (cstringTable h)
{
  cstring res = cstring_newEmpty ();
  unsigned long i;

  if (cstringTable_isDefined (h)) 
    {
      for (i = 0; i < h->size; i++)
	{
	   hbucket hb = h->buckets[i];
	  
	  if (hb != NULL)
	    {
	      res = message ("%q%wl. %q\n", res, i, hbucket_unparse (hb));
	    }
	}
      
      res = message ("%qsize: %wl, collisions: %d, empty: %d", 
		     res,
		     h->size, 
		     cstringTable_countCollisions (h),
		     cstringTable_countEmpty (h));
    } 
  else
    {
      cstring_free (res);
      res = cstring_makeLiteral ("< empty cstring table >");
    }
  
  return res;
}


/*@only@*/ cstring
cstringTable_stats (cstringTable h)
{
  llassert (cstringTable_isDefined (h)); 
  return (message ("size: %wl, collisions: %d, empty: %d\n", 
		   h->size, cstringTable_countCollisions (h),
		   cstringTable_countEmpty (h)));
}

static void
cstringTable_rehash (/*@notnull@*/ cstringTable h)
{
  /*
  ** rehashing based (loosely) on code by Steve Harrison
  */

  unsigned long i;
  /* Fix provided by Thomas Mertz (int -> unsigned long), 21 Apr 2004 */
  unsigned long oldsize = h->size;
  unsigned long newsize = 1 + ((oldsize * 26244) / 10000); /* 26244 = 162^2 */
  hbucket *oldbuckets = h->buckets;
  
  h->size = newsize;  
  h->nentries = 0;
  h->buckets = (hbucket *) dmalloc (sizeof (*h->buckets) * newsize);

  /*@+loopexec@*/
  for (i = 0; i < newsize; i++)
    {
       h->buckets[i] = hbucket_undefined;
    }
  /*@=loopexec@*/
  
  for (i = 0; i < oldsize; i++)
    {
       hbucket bucket = oldbuckets[i];

       oldbuckets[i] = NULL;

      if (!hbucket_isNull (bucket))
	{
	  int j;
	  
	  for (j = 0; j < bucket->size; j++)
	    {
	       cstringTable_addEntry (h, bucket->entries[j]);
	    }
	  
	  /* 
	  ** evans 2001-03-24: new memory leak detected by Splint
	  **   after I fixed the checkCompletelyDestroyed.
	  */

	  sfree (bucket->entries);
	  sfree (bucket);
	}
    }
  
  sfree (oldbuckets);
}

static void
cstringTable_addEntry (/*@notnull@*/ cstringTable h, /*@only@*/ hentry e)
{
  unsigned int hindex = cstringTable_hashValue (h, e->key);

  /*
  ** using
  **   hbucket hb = h->buckets[hindex];  
  ** instead reveals a bug I don't want to deal with right now!
  */

   if (hbucket_isNull (h->buckets[hindex]))
    {
       h->buckets[hindex] = hbucket_single (e); 
      h->nentries++;
    }
  else
    {
      if (hbucket_contains (h->buckets[hindex], e->key)) {
	llcontbug 
	  (message
	   ("cstringTable: Attempt to add duplicate entry: %s "
	    "[previous value %d, new value %d]", 
	    e->key, cstringTable_lookup (h, e->key), e->val));
	hentry_free (e);
	return;
      }

      hbucket_add (h->buckets[hindex], e);
      h->nentries++;
    }
}

void
cstringTable_insert (cstringTable h, cstring key, int value)
{
  unsigned long hindex;
  hbucket hb;
  hentry e;  

  llassert (cstringTable_isDefined (h)); 

  h->nentries++;

  if (h->nentries * 162 > h->size * 100) 
    {
      cstringTable_rehash (h);
    }
  
  hindex = cstringTable_hashValue (h, key);
  e = hentry_create (key, value);

   hb = h->buckets[hindex];
  
  if (hbucket_isNull (hb))
    {
       h->buckets[hindex] = hbucket_single (e);
    }
  else
    {
      llassert (!hbucket_contains (hb, e->key));
      hbucket_add (hb, e);
    }
}

int
cstringTable_lookup (cstringTable h, cstring key)
{
  hbucket hb;
  llassert (cstringTable_isDefined (h));

  hb = cstringTable_hash (h, key);
  return (hbucket_lookup (hb, key));
}

void
cstringTable_update (cstringTable h, cstring key, int newval)
{
  hbucket hb;

  llassert (cstringTable_isDefined (h));

  hb = cstringTable_hash (h, key);

  if (!hbucket_isNull (hb))
    {
      int i;
      
      for (i = 0; i < hb->size; i++)
	{
	   if (cstring_equal (hb->entries[i]->key, key))
	    {
	           hb->entries[i]->val = newval;
	      return;
	    }
	}
    }

  llbug (message ("cstringTable_update: %s not found", key));
}

/*
** This is needed if oldkey is going to be released.
*/

void
cstringTable_replaceKey (cstringTable h, cstring oldkey, /*@only@*/ cstring newkey)
{
  hbucket hb;
  llassert (cstringTable_isDefined (h));
  
  hb = cstringTable_hash (h, oldkey);
  llassert (cstring_equal (oldkey, newkey));

  if (!hbucket_isNull (hb))
    {
      int i;
      
      for (i = 0; i < hb->size; i++)
	{
	   if (cstring_equal (hb->entries[i]->key, oldkey))
	    {
	      hb->entries[i]->key = newkey;
	      return;
	    }
	}
    }

  llbug (message ("cstringTable_replaceKey: %s not found", oldkey));
}

void
cstringTable_remove (cstringTable h, cstring key)
{
  hbucket hb;

  llassert (cstringTable_isDefined (h));
  hb = cstringTable_hash (h, key);

  if (!hbucket_isNull (hb))
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

  llbug (message ("cstringTable_removeKey: %s not found", key));
}


