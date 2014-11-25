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
** cscannerHelp.c - procedures for scanning C
**
** Most of this code was in cscanner.l, but moved here to separate it
** from the flex-generated code.
*/

# include "splintMacros.nf"
# include "basic.h"
# include "cscannerHelp.h"
# include "cscanner.h"
# include "cgrammar_tokens.h"
# include "osd.h"

static int lminput (void);
static int s_tokLength = 0;

static /*@owned@*/ cstring s_lastidprocessed = cstring_undefined;
static bool s_inSpecPart = FALSE;
static int s_whichSpecPart;
static char s_savechar = '\0';
static bool s_expectingMetaStateName = FALSE;
static bool s_lastWasString = FALSE;
static bool s_expectingTypeName = TRUE;

struct skeyword
{
  /*@null@*/ /*@observer@*/ char *name;
  int token;
} ;

/*
** These tokens are followed by syntax that is parsed by the 
** grammar proper.
*/

static struct skeyword s_parsetable[] = {
  { "modifies", QMODIFIES } ,
  { "globals", QGLOBALS } ,
  { "alt", QALT } ,
  { "warn", QWARN } ,
  { "constant", QCONSTANT } ,
  { "function", QFUNCTION } ,
  { "iter", QITER } ,
  { "defines", QDEFINES } ,
  { "uses", QUSES } ,
  { "allocates", QALLOCATES } ,
  { "sets", QSETS } ,
  { "releases", QRELEASES } ,
  { "pre", QPRECLAUSE } ,
  { "post", QPOSTCLAUSE } ,
  { "setBufferSize", QSETBUFFERSIZE},
  { "setStringLength", QSETSTRINGLENGTH},
  { "testinRange", QTESTINRANGE},
  { "requires", QPRECLAUSE } ,
  { "ensures", QPOSTCLAUSE } ,
  { "invariant", QINVARIANT} ,
  { NULL, BADTOK } 
} ;

/*
** These tokens are either stand-alone tokens, or followed by 
** token-specific text.
*/

static struct skeyword s_keytable[] = {
  { "anytype", QANYTYPE } ,
  { "integraltype", QINTEGRALTYPE } ,
  { "unsignedintegraltype", QUNSIGNEDINTEGRALTYPE } ,
  { "signedintegraltype", QSIGNEDINTEGRALTYPE } ,
  { "out", QOUT } ,
  { "in", QIN } ,
  { "only", QONLY } , 
  { "owned", QOWNED } ,
  { "dependent", QDEPENDENT } ,
  { "partial", QPARTIAL } ,
  { "special", QSPECIAL } ,
  { "truenull", QTRUENULL } ,
  { "falsenull", QFALSENULL } ,
  { "nullwhentrue", QTRUENULL } ,
  { "falsewhennull", QFALSENULL } ,
  { "keep", QKEEP } ,
  { "kept", QKEPT } ,
  { "notnull", QNOTNULL } ,
  { "abstract", QABSTRACT } ,
  { "numabstract", QNUMABSTRACT } ,
  { "concrete", QCONCRETE } ,
  { "mutable", QMUTABLE } ,
  { "immutable", QIMMUTABLE } ,
  { "unused", QUNUSED } ,
  { "external", QEXTERNAL } ,
  { "sef", QSEF } ,
  { "unique", QUNIQUE } ,
  { "returned", QRETURNED } ,
  { "exposed", QEXPOSED } ,
  { "refcounted", QREFCOUNTED } ,
  { "refs", QREFS } ,
  { "newref", QNEWREF } ,
  { "tempref", QTEMPREF } ,
  { "killref", QKILLREF } ,
  { "null", QNULL } ,
  { "relnull", QRELNULL } ,
  { "nullterminated", QNULLTERMINATED }, 
  { "setBufferSize", QSETBUFFERSIZE },
  { "testInRange", QTESTINRANGE},
  { "isnull", QISNULL }, 
  { "MaxSet", QMAXSET},
  { "MaxRead", QMAXREAD},
  { "maxSet", QMAXSET},
  { "maxRead", QMAXREAD},
  { "reldef", QRELDEF } ,
  { "observer", QOBSERVER } ,
  { "exits", QEXITS } ,
  { "noreturn", QEXITS } ,
  { "mayexit", QMAYEXIT } ,
  { "maynotreturn", QMAYEXIT } ,
  { "trueexit", QTRUEEXIT } ,
  { "falseexit", QFALSEEXIT } ,
  { "noreturnwhentrue", QTRUEEXIT } ,
  { "noreturnwhenfalse", QFALSEEXIT } ,
  { "neverexit", QNEVEREXIT } ,
  { "alwaysreturns", QNEVEREXIT } ,
  { "temp", QTEMP } ,
  { "shared", QSHARED } ,
  { "ref", QREF } ,
  { "unchecked", QUNCHECKED } ,
  { "checked", QCHECKED } ,
  { "checkmod", QCHECKMOD } ,
  { "checkedstrict", QCHECKEDSTRICT } ,
  { "innercontinue", QINNERCONTINUE } ,
  { "innerbreak", QINNERBREAK } ,
  { "loopbreak", QLOOPBREAK } ,
  { "switchbreak", QSWITCHBREAK } ,
  { "safebreak", QSAFEBREAK } , 
  { "fallthrough", QFALLTHROUGH } ,
  { "l_fallthrou", QLINTFALLTHROUGH } , 
  { "l_fallth", QLINTFALLTHRU } ,
  { "notreached", QNOTREACHED } ,
  { "l_notreach", QLINTNOTREACHED } ,
  { "printflike", QPRINTFLIKE } ,
  { "l_printfli", QLINTPRINTFLIKE } ,
  { "scanflike", QSCANFLIKE } ,
  { "messagelike", QMESSAGELIKE } ,
  { "l_argsus", QARGSUSED } ,
  { NULL, BADTOK } 
} ;

/*
** would be better if these weren't hard coded...
*/

static bool isArtificial (cstring s)
{
  return (cstring_equalLit (s, "modifies") 
	  || cstring_equalLit (s, "globals") 
	  || cstring_equalLit (s, "warn")
	  || cstring_equalLit (s, "alt"));
}

void cscannerHelp_swallowMacro (void)
{
  int i;
  bool skipnext = FALSE;

  while ((i = lminput ()) != EOF)
    {
      char c = (char) i;
      
      if (c == '\\')
	{
	  skipnext = TRUE;
	}
      else if (c == '\n')
	{
	  if (skipnext)
	    {
	      skipnext = FALSE;
	    }
	  else
	    {
	      reader_checkUngetc (i, yyin);
	      return;
	    }
	}
      else
	{
	  ;
	}
    }

  if (i != EOF)
    {
      reader_checkUngetc (i, yyin);
    }
}

static int commentMarkerToken (cstring s)
{
  int i = 0;
  
  while (s_parsetable[i].name != NULL) 
    {
      DPRINTF (("Try :%s:%s:", s, s_parsetable[i].name));

      if (cstring_equalLit (s, s_parsetable[i].name))
	{
	  return s_parsetable[i].token;
	}

      i++;
    }

  return BADTOK;
}

static int tokenMacroCode (cstring s)
{
  int i = 0;
  
  while (s_keytable[i].name != NULL) 
    {
      if (cstring_equalLit (s, s_keytable[i].name)) 
	{
	  if (s_keytable[i].token == QLINTFALLTHROUGH) 
	    {
	      voptgenerror
		(FLG_WARNLINTCOMMENTS,
		 cstring_makeLiteral
		 ("Traditional lint comment /*FALLTHROUGH*/ used. "
		  "Splint interprets this in the same way as most Unix lints, but it is "
		  "preferable to replace it with the /*@fallthrough@*/ "
		  "semantic comment"),
		 g_currentloc);
	      return QFALLTHROUGH;	      
	    }
	  else if (s_keytable[i].token == QLINTFALLTHRU)
	    {
	      voptgenerror 
		(FLG_WARNLINTCOMMENTS,
		 cstring_makeLiteral
		 ("Traditional lint comment /*FALLTHRU*/ used. "
		  "Splint interprets this in the same way as most Unix lints, but it is "
		  "preferable to replace it with the /*@fallthrough@*/ "
		  "semantic comment"),
		 g_currentloc);
	      return QFALLTHROUGH;
	    }
	  else if (s_keytable[i].token == QLINTNOTREACHED)
	    {
	      voptgenerror 
		(FLG_WARNLINTCOMMENTS,
		 cstring_makeLiteral
		 ("Traditional lint comment /*NOTREACHED*/ used. "
		  "Splint interprets this in the same way as most Unix lints, but it is "
		  "preferable to replace it with the /*@notreached@*/ "
		  "semantic comment."),
		 g_currentloc);
	      
	      return QNOTREACHED;
	    }
	  else if (s_keytable[i].token == QPRINTFLIKE)
	    {
	      setSpecialFunction (qual_createPrintfLike ());
	      return SKIPTOK;
	    }
	  else if (s_keytable[i].token == QLINTPRINTFLIKE)
	    {	      
	      voptgenerror 
		(FLG_WARNLINTCOMMENTS,
		 cstring_makeLiteral
		 ("Traditional lint comment /*PRINTFLIKE*/ used. "
		  "Splint interprets this in the same way as most Unix lints, but it is "
		  "preferable to replace it with either /*@printflike@*/, "
		  "/*@scanflike@*/ or /*@messagelike@*/."),
		 g_currentloc);
	      
	      setSpecialFunction (qual_createPrintfLike ());
	      return SKIPTOK;
	    }
	  else if (s_keytable[i].token == QSCANFLIKE)
	    {
	      setSpecialFunction (qual_createScanfLike ());
	      return SKIPTOK;
	    }
	  else if (s_keytable[i].token == QMESSAGELIKE)
	    {
	      setSpecialFunction (qual_createMessageLike ());
	      return SKIPTOK;
	    }
	  else if (s_keytable[i].token == QARGSUSED)
	    {
	      voptgenerror
		(FLG_WARNLINTCOMMENTS,
		 cstring_makeLiteral
		 ("Traditional lint comment /*ARGSUSED*/ used. "
		  "Splint interprets this in the same way as most Unix lints, but it is "
		  "preferable to use /*@unused@*/ annotations on "
		  "the unused parameters."),
		 g_currentloc);
	      
	      setArgsUsed ();
	      return SKIPTOK;
	    }
	  else
	    {
	      return s_keytable[i].token;
	    }
	}

      i++;
    }
  
  return BADTOK;
}

