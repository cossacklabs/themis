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
** mtscanner.c
**
** Because flex is so lame, we can't use two flex scanners at once.  Instead,
** we have to write a manual scanner.  Should look into some real parser 
** generator tools one day...
*/

# include "splintMacros.nf"
# include "basic.h"
# include "mtgrammar.h"
# include "mtscanner.h"

static inputStream scanFile;  /* file to scan */
static mttok mtscanner_getNextToken (void);
static /*@only@*/ cstringTable tokenTable = cstringTable_undefined;
static bool isInitialized = FALSE;

/*@constant int MT_TOKENTABLESIZE@*/
# define MT_TOKENTABLESIZE 64

static void mtscanner_initMod (void)
{
  llassert (cstringTable_isUndefined (tokenTable));
  tokenTable = cstringTable_create (MT_TOKENTABLESIZE);

  cstringTable_insert (tokenTable, cstring_makeLiteral ("attribute"), MT_STATE);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("global"), MT_GLOBAL);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("context"), MT_CONTEXT);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("oneof"), MT_ONEOF);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("defaults"), MT_DEFAULTS);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("default"), MT_DEFAULT);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("parameter"), MT_PARAMETER);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("result"), MT_RESULT);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("literal"), MT_LITERAL);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("null"), MT_NULL);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("reference"), MT_REFERENCE);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("clause"), MT_CLAUSE);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("annotations"), MT_ANNOTATIONS);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("merge"), MT_MERGE);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("transfers"), MT_TRANSFERS);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("preconditions"), MT_PRECONDITIONS);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("postconditions"), MT_POSTCONDITIONS);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("losereference"), MT_LOSEREFERENCE);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("error"), MT_ERROR);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("end"), MT_END);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("as"), MT_AS);

  /*
  ** C Types
  */

  cstringTable_insert (tokenTable, cstring_makeLiteral ("char"), MT_CHAR);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("int"), MT_INT);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("float"), MT_FLOAT);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("double"), MT_DOUBLE);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("void"), MT_VOID);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("anytype"), MT_ANYTYPE);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("integraltype"), MT_INTEGRALTYPE);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("unsignedintegraltype"), MT_UNSIGNEDINTEGRALTYPE);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("signedintegraltype"), MT_SIGNEDINTEGRALTYPE);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("const"), MT_CONST);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("volatile"), MT_VOLATILE);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("restrict"), MT_RESTRICT);

  /*
  ** Punctuation
  */

  cstringTable_insert (tokenTable, cstring_makeLiteral ("==>"), MT_ARROW);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("+"), MT_PLUS);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("*"), MT_STAR);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("{"), MT_LBRACE);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("}"), MT_RBRACE);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("("), MT_LPAREN);
  cstringTable_insert (tokenTable, cstring_makeLiteral (")"), MT_RPAREN);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("["), MT_LBRACKET);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("]"), MT_RBRACKET);
  cstringTable_insert (tokenTable, cstring_makeLiteral (","), MT_COMMA);
  cstringTable_insert (tokenTable, cstring_makeLiteral ("|"), MT_BAR);

  isInitialized = TRUE;
}

void mtscanner_reset (inputStream sourceFile)
{
  if (!isInitialized) 
    {
      mtscanner_initMod ();
    }

  scanFile = sourceFile;
}

int mtlex (YYSTYPE *mtlval)
{
  llassert (isInitialized);

  /* This is important!  Bison expects this */

  /*@ignore@*/
  mtlval->tok = mtscanner_getNextToken ();
  DPRINTF (("Return token: %s", mttok_unparse (mtlval->tok)));
  llassert (fileloc_isDefined (mttok_getLoc (mtlval->tok)));
  return (mttok_getTok (mtlval->tok));
  /*@end@*/
}

static void skipComments (void)
{
  int tchar;
  bool gotone = FALSE;
  tchar = inputStream_peekChar (scanFile);

  if (tchar == (int) '/' && inputStream_peekNChar (scanFile, 1) == (int) '*') 
    {
      check ((int) '/' == inputStream_nextChar (scanFile));
      check ((int) '*' == inputStream_nextChar (scanFile));
      
      while ((tchar = inputStream_nextChar (scanFile)) != EOF) 
	{
	  if (tchar == (int) '*' && inputStream_peekChar (scanFile) == (int) '/')
	    {
	      tchar = inputStream_nextChar (scanFile);
	      break;
	    }
	}

      if (tchar == EOF)
	{
	  llfatalerror 
	    (cstring_makeLiteral ("Reached end of metastate file inside comment."));
	  BADBRANCH;
	}
      else
	{
	  check ((int) '/' == tchar);
	  gotone = TRUE;
	}
    }

  if (isspace (tchar)) 
    {
      while (isspace (inputStream_peekChar (scanFile)))
	{
	  tchar = inputStream_nextChar (scanFile);
	}

      gotone = TRUE;
    }

  if (gotone)
    {
      /* If there was a comment or whitespace, need to skip again... */
      skipComments ();
    }
}

