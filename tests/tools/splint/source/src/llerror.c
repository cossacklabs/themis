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
** llerror.c
**
** error reporting procedures
*/

# include "splintMacros.nf"
# include <string.h>
# include <errno.h>
# include "basic.h"
# include "llmain.h"
# include "cpperror.h"
# include "Headers/version.h" /* Visual C++ finds a different version.h on some path! */

/* Don't allow possibly-recursive assertion failures. */
# undef llassert
# define llassert llassertprotect

static void printIndentMessage (FILE *p_stream, /*@only@*/ cstring p_sc, int p_indent)
   /*@modifies *p_stream@*/ ;

static bool s_scanOpen = FALSE;
static int s_lclerrors = 0;
static size_t s_lastfileloclen = 10;
static /*@only@*/ cstring s_lastmsg = cstring_undefined;
static int s_mcount = 0;
static /*@only@*/ cstring saveOneMessage = cstring_undefined;
static /*@only@*/ fileloc lastparseerror = fileloc_undefined;
static /*@only@*/ fileloc lastbug = fileloc_undefined;
static bool llgenerrorreal (flagcode p_code, 
			    char *p_srcFile, int p_srcLine, 
			    /*@only@*/ cstring p_s,
			    /*@temp@*/ cstring p_addtext,
			    fileloc p_fl, bool p_iserror, bool p_indent)
                 /*@modifies g_warningstream@*/ ;
static bool llgenerroraux (flagcode p_code, char *p_srcFile, int p_srcLine, 
			   /*@only@*/ cstring p_s,
			   /*@temp@*/ cstring p_addtext,
			   fileloc p_fl, bool p_iserror, bool p_indent)
                 /*@modifies g_warningstream@*/ ;

static void generateCSV (flagcode p_code, cstring p_s, cstring p_addtext, fileloc p_fl) 
     /*@modifies g_csvstream@*/ ;

static void printError (FILE *p_stream, /*@only@*/ cstring p_sc)
   /*@globals s_lastfileloclen @*/
   /*@modifies *p_stream@*/ ;
static void printMessage (FILE *p_stream, /*@only@*/ cstring p_s)
   /*@modifies *p_stream@*/ ;

static void llgenhint (/*@only@*/ cstring p_s) /*@modifies g_warningstream@*/ ;

static void showSourceLoc (char *srcFile, int srcLine)
     /*@modifies g_warningstream@*/
{
  if (context_getFlag (FLG_SHOWSOURCELOC)) {
    llgenhint (message ("%s:%d: Source code error generation point.",
			cstring_fromChars (srcFile), srcLine));
  }
  
}

static /*@null@*/ char *
maxcp (/*@null@*/ /*@returned@*/ char *a, /*@null@*/ /*@returned@*/ char *b)
{
  if (a > b) return a;
  else return b;
}

static void
printBugReport (void)
{
  fprintf (g_errorstream, "     *** Please report bug to %s ***\n",
	   SPLINT_MAINTAINER);
  llflush ();
  /* don't exit (EXIT_FAILURE); */
}

static bool s_needsPrepare = TRUE;

void prepareMessage ()
{
  DPRINTF (("Prepare message: %s", bool_unparse (context_loadingLibrary ())));
  showHerald ();

  if ((context_isPreprocessing () || context_loadingLibrary ())
      && s_needsPrepare
      && context_getFlag (FLG_SHOWSCAN))
    {
      llflush ();
      displayScanClose ();
      s_needsPrepare = FALSE;
    }

  llflush ();
}

void closeMessage (void)
{
  if (context_isPreprocessing ()
      && context_getFlag (FLG_SHOWSCAN))
    {
      llflush ();
      displayScanOpen (cstring_makeLiteral ("more preprocessing ."));
      llassertprotect (!s_needsPrepare);
      s_needsPrepare = TRUE;
    }
  else
    {
      llflush ();
    }
}

void
llmsg (/*@only@*/ cstring s)
{
  context_setNeednl ();
  prepareMessage ();
  printMessage (g_messagestream, s);
  closeMessage ();
}

void
lldiagmsg (/*@only@*/ cstring s)
{
  static bool inmsg = FALSE;

  if (inmsg)
    {
      fprintf (g_errorstream,
	       "Recursive message call detected: %s\n", 
	       cstring_toCharsSafe (s));
      llexit (LLFAILURE);
    }

  inmsg = TRUE;

  context_setNeednl ();
  prepareMessage ();
  printMessage (g_messagestream, s);
  closeMessage ();

  inmsg = FALSE;
}

void
llmsgplain (/*@only@*/ cstring s)
{
  context_setNeednl ();
  prepareMessage ();
  printMessage (g_messagestream, s);
  closeMessage ();
}

void llerror_flagWarning (cstring s)
{
  if (context_getFlag (FLG_WARNFLAGS))
    {
      llgenmsg (s, g_currentloc);
    }
  else
    {
      cstring_free (s);
    }
}

static void
llgenhint (/*@only@*/ cstring s) /*@modifies g_warningstream@*/
{
  int indent = context_getIndentSpaces () - 1;

  if (indent < 0) indent = 0;

  context_setNeednl ();
  printIndentMessage (g_warningstream, s, indent);
}

void
llhint (cstring s)
{
  if (context_getFlag (FLG_HINTS) &&
      !(context_inSuppressRegion () || context_inSuppressZone (g_currentloc)))
    {
      llgenhint (s);
    }
  else
    {
      cstring_free (s);
    }
}

static void
llshowhint (flagcode f)
{
  if (context_getFlag (FLG_HINTS))
    {
      if ((flagcode_numReported (f) == 0) || context_getFlag (FLG_FORCEHINTS))
	{
	  cstring desc = flagcodeHint (f);

	  if (cstring_isDefined (desc))
	    {
	      llgenhint (cstring_copy (desc));
	    }
	}
    }
}

static void
llsuppresshint2 (char c, flagcode f1, flagcode f2)
{

  if (context_getFlag (FLG_HINTS))
    {
      if ((flagcode_numReported (f1) == 0
	   || flagcode_numReported (f2) == 0)
	  || context_getFlag (FLG_FORCEHINTS))
	{
	  cstring desc = flagcodeHint (f1);
	  context_setNeednl ();
	  s_lastfileloclen = 8;

	  if (cstring_isUndefined (desc))
	    {
	      desc = flagcodeHint (f2);
	    }

	  if (flagcode_isNamePrefixFlag (f1))
	    {
	      f1 = FLG_NAMECHECKS;
	    }

	  if (flagcode_isNamePrefixFlag (f2))
	    {
	      f2 = FLG_NAMECHECKS;
	    }

	  if (f1 == f2)
	    {
	      if (cstring_isDefined (desc))
		{
		  llgenhint (message ("%s (Use %h%s to inhibit warning)", desc,
				      c,
				      flagcode_unparse (f1)));
		}
	      else
		{
		  llgenhint (message ("(Use %h%s to inhibit warning)", 
				      c, flagcode_unparse (f1)));
		}
	    }
	  else
	    {
	      if (cstring_isDefined (desc))
		{
		  llgenhint (message ("%s (Use either %h%s or %h%s to inhibit warning)", desc, 
				      c,
				      flagcode_unparse (f1),
				      c,
				      flagcode_unparse (f2)));
		}
	      else
		{
		  llgenhint (message ("(Use either %h%s or %h%s to inhibit warning)", c,
				      flagcode_unparse (f1),
				      c, flagcode_unparse (f2)));
		}
	    }
	}
    }
}

