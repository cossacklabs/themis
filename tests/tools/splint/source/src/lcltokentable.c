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
** tokentable.c
**
** Larch processors token table
** This table stores predefined tokens for LCL.
*/

# include "splintMacros.nf"
# include "basic.h"
# include "lcltokentable.h"

static long unsigned MaxToken; 
static /*@only@*/ /*@null@*/ o_ltoken *LCLTokenTable = NULL;    

static void AllocTokenTable (void) 
   /*@globals LCLTokenTable@*/
   /*@modifies LCLTokenTable, MaxToken@*/;

ltoken 
  LCLInsertToken (ltokenCode cod, lsymbol sym, lsymbol rTxt,
		  bool isPredefined)
{
  /*
  ** If the token is already in the token table, it is returned.  Otherwise,
  ** the token is inserted into the table and then returned.
  **
  ** A new TokenTable is allocated when:
  **   . The TokenTable[] is empty (initial case)
  **   . The location where to insert the token is not in TokenTable[]
  */

  setCodePoint ();

  while (sym >= MaxToken)
    {
      setCodePoint ();
      /* No more space available.  Allocate more. */
      AllocTokenTable ();
    }
  
  llassert (LCLTokenTable != NULL);

  if (ltoken_isUndefined (LCLTokenTable[sym]))
    {
      LCLTokenTable[sym] = ltoken_create (cod, sym);      
      ltoken_setRawText (LCLTokenTable[sym], rTxt);
      ltoken_setDefined (LCLTokenTable[sym], isPredefined);
            return LCLTokenTable[sym];
    }

    return LCLTokenTable[sym];
}

void LCLUpdateToken (ltokenCode cod, lsymbol sym, bool def)
{
  llassert (LCLTokenTable != NULL);

  if (!ltoken_isUndefined (LCLTokenTable[sym]))
    {
      ltoken_setCode (LCLTokenTable[sym], cod);
      ltoken_setDefined (LCLTokenTable[sym], def);
    }
  else
    {
      llfatalbug (message ("LCLUpdateToken: %s", 
			   cstring_fromChars (lsymbol_toChars (sym))));
    }
}

void LCLSetTokenHasSyn (lsymbol sym, bool syn)
{
  llassert (LCLTokenTable != NULL);

  if (!ltoken_isUndefined (LCLTokenTable[sym]))
    {
      ltoken_setHasSyn (LCLTokenTable[sym], syn);
    }
  else
    {
      llfatalbug (message ("LCLSetTokenHasSyn: null token (%d)", (int)sym));
    }
}

ltoken LCLGetToken (lsymbol sym)
{
  llassert (LCLTokenTable != NULL);
  llassert (sym < MaxToken);

  return LCLTokenTable[sym];
}

#if 0
bool LCLTokenTableContainsToken (ltoken tok)
{
  unsigned long i;

  if (LCLTokenTable != NULL) {
    for (i = 0; i < MaxToken; i++) {
      if (LCLTokenTable[i] == tok) {
	return TRUE;
      }
    }
  }

  return FALSE;
}
# endif

/*@exposed@*/ ltoken
LCLReserveToken (ltokenCode cod, char *txt)
{
  /*
  ** The same context that was active when the string-handle
  ** was derived by a previous call to lsymbol_fromChars (),
  ** must be established.
  */
  lsymbol sym;

  setCodePoint ();
  sym = lsymbol_fromChars (txt);
  
  /* 
  ** Reserved tokens never have raw text like synonyms.
  */

    return (LCLInsertToken (cod, sym, lsymbol_undefined, TRUE));
}

static void
  AllocTokenTable (void) /*@globals LCLTokenTable; @*/ 
{
  long unsigned oldSize, newSize;
  long unsigned int i;
  
  oldSize = MaxToken;

  if (oldSize == 0)
    {
      newSize = INITTOKENTABLE;
      llassert (LCLTokenTable == NULL);
      LCLTokenTable = (ltoken *) dmalloc 
	(size_fromLongUnsigned (newSize * sizeof (*LCLTokenTable)));
    }
  else
    {
      o_ltoken *oldLCLTokenTable = LCLTokenTable;

      llassert (oldLCLTokenTable != NULL);

      newSize = (long unsigned) (DELTATOKENTABLE * oldSize);
      LCLTokenTable = (ltoken *) dmalloc 
	(size_fromLongUnsigned (newSize * sizeof (*LCLTokenTable)));

      for (i = 0; i < oldSize; i++)
	{
	  LCLTokenTable[i] = oldLCLTokenTable[i];
	}
      
      sfree (oldLCLTokenTable);
    }

  MaxToken = newSize;
  
  /*@+loopexec@*/
  for (i = oldSize; i < newSize; i++)
    {
      LCLTokenTable[i] = ltoken_undefined;
    }
  /*@=loopexec@*/
/*@-compdef@*/ } /*=compdef@*/

void
LCLTokenTableInit (void)
{
    MaxToken = 0;
}

void
LCLTokenTableCleanup (void)
{
    
  if (LCLTokenTable != NULL)
    {
      long unsigned i;
      
      for (i = 0; i < MaxToken; i++)
	{
	  ltoken tok = LCLTokenTable[i];
	  
	  LCLTokenTable[i] = NULL;
	  /*@-dependenttrans@*/ ltoken_free (tok);
	  /*@=dependenttrans@*/
	}
      
      sfree (LCLTokenTable); 
      LCLTokenTable = NULL;
    }

  }










