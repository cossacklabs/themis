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
** syntable.c
**
** Larch/C Interface language synonym table
**
**	This table stores synonyms for the Larch/C Interface Language.  It is
**	essentially an array of token-handles indexed by string-handles.
**	Therefore, synonyms (strings) can be converted to the actual token.
**
**  AUTHORS:
**	J.P. Wild
**
**
**  CREATION DATE:  90.08.10
*/

# include "splintMacros.nf"
# include "basic.h"
# include "lcltokentable.h"
# include "lclsyntable.h"

static long unsigned MaxSyn;	/* size of SynTable[]              */
static /*@only@*/ /*@reldef@*/ /*@null@*/ lsymbol *SynTable;
static void
  AllocSynTable (void)
  /*@globals SynTable, MaxSyn@*/
  /*@modifies *SynTable, MaxSyn@*/;

void
LCLAddSyn (lsymbol ntok, lsymbol otok)
{
  while (otok >= MaxSyn)
    {
      /* No more space available.  Allocate more. */
      AllocSynTable ();
    }

  llassert (SynTable != NULL);

  if (SynTable[ntok] == 0)
    {
     /* Entry is empty. Fill it in. */
      SynTable[ntok] = otok;

      /* Mark oldToken as having a synonym. */
      LCLSetTokenHasSyn (otok, TRUE);
    }
  else
    {
      llbuglit ("LCLAddSyn: invalid argument");
    }
}

/*@exposed@*/ ltoken
LCLGetTokenForSyn (lsymbol ntok)
{
  llassert (SynTable != NULL);

  if (!((ntok < MaxSyn) || (SynTable[ntok] != 0)))
    llbuglit ("LCLGetSyn: bad argument");

  return LCLGetToken (SynTable[ntok]);
}

bool
LCLIsSyn (lsymbol str)
{
  if (MaxSyn == 0)
    {
      return FALSE;
    }
  else
    {
      llassert (SynTable != NULL);

      if (str < MaxSyn)
	{
	  /* Check for synonym entry in table. */
	  return (SynTable[str] != 0);
	}
      else
	{
	  /* No token for synonym.  Return FALSE. */
	  return FALSE;
	}
    }
}
  
static void
AllocSynTable (void) /*@globals SynTable; @*/
{
  long unsigned newSize, oldSize;
  long unsigned int i;

  oldSize = MaxSyn;

  if (oldSize == 0)
    {
      /* First time SynTable allocated.  Set initial size. */
      newSize = INITSYNTABLE;
      SynTable = (lsymbol *) dmalloc 
	(size_fromLongUnsigned (newSize * sizeof (*SynTable)));
    }
  else
    {
      lsymbol *oldSynTable = SynTable; 

      llassert (oldSynTable != NULL);

      /* Synonym table already allocated.  Calulate extension size. */
      newSize = (unsigned long) (DELTASYNTABLE * oldSize);
      SynTable = (lsymbol *) dmalloc 
	(size_fromLongUnsigned (newSize * sizeof (*SynTable)));

      for (i = 0; i < oldSize; i++)
	{
	  SynTable[i] = oldSynTable[i];
	}
      
      sfree (oldSynTable);
    }

  /* Zero out new allocated space.  Need to detect when cells are empty     */
  /* and do this by checking that SynTable[x] == 0.			    */

  /* ###  Should the "for" loop be replaced with the following?	            */
  /* #if VMS								    */
  /* # include <string.h>;						    */
  /* #else								    */
  /* # include <memory.h>;						    */
  /*									    */
  /* memset (SynTable[oldSize], 0,					    */
  /*	      (newSize - oldSize) * sizeof (*SynTable));		            */
  
  for (i = oldSize; i < newSize; i++)
    {
      SynTable[i] = 0;
    }

  MaxSyn = newSize;
}


void
LCLSynTableInit (void)
{
  MaxSyn = 0;
}

void
LCLSynTableReset (void)
{
}

void
LCLSynTableCleanup (void)
{
  sfree (SynTable);
  SynTable = NULL;
}