static int lminput ()
{
  if (s_savechar == '\0')
    {
      incColumn ();
      return (cscanner_input ());
    }
  else
    {
      int save = (int) s_savechar;
      s_savechar = '\0';
      return save;
    }
}

static void lmsavechar (char c)
{
  if (s_savechar == '\0')
    {
      s_savechar = c;
    }
  else
    {
      llbuglit ("lmsavechar: override");
    }
}

int cscannerHelp_ninput ()  
{
  int c = lminput ();

  if (c != EOF && ((char)c == '\n'))
    {
      context_incLineno ();
    }

  return c;
}

static char macro_nextChar (void)
{
  static bool in_quote = FALSE, in_escape = FALSE, in_char = FALSE;
  int ic;
  char c;

  ic = lminput ();
  c = char_fromInt (ic);
  
  if (!in_quote && !in_char && (c == '\\' || c == BEFORE_COMMENT_MARKER[0]))
    {
      if (c == '\\')
	{
	  while ((c = char_fromInt (lminput ())) != '\0' && c != '\n')
	    {
	      ; /* skip to newline */
	    }
	  
	  context_incLineno ();
	  
	  if (c != '\0')
	    {
	      return macro_nextChar ();
	    }
	  else 
	    {
	      return c;
	    }
	}
      else /* if (c == '@') */
	{
	  llassert (FALSE); 

	  if (cscannerHelp_handleLlSpecial () != BADTOK)
	    {
	      llerrorlit (FLG_SYNTAX, "Macro cannot use special syntax");
	    }

	  return macro_nextChar ();
	}
    }
  else if (!in_escape && c == '\"')
    {
      in_quote = !in_quote;
    }
  else if (!in_escape && c == '\'')
    {
      in_char = !in_char;
    }
  else if ((in_quote || in_char) && c == '\\')
    {
      in_escape = !in_escape;
    }
  else if ((in_quote || in_char) && in_escape)
    {
      in_escape = FALSE;
    }
  else if (!in_quote && c == '/')
    {
      char c2;
      
      if ((c2 = char_fromInt (lminput ())) == '*')
	{
	  while (c2 != '\0')
	    {
	      while ((c2 = char_fromInt (lminput ())) != '\0'
		     && c2 != '\n' && c2 != '*')
		{
		  ;
		}
	      
	      if (c2 == '*')
		{
		  while ((c2 = char_fromInt (lminput ())) != '\0' 
			 && c2 == '*')
		    {
		      ;
		    }

		  if (c2 == '/')
		    {
		      goto outofcomment;
		    }
		}
	      else 
		{
		  llfatalerror (cstring_makeLiteral ("Macro: bad comment!"));
		}
	    }
	outofcomment:
	  return macro_nextChar ();
	}
      else
	{
          /*** putchar does not work!  why?  puts to stdio...??! ***/
          lmsavechar (c2);
	}
    }
  else
    {
      ;
    }

  return c;
}

/*
** keeps semantic comments
*/

static char macro_nextCharC (void)
{
  static bool in_quote = FALSE, in_escape = FALSE, in_char = FALSE;
  char c;

  c = char_fromInt (lminput ());

  if (!in_quote && !in_char && c == '\\')
    {
      while ((c = char_fromInt (lminput ())) != '\0' && c != '\n')
	{
	  ; /* skip to newline */
	}
      
      context_incLineno ();
      
      if (c != '\0')
	{
	  return macro_nextCharC ();
	}
      else
	{
	  return c;
	}
    }
  else if (!in_escape && c == '\"')
    {
      in_quote = !in_quote;
    }
  else if (!in_escape && c == '\'')
    {
      in_char = !in_char;
    }
  else if ((in_quote || in_char) && c == '\\')
    {
      in_escape = !in_escape;
    }
  else if ((in_quote || in_char) && in_escape)
    {
      in_escape = FALSE;
    }
  else if (!in_quote && c == '/')
    {
      char c2;
      
      if ((c2 = char_fromInt (lminput ())) == '*')
	{
	  while (c2 != '\0')
	    {
	      while ((c2 = char_fromInt (lminput ())) != '\0' 
		     && c2 != '\n' && c2 != '*')
		{
		  ;
		}
	      
	      if (c2 == '*')
		{
		  while ((c2 = char_fromInt (lminput ())) != '\0'
			 && c2 == '*')
		    {
		      ;
		    }

		  if (c2 == '/') 
		    {
		      goto outofcomment;
		    }
		}
	      else 
		{
		  llfatalerror (cstring_makeLiteral ("Macro: bad comment!"));
		}
	    }
	outofcomment:
	  return macro_nextCharC ();
	}
      else
	{
	  lmsavechar (c2);
	}
    }
  else /* normal character */
    {
      ;
    }

  return c;
}

/*
** skips whitespace (handles line continuations)
** returns first non-whitespace character
*/

static char skip_whitespace (void)
{
  char c;

  while ((c = macro_nextChar ()) == ' ' || c == '\t')
    {
      ;
    }

  return c;
}

void cscannerHelp_handleMacro ()
{
  cstring mac = cstring_undefined;
  int macrocode;
  char c;

  while (currentColumn () > 2)
    {
      mac = cstring_appendChar (mac, ' ');
      cscannerHelp_setTokLength (-1);
    }

  c = macro_nextCharC ();

  if (c >= '0' && c <= '9')
    {
      int i;

      for (i = 0; i < (((int) (c - '0')) + 1); i++)
	{
	  mac = cstring_appendChar (mac, ' ');
	}
    }
  else
    {
      BADBRANCH;
    }

  while (((c = macro_nextCharC ()) != '\0') && (c != '\n'))
    {
      mac = cstring_appendChar (mac, c);
    }

  
  macrocode = tokenMacroCode (mac);

  if (macrocode == BADTOK && !isArtificial (mac))
    {
      context_addMacroCache (mac);
    }
  else
    {
      cstring_free (mac);
    }

  if (c == '\n')
    {
      context_incLineno ();
    }
}