static mttok mtscanner_getNextToken ()
{
  int tchar;
  int mtcode;
  cstring tok;
  mttok res;
  fileloc loc;

  skipComments ();
  loc = fileloc_copy (g_currentloc);
  tchar = inputStream_nextChar (scanFile);

  if (tchar == EOF) 
    {
      return mttok_create (EOF, cstring_undefined, loc);
    }

  tok = cstring_newEmpty ();

  DPRINTF (("tchar: %c", (char) tchar));

  if (tchar == (int) '\"') 
    {
      bool escaped = FALSE;

      /* String literal */
      while ((tchar = inputStream_peekChar (scanFile)) != EOF) {
	if (escaped) {
	  escaped = FALSE;
	} else if (tchar == (int) '\\') {
	  escaped = TRUE;
	} else if (tchar == (int) '\"') {
	  break;
	} else {
	  ;
	}

	tok = cstring_appendChar (tok, (char) tchar);
	check (tchar == inputStream_nextChar (scanFile));
      }

      if (tchar == EOF)
	{
	  llfatalerror 
	    (cstring_makeLiteral ("Reached end of metastate file inside string literal."));
	}
      else
	{
	  check ((int) '\"' == inputStream_nextChar (scanFile));
	  return mttok_create (MT_STRINGLIT, tok, loc);
	}
    }
  
  tok = cstring_appendChar (tok, (char) tchar);

  DPRINTF (("tok: %s", tok));

  if (isalpha (tchar)) 
    {
      while ((tchar = inputStream_peekChar (scanFile)) != EOF) {
	if (!isalnum (tchar) && (tchar != (int) '_') && (tchar != (int) '$')) {
	  break;
	}
	
	tok = cstring_appendChar (tok, (char) tchar);
	check (tchar == inputStream_nextChar (scanFile));
      }

      mtcode = cstringTable_lookup (tokenTable, tok);

      if (mtcode == NOT_FOUND) {
	DPRINTF (("Returning identifier: %s", tok));
	return mttok_create (MT_IDENT, tok, loc);
      }
    } 
  else 
    {
      /* Read until next space */
      DPRINTF (("Here we are: %s", tok));

      while ((tchar = inputStream_peekChar (scanFile)) != EOF) {
	if (isspace (tchar) || isalnum (tchar)) {
	  break;
	}
	
	tok = cstring_appendChar (tok, (char) tchar);
	DPRINTF (("Here we are: %s", tok));
	check (tchar == inputStream_nextChar (scanFile));
      }

      DPRINTF (("Here we are: [%s]", tok));
      mtcode = cstringTable_lookup (tokenTable, tok);
      
      if (mtcode == NOT_FOUND) {
	mtcode = MT_BADTOK;
      }
    }
  
  DPRINTF (("Read %s / %d", tok, mtcode));
  cstring_free (tok);

  res = mttok_create (mtcode, cstring_undefined, loc);
  DPRINTF (("Return token: %s", mttok_unparse (res)));
  return res;
}

ctype mtscanner_lookupType (mttok tok)
{
  cstring tname;
  uentry ue;

  llassert (mttok_isIdentifier (tok));
  tname = mttok_observeText (tok);

  DPRINTF (("Lookup type:  %s", tname));

  ue = usymtab_lookupSafe (tname);

  if (uentry_isValid (ue) && uentry_isDatatype (ue))
    {
      DPRINTF (("Found it: %s / %s", uentry_unparse (ue),
		ctype_unparse (uentry_getAbstractType (ue))));
    
      return uentry_getAbstractType (ue);
    }
  else
    {
      ctype ct;
      ue = uentry_makeDatatype (tname, ctype_unknown, MAYBE, qual_createUnknown(),
				mttok_stealLoc (tok));
      DPRINTF (("Making mts entry: %s", uentry_unparse (ue)));
      ct = usymtab_supForwardTypeEntry (ue);
      DPRINTF (("Type: %s", ctype_unparse (ct)));
      return ct;
    }
}