static void
llsuppresshint (char c, flagcode f)
{
  if (context_getFlag (FLG_HINTS))
    {
      if ((flagcode_numReported (f) == 0) || context_getFlag (FLG_FORCEHINTS))
	{
	  cstring desc = flagcodeHint (f);
	  context_setNeednl ();
	  s_lastfileloclen = 8;

	  if (flagcode_isNamePrefixFlag (f))
	    {
	      f = FLG_NAMECHECKS;
	    }

	  if (cstring_isDefined (desc))
	    {
	      llgenhint (message ("%s (Use %h%s to inhibit warning)", desc, c,
				  flagcode_unparse (f)));
	    }
	  else
	    {
	      llgenhint (message ("(Use %h%s to inhibit warning)", c,
				  flagcode_unparse (f)));
	    }
	}
    }
}

static void
llnosuppresshint (flagcode f)
{
  if (context_getFlag (FLG_FORCEHINTS))
    {
      cstring desc = flagcodeHint (f);
      context_setNeednl ();
      s_lastfileloclen = 8;

      if (cstring_isDefined (desc))
	{
	  printError (g_warningstream, message ("    %s", desc));
	}
    }
}

/*@constant int MAXSEARCH; @*/
# define MAXSEARCH 20

/*@constant int MINLINE; @*/
# define MINLINE 35

typedef /*@null@*/ /*@dependent@*/ char *nd_charp;

/*
** mstring_split
**
** Divides a string into lines of up to maxline characters.
**
** Initial string: *sp
**
** Output split: *sp / *tp
**                     possibly null
*/

static void
mstring_split (/*@returned@*/ char **sp,
	       /*@out@*/ nd_charp *tp,
	       int maxline, /*@in@*/ int *indentchars)
{
  char *nl;
  char *t;
  char *s = *sp;
  char *osp = *sp;

  *tp = NULL;

  DPRINTF (("Split: %s / %d", *sp, maxline));

  if (maxline < MINLINELEN)
    {
      maxline = MINLINELEN;
    }

  if (*indentchars > 0)
    {
      s = *sp = mstring_concatFree1 (mstring_spaces (*indentchars), s);
      osp = s;
    }

  /*
  ** splitting:
  **
  **    if there is a newline in first maxline characters, split there
  **    if line len is <= maxline, return no split
  **    if there is a ':' or ';' or ',' followed by ' ' in first maxline characters,
  **       split there unless the ' ' is followed by a '}', then
  **       split after '}'
  **       of the ';' is inside quotation marks
  **    if there is a space or tab in last maxsearch characters, split there
  **    else, split at maxline
  **
  **    special code: slash [1-9] after a newline means indent the rest <n> chars
  **
  */

  nl = strchr (s, '\n');

  if ((nl != NULL) && ((nl - s) < maxline))
    {
      *nl = '\0';
      t = nl + 1;

      if (*t == '\0')
	{
	  llassertprotect (*tp == NULL || (*tp > osp));
	  return;
	}

      if (*t >= '\1' && *t <= '\7')
	{
	  *indentchars += (int) (*t - '\1') + 1;
	  t++;
	}
      
      *tp = t;
      return;
    }
  else if (size_toInt (strlen (s)) < maxline)
    {
      llassertprotect (*tp == NULL);
      return;
    }
  else
    {
      int i = 0;
      char savechar;
      char *lcolon, *lsemi, *lcomma;
      char *splitat;
      
      splitat = NULL;

      t = s + maxline - 1;
      savechar = *t;

      *t = '\0';
      lcolon = strrchr (s, ':');
      lsemi = strrchr (s, ';');
      lcomma = strrchr (s, ',');

      *t = savechar;

      splitat = maxcp (lcolon, lsemi);

      if (splitat != NULL && ((int)(splitat - s) > MINLINE)
	  && *(splitat) != '\0'
	  && *(splitat + 1) == ' ' 
	  && (*(splitat + 2) != '}'
	      && *(splitat + 2) != ',' 
	      && (*(splitat + 2) != '\0'))) 
	{
	  *(splitat + 1) = '\0';
	  t = splitat + 2;
	  *tp = t;
	  llassertprotect (*tp == NULL || (*tp > osp));
	  return;
	}

      if (lcomma != NULL && ((lcomma - s) > maxline - 5))
	{
	  splitat = lcomma;
	  
	  if (splitat != NULL && ((int)(splitat - s) > MINLINE)
	      && *(splitat) != '\0'
	      && *(splitat + 1) == ' ' 
	      && (*(splitat + 2) != '}'
		  && (*(splitat + 2) != '\0'))) 
	    {
	      *(splitat + 1) = '\0';
	      t = splitat + 2;
	      *tp = t;
	      llassertprotect (*tp == NULL || (*tp > osp));
	      return;
	    }
	}
      
      /*
      ** Search for any breaking point (at least 4 letters past s)
      */

      while (*t != ' ' && *t != '\t' && i < MAXSEARCH && t > (s + 4))
	{
	  t--;
	  i++;
	}

      if (*t != ' ' && *t != '\t')
	{
	  llassertprotect (maxline > 0);
	  t = mstring_copy (s + maxline);
	  *(s + maxline) = '\0';

	  if (*t == '\0')
	    {
	      sfree (t);
	      llassertprotect (*tp == NULL || (*tp > osp));
	      return;
	    }

	  mstring_markFree (t);
	  *tp = t;
	  /* Won't be true since t is a copy: llassertprotect (*tp == NULL || (*tp > osp)); */
	  return;
	}
      else
	{

	  *t = '\0';
	  t++;
	  
	  if (*t == '\0') return;

	  /*
	  ** returns unqualified as only
	  */

	  *tp = t;

	  llassert (*sp != *tp);
	  return;
	}
    }

  BADBRANCH;
}

static
void limitmessage (/*@only@*/ cstring s, fileloc loc)
{
  if (s_mcount > context_getLimit () + 1)
    {
      cstring_free (s);
    }
  else
    {
      cstring flstring = fileloc_unparse (loc);

      s_lastfileloclen = cstring_length (flstring);
      cstring_free (saveOneMessage);
      saveOneMessage = message ("%q: %q", flstring, s);
    }
}

static int parseerrorcount = 0;

void cleanupMessages ()
{
  parseerrorcount = 0;

  if (context_unlimitedMessages ())
    {
     ;
    }
  else
    {
      int unprinted = s_mcount - context_getLimit ();

      if (unprinted > 0)
	{
	  if (unprinted == 1 && cstring_isDefined (saveOneMessage))
	    {
	      prepareMessage ();
	      printError (g_warningstream, saveOneMessage);
	      closeMessage ();
	      saveOneMessage = cstring_undefined;
	    }
	  else
	    {
	      if (cstring_isDefined (saveOneMessage))
		{
		  /* cstring_free (saveOneMessage); */
		  saveOneMessage = cstring_undefined;
		}

	      fprintf (g_warningstream, "%s: (%d more similar errors unprinted)\n",
		       cstring_toCharsSafe (fileloc_filename (g_currentloc)),
		       s_mcount - context_getLimit ());
	    }
	}
    }

  s_mcount = 0;
}