bool cscannerHelp_handleSpecial (char *yyt)
{
  char *l; /* !!  = mstring_create (MAX_NAME_LENGTH); */
  int lineno = 0;
  char c;
  char *ol;
  cstring olc;
  size_t len_yyt;

  len_yyt = strlen (yyt +1) ;

  l = mstring_copy (yyt + 1);
  
  while ((c = char_fromInt (lminput ())) != '\n' && c != '\0')
    {
      l = mstring_append(l, c);
    }

    /* Need to safe original l for deallocating. */
  ol = l;

  l += strlen (l);

  olc = cstring_fromChars (ol);
  
  if (cstring_equalPrefixLit (olc, "pragma"))
    {
      char *pname = mstring_create (size_fromInt (MAX_PRAGMA_LEN));
      char *opname = pname;
      char *ptr = ol + 6; /* pragma is six characters, plus space */
      int len = 0;
      
      
      /* skip whitespace */
      while (((c = *ptr) != '\0') && isspace (c))
	{
	  ptr++;
	}

      
      while (((c = *ptr) != '\0') && !isspace (c))
	{
	  len++;

	  if (len > MAX_PRAGMA_LEN)
	    {
	      break;
	    }

	  ptr++;
	  *pname++ = c;
	}

      *pname = '\0';
      
      if (len == PRAGMA_LEN_EXPAND 
	  && mstring_equal (opname, PRAGMA_EXPAND))
	{
	  cstring exname = cstring_undefined;
	  uentry ue;
	  
	  ptr++; 
	  while (((c = *ptr) != '\0') && !isspace (c))
	    {
	      exname = cstring_appendChar (exname, c);
	      ptr++;
	    }
	     
	  
	  ue = usymtab_lookupExposeGlob (exname);
	  
	  if (uentry_isExpandedMacro (ue))
	    {
	      if (fileloc_isPreproc (uentry_whereDefined (ue)))
		{
		  fileloc_setColumn (g_currentloc, 1);
		  uentry_setDefined (ue, g_currentloc);
		}
	    }

	  cstring_free (exname);
	}

      (void) cscannerHelp_handleNewLine (); /* evans 2003-10-27: pragment increments line */
    }
  else if (cstring_equalPrefixLit (olc, "ident"))
    {
      /* Some pre-processors will leave these in the code.  Ignore rest of line */
      (void) cscannerHelp_handleNewLine (); /* evans 2003-10-27: ident increments line */
    }

  /*
  ** Yuk...Win32 filenames can have spaces in them...we need to read
  ** to the matching end quote.
  */
  else if ((sscanf (ol, "line %d \"", &lineno) == 1)
	   || (sscanf (ol, " %d \"", &lineno) == 1))
    {
      char *tmp = ol;
      cstring fname;
      fileId fid;

      /*@access cstring@*/
      while (*tmp != '\"' && *tmp != '\0')
	{
	  tmp++;
	}

      llassert (*tmp == '\"');
      tmp++;
      fname = tmp;
      
      while (*tmp != '\"' && *tmp != '\0')
	{
	  tmp++;
	}

      llassert (*tmp == '\"');
      *tmp = '\0';

# if defined(OS2) || defined(MSDOS) || defined(WIN32)

      /*
      ** DOS-like path delimiters get delivered in pairs, something like 
      ** \"..\\\\file.h\", so we have to make it normal again. We do NOT
      ** remove the pre dirs yet as we usually specify tmp paths relative
      ** to the current directory, so tmp files would not get found in
      ** the hash table.  If this method fails we try it again later. 
      */

      {
	char *stmp = fname;
	
	/*
	** Skip past the drive marker.
	*/
	
	if (strchr (stmp, ':') != NULL)
	  {
	    stmp = strchr (stmp, ':') + 1;
	  }
	
	while ((stmp = strchr (stmp, CONNECTCHAR)) != NULL )
	  {
	    if (*(stmp+1) == CONNECTCHAR)
	      {
		memmove (stmp, stmp+1, strlen (stmp));
	      }
	    
	    stmp++;
	  }
	
	fid = fileTable_lookupBase (context_fileTable (), fname);
	if (!(fileId_isValid (fid)))
	  {
	    fname = removePreDirs (fname);
	    fid = fileTable_lookupBase (context_fileTable (), fname);
	  }
      }
# else  /* !defined(OS2) && !defined(MSDOS) */
      fname = removePreDirs (fname);
      fid = fileTable_lookupBase (context_fileTable (), fname);      
# endif /* !defined(OS2) && !defined(MSDOS) */
      
      if (!(fileId_isValid (fid)))
	{
	  if (context_inXHFile ())
	    {
	      fid = fileTable_addXHFile (context_fileTable (), fname);
	    }
	  else if (isHeaderFile (fname))
	    {
	      fid = fileTable_addHeaderFile (context_fileTable (), fname);
	    }
	  else
	    {
	      fid = fileTable_addFile (context_fileTable (), fname);
	    }
	}
      
      setFileLine (fid, lineno);
      /*@noaccess cstring@*/
    }
  else if ((sscanf (ol, "line %d", &lineno) == 1) 
	   || (sscanf (ol, " %d", &lineno) == 1))
    {
      setLine (lineno); /* next line is <cr> */
    }
  else
    {
      if (mstring_equal (ol, "")) {
	DPRINTF (("Empty pp command!"));
	/*
	** evs 2000-05-16: This is a horrible kludge, to get around a bug (well, difficulty) in the pre-processor.
	** We handle a plain # in the input file, by echoing it, and ignoring it in the post-pp-file.
	*/
	mstring_free (ol);
	(void) cscannerHelp_handleNewLine (); /* evans 2003-10-27: increments line */
	return FALSE;
      } else {
	voptgenerror
	  (FLG_UNRECOGDIRECTIVE,
	   message ("Unrecognized pre-processor directive: #%s", 
		    cstring_fromChars (ol)),
	   g_currentloc);
	(void) cscannerHelp_handleNewLine (); /* evans 2003-10-27: increments line */
      }
      
      sfree (ol);
      return FALSE; /* evans 2001-12-30: was: TRUE; */
    }

  sfree (ol);
  return FALSE;
}
  
