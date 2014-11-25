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
** lsymbol.c
**
** String manager
**
**	This module implements an abstraction for efficiently managing
**      string comparisons.  It alloctes and manages a string context,
**      which consists of three major data structures:
**       - a StringEntry table
**       - a character-string table
**       - a hash table
**
**      A StringEntry table is made of of StringEntries. StringEntries
**      are linked in hash-chains to support fast lookup. Every
**      allocated StringEntry corresponds to a unique character-string
**	in the character-string table. StringEntries can be referenced
**      via Symbol values.
**
**	A character-string table is composed of character-strings. A
**      character-string is a variable length byte array that is always
**	null-terminated ('\0').
**
**	A hash table manages the start of each hash-list of StringEntries.
**	StringEntries are entered into the hash-list by hashing on its
**      character-string representation.
**
**	This module provides routines for retrieving a unique Symbol for a
**	given character-string, and returning the character-string given its
**	corresponding Symbol. An item is allocated in both tables whenever a
**	new character-string is encountered, otherwise the Symbol for the
**	character-string is found and returned.
**
**  AUTHORS:
**
**      Shota Aki
**
**  MODIFICATION HISTORY:
**
**	{0} Aki      at Digital -- 89.08.07 -- original
**	{1} Aki      at Digital -- 89.11.13 -- context switchable
**	{2} Aki      at Digital -- 89.11.28 -- removed use of TABLES interface
**	{3} Aki      at Digital -- 89.11.29 -- moved to RC
**	{4} Aki	     at Digital -- 90.04.10 -- support primary-context
**	{5} McKeeman at Digital -- 90.05.08 -- C to Larch SL
**	{6} Wild     at Digital	-- 91.06.26 -- Update copyright notice.
**	{n} Who	     at Where   -- yy.mm.dd -- what
*/

# include "splintMacros.nf"
# include "basic.h"

/*@+ignorequals@*/

/*@constant int NULLFACTOR; @*/
# define NULLFACTOR 1

typedef Handle CharIndex;     

typedef struct
{
  lsymbol HashNext;	
  CharIndex i;			
} StringEntry;

static void AllocCharSpace (unsigned p_newSize) /*@modifies internalState@*/ ;
static CharIndex AllocChar (/*@unique@*/ char *p_name) /*@modifies internalState@*/ ;
static void AllocEntrySpace (unsigned p_newSize) /*@modifies internalState@*/ ;
static lsymbol AllocEntry (char *p_name, long unsigned p_hashValue)
   /*@modifies internalState@*/ ;

static /*@only@*/ /*@null@*/ lsymbol *hashArray = NULL; 

static long unsigned MaxChar;	
static CharIndex FreeChar;	
static /*@only@*/ /*@null@*/ char *CharString;

static long unsigned MaxEntry;	
static lsymbol FreeEntry;	
static /*@only@*/ /*@null@*/ StringEntry *Entry;	

lsymbol
lsymbol_fromString (cstring s)
{
  if (cstring_isUndefined (s))
    {
      return lsymbol_undefined;
    }
  else
    {
      return (lsymbol_fromChars (cstring_toCharsSafe (s)));
    }
}

lsymbol
lsymbol_fromChars (/*@temp@*/ char *name)
{
  lsymbol ss;
  long unsigned hashValue;	
  unsigned h = 0;            
  char *p = name;

  while (*p != '\0')
    { 
      h = (h << 1) + (unsigned) (*p++); 
    } 
  
  hashValue = h & HASHMASK;         

  if (hashArray == NULL) /* evs - was MaxIndex == 0 */
    {
      /* nothing initialized */
      ss = AllocEntry (name, hashValue);	
    }
  else
    {
      ss = hashArray[hashValue]; /* start of hash chain */

      if (ss == lsymbol_undefined)
	{
	  /* hash not initialized */
	  ss = AllocEntry (name, hashValue);
	}
      else
	{
	 /*
          * Traverse hash-chain. Loop terminates when
          * a match is found or end of chain is encountered.
          */

	  llassert (Entry != NULL);
	  llassert (CharString != NULL);

	  while (strcmp (&CharString[Entry[ss].i], name) != 0)
	    {
	      if (lsymbol_undefined == (ss = Entry[ss].HashNext))
		{
		  ss = AllocEntry (name, hashValue);
		  break;
		}
	    }
	}
    }

  return ss;
}

