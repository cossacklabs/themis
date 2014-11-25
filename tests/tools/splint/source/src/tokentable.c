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
*/

# include "splintMacros.nf"
# include "basic.h"
# include "osd.h"
# include "tokentable.h"

static long unsigned MaxToken;	
static /*@null@*/ /*@only@*/ o_ltoken *TokenTable;       

static void AllocTokenTable (void) /*@modifies TokenTable, MaxToken@*/ ;

ltoken
  LSLInsertToken (ltokenCode cod, lsymbol sym, lsymbol rTxt, bool def)
{
  while (sym >= MaxToken)
    {
      AllocTokenTable ();
    }

  llassert (TokenTable != NULL);

  if (ltoken_isUndefined (TokenTable[sym]))
    {
      TokenTable[sym] = ltoken_create (cod, sym);        
      ltoken_setRawText (TokenTable[sym], rTxt);
      ltoken_setDefined (TokenTable[sym], def);
    }
  
  return TokenTable[sym];
}

void
LSLUpdateToken (ltokenCode cod, lsymbol sym, bool def)
{
  llassert (TokenTable != NULL);

  if (!ltoken_isUndefined (TokenTable[sym]))
    {
      ltoken_setCode (TokenTable[sym], cod);
      ltoken_setDefined (TokenTable[sym], def);
    }
  else
    {
      llfatalbug (message ("LSLUpdateToken: token not in table: %d, text: %s", 
			   (int) cod, cstring_fromChars (lsymbol_toChars (sym))));
    }
}

void
LSLSetTokenHasSyn (lsymbol sym, bool syn)
{
  llassert (TokenTable != NULL);
    
  if (!ltoken_isUndefined (TokenTable[sym]))
    {
      ltoken_setHasSyn (TokenTable[sym], syn);
    }
  else
    {
      llbuglit ("LSLSetTokenHasSyn: null token");
    }
}

ltoken LSLGetToken (lsymbol sym)
{
  llassert (TokenTable != NULL);

  if (!((sym < MaxToken) || (!ltoken_isUndefined (TokenTable[sym]))))
    {
      llcontbuglit ("LSLGetToken: bad argument");
      return TokenTable[0];
    }

  return TokenTable[sym];
}

/*@exposed@*/ ltoken
LSLReserveToken (ltokenCode cod, char *txt)
{
  lsymbol sym;
  
  sym = lsymbol_fromChars (txt);

  /* 
  ** Reserved tokens never have raw text like synonyms.
  */

  return LSLInsertToken (cod, sym, lsymbol_undefined, TRUE);
}

static void
AllocTokenTable (void)
{
  long unsigned oldSize, newSize;
  long unsigned int i;

  oldSize = MaxToken;

  if (oldSize == 0)
    {
      newSize = INITTOKENTABLE;
      llassert (TokenTable == NULL);
      TokenTable = (ltoken *) 
	dmalloc (size_fromLongUnsigned (newSize * sizeof (*TokenTable))); 
    }
  else
    {
      o_ltoken *oldTokenTable = TokenTable;

      newSize = (long unsigned) (DELTATOKENTABLE * oldSize);
      TokenTable = (ltoken *) 
	dmalloc (size_fromLongUnsigned (newSize * sizeof (*TokenTable)));

      llassert (oldSize > 0);
      llassert (oldTokenTable != NULL);
      
      for (i = 0; i < oldSize; i++)
	{
	  TokenTable[i] = oldTokenTable[i];
	}

      sfree (oldTokenTable);
    }

  /*@+loopexec@*/
  for (i = oldSize; i < newSize; i++)
    {
      TokenTable[i] = ltoken_undefined;
    }
  /*@=loopexec@*/

  MaxToken = newSize;
/*@-compdef@*/ } /*=compdef@*/

void
ltokenTableInit (void)
{
  MaxToken = 0;
}

void
ltokenTableCleanup (void)
{
  if (TokenTable != NULL)
    {
      long unsigned i;
      
      for (i = 0; i < MaxToken; i++)
	{
	  ltoken_free (TokenTable[i]);
	}

      sfree (TokenTable); 
      TokenTable = NULL;
    }
}