int cscannerHelp_handleLlSpecial (void)
{ 
  bool hasnl = FALSE;
  int ic; 
  char c;
  char *s = mstring_createEmpty ();
  char *os; 
  int tok;
  int charsread = 0;
  fileloc loc;

  loc = fileloc_copy (g_currentloc);
  DPRINTF (("Handle special: %s", fileloc_unparse (loc)));

  while (((ic = cscannerHelp_ninput ()) != 0) && isalpha (ic))
    {
      c = (char) ic;
      s = mstring_append (s, c);
      charsread++;
    }

  DPRINTF (("Read: %s / %s", s, fileloc_unparse (g_currentloc)));
  os = s;

  if (charsread == 0 && ic == (int) AFTER_COMMENT_MARKER[0])
    {
      ic = cscannerHelp_ninput ();

      llassert (ic == (int) AFTER_COMMENT_MARKER[1]);
            
      if (*s == '\0')
	{
	  sfree (os);
	  fileloc_free (loc);
	  return QNOMODS; /* special token no modifications token */
	}
    }
  
  DPRINTF (("Coment marker: %s", os));
  tok = commentMarkerToken (cstring_fromChars (os));

  if (tok != BADTOK)
    {
      s_tokLength = charsread;
      sfree (os);
      s_inSpecPart = TRUE;
      s_whichSpecPart = tok;
      fileloc_free (loc);
      return tok;
    }

  DPRINTF (("Not a comment marker..."));
  /* Add rest of the comment */
  
  if (ic != 0 && ic != EOF)
    {
      c = (char) ic;
      
      s = mstring_append (s, c);
      charsread++;

      while (((ic = cscannerHelp_ninput ()) != 0) && (ic != EOF)
	     && (ic != (int) AFTER_COMMENT_MARKER[0]))
	{
	  c = (char) ic;

	  /* evans 2001-09-01 added to prevent assertion failures for uncloses syntactic comments */

	  if (c == '\n') {
	    hasnl = TRUE; /* This prevents tokLength from being set later. */
	    s_tokLength = 0; 

	    voptgenerror
	      (FLG_SYNTAX, 
	       message ("Likely parse error: syntactic comment token spans multiple lines: %s",
			cstring_fromChars (s)),
	       loc);
	  }

	  s = mstring_append (s, c);
	  charsread++;
	}
      /*@-branchstate@*/ 
    } /* spurious (?) warnings about s */
  /*@=branchstate@*/ 

  DPRINTF (("Read: %s / %s", s, fileloc_unparse (g_currentloc)));

  if (ic == (int) AFTER_COMMENT_MARKER[0]) 
    {
      int nc = cscannerHelp_ninput ();
      llassert ((char) nc ==  AFTER_COMMENT_MARKER[1]);
      charsread++;
    }

  os = s;

  while (*s == ' ' || *s == '\t' || *s == '\n') 
    {
      s++;
    }

  if (*s == '-' || *s == '+' || *s == '=') /* setting flags */
    {
      c = *s;

      while (c == '-' || c == '+' || c == '=')
	{
	  ynm set = ynm_fromCodeChar (c);
	  cstring thisflag;

	  s++;
	  
	  thisflag = cstring_fromChars (s);
	  
	  while ((c = *s) != '\0' && (c != '-') && (c != '=')
		 && (c != '+') && (c != ' ') && (c != '\t') && (c != '\n'))
	    {
	      s++;
	    }

	  *s = '\0';

	  if (!context_getFlag (FLG_NOCOMMENTS))
	    {
	      cstring flagname = thisflag;
	      flagcode fflag = flags_identifyFlag (flagname);
	      
	      if (flagcode_isSkip (fflag))
		{
		  ;
		}
	      else if (flagcode_isModeName (fflag))
		{
		  if (ynm_isMaybe (set))
		    {
		      llerror
			(FLG_BADFLAG, 
			 message 
			 ("Semantic comment attempts to restore flag %s.  "
			  "A mode flag cannot be restored.",
			  flagname));
		    }
		  else
		    {
		      context_setMode (flagname);
		    }
		}
	      else if (flagcode_isInvalid (fflag))
		{
		  voptgenerror
		    (FLG_UNRECOGFLAGCOMMENTS,
		     message ("Unrecognized option in semantic comment: %s", 
			      flagname),
		     loc);
		}
	      else if (flagcode_isGlobalFlag (fflag))
		{
		  voptgenerror
		    (FLG_BADFLAG, 
		     message 
		     ("Semantic comment attempts to set global flag %s.  "
		      "A global flag cannot be set locally.",
		      flagname),
		     loc);
		}
	      else
		{
		  context_fileSetFlag (fflag, set, loc);
		  
		  if (flagcode_hasArgument (fflag))
		    {
		      if (ynm_isMaybe (set))
			{
			  voptgenerror
			    (FLG_BADFLAG, 
			     message 
			     ("Semantic comment attempts to restore flag %s.  "
			      "A flag for setting a value cannot be restored.",
			      flagname),
			     loc);
			}
		      else
			{ /* cut-and-pastied from llmain...blecch */
			  cstring extra = cstring_undefined;
			  char *rest;
			  char *orest;
			  char rchar;
			  
			  *s = c;
			  rest = mstring_copy (s);
			  orest = rest;
			  *s = '\0';
			  
			  while ((rchar = *rest) != '\0'
				 && (isspace (rchar)))
			    {
			      rest++;
			      s++;
			    }
			  
			  while ((rchar = *rest) != '\0'
				 && !isspace (rchar))
			    {
			      extra = cstring_appendChar (extra, rchar);
			      rest++; 
			      s++;
			    }
			  s--; /* evans 2002-07-12: this was previously only in the else branch.
				  Leads to an invalid read on the true branch.
			       */

			  sfree (orest);
			  
			  if (cstring_isUndefined (extra))
			    {
			      llerror 
				(FLG_BADFLAG,
				 message
				 ("Flag %s (in semantic comment) must be followed by an argument",
				  flagcode_unparse (fflag)));

			      cstring_free (extra);
			    }
			  else
			    {
			      if (flagcode_hasNumber (fflag))
				{
				  flags_setValueFlag (fflag, extra);
				}
			      else if (flagcode_hasChar (fflag))
				{
				  flags_setValueFlag (fflag, extra);
				}
			      else if (flagcode_hasString (fflag))
				{
				  flags_setStringFlag (fflag, extra);
				}
			      else
				{
				  cstring_free (extra);
				  BADEXIT;
				}
			    }
			}
		    }
		}
	    }
	  else
	    {
	      ;
	    }

	  *s = c;
	  while ((c == ' ') || (c == '\t') || (c == '\n'))
	    {
	      c = *(++s);
	    }
	} 

      if (context_inHeader () && !isArtificial (cstring_fromChars (os)))
	{
	  DPRINTF (("Here adding comment: %s", os));
	  context_addComment (cstring_fromCharsNew (os), loc);
	}
      else
	{
	  ;
	}
    }
  else
    {
      char *t = s;
      int macrocode;
      char tchar = '\0';
      annotationInfo ainfo;

      while (*s != '\0' && *s != ' ' && *s != '\t' && *s != '\n') 
	{
	  s++;
	}

      if (*s != '\0') 
	{
	  tchar = *s;
	  *s = '\0';
	  s++;
	}
      
      t = cstring_toCharsSafe (cstring_downcase (cstring_fromChars (t)));
      macrocode = tokenMacroCode (cstring_fromChars (t));

      if (macrocode != BADTOK)
	{
	  s_tokLength = hasnl ? 0 : size_toInt (mstring_length (t));
	  
	  sfree (t);
	  sfree (os);
	  fileloc_free (loc);

	  if (macrocode == SKIPTOK)
	    {
	      return BADTOK;
	    }

	  return macrocode;
	}
      
      ainfo = context_lookupAnnotation (cstring_fromChars (os));
      
      if (annotationInfo_isDefined (ainfo)) {
	DPRINTF (("Found annotation: %s", annotationInfo_unparse (ainfo)));
	yylval.annotation = ainfo;
	s_tokLength = 0;
	sfree (os);
	sfree (t);
	fileloc_free (loc);
	return CANNOTATION;
      } 

      if (context_inHeader ())
	{
	  if (tchar != '\0')
	    {
	      *(s-1) = tchar;
	    }
	  
	  if ((context_inMacro () || context_inGlobalContext ())
	      && macrocode != SKIPTOK
	      && !isArtificial (cstring_fromChars (os))) 
	    {
	      if (context_processingMacros ())
		{
		  /* evans 2002-02-24: don't add comments when procssing macros */
		}
	      else
		{
		  context_addComment (cstring_fromCharsNew (os), loc);
		}
	    }
	  else
	    {
	      ; 
	    }
	  
	  if (tchar != '\0')
	    {
	      *(s-1) = '\0';
	    }
	}

      if (mstring_equal (t, "ignore"))
	{
	  if (!context_getFlag (FLG_NOCOMMENTS))
	    {
	      context_enterSuppressRegion (loc);
	    }
	}
      else if ((*t == 'i' || *t == 't')
	       && (*(t + 1) == '\0'))
	{
	  if (!context_getFlag (FLG_NOCOMMENTS)
	      && (*t == 'i' || context_getFlag (FLG_TMPCOMMENTS)))
	    {
	      context_enterSuppressLine (-1, loc); /* infinite suppression */
	    }
	}
      else if (((*t == 'i') || (*t == 't'))
	       && ((*(t + 1) >= '0' && *(t + 1) <= '9')))
	{
	  bool tmpcomment = (*t == 't');
	  int val = -1; 
	  char *tt = t; /* don't mangle t, since it is free'd */
	  char lc = *(++tt);

	  if (lc >= '0' && lc <= '9')
	    {
	      val = (int)(lc - '0');
	      
	      lc = *(++tt);	  
	      while (lc >= '0' && lc <= '9')
		{
		  val *= 10;
		  val += (int) (lc - '0');
		  lc = *(++tt);
		}
	    }
	  
	  if (!context_getFlag (FLG_NOCOMMENTS)
	      && (!tmpcomment || context_getFlag (FLG_TMPCOMMENTS)))
	    {
	      DPRINTF (("Here: enter suppress: %s", fileloc_unparse (loc)));
	      context_enterSuppressLine (val, loc);
	    }
	}
      else if (mstring_equal (t, "end"))
	{
	  if (!context_getFlag (FLG_NOCOMMENTS))
	    {
	      context_exitSuppressRegion (loc);
	    }
	}
      else if (mstring_equal (t, "notfunction"))
	{
	 ; /* handled by pcpp */
	}
      else if (mstring_equal (t, "access"))
	{
	  cstring tname;
	  
	  while (TRUE)
	    {
	      while (((c = *s) != '\0') && (c == ' ' || c == '\t' || c == '\n'))
		{
		  s++;
		}
	      
	      if (c == '\0')
		{
                   break;
		}

	      tname = cstring_fromChars (s);
	      
	      while ((c = *s) != '\0' && c != ' ' 
		     && c != '\t' && c != '\n' && c != ',') 
		{
		  s++;
		}

	      *s = '\0';

	      DPRINTF (("Access %s", tname));

	      if (!context_getFlag (FLG_NOCOMMENTS) 
		  && !context_getFlag (FLG_NOACCESS))
		{
		  if (usymtab_existsType (tname))
		    {
		      typeId uid = usymtab_getTypeId (tname);
		      uentry ue = usymtab_getTypeEntry (uid);

		      if (uentry_isAbstractDatatype (ue))
			{
			  context_addFileAccessType (uid);
			  DPRINTF (("Adding access to: %s / %d", tname, uid));
			}
		      else
			{
			  voptgenerror
			    (FLG_COMMENTERROR,
			     message
			     ("Non-abstract type %s used in access comment",
			      tname),
			     loc);
			}
		    }
		  else
		    {
		      if (!(context_inSuppressRegion ()
			    || context_inSuppressZone (loc)))
			{
			  voptgenerror
			    (FLG_COMMENTERROR,
			     message
			     ("Unrecognized type %s used in access comment",
			      tname),
			     loc);
			}
		    }
		}
	      
	      if (c != '\0') 
		{
		  s++;
		}
	      
	      if (c != ',' && c != ' ')
		{
		  break;
		}
	    }
	}
      else if (mstring_equal (t, "noaccess"))
	{
	  cstring tname;
	  char lc;
	  
	  while (TRUE)
	    {
	      while (((lc = *s) != '\0') && (lc == ' ' || lc == '\t' || lc == '\n')) 
		{
		  s++;
		}
	      
	      if (lc == '\0')
		{
		 break;
		}

	      tname = cstring_fromChars (s);
	      
	      while ((lc = *s) != '\0' && lc != ' ' && lc != '\t' 
		     && lc != '\n' && lc != ',') 
		{
		  s++;
		}

	      *s = '\0';

	      if (!context_getFlag (FLG_NOCOMMENTS) 
		  && !context_getFlag (FLG_NOACCESS))
		{
		  if (usymtab_existsType (tname))
		    {
		      typeId tuid = usymtab_getTypeId (tname);
		      
		      if (context_couldHaveAccess (tuid))
			{
			  DPRINTF (("Removing access: %s", tname));
			  context_removeFileAccessType (tuid);
			}
		      else
			{
			  if (!(context_inSuppressRegion () 
				|| context_inSuppressZone (loc)))
			    {
			      uentry ue = usymtab_getTypeEntry (tuid);
			      
			      if (uentry_isAbstractDatatype (ue))
				{
				  voptgenerror
				    (FLG_COMMENTERROR,
				     message
				     ("Non-accessible abstract type %s used in noaccess comment",
				      tname),
				     loc);
				}
			      else
				{
				  voptgenerror
				    (FLG_COMMENTERROR,
				     message
				     ("Non-abstract type %s used in noaccess comment",
				      tname),
				     loc);
				}
			    }
			}
		    }
		  else
		    {
		      if (!(context_inSuppressRegion () 
			    || context_inSuppressZone (loc)))
			{
			  voptgenerror
			    (FLG_COMMENTERROR,
			     message
			     ("Unrecognized type %s used in noaccess comment",
			      tname),
			     loc);
			}
		    }
		}
	      
	      if (lc != '\0') 
		{
		  s++;
		}
	      
	      if (lc != ',' && lc != ' ')
		{
		  break;
		}
	    }
	}
      else
	{
	  voptgenerror (FLG_UNRECOGCOMMENTS, 
			message ("Semantic comment unrecognized: %s", 
				 cstring_fromChars (os)),
			loc);
	  /*@-branchstate@*/
	} /* spurious (?) warning about t */
      /*@=branchstate@*/
      
      sfree (t);
    }
  
  sfree (os);
  fileloc_free (loc);
  return BADTOK;
}