cstring lsymbol_toString (lsymbol ss)
{
  return (cstring_fromChars (lsymbol_toChars (ss)));
}

char *
lsymbol_toCharsSafe (lsymbol ss)
{
  char *ret = lsymbol_toChars (ss);

  if (ret == NULL) 
    {
      ret = mstring_create (0);
    } 

  return ret;
}

char *lsymbol_toChars (lsymbol ss)
{
  if (lsymbol_isDefined (ss))
    {
      if (ss >= FreeEntry)
	{
	  llcontbug (message ("lsymbol_toChars: invalid lsymbol: %d", ss));
	  return NULL;
	}
      
      llassert (Entry != NULL);
      llassert (CharString != NULL);
      
      return &CharString[Entry[ss].i];
    }
  else
    {
      return NULL;
    }
}

static void
AllocCharSpace (unsigned newSize)
{
  llassert (newSize > MaxChar);
  
  CharString = (char *) drealloc ((void *) CharString, newSize * sizeof (*CharString));
  MaxChar = newSize;
/*@-compdef@*/
} /*@=compdef@*/

static CharIndex
AllocChar (/*@unique@*/ char *name)
{
  int namelength;
  CharIndex retVal;
  long unsigned size;
  CharIndex unused;

  namelength = size_toInt (strlen (name));
  unused = FreeChar;
  size = MaxChar;

  if ((unused + namelength + NULLFACTOR) > size)
    {
      if (size == 0)
	size = INITCHARSTRING;
      else
	size = (unsigned) (DELTACHARSTRING * size);

      AllocCharSpace (size);
    }

  llassert (CharString != NULL);

  retVal = unused;		
  strcpy (&CharString[unused], name);	
  unused += namelength;
  CharString[unused] = '\0';	
  unused += 1;

  FreeChar = unused;
  return retVal;
}

static void
AllocEntrySpace (unsigned newSize)
{
  llassert (newSize > MaxEntry);

  /* Casts mess up checking here. */
  /*@-mustfree@*/
  Entry = (StringEntry *) drealloc ((void *) Entry, newSize * sizeof (*Entry));
  /*@=mustfree@*/

  if (MaxEntry == 0) MaxEntry = 1;

  FreeEntry = MaxEntry;
  MaxEntry = newSize;
/*@-compdef@*/
} /*@=compdef@*/

static lsymbol AllocEntry (char *name, long unsigned hashValue)
{
  lsymbol retVal;
  long unsigned size;

  size = MaxEntry;

  if ((retVal = FreeEntry) == size)
    {
      if (size == 0)
	{
	  size = INITSTRINGENTRY;
	}
      else
	{
	  size = (unsigned) (DELTASTRINGENTRY * size);
	}

      AllocEntrySpace (size);
      retVal = FreeEntry;
    }
  
  FreeEntry = retVal + 1;

  llassert (hashArray != NULL);
  llassert (Entry != NULL);
  
  Entry[retVal].HashNext = hashArray[hashValue];
  hashArray[hashValue] = retVal;
  Entry[retVal].i = AllocChar (name);
  
  return retVal;
}

void
lsymbol_initMod (void)
   /*@globals undef CharString, undef Entry; @*/
{
  int i;

  if (hashArray != NULL)
    {
      sfree (hashArray); 
    }
  
  hashArray = (lsymbol *) dmalloc (HASHSIZE * sizeof (*hashArray));

  for (i = 0; i < HASHSIZE; i++)
    {
      hashArray[i] = lsymbol_undefined;
    } 

  MaxChar = 0;
  MaxEntry = 0;

  FreeChar = 0;
  FreeEntry = 0; 

  CharString = (char *) 0;
  Entry = (StringEntry *) 0;
/*@-compdef@*/ 
} 
/*@=compdef@*/ 

void
lsymbol_destroyMod (void)
   /*@globals killed Entry, killed CharString, killed hashArray@*/
{
   sfree (Entry);      
   sfree (CharString); 
   sfree (hashArray); 
}

void
lsymbol_printStats (void)
{
  /* only for debugging */
  printf ("Number of lsymbols generated = %d\n", (int) FreeEntry);
}

/*
** note lsymbol_setbool, etc. defined in abstract.c
*/








