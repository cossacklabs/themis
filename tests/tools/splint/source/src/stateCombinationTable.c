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
** stateCombinationTable.c
**
** A stateCombinationTable is a mapping from keys to value tables.
*/

# include "splintMacros.nf"
# include "basic.h"

/*
** (key, value, value) => value
*/

static stateEntry stateEntry_create (void)
{
  stateEntry res = (stateEntry) dmalloc (sizeof (*res));
  res->value = 0;
  res->msg = cstring_undefined;
  return res;
}

static cstring stateEntry_unparse (stateEntry e) 
{
  if (cstring_isDefined (e->msg))
    {
      return message ("[%d: %s]", e->value, e->msg);
    }
  else
    {
      return message ("%d", e->value);
    }
}

stateCombinationTable stateCombinationTable_create (int size) 
{
  stateCombinationTable res = (stateCombinationTable) dmalloc (sizeof (*res));
  int i;
  
  res->size = size;
  res->rows = (stateRow *) dmalloc (sizeof (*(res->rows)) * size);
  
  for (i = 0; i < size; i++) 
    {
      int j;
      
      res->rows[i] = (stateRow) dmalloc (sizeof (*res->rows[i]));
      res->rows[i]->size = size;

      /* Rows have an extra entry (for lose ref transfers) */
      res->rows[i]->entries = (stateEntry *) 
	dmalloc (sizeof (*res->rows[i]->entries) * (size + 1));

      for (j = 0; j < size + 1; j++) 
	{
	  stateEntry s = stateEntry_create ();

	  /* Default transfer changes no state and is permitted without error. */

	  s->value = i;
	  llassert (cstring_isUndefined (s->msg));
	  
	  /*@-usedef@*/
	  res->rows[i]->entries[j] = s;
	  /*@=usedef@*/ 
	}
    }
  
  /*@-compmempass@*/ /*@-compdef@*/
  return res;
  /*@=compmempass@*/ /*@=compdef@*/
}

cstring stateCombinationTable_unparse (stateCombinationTable t)
{
  int i;
  cstring res = cstring_newEmpty ();

  for (i = 0; i < t->size; i++) 
    {
      int j;

      for (j = 0; j < (t->size + 1); j++)
	{
	  if (j == 0)
	    {
	      res = message ("%q[%d: ] %q", res, i, 
			     stateEntry_unparse (t->rows[i]->entries[j]));
	    }
	  else
	    {
	      res = message ("%q . %q", res, 
			     stateEntry_unparse (t->rows[i]->entries[j]));
	    }
	}

      res = cstring_appendChar (res, '\n');
    }

  return res;
}

static void stateEntry_free (/*@only@*/ stateEntry s)
{
  cstring_free (s->msg);
  sfree (s);
}

static void stateRow_free (/*@only@*/ stateRow r)
{
  int i;

  for (i = 0; i < r->size + 1; i++) 
    {
      stateEntry_free (r->entries[i]);
    }

  sfree (r->entries);
  sfree (r);
}
  
void stateCombinationTable_free (/*@only@*/ stateCombinationTable t)
{
  int i;

  for (i = 0; i < t->size; i++) {
    stateRow_free (t->rows[i]);
  }

  sfree (t->rows);
  sfree (t);
}

static /*@exposed@*/ stateEntry 
stateCombintationTable_getEntry (stateCombinationTable h,
				 int rkey, int ckey)
{
  llassert (rkey < h->size);
  llassert (ckey < h->size + 1);

  return h->rows[rkey]->entries[ckey];
}

void stateCombinationTable_set (stateCombinationTable h, 
				int p_from, int p_to, 
				int value, cstring msg)
{
  stateEntry entry = stateCombintationTable_getEntry (h, p_from, p_to);

  llassert (entry != NULL);

  entry->value = value;
  llassert (cstring_isUndefined (entry->msg));
  entry->msg = msg;

  DPRINTF (("Set entry: [%p] %d / %d => %s", entry,
	    p_from, p_to, cstring_toCharsSafe (msg)));

}

/*
** Like set, but may already have value.
** (Only different is error checking.)
*/

void stateCombinationTable_update (stateCombinationTable h, 
				   int p_from, int p_to, 
				   int value, cstring msg)
{
  stateEntry entry = stateCombintationTable_getEntry (h, p_from, p_to);

  llassert (entry != NULL);

  entry->value = value;
  cstring_free (entry->msg);
  entry->msg = msg;

  DPRINTF (("Update entry: [%p] %d / %d => %s", entry,
	    p_from, p_to, cstring_toCharsSafe (msg)));
}

int stateCombinationTable_lookup (stateCombinationTable h, int p_from, int p_to, /*@out@*/ ob_cstring *msg)
{
  stateEntry entry;
  llassert (p_from != stateValue_error);
  llassert (p_to != stateValue_error);

  entry = stateCombintationTable_getEntry (h, p_from, p_to);
  llassert (entry != NULL);

  *msg = entry->msg;
  return entry->value;
}

extern int 
stateCombinationTable_lookupLoseReference (stateCombinationTable h, int from,
					   /*@out@*/ /*@observer@*/ cstring *msg)
{
  return stateCombinationTable_lookup (h, from, h->size, msg);
}