/*@only@*/ cstring cscannerHelp_makeIdentifier (char *s)
{
  char *c = mstring_create (strlen (s) + 1);
  cstring id = cstring_fromChars (c);

  while (isalnum (*s) || (*s == '_') || (*s == '$')) 
    {
      *c++ = *s++;
    }

  *c = '\0';
  return (id);
}

/*@observer@*/ /*@dependent@*/ uentry cscannerHelp_coerceId (cstring cn)
{
  if (!(usymtab_exists (cn)))
    {
      fileloc loc = fileloc_createExternal ();
      
      /*
      ** We need to put this in a global scope, otherwise the sRef will be deallocated.
      */
      
      uentry ce = uentry_makeUnrecognized (cn, loc);
      
      if (!context_inIterEnd ())
	{
	  voptgenerror 
	    (FLG_SYSTEMUNRECOG, 
	     message ("Unrecognized (possibly system) identifier: %q", 
		      uentry_getName (ce)), 
	     g_currentloc);
	}
      
      return ce;
    }
  
  return (usymtab_lookup (cn));
}

/*
** like, cscannerHelp_coerceId, but doesn't supercede for iters
*/

/*@observer@*/ uentry cscannerHelp_coerceIterId (cstring cn)
{
  if (!(usymtab_exists (cn)))
    {
      return uentry_undefined;
    }
  
  return (usymtab_lookup (cn));
}

/*
** Need to keep this in case there is a declaration that isn't processed until
** the scope exits.  Would be good to rearrange the symbol table so this doesn't
** happen, and save all the cstring copying.
*/

/*@observer@*/ cstring cscannerHelp_observeLastIdentifier ()
{
  cstring res = s_lastidprocessed;
  return res;
}

static void cscanner_setLastIdentifier (/*@keep@*/ cstring id) /*@modifies s_lastidprocessed@*/
{
  if (cstring_isDefined (s_lastidprocessed))
    {
      cstring_free (s_lastidprocessed); 
    }

  s_lastidprocessed = id;
}

int cscannerHelp_processIdentifier (cstring id)
{
  uentry le;

  if (context_getFlag (FLG_GRAMMAR))
    {
      lldiagmsg (message ("Process identifier: %s", id));
    }

  context_clearJustPopped ();
  cscanner_setLastIdentifier (id);

  DPRINTF (("Context: %s", context_unparse ()));

  if (context_inFunctionHeader ())
    {
      int tok = commentMarkerToken (id);
      DPRINTF (("in function decl: %s", id));

      if (tok != BADTOK)
	{
	  return tok;
	}
      else 
	{
	  tok = tokenMacroCode (id);
	  
	  if (tok != BADTOK)
	    {
	      return tok;
	    }
	  else 
	    {
	      annotationInfo ainfo;

	      if (s_expectingMetaStateName) 
		{
		  metaStateInfo msinfo = context_lookupMetaStateInfo (id);

		  if (metaStateInfo_isDefined (msinfo))
		    {
		      yylval.msinfo = msinfo;
		      return METASTATE_NAME;
		    }
		  else
		    {
		      DPRINTF (("Not meta state name: %s", cstring_toCharsSafe (id)));
		    }
		}
	      
	      ainfo = context_lookupAnnotation (id);
	      
	      if (annotationInfo_isDefined (ainfo)) 
		{
		  DPRINTF (("Found annotation: %s", annotationInfo_unparse (ainfo)));
		  yylval.annotation = ainfo;
		  return CANNOTATION;
		}
	      else
		{
		  DPRINTF (("Not annotation: %s", id));
		}
	    }
	}
    }

  DPRINTF (("Here!"));

  /* Consider handling: Defined by C99 as static const char __func__[] */

  if (context_getFlag (FLG_GNUEXTENSIONS))
    {
      int tok = BADTOK;
      
      if (cstring_equalLit (id, "__stdcall")
	  || cstring_equalLit (id, "__cdecl")
	  || cstring_equalLit (id, "__extension__"))
	{
	  return BADTOK;
	}
      else if (cstring_equalLit (id, "__volatile__"))
	{
	  tok = QVOLATILE;
	}
      else if (cstring_equalLit (id, "__signed"))
	{
	  tok = QSIGNED;
	}
      else if (cstring_equalLit (id, "__unsigned"))
	{
	  tok = QUNSIGNED;
	}
      else if (cstring_equalLit (id, "__const__"))
	{
	  tok = QCONST;
	}
      else if (cstring_equalLit (id, "__alignof__")) 
	{
	  tok = CALIGNOF; /* alignof is parsed like sizeof */
	}
      else if (cstring_equalLit (id, "__typeof__")) 
	{
	  tok = CTYPEOF;
	}
      else if (cstring_equalLit (id, "typeof")) 
	{
	  tok = CTYPEOF;
	}
      else if (cstring_equalLit (id, "__FUNCTION__")
	       || cstring_equalLit (id, "__PRETTY_FUNCTION__")) 
	{
	  /* These tokens hold the name of the current function as strings */
	  /* evans 2001-12-30: changed from exprNode_stringLiteral; bug reported by Jim Zelenka. */
	  yylval.expr = exprNode_makeConstantString (id, fileloc_copy (g_currentloc));
	  s_tokLength = 0;
	  s_lastWasString = TRUE;
	  tok = CCONSTANT;
	  return tok;
	}
      else if (cstring_equalLit (id, "__attribute__")
	       || cstring_equalLit (id, "__asm__")
	       || cstring_equalLit (id, "_asm")
	       || cstring_equalLit (id, "__asm")
	       || cstring_equalLit (id, "__declspec"))
	{
	  int depth = 0;
	  bool useparens = FALSE;
	  bool usebraces = FALSE;
	  bool inquote = FALSE;
	  bool inescape = FALSE;
	  int ic;

	  while ((ic = cscanner_input ()) != EOF)
	    {
	      char cc = (char) ic;

	      if (inescape)
		{
		  inescape = FALSE;
		}
	      else if (cc == '\\')
		{
		  inescape = TRUE;
		}
	      else if (cc == '\"')
		{
		  inquote = !inquote;
		}
	      else if (!inquote)
		{
		  if (cc == '(')
		    {
		      if (!useparens)
			{
			  if (!usebraces)
			    {
			      useparens = TRUE;
			    }
			}

		      if (useparens)
			{
			  depth++;
			}
		    }
		  else if (cc == '{')
		    {
		      if (!usebraces)
			{
			  if (!useparens)
			    {
			      usebraces = TRUE;
			    }
			}

		      if (usebraces)
			{
			  depth++;
			}
		    }
		  else if (cc == ')' && useparens)
		    {
		      depth--;
		      if (depth == 0) break;
		    }
		  else if (cc == '}' && usebraces)
		    {
		      depth--;
		      if (depth == 0) break;
		    }
		  else if (cc == '}' 
			   && !usebraces && !useparens
			   && cstring_equalLit (id, "__asm"))
		    {
		      /*
		      ** We need this because some MS VC++ include files
		      ** have __asm mov ... }
		      ** Its a kludge, but otherwise would need to parse
		      ** the asm code!
		      */ 
		      return TRBRACE;
		    }
		  else
		    {
		      ;
		    }
		}
	      else
		{
		  ;
		}

	      if (cc == '\n')
		{
		  context_incLineno ();

		  if (cstring_equalLit (id, "__asm")
		      && !useparens && !usebraces)
		    {
		      break;
		    }
		}
	    }
	  
	  llassert ((useparens && ic == (int) ')')
		    || (usebraces && ic == (int) '}')
		    || (!useparens && !usebraces));

	  return BADTOK;
	}
      else if (cstring_equalLit (id, "inline")
	       || cstring_equalLit (id, "__inline")
	       || cstring_equalLit (id, "_inline")
	       || cstring_equalLit (id, "__inline__"))
	{
	  tok = QINLINE;
	}
      else
	{
	  ;
	}
      
      if (tok != BADTOK)
	{
	  return (cscannerHelp_returnToken (tok));
	}
    }

  le = usymtab_lookupSafe (id);

  /*@-dependenttrans@*/
  
  if (uentry_isIter (le))
    {
      yylval.entry = le;
      return (ITER_NAME);
    }
  else if (uentry_isEndIter (le))
    {
      yylval.entry = le;
      return (ITER_ENDNAME);
    }
  else if (uentry_isUndefined (le))
    {
      yylval.cname = cstring_copy (id);

      /* avoid parse errors for certain system built ins */

      if (s_expectingTypeName && (cstring_firstChar (id) == '_')
	  && (cstring_secondChar (id) == '_'))
	{
	  return (TYPE_NAME_OR_ID);
	}

      return (NEW_IDENTIFIER);
    }
  else if (!uentry_isDeclared (le) && !uentry_isCodeDefined (le))
    {
      if (uentry_isDatatype (le))
	{
	  yylval.cname = cstring_copy (id);
	  return (NEW_IDENTIFIER);
	}
      else
	{
	  yylval.entry = le;	  	  
	  return (IDENTIFIER); 
	}
    }
  else if (uentry_isDatatype (le))
    {
      if (!s_expectingTypeName)
	{
	  yylval.cname = cstring_copy (id);

	  return (NEW_IDENTIFIER);
	}
      else
	{
	  yylval.ctyp = uentry_getAbstractType (le);
	  
	  uentry_setUsed (le, g_currentloc);
	  return (TYPE_NAME);
	}
    }
  else
    {
      yylval.entry = le;            
      return (IDENTIFIER); 
    }

  /*@=dependenttrans@*/
}

