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
** Larch shared language synonym table
**
**	This table stores synonyms for the Larch Shared Language.  It is
**	essentially a array of token-handles indexed by string-handles.
**	Therefore, synonyms (strings) can be converted to the actual token.
**
**  AUTHORS:
**	J.P. Wild
*/

# include "splintMacros.nf"
# include "basic.h"
# include "tokentable.h"
# include "syntable.h"

/*@+ignorequals@*/

typedef lsymbol *lsymbolTable;

static /*@only@*/ /*@null@*/ lsymbolTable SynTable;	
static unsigned long int SynTableEntries;

static void SynTable_grow (int p_size);

/*
**++
**  FUNCTION NAME:
**
**      LSLAddSyn ()
**
**  FORMAL PARAMETERS:
**
**      otok  - token-handle for token associated with oldToken
**	ntok  - string-handle for the string to be a synonym with oldToken.
**
**  RETURN VALUE:
**
**  INVARIANTS:
**
**      A context must be established.
**
**  DESCRIPTION:
**
**    This routine inserts a synonym into the synonym table.  The synonym table
**    is used to define synonyms in the form:
**
**	    synonym oldToken newToken
**
**    The table associates the string for newToken with the token for oldToken.
**    This table is used to find the the actual token (oldToken) from a synonym
**    string (newToken).
**
**    A new SynTable is allocated when:
**	. The SynTable[] is empty (initial case)
**      . The location where to insert the synonym is not in SynTable[]
**
**  IMPLICIT INPUTS/OUTPUT:
**
**    SynTable      - (input/output) SynTable array
**
**  EXCEPTIONS:
**    A synonym already exists at the location where the it is to be added.
**
**--
*/

void
LSLAddSyn (lsymbol ntok, lsymbol otok)
{
  if (ntok >= SynTableEntries) /* was otok */
    {
      SynTable_grow (otok);
    }

  llassert (SynTable != NULL);

  if (SynTable[ntok] == (lsymbol) 0)
    {				/* Entry is empty. Fill it in. */
      SynTable[ntok] = otok;
      LSLSetTokenHasSyn (otok, TRUE);	/* Mark oldToken as having a synonym. */
    }
  else
    {
      llbuglit ("LSLAddSyn: duplicate SynTable entry");
    }
}

/*@exposed@*/ ltoken
LSLGetTokenForSyn (lsymbol ntok)
{
  llassert (SynTable != NULL);
  llassert (!(!((ntok < SynTableEntries) || (SynTable[ntok] != 0))));

  return LSLGetToken (SynTable[ntok]);
}

bool
LSLIsSyn (lsymbol str)
{
  if (str < SynTableEntries)
    {
      llassert (SynTable != NULL);
      return (SynTable[str] != 0);
    }
  else
    {
      return FALSE;
    }
}

static void
SynTable_grow (int size)
{
  int oldSize;
  int i;
  lsymbolTable oldSynTable = SynTable;
  
  llassert (oldSynTable != NULL);
  oldSize = SynTableEntries;
  
  if (size <= oldSize)
    {
      llcontbuglit ("SynTable_grow: goal size is smaller than oldSize");
      return;
    }
  
  if (size < (oldSize + SYNTABLE_BASESIZE))
    {
      size = oldSize + SYNTABLE_BASESIZE;
    }

  SynTable = (lsymbolTable) dmalloc (size * sizeof (*SynTable));
  SynTableEntries = size;

  for (i = 0; i < oldSize; i++)
    {
      SynTable[i] = oldSynTable[i];
    }

  /* Zero out new allocated space.  Need to detect when cells are empty   */
  /* and do this by checking that SynTable[x] == 0.			  */

  /*@+loopexec@*/
  for (i = oldSize; i < size; i++)
    {
      SynTable[i] = (lsymbol) 0;
    }
  /*@=loopexec@*/

  sfree (oldSynTable);
/*@-compdef@*/ } /*=compdef@*/

void
lsynTableInit (void) /*@globals undef SynTable; @*/
{
  int i;

  SynTable = (lsymbolTable) dmalloc (sizeof (*SynTable) * SYNTABLE_BASESIZE);

  /*@+loopexec@*/
  for (i = 0; i < SYNTABLE_BASESIZE; i++)
    {
      SynTable[i] = (lsymbol) 0;
    }
  /*@=loopexec@*/

  SynTableEntries = SYNTABLE_BASESIZE;
/*@-compdef@*/ } /*@=compdef@*/

void
lsynTableReset (void)
{
}

void
lsynTableCleanup (void)
{
  sfree (SynTable);
  SynTable = NULL;
}












