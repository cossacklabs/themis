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
** lclscan.c
**
** Deliver tokens one at a time
**
**	METHOD:
**	The input arrives via LSLScanFreshToken ().
**	The output is reported via LSLScanNextToken ().
**
**	The tokens are built in module ScanLine.
**	The tokens are delivered from this module.
**	Meantimes, they are saved in a static array.
**
**	The tokenizing is split off from the delivery of tokens
**	to facilitate incremental scanning at a later date.
**	The essential is that scanline () can be called or not
**	if the input text is dirty or not.  Clean lines cause
**	tokens to be played out from the saved token list (not
**	yet implemented in this version).
*/

# include "splintMacros.nf"
# include "basic.h"

/*@-redecl@*/ /* from llgrammar.y */
extern bool g_inTypeDef;
/*@=redecl@*/

/*@ignore@*/
# include "llgrammar2.h" /* hack to force real include */
/*@end@*/

# include "lclscan.h"
# include "scanline.h"
# include "lclscanline.h"
# include "lcltokentable.h"

static inputStream scanFile;	/* file to scan		*/
static o_ltoken TokenList[MAXLINE];	/* available tokens	*/
static bool restore = FALSE;      /* wasn't static! */
static YYSTYPE restoretok;
static int nextToken;	        /* next available token */
static int lastToken;		/* next available slot	*/

static /*@dependent@*/ /*@null@*/ char *line;  /* input text */
static unsigned int lineNumber;	               /* current line number */

ltokenCode yllex (void)
  /*@globals killed restoretok@*/ /* only if restore is TRUE */
{
  lsymbol tokenSym;

  if (restore)
    {
      yllval = restoretok;
      restore = FALSE;
    }
  else
    {
      /*@-onlyunqglobaltrans@*/
      yllval.ltok = ltoken_copy (LCLScanNextToken ());
      /*@=onlyunqglobaltrans@*/
    }

  tokenSym = ltoken_getText (yllval.ltok);

  if (ltoken_getCode (yllval.ltok) == simpleId)
    {
      if (g_inTypeDef)
	{
	  ltoken_setCode (yllval.ltok, LLT_TYPEDEF_NAME);
	  LCLUpdateToken (LLT_TYPEDEF_NAME, tokenSym, 
			  ltoken_isStateDefined (yllval.ltok));
	}
      else
	{
	 /* or if it is already declared as a type, so
            typedef int foo; typedef foo bar;      works*/
	  if (symtable_exists (g_symtab, tokenSym))
	    {
	      if (typeInfo_exists (symtable_typeInfo (g_symtab, tokenSym)))
		{
		  ltoken_setCode (yllval.ltok, LLT_TYPEDEF_NAME);
		  LCLUpdateToken (LLT_TYPEDEF_NAME, tokenSym, 
				  ltoken_isStateDefined (yllval.ltok));
		}
	    }
	}
    }

  /*@-onlyunqglobaltrans@*/ /* restoretok not released on non-restore path */
  /*@-globstate@*/
  return (ltoken_getCode (yllval.ltok));
  /*@=onlyunqglobaltrans@*/
  /*@=globstate@*/
}

/* useful for scanning LCL init files and LSL init files ? */

/*@dependent@*/ ltoken
LCLScanNextToken (void)
{
  ltoken ret;

  if (nextToken < lastToken)
    {			
      ret = TokenList[nextToken++];
    }
  else
    {
      lastToken = 0;
      lineNumber++;
      line = inputStream_nextLine (scanFile);	

      if (line != (char *) 0)
	{
	  
	  LCLScanLine (line);	
	  nextToken = 0;
	  ret = LCLScanNextToken ();	
	  return ret;
	}
      else
	{
	  ret = LCLScanEofToken ();
	}
    }


    return ret;
}

static /*@exposed@*/ /*@dependent@*/ ltoken
LCLScanLookAhead (void)
{
  if (nextToken < lastToken)
    {			
      return TokenList[nextToken];
    }
  else
    {
      lastToken = 0;	
      line = inputStream_nextLine (scanFile);
      if (line != (char *) 0)
	{
	  LCLScanLine (line);	
	  nextToken = 0;	
	  return LCLScanLookAhead ();
	}
      else
	{
	  return LCLScanEofToken ();	
	}
    }
}

void
LCLScanFreshToken (/*@only@*/ ltoken tok)
{
  if (lastToken < MAXLINE)
    {		
      TokenList[lastToken++] = tok;
    }
  else
    {
      llbugexitlit ("LCLScanFreshToken: out of range");
    }
}

inputStream LCLScanSource (void)
{
  return scanFile;
}


void
LCLScanInit (void)
{
}

void
LCLScanReset (inputStream  s)
{
  scanFile = s;
  lastToken = 0;
  nextToken = lastToken + 1;	
  lineNumber = 0;
}

void
LCLScanCleanup (void)
{
}