bool cscannerHelp_processHashIdentifier (/*@only@*/ cstring id)
{
  if (context_inMacro () || context_inIterDef () ||
      context_inIterEnd ())
    {
      uentry le;
      
      context_clearJustPopped ();

      le = usymtab_lookupSafe (id);
      cscanner_setLastIdentifier (id);

      if (uentry_isParam (le) || uentry_isRefParam (le))
	{
	  return TRUE;
	}
      else
	{
	  return FALSE;
	}
    }
  else
    {
      /*
      ** Will be handled by handleLlSpecial
      */

      cstring_free (id);
      return FALSE;
    }
}

/*@only@*/ exprNode cscannerHelp_processString (void)
{
  exprNode res;
  fileloc loc;
  char *nl = strchr (yytext, '\n');
  cstring ns = cstring_fromCharsNew (yytext);

  if (nl == NULL)
    {
      loc = fileloc_copy (g_currentloc);
      addColumn (size_toInt (cstring_length (ns)));
    }
  else
    {
      char *lastnl = nl;

      loc = fileloc_copy (g_currentloc);

      context_incLineno ();
      
      while ((nl = strchr ((nl + 1), '\n')) != NULL)
	{
	  context_incLineno ();
	  lastnl = nl;
	}
    }

    
  res = exprNode_stringLiteral (ns, loc);
  return (res);
}

/*
** process a wide character string L"...."
*/

/*@only@*/ exprNode cscannerHelp_processWideString ()
{
  exprNode res;
  fileloc loc;
  char *nl = strchr (yytext, '\n');
  cstring ns;

  llassert (*yytext == 'L');
  yytext++;

  ns = cstring_fromCharsNew (yytext);
  
  if (nl == NULL)
    {
      loc = fileloc_copy (g_currentloc);
      addColumn (size_toInt (cstring_length (ns)));
    }
  else
    {
      char *lastnl = nl;

      loc = fileloc_copy (g_currentloc);

      context_incLineno ();
      
      while ((nl = strchr ((nl + 1), '\n')) != NULL)
	{
	  context_incLineno ();
	  lastnl = nl;
	}
    }

  res = exprNode_wideStringLiteral (ns, loc);
  return (res);
}

char cscannerHelp_processChar ()
{
  char fchar;
  char next;

  llassert (*yytext != '\0');
  fchar = *(yytext + 1);
  if (fchar != '\\') return fchar;
  
  next = *(yytext + 2);
  
  switch (next)
    {
    case 'n': return '\n';
    case 't': return '\t';
    case '\"': return '\"';
    case '\'': return '\'';
    case '\\': return '\\';
    default: return '\0';
    }
}

double cscannerHelp_processFloat ()
{
  double ret = atof (yytext);

    return (ret);
}

long cscannerHelp_processHex ()
{
  int index = 2;
  long val = 0;

  llassert (yytext[0] == '0'
	    && (yytext[1] == 'X' || yytext[1] == 'x'));

  while (yytext[index] != '\0') {
    int tval;
    char c = yytext[index];

    if (c >= '0' && c <= '9') {
      tval = (int) c - (int) '0';
    } else if (c >= 'A' && c <= 'F') {
      tval = (int) c - (int) 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
      tval = (int) c - (int) 'a' + 10;
    } else if (c == 'U' || c == 'L' || c == 'u' || c == 'l') {
      index++;
      while (yytext[index] != '\0') {
	if (c == 'U' || c == 'L' || c == 'u' || c == 'l') {
	  ;
	} else {
	  voptgenerror
	    (FLG_SYNTAX, 
	     message ("Invalid character (%c) following specifier in hex constant: %s",
		      c, cstring_fromChars (yytext)),
	     g_currentloc);
	}
	index++;
      }

      break;
    } else {
      voptgenerror
	(FLG_SYNTAX, 
	 message ("Invalid character (%c) in hex constant: %s",
		  c, cstring_fromChars (yytext)),
	 g_currentloc);
      break;
    }

    val = (val * 16) + tval;
    index++;
  }

  DPRINTF (("Hex constant: %s = %ld", yytext, val));
  return val;
}

long cscannerHelp_processOctal ()
{
  int index = 1;
  long val = 0;

  llassert (yytext[0] == '0' && yytext[1] != 'X' && yytext[1] != 'x');
    
  while (yytext[index] != '\0') {
    int tval;
    char c = yytext[index];
    
    if (c >= '0' && c <= '7') {
      tval = (int) c - (int) '0';
    } else if (c == 'U' || c == 'L' || c == 'u' || c == 'l') {
      index++;
      while (yytext[index] != '\0') {
	if (c == 'U' || c == 'L' || c == 'u' || c == 'l') {
	  ;
	} else {
	  voptgenerror
	    (FLG_SYNTAX, 
	     message ("Invalid character (%c) following specifier in octal constant: %s",
		      c, cstring_fromChars (yytext)),
	     g_currentloc);
	}
	index++;
      }

      break;
    } else {
      voptgenerror
	(FLG_SYNTAX, 
	 message ("Invalid character (%c) in octal constant: %s",
		  c, cstring_fromChars (yytext)),
	 g_currentloc);
      break;
    }

    val = (val * 8) + tval;
    index++;
  }

  DPRINTF (("Octal constant: %s = %ld", yytext, val));
  return val;
}

long cscannerHelp_processDec ()
{
  return (atol (yytext));
}

int cscannerHelp_processSpec (int tok)
{
  size_t length = strlen (yytext);
  
  if (s_inSpecPart)
    {

      /*drl 12/11/2002
	patched to fix assert failures in constraint code.
	Added the else if test so that splint does not treat MaxSet and MaxRead
	as identifies*/
	
      if (s_whichSpecPart == QMODIFIES
	  || s_whichSpecPart == QDEFINES
	  || s_whichSpecPart == QUSES 
	  || s_whichSpecPart == QALLOCATES
	  || s_whichSpecPart == QSETS
	  || s_whichSpecPart == QRELEASES)
	
	{
	  DPRINTF((message("Treating specifaction keyword %s as an identifiers.  (This corresponds to"
			   " token %d and we're in the  specification denoted by %d see cgrammar_tokens.h"
			   " for an explanation of these numbers",
			   yytext, tok, s_whichSpecPart)
		   ));
			   
	  ; /* Allow specificiation keywords to be used as identifiers in these contexts. */
	}
      else if ( (s_whichSpecPart == QPRECLAUSE
		 || s_whichSpecPart == QPOSTCLAUSE
		 || s_whichSpecPart == QINVARIANT )
		&&  (!cscannerHelp_isConstraintToken(tok) )
	      )
	{
	  DPRINTF((message("Treating specifaction keyword %s as an identifiers.  (This corresponds to"
			   " token %d and we're in the  specification denoted by %d see cgrammar_tokens.h"
			   " for an explanation of these numbers",
			   yytext, tok, s_whichSpecPart)
		   ));
	  
	  /* Allow specificiation keywords to be used as identifiers in these contexts. */
	}
      else
	{
	  cscannerHelp_setTokLengthT (length);
	  return cscannerHelp_returnToken (tok);
	}
    }
  
  context_saveLocation ();
  cscannerHelp_setTokLengthT (length);
  return (cscannerHelp_processIdentifier (cscannerHelp_makeIdentifier (yytext)));
}

void cscannerHelp_expectingMetaStateName ()
{
  llassert (!s_expectingMetaStateName);
  llassert (context_inFunctionHeader ());
  s_expectingMetaStateName = TRUE;
}

void cscannerHelp_clearExpectingMetaStateName ()
{
  llassert (s_expectingMetaStateName);
  s_expectingMetaStateName = FALSE;
}

bool cscannerHelp_isConstraintToken (int tok)
   /* drl added 12/11/2002
      Tell whether a token has special meaning
      within a function constraint
   */
{
  return (tok == QMAXSET || tok == QMAXREAD);
          /* || tok == QMINREAD || tok == QMINSET */
  /* uncomment the additional if statement tests when minSet and minRead are supported */
}

