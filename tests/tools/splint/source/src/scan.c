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
** scan.c
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
# include "signature.h"
# include "signature2.h"
# include "scan.h"
# include "scanline.h"

static /*@exposed@*/ ltoken LSLScanLookAhead (void);
static inputStream scanFile;	  /* file to scan */
static o_ltoken TokenList[MAXLINE]; /* available tokens */
static int nextToken;		  /* next available token */
static int lastToken;		  /* next available slot */

static /*@dependent@*/ /*@null@*/ char *line;	/* input text */
static unsigned int lineNumber;	/* current line number */

unsigned int lsllex (YYSTYPE *lval)
{
  /* This is important!  Bison expects this */
  /* splint doesn't know the type of YYSTYPE, so we need to ignore warnings here */
  /*@i1@*/ lval->ltok = LSLScanNextToken ();
  /*@i1@*/ return (ltoken_getCode (lval->ltok));
}

ltoken LSLScanNextToken (void)
{
  if (nextToken < lastToken)
    {	
      ltoken res = TokenList[nextToken];
      TokenList[nextToken] = ltoken_undefined;
      nextToken++;
      /*@-dependenttrans@*/
      return res; /* Its the only reference now. */
      /*@=dependenttrans@*/

    }
  else
    {
      lastToken = 0;		
      lineNumber++;

      line = inputStream_nextLine (scanFile);       
      
      if (line != (char *) 0)
	{
	  lscanLine (line);	/* tokenize */
	  nextToken = 0;
	  return LSLScanNextToken ();	
	}
      else
	{
	  return LSLScanEofToken ();
	}
    }
}

static /*@exposed@*/ ltoken
LSLScanLookAhead (void)
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
	  lscanLine (line);	
	  nextToken = 0;	
	  return LSLScanLookAhead ();	
	}
      else
	{
	  /* 
          ** This is a real memory leak.  Its only a few bytes
	  ** per file though, and lsl files are hardly ever used.
	  */

	  /*@-onlytrans@*/ 
	  return LSLScanEofToken ();
	  /*@=onlytrans@*/
	}
    }
}

void
LSLScanFreshToken (ltoken tok)
{
  if (lastToken < MAXLINE)
    {				
      TokenList[lastToken++] = ltoken_copy (tok);	
    }
  else
    {
      llfatalbug (message ("LSLScanFreshToken: out of range: %s", 
			   cstring_fromChars (lsymbol_toChars (ltoken_getText (tok)))));
    }
}

/*@exposed@*/ inputStream LSLScanSource (void)
{
  return scanFile;
}


void
LSLScanInit (void)
{
}

void
LSLScanReset (inputStream s)
{
  scanFile = s;
  lastToken = 0;
  nextToken = lastToken + 1;	/* force call to scanline   */
  lineNumber = 0;
}

void
LSLScanCleanup (void)
{
}
