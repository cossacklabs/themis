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
** typeIdSet.c
*/

# include "splintMacros.nf"
# include "basic.h"

/*@constant int TISTABLEBASESIZE;@*/
# define TISTABLEBASESIZE LARGEBASESIZE

static int tistableentries = 0;
static int tistablefree = 0;
typedef /*@only@*/ usymIdSet o_usymIdSet;
static /*@only@*/ o_usymIdSet *tistable;
static void tistable_addDirectEntry (/*@only@*/ usymIdSet p_s) 
   /*@modifies tistable, tistableentries, tistablefree@*/;

void typeIdSet_initMod (void)
   /*@globals undef tistable;@*/
   /*@modifies tistable, tistablefree;@*/
{
  llassert (tistableentries == 0 && tistablefree == 0);

  tistablefree = TISTABLEBASESIZE;
  tistable = (usymIdSet *) dmalloc (sizeof (tistable) * tistablefree);
  tistable[0] = usymIdSet_undefined;
  tistableentries = 1;
  tistablefree--;
}

void typeIdSet_destroyMod (void)
   /*@globals killed tistable, tistableentries@*/
{
  int i;

  for (i = 0; i < tistableentries; i++)
    {
      usymIdSet_free (tistable[i]);
    }

  sfree (tistable);
  tistableentries = 0;
}

void typeIdSet_dumpTable (FILE *fout)
{
  int i;

  /*
  ** Don't dump 0th entry
  */

  for (i = 1; i < tistableentries; i++)
    {
      cstring s = usymIdSet_dump (tistable[i]);

      fprintf (fout, "%s\n", cstring_toCharsSafe (s));
      cstring_free (s);
    }
}

# if 0
static /*@unused@*/ void tistable_printOut (void)
{
  int i;

  /*
  ** Don't dump 0th entry
  */

  for (i = 1; i < tistableentries; i++)
    {
      cstring s = usymIdSet_unparse (tistable[i]);

      fprintf (g_warningstream, "%d: %s\n", i, cstring_toCharsSafe (s));
      cstring_free (s);
    }
}
# endif

void typeIdSet_loadTable (FILE *fin)
{
  char *s = mstring_create (MAX_DUMP_LINE_LENGTH);  
  char *os = s;

  llassert (tistableentries == 1);

  s = reader_readLine (fin, s, MAX_DUMP_LINE_LENGTH);

  while (s != NULL && *s != ';')
    {
      usymIdSet u = usymIdSet_undump (&s);
      
      llassert (*s == '\0' || *s == '\n');
      
      tistable_addDirectEntry (u);
      s = reader_readLine (fin, os, MAX_DUMP_LINE_LENGTH);
    }
}
  
static void tistable_grow (void)
{
  o_usymIdSet *oldtable = tistable;
  int newsize = tistableentries + TISTABLEBASESIZE;
  int i;

  llassert (tistablefree == 0);

  tistable = (usymIdSet *) dmalloc (sizeof (tistable) * newsize);

  for (i = 0; i < tistableentries; i++)
    {
      tistable[i] = oldtable[i];
    }

  tistablefree = TISTABLEBASESIZE;
  sfree (oldtable);
} 

static void tistable_addDirectEntry (/*@only@*/ usymIdSet s)
{
  if (tistablefree == 0)
    {
      tistable_grow ();
    }

  tistable[tistableentries] = s;
  tistableentries++;
  tistablefree--;
}

static int tistable_addEntry (/*@only@*/ usymIdSet s)
{
  int i;

  
  for (i = 0; i < tistableentries; i++)
    {
      if (usymIdSet_compare (tistable[i], s) == 0)
	{
	  /*@access usymIdSet@*/
	  llassert (i == 0 || s != tistable[i]);
	  /*@noaccess usymIdSet@*/

	  usymIdSet_free (s);
	  	  return i;
	}
    }

  tistable_addDirectEntry (s);
  return (tistableentries - 1);
}
  
static /*@observer@*/ usymIdSet tistable_fetch (typeIdSet t)
   /*@globals tistableentries, tistable@*/
   /*@modifies nothing;@*/
{
  llassert (t >= 0 && t < tistableentries);

  return tistable[t];
}

typeIdSet typeIdSet_emptySet (void)
{
  if (tistableentries == 0)
    {
      int val = tistable_addEntry (usymIdSet_new ());

      llassert (val == 0);
    }

  llassert (usymIdSet_isUndefined (tistable[0]));
  return 0;
}
    
bool typeIdSet_member (typeIdSet t, typeId el)
{
  usymIdSet u = tistable_fetch (t);
  return usymIdSet_member (u, typeId_toUsymId (el));
}

bool typeIdSet_isEmpty (typeIdSet t)
{
  return (t == 0);
}

typeIdSet typeIdSet_single (typeId t)
{
  return (tistable_addEntry (usymIdSet_single (typeId_toUsymId (t))));
}

typeIdSet typeIdSet_singleOpt (typeId t)
{
  if (typeId_isValid (t))
    {
      return (tistable_addEntry (usymIdSet_single (typeId_toUsymId (t))));
    }
  else
    {
      return typeIdSet_empty;
    }
}

typeIdSet typeIdSet_insert (typeIdSet t, typeId el)
{
  usymIdSet u = tistable_fetch (t);

  if (usymIdSet_member (u, typeId_toUsymId (el)))
    {
      return t;
    }
  else
    {
      return (tistable_addEntry (usymIdSet_add (u, typeId_toUsymId (el))));
    }
}

typeIdSet typeIdSet_removeFresh (typeIdSet t, typeId el)
{
  return (tistable_addEntry (usymIdSet_removeFresh (tistable_fetch (t), typeId_toUsymId (el))));
}

cstring typeIdSet_unparse (typeIdSet t)
{
  return (usymIdSet_unparse (tistable_fetch (t)));
}

int typeIdSet_compare (typeIdSet t1, typeIdSet t2)
{
  return (int_compare (t1, t2));
}

typeIdSet typeIdSet_subtract (typeIdSet s, typeIdSet t)
{
  if (typeIdSet_isEmpty (t))
    {
      return s;
    }
  else
    {
      return (tistable_addEntry (usymIdSet_subtract (tistable_fetch (s),
						     tistable_fetch (t))));
    }
}

cstring typeIdSet_dump (typeIdSet t)
{
  return (message ("%d", t));
}

typeIdSet typeIdSet_undump (char **s)
{
  int i;

  
  i = reader_getInt (s);

  llassert (i >= 0 && i < tistableentries);
  return (typeIdSet) i;
}

typeIdSet typeIdSet_union (typeIdSet t1, typeIdSet t2)
{
  if (t1 == typeIdSet_undefined)
    {
      return t2;
    }
  else if (t2 == typeIdSet_undefined)
    {
      return t1;
    }
  else
    {
      return (tistable_addEntry (usymIdSet_newUnion (tistable_fetch (t1),
						     tistable_fetch (t2))));
    }
}