bool cscannerHelp_processMacro (void)
{
  uentry e2;
  ctype ct;
  int noparams = 0;
  cstring fname = cstring_undefined;
  bool res = TRUE;
  bool isspecfcn = FALSE;
  bool isiter = FALSE;
  bool skipparam = FALSE;
  bool isenditer = FALSE;
  bool unknownm = FALSE;
  bool hasParams = FALSE;
  bool emptyMacro = FALSE;
  char c = skip_whitespace ();
  fileloc loc = fileloc_noColumn (g_currentloc);

  /* are both of these necessary?  what do they mean? */
  uentryList specparams = uentryList_undefined;
  uentryList pn = uentryList_undefined;

  context_resetMacroMissingParams ();

  if (c == '\0' || c == '\n')
    {
      llcontbug (cstring_makeLiteral ("Bad macro"));
      fileloc_free (loc);
      return FALSE;
    }
  
  fname = cstring_appendChar (fname, c);  

  while ((c = macro_nextChar ()) != '(' && c != '\0'
	 && c != ' ' && c != '\t' && c != '\n')
    {
      fname = cstring_appendChar (fname, c);
    }

  if (c == ' ' || c == '\t' || c == '\n')
    {
      char oldc = c;

      if (c != '\n')
	{
	  while (c == ' ' || c == '\t')
	    {
	      c = macro_nextChar ();
	    }
	  cscanner_unput ((int) c);
	}

      if (c == '\n')
	{
	  emptyMacro = TRUE;
	  cscanner_unput ((int) c);
	}

      c = oldc;
    }

  hasParams = (c == '(');
  
  if (usymtab_exists (fname))
    {
      e2 = usymtab_lookupExpose (fname);
      ct = uentry_getType (e2);

      if (uentry_isCodeDefined (e2) 
	  && fileloc_isUser (uentry_whereDefined (e2)))
	{
	  if (optgenerror 
	      (FLG_MACROREDEF,
	       message ("Macro %s already defined", fname),
	       loc))
	    {
	      uentry_showWhereDefined (e2);
	      uentry_clearDefined (e2);
	    }

	  if (uentry_isFunction (e2))
	    {
	      uentry_setType (e2, ctype_unknown);
	      ct = ctype_unknown;
	      unknownm = TRUE;
	      context_enterUnknownMacro (e2); 
	    }
	  else
	    {
	      context_enterConstantMacro (e2);
	    }
	}
      else
	{
	  if (uentry_isForward (e2) && uentry_isFunction (e2))
	    {
	      unknownm = TRUE;

	      voptgenerror 
		(FLG_MACROFCNDECL,
		 message
		 ("Parameterized macro has no prototype or specification: %s ", 
		  fname),
		 loc);
	      
	      ct = ctype_unknown;
	      uentry_setType (e2, ctype_unknown);
	      uentry_setFunctionDefined (e2, loc); 
	      uentry_setUsed (e2, fileloc_undefined);
	      context_enterUnknownMacro (e2); 
	    }
	  else
	    {
	      if (uentry_isIter (e2))
		{
		  isiter = TRUE;
		  specparams = uentry_getParams (e2);
		  noparams = uentryList_size (specparams);
		  uentry_setDefined (e2, loc);
		  context_enterIterDef (e2); 
		}
	      else if (uentry_isEndIter (e2))
		{
		  isenditer = TRUE;
		  uentry_setDefined (e2, loc);
		  context_enterIterEnd (e2); /* don't care about it now */
		  /* but should parse like an iter! */
		}
	      else if (uentry_isConstant (e2))
		{
		  if (hasParams)
		    {
		      voptgenerror 
			(FLG_INCONDEFS, 
			 message ("Constant %s implemented as parameterized macro",
				  fname),
			 g_currentloc);
		      
		      uentry_showWhereSpecified (e2);
		      uentry_setType (e2, ctype_unknown);
		      uentry_makeConstantFunction (e2);
		      uentry_setDefined (e2, g_currentloc);
		      uentry_setFunctionDefined (e2, g_currentloc);
		      context_enterUnknownMacro (e2); 
		    }
		  else
		    {
		      if (!uentry_isSpecified (e2))
			{
			  fileloc oloc = uentry_whereDeclared (e2);

			  if (fileloc_isLib (oloc))
			    {
			      ;
			    }
			  else if (fileloc_isUndefined (oloc)
				   || fileloc_isPreproc (oloc))
			    {
			      if (!emptyMacro)
				{
				  voptgenerror
				    (FLG_MACROCONSTDECL,
				     message 
				     ("Macro constant %q not declared",
				      uentry_getName (e2)),
				     loc);			 
				}
			    }
			  else if (!fileloc_withinLines (oloc, loc, 2))
			    { /* bogus!  will give errors if there is too much whitespace */
			      voptgenerror
				(FLG_MACROCONSTDIST,
				 message 
				 ("Macro constant name %s matches name in "
				  "distant constant declaration.  This constant "
				  "is declared at %q", fname, 
				  fileloc_unparse (oloc)),
				 loc);
			    }
			  else
			    {
			      /* No warning */
			    }
			}

		      context_enterConstantMacro (e2);	      
		      cstring_free (fname);
		      fileloc_free (loc);
		      return res;
		    }

		}
	      else if (ctype_isFunction (ct))
		{
		  isspecfcn = TRUE;
		  specparams = ctype_argsFunction (ct);
		  noparams = uentryList_size (specparams);
		  
		  uentry_setFunctionDefined (e2, loc); 
		  context_enterMacro (e2);
		}
	      else if (uentry_isVar (e2))
		{
		  if (hasParams)
		    {
		      voptgenerror
			(FLG_INCONDEFS,
			 message ("Variable %s implemented as parameterized macro", 
				  fname),
			 loc);

		      uentry_showWhereSpecified (e2);
		      uentry_setType (e2, ctype_unknown);
		      uentry_makeVarFunction (e2);
		      uentry_setDefined (e2, g_currentloc);
		      uentry_setFunctionDefined (e2, g_currentloc);
		      context_enterUnknownMacro (e2); 
		    }
		  else
		    {
		      uentry ucons = uentry_makeConstant (fname,
							  ctype_unknown,
							  loc);
		      if (uentry_isExpandedMacro (e2))
			{
			  ; /* okay */
			}
		      else
			{
			  if (optgenerror 
			      (FLG_INCONDEFS,
			       message ("Variable %s implemented by a macro",
					fname),
			       loc))
			    {
			      uentry_showWhereSpecified (e2);
			    }
			}

		      uentry_setDefined (e2, loc);
		      uentry_setUsed (ucons, loc);

		      context_enterConstantMacro (ucons);
		      uentry_markOwned (ucons);
		      cstring_free (fname);
		      return res;
		    }
		}
	      else
		{
		  if (uentry_isDatatype (e2))
		    {
		      vgenhinterror 
			(FLG_SYNTAX,
			 message ("Type implemented as macro: %x", 
				  uentry_getName (e2)),
			 message ("A type is implemented using a macro definition.  A "
				  "typedef should be used instead."),
			 g_currentloc);

		      cscannerHelp_swallowMacro ();
		      /* Must exit scope (not sure why a new scope was entered?) */
		      usymtab_quietExitScope (g_currentloc);
		      uentry_setDefined (e2, g_currentloc);
		      res = FALSE;
		    }
		  else
		    {
		      llcontbug 
			(message ("Unexpanded macro not function or constant: %q", 
				  uentry_unparse (e2)));
		      uentry_setType (e2, ctype_unknown);
		      
		      if (hasParams)
			{
			  uentry_makeVarFunction (e2);
			  uentry_setDefined (e2, g_currentloc);
			  uentry_setFunctionDefined (e2, g_currentloc);
			  context_enterUnknownMacro (e2); 
			}
		    }
		}
	    }
	}
    }
  else
    {
      uentry ce;

      /* evans 2001-09-09 - if it has params, assume a function */
      if (hasParams)
	{
	  voptgenerror 
	    (FLG_MACROMATCHNAME,
	     message ("Unexpanded macro %s does not match name of a declared "
		      "function. The name used in the control "
		      "comment on the previous line should match.",
		      fname),
	     loc);
	  
	  ce = uentry_makeFunction (fname, ctype_unknown, 
				    typeId_invalid,
				    globSet_undefined,
				    sRefSet_undefined,
				    warnClause_undefined,
				    fileloc_undefined);      
	  uentry_setUsed (ce, loc); /* perhaps bogus? */
	  e2 = usymtab_supEntryReturn (ce);
	  context_enterUnknownMacro (e2);	      
	}
      else
	{
	  voptgenerror 
	    (FLG_MACROMATCHNAME,
	     message ("Unexpanded macro %s does not match name of a constant "
		      "or iter declaration.  The name used in the control "
		      "comment on the previous line should match.  "
		      "(Assuming macro defines a constant.)", 
		      fname),
	     loc);
	  
	  ce = uentry_makeConstant (fname, ctype_unknown, fileloc_undefined);      
	  uentry_setUsed (ce, loc); /* perhaps bogus? */
	  e2 = usymtab_supEntryReturn (ce);
	  
	  context_enterConstantMacro (e2);	      
	  cstring_free (fname);
	  fileloc_free (loc);
	  return res;
	}
    }
  
  /* in macros, ( must follow immediatetly after name */
  
  if (hasParams)
    {
      int paramno = 0;
      
      c = skip_whitespace ();

      while (c != ')' && c != '\0')
	{
	  uentry  param;
	  bool    suppress = context_inSuppressRegion ();
	  cstring paramname = cstring_undefined;

	  /*
	  ** save the parameter location
	  */

	  decColumn ();
	  context_saveLocation ();
	  incColumn ();

	  while (c != ' ' && c != '\t' && c != ',' && c != '\0' && c != ')')
	    {
	      paramname = cstring_appendChar (paramname, c);
	      c = macro_nextChar ();
	    }
	  
	  if (c == ' ' || c == '\t') c = skip_whitespace ();

	  if (c == ',')
	    {
	      c = macro_nextChar ();
	      if (c == ' ' || c == '\t') c = skip_whitespace ();
	    }
	  
	  if (c == '\0')
	    {
	      llfatalerror (cstring_makeLiteral
			    ("Bad macro syntax: uentryList"));
	    }
	  
	  if ((isspecfcn || isiter) && (paramno < noparams)
	      && !uentry_isElipsisMarker (uentryList_getN 
					  (specparams, paramno)))
	    {
	      fileloc sloc = context_getSaveLocation ();
	      uentry decl = uentryList_getN (specparams, paramno);
	      sRef sr;
	      
	      param = uentry_nameCopy (paramname, decl);
	      	      	      
	      uentry_setParam (param);
	      sr = sRef_makeParam (paramno, uentry_getType (param), 
				   stateInfo_makeLoc (sloc, SA_DECLARED));

	      if (sRef_getNullState (sr) == NS_ABSNULL)
		{
		  ctype pt = ctype_realType (uentry_getType (param));

		  if (ctype_isUser (pt))
		    {
		      uentry te = usymtab_getTypeEntrySafe (ctype_typeId (pt));
		      
		      if (uentry_isValid (te))
			{
			  sRef_setStateFromUentry (sr, te);
			}
		    }
		  else
		    {
		      sRef_setNullState (sr, NS_UNKNOWN, sloc);
		    }
		}

	      uentry_setSref (param, sr);
	      uentry_setDeclaredForceOnly (param, sloc);

	      skipparam = isiter && uentry_isOut (uentryList_getN (specparams, paramno));
	    }
	  else
	    {
	      fileloc sloc = context_getSaveLocation ();

	      param = uentry_makeVariableSrefParam 
		(paramname, ctype_unknown, fileloc_copy (sloc), 
		 sRef_makeParam (paramno, ctype_unknown,
				 stateInfo_makeLoc (sloc, SA_DECLARED)));
	      DPRINTF (("Unknown param: %s", uentry_unparseFull (param)));
	      cstring_free (paramname);

	      sRef_setPosNull  (uentry_getSref (param), sloc);
	      uentry_setDeclaredForce (param, sloc);

	      skipparam = FALSE;
	      fileloc_free (sloc);
	    }

	  if (!skipparam)
	    {
	      llassert (!uentry_isElipsisMarker (param));

	      if (!suppress)
		{
		  sRef_makeUnsafe (uentry_getSref (param));
		}
	      
	      pn = uentryList_add (pn, uentry_copy (param));
	      usymtab_supEntry (param);
	    }
	  else
	    {
	      /* don't add param */
	      uentry_free (param);
	    }

	  if (c == ',') 
	    {
	      (void) macro_nextChar ();
	      c = skip_whitespace ();
	    }

	  paramno++;
	}
      
      if (c == ')')
	{
	  if (isspecfcn || isiter)
	    {
	      if (paramno != noparams && noparams >= 0)
		{
		  cscannerHelp_advanceLine ();

		  voptgenerror 
		    (FLG_INCONDEFS,
		     message ("Macro %s specified with %d args, defined with %d", 
			      fname, noparams, paramno),
		     g_currentloc);

		  uentry_showWhereSpecified (e2);
		  uentry_resetParams (e2, pn);
		}
	    }
	  else
	    {
	      uentry_resetParams (e2, pn);
	    }
	}
    }
  else
    {
      /*
      ** the form should be:
      **
      ** # define newname oldname
      ** where oldname refers to a function matching the specification
      ** of newname.
      */

      if (unknownm)
	{
	  sRef_setGlobalScope ();
	  usymtab_supGlobalEntry (uentry_makeVariableLoc (fname, ctype_unknown));
	  sRef_clearGlobalScope ();
	}
      else
	{
	  context_setMacroMissingParams ();
	}
    }
  
  
  /* context_setuentryList (pn); */
  usymtab_enterScope ();

  fileloc_free (loc);
  cstring_free (fname);

  return res;
}