void
llgenmsg (/*@only@*/ cstring s, fileloc fl)
{
  cstring flstring = fileloc_unparse (fl);
  s_lastfileloclen = cstring_length (flstring);

  prepareMessage ();
  (void) printError (g_warningstream, message ("%q: %q", flstring, s));
  closeMessage ();
}

void
llgenindentmsg (/*@only@*/ cstring s, fileloc fl)
{
  cstring flstring = fileloc_unparse (fl);
  int indentspaces = context_getLocIndentSpaces ();
  prepareMessage ();
  
  (void) printIndentMessage (g_warningstream, message ("%q: %q", flstring, s),
			     indentspaces);

  closeMessage ();
}

void
llgenindentmsgnoloc (/*@only@*/ cstring s)
{
  prepareMessage ();
  (void) printIndentMessage (g_warningstream, s, context_getIndentSpaces ());
  closeMessage ();
}

static bool
llgentypeerroraux (char *srcFile, int srcLine,
		   flagcode ocode, ctype t1, exprNode e1, ctype t2, exprNode e2,
		   /*@only@*/ cstring s, fileloc fl)
{
  cstring hint = cstring_undefined;
  flagcode code = ocode;
  flagcode hcode = INVALID_FLAG;
  ctype ut1 = t1;
  ctype ut2 = t2;

  DPRINTF (("Type error [%s]: %s / %s : %s / %s",
	    flagcode_unparse (ocode),
	    exprNode_unparse (e1), exprNode_unparse (e2),
	    ctype_unparse (t1), ctype_unparse (t2)));
  
  DPRINTF (("Bool: %s / %s",
	    bool_unparse (ctype_isBool (t1)),
	    bool_unparse (ctype_isBool (t2))));

  /* 
  ** Set the flag using the underlying types that didn't match.
  */

  while (ctype_isPointer (ut1) && ctype_isPointer (ut2)) {
    ut1 = ctype_baseArrayPtr (ut1);
    ut2 = ctype_baseArrayPtr (ut2);
  }

  if (ctype_isRealNumAbstract (ut1) && exprNode_isNumLiteral (e2)) 
    {
      hcode = FLG_NUMABSTRACTLIT;
    }
  else if ((ctype_isFloat (ut1) && ctype_isDouble (ut2))
	   || (ctype_isFloat (ut1) && ctype_isDouble (ut2)))
    {
      hcode = FLG_FLOATDOUBLE;
    }
  else if ((exprNode_isCharLiteral (e1) && ctype_isInt (ut2))
	   || (exprNode_isCharLiteral (e2) && ctype_isInt (ut1)))
    {
      hcode = FLG_CHARINTLITERAL;
    }
  else if ((exprNode_isNumLiteral (e1) && ctype_isReal (ut2))
	   || (exprNode_isNumLiteral (e2) && ctype_isReal (ut1)))
    {
      hcode = FLG_NUMLITERAL;
    }
  else if ((ctype_isManifestBool (ut1) && ctype_isInt (ut2))
	   || (ctype_isInt (ut1) && ctype_isManifestBool (ut2)))
    /* evs 2000-07-24: was ctype_isDirectBool */
    {
      hcode = FLG_BOOLINT;
    }
  else if (((ctype_isChar (ut1) && !ctype_isInt (ut1)) && ctype_isInt (ut2))
	   || ((ctype_isInt (ut1) && (ctype_isChar (ut2) && !ctype_isInt (ut2)))))
    {
      hcode = FLG_CHARINT;
    }
  else if ((ctype_isInt (ut1) && ctype_isInt (ut2))
	   || (ctype_isChar (ut1) && ctype_isChar (ut2))
	   || (ctype_isDouble (ut1) && ctype_isDouble (ut2)))
    {
      if (!bool_equal (ctype_isSigned (ut1), ctype_isSigned (ut2)))
	{
	  if (ctype_isArbitraryIntegral (ctype_realType (ut1))
	      && !ctype_isArbitraryIntegral (ctype_realType (ut2)))
	    {
	      hcode = FLG_MATCHANYINTEGRAL;
	    }
	  else if (ctype_isArbitraryIntegral (ctype_realType (ut2))
		   && !ctype_isArbitraryIntegral (ctype_realType (ut1)))
	    {
	      hcode = FLG_MATCHANYINTEGRAL;
	    }
	  else
	    /*drl 4-270-2003 even if ignoresigns is set there may be another
	      problem that is causing splint to complain about a type error.
	      Don't tell the user that they can add +ignoresigns if it's
	      already on*/
	    {
	      DPRINTF(("TEST INGORESIGNS"));
	      if (context_getFlag(FLG_IGNORESIGNS) )
		{
		  DPRINTF(("INGORESIGNS SET"));
		  hcode = FLG_IGNOREQUALS;
		}
	    
	      else
		{
		  DPRINTF(("INGORESIGNS NOT SET"));
		  hcode = FLG_IGNORESIGNS;
		}
	    }
	}
      else
	{
	  hcode = FLG_IGNOREQUALS;
	}
    }
  else if (ctype_isArbitraryIntegral (ctype_realType (ut1)))
    {
      DPRINTF (("HERE: %s", ctype_unparse (ctype_realType (ut2))));

      if (ctype_isArbitraryIntegral (ctype_realType (ut2)))
	{
	  hcode = FLG_MATCHANYINTEGRAL;
	}
      else if (ctype_equal (ut2, ctype_ulint))
	{
	  hcode = FLG_LONGUNSIGNEDINTEGRAL;
	}
      else if (ctype_equal (ut2, ctype_lint))
	{
	  hcode = FLG_LONGINTEGRAL;
	}
      else if (ctype_isInt (ut2))
	{
	  hcode = FLG_MATCHANYINTEGRAL;
	}	  
      else
	{
	  hcode = FLG_TYPE;
	}
    }
  else if (ctype_isArbitraryIntegral (ctype_realType (ut2)))
    {
      ctype tr = ctype_realType (ut1);

      if (ctype_isArbitraryIntegral (tr))
	{
	  hcode = FLG_MATCHANYINTEGRAL;
	}
      else if (ctype_match (ut1, ctype_ulint))
	{
	  if (ctype_isUnsignedIntegral (tr))
	    {
	      hcode = FLG_LONGUNSIGNEDUNSIGNEDINTEGRAL;
	    }
	  else if (ctype_isSignedIntegral (tr))
	    {
	      ;
	    }
	  else
	    {
	      hcode = FLG_LONGUNSIGNEDINTEGRAL;
	    }
	}
      else if (ctype_match (ut1, ctype_lint))
	{
	  if (ctype_isSignedIntegral (tr))
	    {
	      hcode = FLG_LONGSIGNEDINTEGRAL;
	    }
	  else if (ctype_isSignedIntegral (tr))
	    {
	      ;
	    }
	  else
	    {
	      hcode = FLG_LONGINTEGRAL;
	    }
	}
      else if (ctype_isInt (ut1))
	{
	  hcode = FLG_MATCHANYINTEGRAL;
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

  if (hcode == INVALID_FLAG)
    {
      DPRINTF (("[%s] %s - %s / %s",
		ctype_unparse (ut1),
		bool_unparse (ctype_isEnum (ut1)),
		bool_unparse (ctype_isEnum (ctype_realType (ut1))),
		bool_unparse (ctype_isInt (ut2))));

      if (ctype_isAbstract (ut1) && !ctype_isAbstract (ut2))
	{
	  uentry ue1 = usymtab_getTypeEntry (ctype_typeId (ut1));
	  ctype ct = uentry_getType (ue1);
	  
	  if (ctype_match (ct, ut2))
	    {
	      code = FLG_ABSTRACT;
	      hint = message ("Underlying types match, but %s is an "
			      "abstract type that is not accessible here.",
			      ctype_unparse (t1));
	    }
	}
      else if (ctype_isAbstract (ut2) && !ctype_isAbstract (ut1))
	{
	  uentry ue = usymtab_getTypeEntry (ctype_typeId (ut2));
	  ctype ct = uentry_getType (ue);
	  
	  if (ctype_match (ct, ut1))
	    {
	      if (ctype_isNumAbstract (ut2)) 
		{
		  if (exprNode_isNumLiteral (e1))
		    {
		      code = FLG_NUMABSTRACTLIT;
		      hint = message ("Underlying types match, but %s is a "
				      "numabstract type that is not accessible here. "
				      "(Use +numabstractlit to allow numeric literals "
				      "to be used as numabstract type values.)",
				      ctype_unparse (t2));
		    }
		  else
		    {
		      code = FLG_NUMABSTRACT;
		      hint = message ("Underlying types match, but %s is a "
				      "numabstract type that is not accessible here.",
				      ctype_unparse (t2));
		    }
		}
	      else
		{
		  code = FLG_ABSTRACT;
		  hint = message ("Underlying types match, but %s is an "
				  "abstract type that is not accessible here.",
				  ctype_unparse (t2));
		}
	    }
	}
      else
	{
	  ; /* Not an abstract mismatch. */
	}
				       

      if (hcode == INVALID_FLAG)
	{
	  if ((ctype_isEnum (ut1) && ctype_isInt (ut2))
	      || (ctype_isEnum (ut2) && ctype_isInt (ut1))) 
	    {
	      hcode = FLG_ENUMINT;
	    }
	  else if ((ctype_isEnum (ut1) && ctype_isInt (ut2))
		   || (ctype_isEnum (ut2) && ctype_isInt (ut1))) 
	    {
	      hcode = FLG_ENUMINT;
	    }
	  else if ((ctype_isSignedChar (ut1) && ctype_isUnsignedChar (ut2))
		   || (ctype_isUnsignedChar (ut1) && ctype_isSignedChar (ut2)))
	    {
	      hcode = FLG_CHARUNSIGNEDCHAR;
	    }
	  else if (ctype_isNumeric (ut1) && ctype_isNumeric (ut2)) 
	    {
	      hcode = FLG_RELAXTYPES;
	      DPRINTF (("Setting relax types!"));
	    }
	  else
	    {
	      DPRINTF (("No special type rule: %s / %s", ctype_unparse (ut1),
			ctype_unparse (ut2)));
	    }
	}
    }

  if (cstring_isDefined (hint))
    {
      if (!context_suppressFlagMsg (ocode, fl))
	{
	  return xllgenhinterror (srcFile, srcLine, code, s, hint, fl);
	}
      else
	{
	  cstring_free (s);
	  cstring_free (hint);
	  return FALSE;
	}
    }
  else
    {
      if (hcode != INVALID_FLAG && hcode != ocode)
	{
	  code = hcode;
	}
      
      if (llgenerroraux (ocode, srcFile, srcLine, s,
			 flagcodeHint (code), fl, TRUE, FALSE))
	{
	  if (code != ocode) 
	    {
	      if (context_flagOn (code, fl))
		{
		  /* The flag is alreay set, something buggy in the flag code */
		  llcontbug (message ("No hint available, flag %s is already set.",
				      flagcode_unparse (code)));
		}
	      else
		{
		  llshowhint (code);
		}
	  } 
	  else
	    {
	      llsuppresshint ('-', code);
	    }

	  flagcode_recordError (code);
	  return TRUE;
	}

      return FALSE;
    }
}

bool
xllgentypeerror (char *srcFile, int srcLine,
		 ctype t1, exprNode e1, ctype t2, exprNode e2,
		 /*@only@*/ cstring s, fileloc fl)
{
  return llgentypeerroraux (srcFile, srcLine, FLG_TYPE, t1, e1, t2, e2, s, fl);
}

bool
xllgenformattypeerror (char *srcFile, int srcLine,
		       ctype t1, exprNode e1, ctype t2, exprNode e2,
		       /*@only@*/ cstring s, fileloc fl)
{
  if (!context_suppressFlagMsg (FLG_FORMATTYPE, fl))
    {
      if (ctype_isInt (t1)
	  && ctype_isNumAbstract (t2))
	{
	  if (!context_suppressFlagMsg (FLG_NUMABSTRACTPRINT, fl))
	    {
	      return llgentypeerroraux (srcFile, srcLine, FLG_NUMABSTRACTPRINT, t1, e1, t2, e2, s, fl);
	    }
	  else
	    {
	      return FALSE;
	    }
	}
      else
	{
	  return llgentypeerroraux (srcFile, srcLine, FLG_FORMATTYPE, t1, e1, t2, e2, s, fl);
	}
    }
  else
    {
      cstring_free (s);
      return FALSE;
    }
}

bool
xllgenerror (char *srcFile, int srcLine, flagcode o, /*@only@*/ cstring s, fileloc fl)
{
  if (llgenerroraux (o, srcFile, srcLine, s, flagcodeHint (o), fl, TRUE, FALSE))
    {
      llnosuppresshint (o);
      flagcode_recordError (o);
      closeMessage ();
      return TRUE;
    }
  else
    {
      flagcode_recordSuppressed (o);
      return FALSE;
    }
}

bool
xllgenhinterror (char *srcFile, int srcLine,
		 flagcode o, /*@only@*/ cstring s, /*@only@*/ cstring hint,
		 fileloc fl)
{
  if (!context_suppressFlagMsg (o, fl))
    {
      if (llgenerroraux (o, srcFile, srcLine, s, hint, fl, TRUE, FALSE))
	{
	  flagcode_recordError (o);

	  if (context_getFlag (FLG_HINTS))
	    {
	      llgenhint (hint);
	    }
	  else
	    {
	      cstring_free (hint);
	    }

	  closeMessage ();
	  return TRUE;
	}

      cstring_free (hint);
    }
  else
    {
      cstring_free (hint);
      cstring_free (s);
    }

  flagcode_recordSuppressed (o);
  return FALSE;
}

static bool
llrealerror (flagcode code, char *srcFile, int srcLine, /*@only@*/ cstring s, /*@temp@*/ cstring addtext, fileloc fl)
{
  return (llgenerrorreal (code, srcFile, srcLine, s, addtext, fl, TRUE, FALSE));
}

static bool
llgenerroraux (flagcode code,
	       char *srcFile, int srcLine,
	       /*@only@*/ cstring s, 
	       cstring addtext,
	       fileloc fl, bool iserror, bool indent)
{
  if (context_inSuppressZone (fl))
    {
      cstring_free (s);
      return FALSE;
    }
  
  if (llgenerrorreal (code, srcFile, srcLine, s, addtext, fl, iserror, indent)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

bool
xllforceerror (char *srcFile, int srcLine, 
	       flagcode code, /*@only@*/ cstring s, fileloc fl)
{
  flagcode_recordError (code);

  if (llgenerrorreal (code, srcFile, srcLine, s, cstring_undefined, fl, TRUE, FALSE)) {
    closeMessage ();
    return TRUE;
  } else {
    return FALSE;
  }
}

static void generateCSV (flagcode code, cstring s, cstring addtext, fileloc fl)
{

  if (g_csvstream != NULL) {
    /* Warning, Flag Code, Flag Name, Priority, File, Line, Column, Warning Text, Additional Text */
    fprintf (g_csvstream, "%d,%d,%s,%d,%s,%d,%d,\"%s\"",
	     context_numErrors (),
	     (int) code, /* flag code */
	     cstring_toCharsSafe (flagcode_unparse (code)), /* flag name */
	     flagcode_priority (code), /* priority */
	     cstring_toCharsSafe (fileloc_outputFilename (fl)),
	     fileloc_lineno (fl),
	     fileloc_column (fl),
	     cstring_toCharsSafe (s));

    if (cstring_isDefined (addtext)) {
      fprintf (g_csvstream, ",\"%s\"\n", cstring_toCharsSafe (addtext));
    } else {
      fprintf (g_csvstream, "\n");
    }
  }
}

static bool
llgenerrorreal (flagcode code, char *srcFile, int srcLine, 
		/*@only@*/ cstring s, 
		cstring addtext,
		fileloc fl, bool iserror, bool indent)
{
  cstring flstring;

  /* duplicate message (rescanning a header file */

  if (!messageLog_add (context_messageLog (), fl, s))
    {
      DPRINTF (("Duplicate message suppressed! %s / %s",
		fileloc_unparse (fl), s));
      cstring_free (s);
      return FALSE;
    }

  /*
  ** If herald has not been displayed, display it before the first message.
  */

  showHerald ();

  if (iserror) context_hasError ();

  if (context_unlimitedMessages ())
    {
      ;
    }
  else
    {
      /*
      ** suppress excessive messages:
      **    check up to ':'
      **
      */

      char *sc = cstring_toCharsSafe (s);
      char *tmpmsg = strchr (sc, ':');

      if (tmpmsg == NULL)
	{
	  tmpmsg = sc;
	}
      else
	{
	  char *savechar = tmpmsg;
	  *tmpmsg = '\0';
	  tmpmsg = sc;
	  *savechar = ':';
	}

      if (cstring_equal (s_lastmsg, cstring_fromChars (tmpmsg)))
	{
	  s_mcount++;
	  if (s_mcount == (context_getLimit () + 1))
	    {
	      limitmessage (s, fl);
	      return FALSE;
	    }

	  if (s_mcount > (context_getLimit ()))
	    {
	      cstring_free (s);
	      return FALSE;
	    }
	}
      else
	{
	  cleanupMessages ();
	  s_mcount = 0;
	  cstring_free (s_lastmsg);
	  s_lastmsg = cstring_fromCharsNew (tmpmsg);
	}
    }

  DPRINTF (("Here..."));

  if (context_hasAliasAnnote ())
    {
      char *sc = cstring_toCharsSafe (s);
      char *fcolon = strchr (sc, ':');
      cstring a = context_getAliasAnnote ();


      if (fcolon == NULL)
	{
	  s = message ("%q (%q)", s, a);
	}
      else
	{
	  cstring afterColon;

	  *fcolon = '\0';
	  afterColon = cstring_fromCharsNew (fcolon + 1);

	  s = message ("%q (%q):%q", s, a, afterColon);
	}
          }

  if (context_hasMessageAnnote ())
    {
      char *fcolon = strchr (cstring_toCharsSafe (s), ':');


      if (fcolon == NULL)
	{
	  /*@-dependenttrans@*/ /* s becomes dependent for fcolon */
	  s = message ("%q (%q)", s, context_getMessageAnnote ());
	  /*@=dependenttrans@*/
	}
      else
	{
	  cstring afterColon;

	  *fcolon = '\0';
	  afterColon = cstring_fromCharsNew (fcolon + 1);

	  /*@-dependenttrans@*/ /* s becomes dependent for fcolon */
	  s = message ("%q (%q):%q", s,
		       context_getMessageAnnote (), afterColon);
	  /*@=dependenttrans@*/
	}
     }

  context_setNeednl ();
  prepareMessage ();

  if (context_showFunction ())
    {
      cstring fname = fileloc_unparseFilename (g_currentloc);

      if (context_inIterDef ())
	{
	  fprintf (g_warningstream, "%s: (in iter %s)\n",
		   cstring_toCharsSafe (fname),
		   cstring_toCharsSafe (context_inFunctionName ()));
	}
      else if (context_inIterEnd ())
	{
	  fprintf (g_warningstream, "%s: (in iter finalizer %s)\n",
		   cstring_toCharsSafe (fname),
		   cstring_toCharsSafe (context_inFunctionName ()));
	}
      else if (context_inMacro ())
	{
	  fprintf (g_warningstream, "%s: (in macro %s)\n", cstring_toCharsSafe (fname),
		   cstring_toCharsSafe (context_inFunctionName ()));
	}
      else
	{
	  fprintf (g_warningstream, "%s: (in function %s)\n",
		   cstring_toCharsSafe (fname),
		   cstring_toCharsSafe (context_inFunctionName ()));
	}

      cstring_free (fname);
      context_setShownFunction ();
    }

  flstring = fileloc_unparse (fl);
  s_lastfileloclen = cstring_length (flstring);

  generateCSV (code, s, addtext, fl);

  if (indent)
    {
      printError (g_warningstream, message ("   %q: %q", flstring, s));
    }
  else
    {
      printError (g_warningstream, message ("%q: %q", flstring, s));
    }

  showSourceLoc (srcFile, srcLine);
	     
  return TRUE;
}

/*
** printMessage
**
** message contains no '\n'
**    message fits in one line: print it
**    message fits in two lines with 3-space indent after fileloc: print it
**    split line with 5-space indent from left margin: print it
**
*/

static
void printMessage (FILE *stream, /*@only@*/ cstring s)
{
  printIndentMessage (stream, s, 0);
}

static
void printIndentMessage (FILE *stream, /*@only@*/ cstring sc, int indent)
{
  static bool inbody = FALSE;
  int maxlen = context_getLineLen ();
  char *s = cstring_toCharsSafe (sc);
  char *olds = NULL;

  llassertprotect (!inbody);
  inbody = TRUE;

  do
    {
      char *t = NULL;
      char *st = s;

      llassertprotect (st != olds);
      olds = st;
      mstring_split (&st, &t, maxlen, &indent);
      fprintf (stream, "%s\n", st);
      llassertprotect (t != s);
      s = t;
    } while (s != NULL) ;

  cstring_free (sc);
  inbody = FALSE;
}

static
void printError (FILE *stream, /*@only@*/ cstring sc)
{
  int maxlen = context_getLineLen ();
  size_t nspaces = s_lastfileloclen + 5;
  int nextlen = maxlen - size_toInt (nspaces);
  size_t len = cstring_length (sc);
  int indent = 0;
  char *s = cstring_toCharsSafe (sc);
  char *os = s;
  char *t = NULL;

  DPRINTF (("Print error: [%s]", sc));

  if (size_toInt (len) < (maxlen + nextlen) && (strchr (s, '\n') == NULL))
    {
      mstring_split (&s, &t, maxlen, &indent);

      fprintf (stream, "%s\n", s);

      if (t != NULL)
	{
	  len = mstring_length (t);

	  if (size_toInt (len) < (maxlen - 3) && (strchr (t, '\n') == NULL)
	      && size_toInt (len) > (nextlen - 1))
	    {
	      fprintf (stream, "    %s\n", t);
	    }
	  else
	    {
	      char *spaces = (char *) dmalloc ((nspaces + 1) * sizeof (*spaces));
	      int i;

	      for (i = 0; i < size_toInt (nspaces); i++)
		{
		  spaces[i] = ' ';
		}

	      spaces[nspaces] = '\0';

	      while (t != NULL)
		{
		  char *st = t;
		  mstring_split (&st, &t, nextlen, &indent);
		  fprintf (stream, "%s%s\n", spaces, st);
		}

	      sfree (spaces);
	    }
	}
    }
  else
    {
      DPRINTF (("Here 1: [%s]", sc));

      if (size_toInt (len) < (maxlen + maxlen - 1) && (strchr (s, '\n') != NULL))
	{
	  nspaces = ((maxlen + maxlen - 1) - len) / 2;

	  if (nspaces < 1) nspaces = 1;

	  nextlen = size_toInt (maxlen - nspaces);

	  mstring_split (&s, &t, maxlen, &indent);

	  fprintf (stream, "%s\n", s);

	  if (t != NULL)
	    {
	      char *spaces = (char *) dmalloc ((nspaces + 1) * sizeof (*spaces));
	      int i;

	      for (i = 0; i < size_toInt (nspaces); i++)
		{
		  spaces[i] = ' ';
		}

	      spaces[nspaces] = '\0';

	      while (t != NULL)
		{
		  char *st = t;

		  mstring_split (&st, &t, nextlen, &indent);
		  fprintf (stream, "%s%s\n", spaces, st);
		}

	      sfree (spaces);
	    }
	}
      else
	{
	  nspaces = 4;
	  nextlen = size_toInt (maxlen - nspaces);

	  DPRINTF (("Here 2: [%s]", s));
	  mstring_split (&s, &t, maxlen, &indent);
	  DPRINTF (("Here 3: [%s] [%s]", s, t));

	  fprintf (stream, "%s\n", s);

	  if (t != NULL)
	    {
  	      char *spaces = (char *) dmalloc ((nspaces + 1) * sizeof (*spaces));
	      size_t i;

	      for (i = 0; i < nspaces; i++)
		{
		  spaces[i] = ' ';
		}

	      spaces[nspaces] = '\0';

	      while (t != NULL)
		{
		  char *st = t;
		  DPRINTF (("Loop: [%s]", t));
		  mstring_split (&st, &t, nextlen, &indent);
		  DPRINTF (("Split: [%s] [%s]", st, t));
		  fprintf (stream, "%s%s\n", spaces, st);
		  DPRINTF (("Next..."));
		}

	      sfree (spaces);
	    }
	}
    }

  DPRINTF (("Done"));
  sfree (os);
}

void
xllfatalbug (char *srcFile, int srcLine, /*@only@*/ cstring s)
{
  prepareMessage ();
  printError (g_errorstream, message ("%q: *** Fatal bug: %q",
				      fileloc_unparse (g_currentloc), s));
  showSourceLoc (srcFile, srcLine);
  printCodePoint ();
  printBugReport ();
  llexit (LLFAILURE);
}

void
lclfatalbug (char *msg)
{
  prepareMessage ();
  printError (g_errorstream,
	      message ("*** Fatal Bug: %s", cstring_fromChars (msg)));
  printCodePoint ();
  printBugReport ();
  llexit (LLFAILURE);
}

void
checkParseError (void)
{
  if (fileloc_withinLines (lastparseerror, g_currentloc, 10))
    {
      llfatalerror (message ("%q: Cannot recover from parse error.",
			     fileloc_unparse (g_currentloc)));
    }
}

void llbugaux (cstring file, int line, /*@only@*/ cstring s)
{
  /*@unchecked@*/
  static int numbugs = 0;
  static bool inbug = FALSE;

  context_recordBug ();

  if (inbug)
    {
      cstring temps = fileloc_unparseRaw (file, line);

      fprintf (g_errorstream,
	       "%s: Recursive bug detected: %s\n",
	       cstring_toCharsSafe (temps),
	       cstring_toCharsSafe (s));
      cstring_free (temps);

      llexit (LLFAILURE);
    }

  inbug = TRUE;

  prepareMessage ();

  if (fileloc_withinLines (lastparseerror, g_currentloc, 7))
    {
      llfatalerror (message ("%q: Cannot recover from parse error.",
			     fileloc_unparse (g_currentloc)));
    }

  (void) fflush (g_warningstream);

  printError (g_errorstream,
	      message ("%q: *** Internal Bug at %q: %q [errno: %d]",
		       fileloc_unparse (g_currentloc),
		       fileloc_unparseRaw (file, line),
		       s, errno));
  
  /* printCodePoint (); no longer useful */

  llflush ();
  
  /*
  ** This is confusing, and hardly ever useful.

  if (errno != 0)
    {
      perror ("Possible system error diagnostic: ");
    }

  **
  */

  printBugReport ();
  llflush ();

  numbugs++;

  if (numbugs > context_getBugsLimit () && fileloc_withinLines (lastbug, g_currentloc, 2))
    {
      llfatalerror
	(message ("%q: Cannot recover from last bug. "
		  "(If you really want Splint to try to continue, use -bugslimit <n>.)",
		  fileloc_unparse (g_currentloc)));
    }
  
  fprintf (g_errorstream, "       (attempting to continue, results may be incorrect)\n");
  fileloc_free (lastbug);
  lastbug = fileloc_copy (g_currentloc);
  closeMessage ();
  inbug = FALSE;
}

void
lclbug (/*@only@*/ cstring s)
{
  prepareMessage ();
  printError (g_errorstream, message ("*** Internal Bug: %q", s));
  printCodePoint ();
  printBugReport ();
  fprintf (g_errorstream, "       (attempting to continue, results may be incorrect)\n");
  closeMessage ();
}

void
xllfatalerror (char *srcFile, int srcLine, cstring s)
{
  prepareMessage ();
  printError (g_errorstream, s);
  printError (g_errorstream, cstring_makeLiteral ("*** Cannot continue."));
  showSourceLoc (srcFile, srcLine);
  llexit (LLFAILURE);
}

void
xllfatalerrorLoc (char *srcFile, int srcLine, /*@only@*/ cstring s)
{
  prepareMessage ();
  (void) fflush (g_warningstream);
  printError (g_errorstream, message ("%q: %q", fileloc_unparse (g_currentloc), s));
  printError (g_errorstream, cstring_makeLiteral ("*** Cannot continue."));
  showSourceLoc (srcFile, srcLine);
  (void) fflush (g_warningstream);
  llexit (LLFAILURE);
}

bool
lclHadError (void)
{
  return (s_lclerrors > 0);
}

bool
lclHadNewError (void)
{
  static int lastcall = 0;

  if (s_lclerrors > lastcall)
    {
      lastcall = s_lclerrors;
      return TRUE;
    }
  else
    {
      return FALSE;
    }
}

int
lclNumberErrors (void)
{
  return (s_lclerrors);
}

void
xlclerror (char *srcFile, int srcLine, ltoken t, /*@only@*/ cstring msg)
{
  s_lclerrors++;

  if (ltoken_getCode (t) != NOTTOKEN)
    {
      cstring loc = ltoken_unparseLoc (t);
      s_lastfileloclen = cstring_length (loc);

      printError (g_warningstream, message ("%q: %q", loc, msg));
      showSourceLoc (srcFile, srcLine);
    }
  else
    {
      printError (g_warningstream, msg);
      showSourceLoc (srcFile, srcLine);
    }
}

void
lclplainerror (/*@only@*/ cstring msg)
{
  s_lclerrors++;
  printError (g_warningstream, msg);
}

void
lclfatalerror (ltoken t, /*@only@*/ cstring msg)
{
  if (ltoken_getCode (t) != NOTTOKEN)
    {
      cstring loc = ltoken_unparseLoc (t);
      s_lastfileloclen = cstring_length (loc);
      printError (g_errorstream, message ("%q: %q", loc, msg));
    }
  else
    {
      printError (g_errorstream, msg);
    }

  printError (g_errorstream, cstring_makeLiteral ("*** Cannot continue"));
  llexit (LLFAILURE);
}

void
lclplainfatalerror (/*@only@*/ cstring msg)
{
  (void) fflush (g_warningstream);
  printError (g_errorstream, message ("*** Cannot continue: %q", msg));
  llexit (LLFAILURE);
}

void
lclRedeclarationError (ltoken id)
{
  cstring s = ltoken_getRawString (id);


  if (usymtab_existsEither (s))
    {
      uentry le = usymtab_lookupEither (s);

      lclerror (id, message ("Respecification of %s", s));
      llgenindentmsg (message ("Previous specification of %q",
			       uentry_getName (le)),
		uentry_whereSpecified (le));
    }
  else
    {
      lclerror (id, message ("Identifier redeclared: %s", s));
    }
}

void genppllerror (flagcode code, /*@only@*/ cstring s)
{
  if (context_inSuppressZone (g_currentloc))
    {
      cstring_free (s);
    }
  else
    {
      if (context_getFlag (code))
	{
	  if (s_scanOpen)
	    {
	      displayScanClose ();
	    }

	  llerror (code, s);

	  if (code != FLG_PREPROC)
	    {
	      llsuppresshint ('-', code);
	    }

	  if (!context_isInCommandLine ())
	    {
	      displayScanOpen (cstring_makeLiteral ("< more preprocessing ."));
	    }
	}
      else
	{
	  cstring_free (s);
	}
    }
}

void genppllerrorhint (flagcode code, /*@only@*/ cstring s,
		       /*@only@*/ cstring hint)
{
  if (context_inSuppressZone (g_currentloc))
    {
      cstring_free (s);
      cstring_free (hint);
    }
  else
    {
      if (context_getFlag (code))
	{
	  generateCSV (code, s, hint, g_currentloc);
	  prepareMessage ();
	  context_clearPreprocessing ();
	  llerror (code, s);
	  llgenhint (hint);
	  context_setPreprocessing ();
	  closeMessage ();
	}
      else
	{
	  cstring_free (s);
	  cstring_free (hint);
	}
    }
}

void ppllerror (/*@only@*/ cstring s)
{
  genppllerror (FLG_PREPROC, s);
}

void pplldiagmsg (cstring s)
{
  if (!context_isInCommandLine ())
    {
      if (s_scanOpen) 
	{
	  displayScanClose ();
	}

      lldiagmsg (s);
      displayScanOpen (cstring_makeLiteral ("< more preprocessing ."));
    }
  else
    {
      lldiagmsg (s);
    }
}

void loadllmsg (cstring s)
{
  displayScanClose ();
  lldiagmsg (s);
  displayScanOpen (cstring_makeLiteral ("< ."));
}

static void llreportparseerror (/*@only@*/ cstring s)
{
  if (fileloc_withinLines (lastparseerror, g_currentloc, 5))
    {
      cstring_free (s);
    }
  else
    {
      llerror (FLG_SYNTAX, s);

      fileloc_free (lastparseerror);
      lastparseerror = fileloc_copy (g_currentloc);
    }
}

bool xcppoptgenerror (char *srcFile, int srcLine,
		      flagcode o,
		      /*@only@*/ cstring s,
		      cppReader *pfile)
{
  bool res = FALSE;
  fileloc loc = cppReader_getLoc (pfile);

  if (context_flagOn (o, loc))
    {
      if (xlloptgenerror (srcFile, srcLine, o, s, loc))
	{
	  cppReader_printContainingFiles (pfile);
	  res = TRUE;
	}
    }
  else
    {
      cstring_free (s);
    }

  fileloc_free (loc);

  return res;
}

bool xlloptgenerror (char *srcFile, int srcLine, 
		     flagcode o, /*@only@*/ cstring s, fileloc loc)
{
  DPRINTF (("xllopt: %s", s));

  if (llrealerror (o, srcFile, srcLine, s, flagcodeHint (o), loc))
    {
      DPRINTF (("Here we are!"));
      llsuppresshint ('-', o);
      closeMessage ();
      flagcode_recordError (o);
      return TRUE;
    }
  else
    {
      DPRINTF (("Suppressed!"));
      flagcode_recordSuppressed (o);
      return FALSE;
    }
}

bool xoptgenerror2 (char *srcFile, int srcLine,
		    flagcode f1, flagcode f2, /*@only@*/ cstring s, fileloc loc)
{
  if (context_suppressFlagMsg (f1, loc))
    {
      flagcode_recordSuppressed (f1);
      cstring_free (s);
    }
  else
    {
      if (context_suppressFlagMsg (f2, loc))
	{
	  flagcode_recordSuppressed (f2);
	  cstring_free (s);
	}
      else
	{
	  if (llrealerror (f1, srcFile, srcLine, s, flagcodeHint (f1), loc))
	    {
	      llsuppresshint2 ('-', f1, f2);
	      flagcode_recordError (f2);
	      closeMessage ();
	      return TRUE;
	    }
	  else
	    {
	      flagcode_recordSuppressed (f2);
	    }
	}
    }

  return FALSE;
}

bool xoptgenerror2n (char *srcFile, int srcLine,
		     flagcode f1, flagcode f2, /*@only@*/ cstring s, fileloc loc)
{

  if (context_suppressFlagMsg (f1, loc))
    {
      flagcode_recordSuppressed (f1);
      cstring_free (s);
    }
  else
    {
      if (context_suppressNotFlagMsg (f2, loc))
	{
	  flagcode_recordSuppressed (f2);
	  cstring_free (s);
	}
      else
	{
	  if (llrealerror (f1, srcFile, srcLine, s, flagcodeHint (f2), loc))
	    {
	      llsuppresshint ('+', f2);
	      flagcode_recordError (f2);
	      closeMessage ();
	      return TRUE;
	    }

	  flagcode_recordSuppressed (f2);
	}
    }
  return FALSE;
}

bool xllnoptgenerror (char *srcFile, int srcLine,
		      flagcode o, /*@only@*/ cstring s, fileloc loc)
{
  if (llrealerror (o, srcFile, srcLine, s, flagcodeHint (o), loc))
    {
      llsuppresshint ('+', o);
      flagcode_recordError (o);
      closeMessage ();
      return TRUE;
    }

  flagcode_recordSuppressed (o);
  return FALSE;
}

void xllparseerror (char *srcFile, int srcLine, cstring s)
{
  if (context_getFlag (FLG_TRYTORECOVER))
    {
      parseerrorcount++;

      if (parseerrorcount > GIVEUPPARSE)
	{
	  if (cstring_isDefined (s))
	    {
	      xllfatalerror (srcFile, srcLine,
			     message ("%q: Parse Error: %q.  "
				      "Too many errors, giving up.",
				      fileloc_unparse (g_currentloc), s));
	    }
	  else
	    {
	      xllfatalerror (srcFile, srcLine,
			     message ("%q: Parse Error.  Too many errors, giving up.",
				      fileloc_unparse (g_currentloc)));
	    }
	}
      else
	{
	  if (cstring_isDefined (s))
	    {
	      llreportparseerror (message ("Parse Error: %q. Attempting to continue.",
					   s));
	      showSourceLoc (srcFile, srcLine);
	    }
	  else
	    {
	      llreportparseerror (message ("Parse Error. Attempting to continue."));
	      showSourceLoc (srcFile, srcLine);
	    }
	}
    }
  else
    {
      cstring msg;

      if (cstring_isDefined (s))
	{
	  msg = message ("Parse Error: %q.", s);
	}
      else
	{
	  msg = message ("Parse Error.");
	}

      xllfatalerror
	(srcFile, srcLine,
	 message ("%q: %s (For help on parse errors, "
		  "see splint -help parseerrors.)",
		  fileloc_unparse (g_currentloc), msg));
    }
}

bool xfsgenerror (char *srcFile, int srcLine,
		  flagSpec fs, /*@only@*/ cstring s, fileloc fl) 
{
  if (flagSpec_isOn (fs, fl))
    {
      flagcode firston = flagSpec_getFirstOn (fs, fl);

      if (llgenerroraux (firston, srcFile, srcLine, s, 
			 flagcodeHint (firston),
			 fl, TRUE, FALSE))
	{
	  llsuppresshint ('-', firston);
	  flagcode_recordError (firston);
	  return TRUE;
	}
      else
	{
	  flagcode_recordSuppressed (firston);
	  return FALSE;
	}
    }
  else
    {
      flagcode_recordSuppressed (flagSpec_getDominant (fs));
      cstring_free (s);
      return FALSE;
    }
}

bool doCheck (bool x, cstring pred, cstring file, int line)
{
  if (!x) {
    llbug (message ("%q: Check Failed: %s",
		    fileloc_unparseRaw (file, line),
		    pred));
  }

  return x;
}

/*@observer@*/ cstring lldecodeerror (/*@unused@*/ int errnum)
{
  char *result;

#ifndef VMS
#ifndef HAVE_STRERROR
  result = NULL;
#else
  result = strerror (errnum);
#endif
#else	/* VMS */
  /* VAXCRTL's strerror() takes an optional second argument, which only
     matters when the first argument is EVMSERR.  However, it's simplest
     just to pass it unconditionally.  `vaxc$errno' is declared in
     <errno.h>, and maintained by the library in parallel with `errno'.
     We assume that caller's `errnum' either matches the last setting of
     `errno' by the library or else does not have the value `EVMSERR'.  */

  result = strerror (errnum, vaxc$errno);
#endif

  if (result == NULL)
    {
      result = cstring_toCharsSafe (message ("undocumented I/O error: %d", errnum));
    }

  return cstring_fromChars (result);
}

void llquietbugaux (cstring s, /*@unused@*/ cstring file, /*@unused@*/ int line)
{
# if 0
# ifdef HOMEVERSION
  llflush ();
  printError (g_errorstream, message ("%q: *** Internal Bug at %q: %q [errno: %d]",
				      fileloc_unparse (g_currentloc),
				      fileloc_unparseRaw (file, line),
				      s, errno));
  printCodePoint ();
  llflush ();
# endif
# else
  cstring_free (s);
# endif
}

void llflush (void)
{
  (void) fflush (g_warningstream);
  (void) fflush (g_messagestream);
}

void displayScan (cstring msg)
{
  if (s_scanOpen)
    {
      displayScanClose ();
    }

  llassert (!s_scanOpen);

  if (context_getFlag (FLG_SHOWSCAN))
    {
      showHerald ();
      fprintf (g_messagestream, "< %s >\n", cstring_toCharsSafe (msg));
      (void) fflush (g_messagestream);
    }

  cstring_free (msg);
}

void displayScanOpen (cstring msg)
{
  if (s_scanOpen)
    {
      displayScanClose ();
    }

  llassert (!s_scanOpen);
  s_scanOpen = TRUE;

  if (context_getFlag (FLG_SHOWSCAN))
    {
      fprintf (g_messagestream, "< %s", cstring_toCharsSafe (msg));
      (void) fflush (g_messagestream);
    }

  cstring_free (msg);
}

void displayScanContinue (/*@temp@*/ cstring msg)
{
  if (context_getFlag (FLG_SHOWSCAN))
    {
      if (s_scanOpen) 
	{
	  fprintf (g_messagestream, "%s", cstring_toCharsSafe (msg));
	  (void) fflush (g_messagestream);
	}
      else
	{
	  /*
	  ** Don't call bug recursively
	  */

	  fprintf (stderr, "*** Bug: scan continue scan not open\n");
	}
    }
}

void displayScanClose (void)
{
  if (s_scanOpen)
    {
      if (context_getFlag (FLG_SHOWSCAN))
	{
	  fprintf (g_messagestream, " >\n");
	  (void) fflush (g_messagestream);
	}
    }
  else
    {
      /*
      ** Don't call bug recursively
      */
      
      fprintf (stderr, "*** Bug: scan close scan not open\n");
    }
  
  s_scanOpen = FALSE;
}