void cscannerHelp_setTokLength (int len) 
{
  addColumn (len);
  s_tokLength = len;
  DPRINTF (("Set tok length: %d", len));
}

void cscannerHelp_setTokLengthT (size_t len)
{
  cscannerHelp_setTokLength (size_toInt (len));
}

void cscannerHelp_advanceLine (void)
{
  s_tokLength = 0;
  beginLine ();
}

int cscannerHelp_returnToken (int t)
{
  if (s_tokLength > fileloc_column (g_currentloc)) {
    yylval.tok = lltok_create (t, fileloc_copy (g_currentloc));
  } else {
    yylval.tok = lltok_create (t, fileloc_decColumn (g_currentloc, s_tokLength)); 
  }

  s_tokLength = 0; 
  s_lastWasString = FALSE; 
  return (t); 
}

int cscannerHelp_returnTokenLength (int t, int length)
{
  cscannerHelp_setTokLength (length);
  return cscannerHelp_returnToken (t);
}

int cscannerHelp_returnString (cstring s)
{
  yylval.expr = exprNode_stringLiteral (s, fileloc_decColumn (g_currentloc, s_tokLength)); 
  s_tokLength = 0;
  s_lastWasString = TRUE; 
  return (CCONSTANT); 
}

int cscannerHelp_returnInt (ctype ct, long val)
{
  ctype c = ct;

  if (ctype_equal (ct, ctype_int))
    {
      if (val == 0)
	{
	  c = context_typeofZero ();
	}
      else if (val == 1)
	{
	  c = context_typeofOne ();
	}
      else
	{
	  ;
	}
    }
  
  yylval.expr = exprNode_numLiteral (c, cstring_fromChars (yytext), 
				     fileloc_decColumn (g_currentloc, s_tokLength),
				     val);   
  s_tokLength = 0; 
  s_lastWasString = FALSE;
  return (CCONSTANT);
}

int cscannerHelp_returnFloat (ctype ct, double f)
{
  yylval.expr = exprNode_floatLiteral (f, ct, cstring_fromChars (yytext), 
				       fileloc_decColumn (g_currentloc, s_tokLength));
  s_tokLength = 0; 
  s_lastWasString = FALSE;
  return (CCONSTANT);
}

int cscannerHelp_returnChar (char c)
{
  yylval.expr = exprNode_charLiteral (c, cstring_fromChars (yytext), 
				      fileloc_decColumn (g_currentloc, s_tokLength));
  s_tokLength = 0; 
  s_lastWasString = FALSE;
  return (CCONSTANT);
}

int cscannerHelp_returnType (int tok, ctype ct) 
{
  yylval.ctyp = ct; 
  s_tokLength = 0; 
  s_lastWasString = FALSE;
  return tok;
}

int cscannerHelp_returnExpr (exprNode e)
{
  yylval.expr = e; 
  s_tokLength = 0; 
  s_lastWasString = TRUE; 
  return (CCONSTANT); 
}

void cscannerHelp_setExpectingTypeName ()
{
  s_expectingTypeName = TRUE;
}

void cscannerHelp_clearExpectingTypeName ()
{
  s_expectingTypeName = FALSE;
}

bool cscannerHelp_isExpectingTypeName ()
{
  return s_expectingTypeName;
}

int cscannerHelp_processTextIdentifier (char *text)
{
  context_saveLocation (); 
  cscannerHelp_setTokLength (size_toInt (mstring_length (text))); 
  return cscannerHelp_processIdentifier (cscannerHelp_makeIdentifier (text)); 
}

static bool s_continueLine = FALSE;

int cscannerHelp_handleNewLine ()
{
  context_incLineno ();

  if (s_tokLength != 0) { 
    s_tokLength = 0; 
    /* No error to report 
       voptgenerror
       (FLG_SYNTAX, 
       message ("Likely parse error: token spans multiple lines."),
       g_currentloc);
    */
  }
  
  if (s_continueLine)
    {
      s_continueLine = FALSE;
    }
  else 
    {
      if (context_inMacro ())
	{
	  /* Don't use return cscannerHelp_returnToken */
	  /* !!! evans 2002-03-13 */
	  yylval.tok = lltok_create (TENDMACRO, fileloc_copy (g_currentloc)); 
	  s_lastWasString = FALSE;
	  return TENDMACRO;
	}  
    }

  return BADTOK;
}

void cscannerHelp_setContinueLine ()
{
  s_continueLine = TRUE;
}

void cscannerHelp_exitSpecPart ()
{
  llassert (s_inSpecPart);
  s_inSpecPart = FALSE;
  s_whichSpecPart = BADTOK;
}
