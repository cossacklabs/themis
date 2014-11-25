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
** flags.c
*/

# include "splintMacros.nf"
# include "basic.h"
# include "osd.h"
# include "rcfiles.h"
# include "lslinit.h"
# include "llmain.h"

/*
** from the CC man page:
**
**  -Idir          Search for #include files whose names do not begin with a
**                    slash (/) in the following order: (1) in the directory of
**                    the dir argument, (2) in the directories specified by -I
**                    options, (3) in the standard directory (/usr/include).
*/

/* needed for string literals literals */

typedef struct { 
  flagkind kind;
  /*@null@*/ /*@observer@*/ char *name;
  /*@null@*/ /*@observer@*/ char *describe;
} flagcatinfo;

static flagcatinfo categories[] =
{
  { FK_ABSTRACT, "abstract", "abstraction violations, representation access" } ,
  { FK_ALIAS, "aliasing", "unexpected or dangerous aliasing" } ,
  { FK_USE, "alluse", "all declarations are used" } ,
  { FK_ANSI, "ansi", "violations of constraints imposed by ANSI/ISO standard" } ,
  { FK_ARRAY, "arrays", "special checking involving arrays" } ,
  { FK_BOOL, "booleans", "checking and naming of boolean types" } ,
  { FK_COMMENTS, "comments", "warnings about (normal) comments" } ,
  { FK_SYNCOMMENTS, "syncomments", "interpretation of annotation comments" } ,
  { FK_COMPLETE, "complete", "completely defined, used, or specified system" } ,
  { FK_CONTROL, "controlflow", "suspicious control structures" } ,
  { FK_DEBUG, "debug", "flags for debugging splint" } ,
  { FK_DECL, "declarations", "consistency of declarations" } ,
  { FK_DEF, "definition", "undefined storage errors" } ,
  { FK_DIRECT, "directories", "set directores" } ,
  { FK_DISPLAY, "display", "control what is displayed" } ,
  { FK_EFFECT, "effect", "statements with no effects" } ,
  { FK_ERRORS, "errors", "control expected errors, summary reporting" } ,
  { FK_EXPORT, "export", "control what may be exported" } ,
  { FK_EXPOSURE, "exposure", "representation exposure" } ,
  { FK_EXTENSIBLE, "extensible", "user-defined checks and annotations" },
  { FK_FILES, "files", "control system files" } ,
  { FK_FORMAT, "format", "control format of warning messages" } ,
  { FK_GLOBALS, "globals", "use of global and file static variables" },
  { FK_HEADERS, "headers", "control inclusion and generation of header files" },
  { FK_HELP, "help", "on-line help" },
  { FK_BOUNDS, "memorybounds", "out-of-bounds memory accesses" },
  { FK_HINTS, "hints", "control display of warning hints" },
  { FK_SYSTEMFUNCTIONS, "systemfunctions", "special properties of exit and main" },
  { FK_IMPLICIT, "implicit", "control implicit annotations and interpretations" } ,
  { FK_INIT, "initializations", "initialization files" } ,
  { FK_ITER, "iterators", "checking iterator definitions and uses" } ,
  { FK_LEAK, "leaks", "memory leaks" } ,
  { FK_LIBS, "libraries", "loading and dumping of user and standard libraries" } ,
  { FK_LIMITS, "limits", "violations of set limits" } ,
  { FK_MACROS, "macros", "expansion, definition and use of macros" },
  { FK_MEMORY, "memory", "memory management" } ,
  { FK_MODIFIES, "modification", "modification errors" } ,
  { FK_NAMES, "names", "naming conventions and limits" } ,
  { FK_NULL, "null", "misuses of null pointers" } ,
  { FK_NUMBERS, "numbers", "control type-checking of numeric types" } ,
  { FK_OPS, "operations", "checking of primitive operations" } ,
  { FK_PARAMS, "parameters", "function and macro parameters" } ,
  { FK_SPEED, "performance", "speeding up checking" } ,
  { FK_POINTER, "pointers", "pointers" } ,
  { FK_PRED, "predicates", "condition test expressions" } ,
  { FK_PREFIX, "prefixes", "set naming prefixes and control checking" } ,
  { FK_PREPROC, "preproc", "defines and undefines for the preprocessor" } ,
  { FK_PROTOS, "prototypes", "function prototypes" } ,
  { FK_DEAD, "released", "using storage that has been deallocated" } ,
  { FK_IGNORERET, "returnvals", "ignored return values" },
  { FK_SECURITY, "security", "possible security vulnerability" },
  { FK_SPEC, "specifications", "checks involving .lcl specifications" } ,
  { FK_SUPPRESS, "suppress", "local and global suppression of messages" } ,
  { FK_TYPEEQ, "typeequivalence", "control what types are equivalent" } ,
  { FK_BEHAVIOR, "undefined", "code with undefined or implementation-defined behavior" } ,
  { FK_UNRECOG, "unrecognized", "unrecognized identifiers" } ,
  { FK_UNSPEC, "unconstrained", "checking in the presence of unconstrained functions" } ,
  { FK_WARNUSE, "warnuse", "use of possibly problematic function" } ,
  { FK_ITS4, "its4", "its4 compatibility flags (report warnings for uses of possibly insecure functions)" } ,
  { FK_SYNTAX, NULL, NULL } ,
  { FK_TYPE, NULL, NULL } ,
  { FK_SECRET, NULL, NULL } ,
  { FK_OBSOLETE, NULL, NULL } ,
  { FK_NONE, NULL, NULL }  /* must be last */
} ; 

typedef enum {
  ARG_NONE,
  ARG_NUMBER,    /* number */
  ARG_CHAR,      /* char */
  ARG_STRING,    /* string */
  ARG_FILE,      /* filename (also a string) */
  ARG_DIRECTORY, /* directory (also a string) */
  ARG_PATH,      /* path */
  ARG_SPECIAL   /* ? */
} argcode;

# ifdef WIN32
/* Make Microsoft VC++ happy */
# pragma warning (disable:4715)
# endif

static /*@observer@*/ cstring argcode_unparse (argcode arg)
{
  switch (arg) 
    {
    case ARG_STRING: return cstring_makeLiteralTemp ("string"); 
    case ARG_FILE: return cstring_makeLiteralTemp ("filename"); 
    case ARG_DIRECTORY: return cstring_makeLiteralTemp ("directory");
    case ARG_PATH: return cstring_makeLiteralTemp ("path"); 
    case ARG_NUMBER: return cstring_makeLiteralTemp ("number");
    case ARG_CHAR: return cstring_makeLiteralTemp ("character");
    case ARG_NONE: 
      BADBRANCH;
    case ARG_SPECIAL:
      BADBRANCH;
    }
}      

# ifdef WIN32
# pragma warning (default : 4715)
# endif

typedef struct { 
  flagkind main;
  flagkind sub;
  bool isSpecial;  /* setting this flag may set other flags (or values) */
  bool isIdem;     /* idempotent - always sets to TRUE */
  bool isGlobal;   /* cannot be set locally (using control comments) */
  bool isModeFlag; /* set by modes */
  argcode argtype;
  /*@observer@*/ char *flag;
  flagcode code; 
  /*@observer@*/ /*@null@*/ char *desc;
  bn_mstring hint; 
  int nreported; 
  int nsuppressed; 
} fflag;

typedef fflag flaglist[];

# include "flags.def"

/*@iter allFlags (yield observer fflag f); @*/
# define allFlags(m_f) \
  { /*@+enumint@*/ flagcode m_i; for (m_i = 0; m_i < NUMFLAGS; m_i++) { fflag m_f = flags[m_i]; /*@=enumint@*/
# define end_allFlags }}

static bn_mstring mode_names[] =
{ 
  "weak", "standard", "checks", "strict", NULL, 
};

/*@iter allModes (yield bn_mstring modename)@*/
# define allModes(m_m) \
  { int m_ii = 0; while (mstring_isDefined (mode_names[m_ii])) \
      { bn_mstring m_m = mode_names[m_ii]; m_ii++; 

# define end_allModes }}

/*@+enumint@*/

static cstring getFlagModeSettings (flagcode p_flag) /*@modifies internalState@*/ ;
static cstring describeFlagCode (flagcode p_flag) /*@*/ ;
static cstringSList sortedFlags (void) /*@*/ ;
static /*@observer@*/ cstring categoryName (flagkind p_kind) /*@*/ ;

static flagcode flags_identifyFlagAux (cstring p_s, bool p_quiet) /*@modifies g_warningstream@*/ ;

# if 0
static /*@unused@*/ cstring listModes (void) /*@*/ ;
# endif

bool flagcode_isSpecialFlag (flagcode f)
{
  return (flags[f].isSpecial);
}

bool flagcode_isGlobalFlag (flagcode f)
{
  return (flags[f].isGlobal);
}

bool flagcode_isIdemFlag (flagcode f)
{
  return (flags[f].isIdem);
}

bool flagcode_isModeFlag (flagcode f)
{
  return (flags[f].isModeFlag);
}

bool flagcode_isNameChecksFlag (flagcode f)
{
  return (flags[f].main == FK_NAMES);
}

bool flagcode_isHelpFlag (flagcode f)
{
  return f == FLG_HELP;
}

bool flagcode_isMessageControlFlag (flagcode f)
{
  /*
  ** True if opt controls the display of messages.
  ** These flags must be processed first.
  */

  return (f == FLG_SHOWSCAN 
	  || f == FLG_WARNRC 
	  || f == FLG_PARENFILEFORMAT
	  || f == FLG_MESSAGESTREAMSTDERR
	  || f == FLG_MESSAGESTREAMSTDOUT
	  || f == FLG_WARNINGSTREAMSTDERR
	  || f == FLG_WARNINGSTREAMSTDOUT
	  || f == FLG_ERRORSTREAMSTDERR
	  || f == FLG_ERRORSTREAMSTDOUT
	  || f == FLG_MESSAGESTREAM
	  || f == FLG_WARNINGSTREAM
	  || f == FLG_ERRORSTREAM
	  || f == FLG_STREAMOVERWRITE);
}

/*
** Internal consistency check on the flags.
*/

void flags_initMod ()
{
  allFlagCodes (code)
    {
      /*@+enumint@*/
      if (flags[code].code != code)
	{
	  fprintf (stderr, 
		   "*** ERROR: inconsistent flag %s / %d / %d", 
		   flags[code].flag,
		   flags[code].code, code);
	  
	  llbug (message ("*** ERROR: inconsistent flag %s / %d / %d", 
			  cstring_fromChars (flags[code].flag),
			  flags[code].code, code));
	}
      /*@=enumint@*/
    } end_allFlagCodes;
}

void
summarizeErrors ()
{
  bool hadOne = FALSE;
  int sumrep = 0;
  int sumsup = 0;

  char *buf = mstring_create (128);

  allFlags (f)
    {
      if (f.nreported > 0 || f.nsuppressed > 0)
	{
	  int nrep = f.nreported;
	  int nsup = f.nsuppressed;
	  cstring fs = cstring_fill (cstring_fromChars (f.flag), 23);

	  if (!hadOne)
	    {
	      llmsgplain (cstring_makeLiteral
			  ("\nError Type                Reported  Suppressed\n"
			   "===================       ========  ========="));
	      hadOne = TRUE;
	    }

	  (void) snprintf (buf, 128, "%s%7d   %9d", cstring_toCharsSafe (fs), nrep, nsup);

	  sumrep += nrep;
	  sumsup += nsup;
	  
	  cstring_free (fs);
	  llmsg (cstring_copy (cstring_fromChars (buf)));
	}
    } end_allFlags;

  if (hadOne)
    {
      cstring ts = cstring_fill (cstring_makeLiteralTemp ("Total"), 23);

      llmsglit ("                          ========  =========");

      (void) snprintf (buf, 128, "%s%7d   %9d", cstring_toCharsSafe (ts), sumrep, sumsup);
      cstring_free (ts);
      llmsgplain (cstring_copy (cstring_fromChars (buf)));
    }

  sfree (buf);
}

/*@+enumindex@*/

void
flagcode_recordError (flagcode f)
{
  if (f != INVALID_FLAG)
    {
      if (f == FLG_WARNFLAGS)
	{
	  ; /* don't count these */
	}
      else
	{
	  flags[f].nreported = flags[f].nreported + 1;
	}
    }
  else
    {
      llcontbug (message ("flagcode_recordError: invalid flag: %d", (int) f));
    }
}

void
flagcode_recordSuppressed (flagcode f)
{
  llassertprint (f != INVALID_FLAG, ("flagcode: %s", flagcode_unparse (f)));

  flags[f].nsuppressed = flags[f].nsuppressed + 1;
}

int
flagcode_numReported (flagcode f)
{
  llassert (f != INVALID_FLAG);

  return (flags[f].nreported);
}

/*@observer@*/ cstring
flagcodeHint (flagcode f)
{
  llassert (f != INVALID_FLAG);

  if (mstring_isDefined (flags[f].hint))
    {
      return (cstring_fromChars (flags[f].hint));
    }
  else
    {
      return (cstring_fromChars (flags[f].desc));
    }
}

static int categorySize (flagkind kind) /*@*/ 
{
  int n = 0;

  
  allFlags (f)
    {
      if (f.main == kind || f.sub == kind)
	{
	  	  n++;
	}
    } end_allFlags;

  return n;
}

flagkind identifyCategory (cstring s)
{
  int i;

  for (i = 0; categories[i].kind != FK_NONE; i++)
    {
      if (mstring_isDefined (categories[i].name))
	{
	  if (cstring_equalLit (s, categories[i].name))
	    {
	      return categories[i].kind;
	    }
	}
    }

  return FK_NONE;
}

static /*@observer@*/ cstring categoryName (flagkind kind)
{
  int i;

  for (i = 0; categories[i].kind != FK_NONE; i++)
    {
      if (categories[i].kind == kind)
	{
	  return (cstring_fromChars (categories[i].name));
	}
    }
  
  return (cstring_makeLiteralTemp ("<No Category>"));
}

static int categoryIndex (flagkind kind)
{
  int i;

  for (i = 0; categories[i].kind != FK_NONE; i++)
    {
      if (categories[i].kind == kind)
	{
	  return i;
	}
    }

  return -1;
}

void printCategory (flagkind kind)
{
  int index = categoryIndex (kind);

  llassert (index >= 0);
  llmsg (message ("%s (%d flags)\n\3%s\n\n", 
		  cstring_fromChars (categories[index].name), 
		  categorySize (kind),
		  cstring_fromChars (categories[index].describe)));

  allFlags (f)
    {
      if (f.main == kind || f.sub == kind)
	{
	  llmsg (message ("   %s\n\6%q", cstring_fromChars (f.flag), 
			  describeFlagCode (f.code)));
	}
    } end_allFlags;
}

void 
listAllCategories (void)
{
  int i;

  for (i = 0; categories[i].kind != FK_NONE; i++)
    {
      flagkind kind = categories[i].kind ;

      if (categories[i].describe != NULL)
	{
	  llmsg (message ("%s (%d flags)\n\3%s", 
			  categoryName (kind), 
			  categorySize (kind),
			  cstring_fromChars (categories[i].describe)));
	}
    }
}

void
printAllFlags (bool desc, bool full)
{
  if (full)
    {
      cstringSList fl = sortedFlags ();

      cstringSList_elements (fl, el)
	{
	  cstring tmp;
	  tmp = cstring_copy(el);
	  llmsg (message ("%q\n\n", describeFlag (tmp)));
	  cstring_free(tmp);
	} end_cstringSList_elements ;

      cstringSList_free (fl);
    }
  else
    {
      allFlags (f)
	{
	  if (f.code != INVALID_FLAG && f.main != FK_OBSOLETE)
	    {
	      if (mstring_isDefined (f.desc))
		{
		  if (desc)
		    {
		      llmsg (message ("%s --- %s", cstring_fromChars (f.flag),
				      cstring_fromChars (f.desc)));
		    }
		}
	    }
	} end_allFlags;
    }
}

void
printFlagManual (bool html)
{
  /*
  ** Prints all flags by category, in order they appear in flags.def
  */

  flagkind lastCategory = FK_NONE;

  allFlags (f) {
    cstring flagname;
    cstring flagtype = cstring_undefined;

    if (f.main != lastCategory)
      {
	if (html)
	  {
	    llmsg (message ("\n<h4>%s</h4>\n", categoryName (f.main)));
	  }
	else
	  {
	    llmsg (message ("\n%s\n%s\n",
			    categoryName (f.main),
			    cstring_makeLiteralTemp ("===================================")));
	  }

	lastCategory = f.main;
      }

    if (f.argtype == ARG_NONE || f.argtype == ARG_SPECIAL)
      {
	if (html) 
	  {
	    flagname = message ("<tt>%s</tt>", cstring_fromChars (f.flag));
	  }
	else
	  {
	    flagname = cstring_fromCharsNew (f.flag);
	  }
      }
    else
      {
	if (flagcode_hasString (f.code)) 
	  {
	    if (html)
	      {
		flagname = message ("<tt>%s <em>&lt;%s&gt;</em></tt>",
				    cstring_fromChars (f.flag), argcode_unparse (f.argtype));
	      }
	    else
	      {
		flagname = message ("%s <%s>", cstring_fromChars (f.flag), argcode_unparse (f.argtype));
	      }
	    
	    if (cstring_isDefined (context_getString (f.code)))
	      {
		if (html)
		  {
		    flagname = message ("%q <font color=\"blue\">[%s]</font>", flagname,
					context_getString (f.code));
		  }
		else
		  {
		    flagname = message ("%q [%s]", flagname,
					context_getString (f.code));
		  }
	      }
	  }
	else if (f.argtype == ARG_CHAR)
	  {
	    if (html)
	      {
		flagname = message ("<tt>%s <em>&lt;%s&gt;</em></tt> <font color=\"blue\">[%c]</font>",
				    cstring_fromChars (f.flag), argcode_unparse (f.argtype),
				    (char) context_getValue (f.code));
	      }
	    else
	      {
		flagname = message ("%s <%s> [%c]", cstring_fromChars (f.flag), argcode_unparse (f.argtype),
				    (char) context_getValue (f.code));
	      }
	  }
	else 
	  {
	    llassert (f.argtype == ARG_NUMBER);

	    if (html)
	      {
		flagname = message ("<tt>%s <em>&lt;%s&gt;</em> <font color=\"blue\">[%d]</font>",
				    cstring_fromChars (f.flag), argcode_unparse (f.argtype),
				    context_getValue (f.code));
	      }
	    else
	      {
		flagname = message ("%s <%s> [%d]", cstring_fromChars (f.flag), argcode_unparse (f.argtype),
				    context_getValue (f.code));
	      }
	  }
      }

    if (f.isIdem)
      {
	if (html)
	  {
	    flagtype = message("%q<font color=\"green\">-</font>", flagtype);
	  }
	else
	  {
	    flagtype = message("%q<->", flagtype);
	  }
      }
    
    if (f.isGlobal)
      {
	if (html)
	  {
	    flagtype = message ("%q<font color=\"green\"><em>global</em></font>", flagtype);
	  }
	else
	  {
	    flagtype = message ("%q<G>", flagtype);
	  }
      }

    if (f.isSpecial)
      {
	if (html)
	  {
	    flagtype = message ("%q<font color=\"orange\"><em>shortcut</em></font>", flagtype);
	  }
	else
	  {
	    flagtype = message("%q<S>", flagtype);
	  }
      }
    
    if (f.isModeFlag)
      {
	if (html)
	  {
	    flagtype = message ("%q mode:<tt>%q</tt>>", flagtype, getFlagModeSettings (f.code));
	  }
	else
	  {
	    flagtype = message ("%q<M:%q>", flagtype, getFlagModeSettings (f.code));
	  }
      }
    else /* its a plain flag */
      {
	if (html)
	  {
	    flagtype = message ("%q plain:<tt>%s</tt>", flagtype,
				cstring_makeLiteralTemp (context_getFlag (f.code) ? "+" : "-"));
	  }
	else
	  {
	    flagtype = message ("%q<P:%s>", flagtype,
				cstring_makeLiteralTemp (context_getFlag (f.code) ? "+" : "-"));
	  }
      }
    
    llmsg (message ("%s: %s", flagname, flagtype));

    if (html)
      {
	llgenindentmsgnoloc (cstring_makeLiteral ("<blockquote>"));
      }

    if (mstring_isDefined (f.hint))
      {
	llgenindentmsgnoloc (cstring_fromCharsNew (f.hint));
      }
    else
      {
	llgenindentmsgnoloc (message ("%q.", cstring_capitalize (cstring_fromChars (f.desc))));
      }

    if (html)
      {
	llgenindentmsgnoloc (cstring_makeLiteral ("</blockquote>"));
      }
  } end_allFlags ;
}

cstring 
describeMode (cstring mode)
{
  cstringSList sflags = sortedFlags ();
  cstring res = message ("Predefined mode %s sets: ", mode);

  llassert (flags_isModeName (mode));

  context_setMode (mode);

  cstringSList_elements (sflags, flagname)
    {
      flagcode code = flags_identifyFlag (flagname);
      fflag currentflag = flags[code];
      
      if (mstring_isDefined (currentflag.desc) && flagcode_isModeFlag (code))
	{
	  if (context_getFlag (code))
	    {
	      res = message ("%q\n   +%s", res, cstring_fromChars (currentflag.flag));
	    }
	  else
	    {
	      res = message ("%q\n   -%s", res, cstring_fromChars (currentflag.flag)); 
	    }
	}
    } end_cstringSList_elements;
  
  cstringSList_free (sflags);

  res = cstring_appendChar (res, '\n');
  return (res);
}

cstring
describeFlagCode (flagcode flag)
{
  cstring ret = cstring_undefined;
  fflag f;
  
  if (flagcode_isInvalid (flag))
    {
      return (cstring_makeLiteral ("<invalid>"));
    }

  if (flagcode_isModeName (flag)) 
    {
      return (cstring_makeLiteral ("<mode flag>"));
    }

  context_resetAllFlags ();
  
  f = flags[flag];
  ret = cstring_copy (cstring_fromChars (f.desc));
  
  if (f.sub != FK_NONE)
    {
      ret = message ("%q\nCategories: %s, %s",
		     ret, 
		     categoryName (f.main),
		     categoryName (f.sub));
    }
  else 
    {
      if (f.main != FK_NONE)
	{
	  cstring cname = categoryName (f.main);
	  
	  if (cstring_isDefined (cname))
	    {
	      ret = message ("%q\nCategory: %s",
			     ret, cname);
	    }
	}
    }
  
  if (f.isModeFlag)
    {
      ret = message ("%q\nMode Settings: %q",
		     ret, getFlagModeSettings (flag));
    }
  else
    {
      ret = message ("%q\nDefault Setting: %s",
		     ret, 
		     cstring_makeLiteralTemp (context_getFlag (flag) ? "+" : "-"));
    }
  
  if (f.isGlobal)
    {
      ret = message("%q\nSet globally only", ret);
    }
  else
    {
      ret = message("%q\nSet locally", ret);
    }
  
  switch (f.argtype)
    {
    case ARG_NONE:
    case ARG_SPECIAL:
      break;
    case ARG_NUMBER:
      ret = message("%q\nNumeric Argument.  Default: %d",
		    ret,
		    context_getValue (flag));
      break;
    case ARG_CHAR:
      ret = message("%q\nCharacter Argument.  Default: %h",
		    ret, (char) context_getValue (flag));
      break;
    case ARG_STRING:
    case ARG_FILE:
    case ARG_PATH:
    case ARG_DIRECTORY:
      {
      if (cstring_isDefined (context_getString (flag)))
	{
	  ret = message("%q\n%q argument.  Default: %s",
			ret,
			cstring_capitalize (argcode_unparse (f.argtype)),
			context_getString (flag));
	}
      else
	{
	  ret = message("%q\n%s argument.  No default.", 
			ret,
			cstring_capitalize (argcode_unparse (f.argtype)));
	}
      break;
      }
    }
  
  if (mstring_isDefined (f.hint))
    {
      ret = message("%q\n\3%s", ret, cstring_fromChars (f.hint));
    }
  
  return ret;
}
  
static cstring getFlagModeSettings (flagcode flag)
{
  cstring res = cstring_undefined;
  
  allModes (mname)
    {
      context_setModeNoWarn (cstring_fromChars (mname));
      
      res = message ("%q%s", res, cstring_makeLiteralTemp (context_getFlag (flag) ? "+" : "-"));
    } end_allModes;

  return res;
}

cstring
describeFlag (cstring flagname)
{
  cstring oflagname = cstring_copy (flagname);
  flagcode f = flags_identifyFlag (flagname);

  if (flagcode_isSkip (f))
    {
      cstring_free (oflagname);
      return cstring_undefined;
    }
  else if (flagcode_isValid (f))
    {
      if (cstring_equal (flagname, oflagname))
	{
	  cstring_free (oflagname);
	  return (message ("%s\n\3%q", flagname, describeFlagCode (f)));
	}
      else
	{
	  return (message ("%q (standardized name: %s)\n\3%q",
			   oflagname, flagname, describeFlagCode (f)));
	}
    }
  else
    {
      if (flags_isModeName (flagname))
	{
	  cstring_free (oflagname);
	  return describeMode (flagname);
	}
      else
	{
	  return (message ("%q: <invalid flag>", oflagname));
	}
    }
}

static cstringSList
sortedFlags (void)
{
  cstringSList s = cstringSList_new ();

  allFlags (f)
    {
      if (f.desc != NULL)
	{
	  s = cstringSList_add (s, cstring_fromChars (f.flag));
	}
    } end_allFlags;

  cstringSList_alphabetize (s);

  return s;
}

void printAlphaFlags ()
{
  cstringSList fl = sortedFlags ();

  cstringSList_printSpaced (fl, 3, 1, context_getLineLen () - 25); 
  cstringSList_free (fl);
}

/*@observer@*/ cstring
flagcode_unparse (flagcode code)
{
  if (code == INVALID_FLAG)
    {
      return cstring_makeLiteralTemp ("<invalid flag>");
    }

  return cstring_fromChars (flags[code].flag);
}

/*
** Transforms a flag into its cannonical form.
**
** The following transformations are made:
**
**    function      -> fcn
**    variable      -> var
**    constant      -> const
**    iterator      -> iter
**    parameter     -> param
**    unrecognized  -> unrecog
**    qualifier     -> qual         
**    declaration   -> decl
**    globalias     -> (no change)
**    global        -> glob
**    modifies      -> mods
**    modify        -> mod
**    pointer       -> ptr
**    implies       -> imp
**    implicit      -> imp
**    implied       -> imp
**    unconstrained -> unspec       
**    unconst       -> unspec
**    memory        -> mem
**    length        -> len
*/

static /*@only@*/ cstring
canonicalizeFlag (cstring s)
{
  int i = 0;
  cstring res = cstring_copy (s);
  static bn_mstring transform[] = 
    { 
      "function", "fcn",
      "variable", "var",
      "constant", "const",
      "iterator", "iter",
      "parameter", "param",
      "unrecognized", "unrecog",
      "qualifier", "qual",
      "declaration", "decl",
      "globals", "globs", 
      "modifies", "mods", 
      "modify", "mod",
      "pointer", "ptr",
      "implies", "imp",
      "implicit", "imp",
      "implied", "imp",
      "unconstrained", "uncon",
      "unconst", "uncon",
      "memory", "mem",
      "length", "len",
      "return", "ret",
      "system", "sys",
      NULL
      } ;
  char *current;
  
  while ((current = transform[i]) != NULL)
    {
      if (cstring_containsLit (res, current))
	{
	  cstring_replaceLit (res, current, transform[i+1]);
	}
      i += 2;
    }

  /* remove whitespace, -'s, and _'s */
  cstring_stripChars (res, " -_");
  return res;
}

flagcode
flags_identifyFlag (cstring s)
{
  return flags_identifyFlagAux (s, FALSE);
}

flagcode
flags_identifyFlagQuiet (cstring s)
{
  return flags_identifyFlagAux (s, TRUE);
}

static flagcode
flags_identifyFlagAux (cstring s, bool quiet)
{
  cstring cflag;
  flagcode res;

  if (cstring_length (s) == 0) {
    /* evs 2000-06-25: A malformed flag. */
    return INVALID_FLAG;
  }

  if (cstring_firstChar (s) == 'I')
    {
      return FLG_INCLUDEPATH; /* no space required after -I */
    }

  if (cstring_firstChar (s) == 'S') 
    {
      return FLG_SPECPATH;    /* no space required after -S */
    }

  if (cstring_firstChar (s) == 'D') 
    {
      return FLG_DEFINE;      /* no space required after -D */
    }

  if (cstring_firstChar (s) == 'U') 
    {
      return FLG_UNDEFINE;    /* no space required after -D */
    }

  cflag = canonicalizeFlag (s);
  res = INVALID_FLAG;

  allFlags (f)
    {
      if (cstring_equal (cstring_fromChars (f.flag), cflag))
	{
	  res = f.code;
	  break;
	}
    } end_allFlags;
  
  if (res == INVALID_FLAG)
    {
      /*
      ** Synonyms
      */
      
      if (cstring_equalLit (cflag, "pred"))
	{
	  res = FLG_PREDBOOL;
	}
      else if (cstring_equalLit (cflag, "modobserverstrict"))
	{
	  res = FLG_MODOBSERVERUNCON;
	}
      else if (cstring_equalLit (cflag, "czechnames"))
	{
	  res = FLG_CZECH;
	}
      else if (cstring_equalLit (cflag, "slovaknames"))
	{
	  res = FLG_SLOVAK;
	}
      else if (cstring_equalLit (cflag, "czechoslovaknames"))
	{
	  res = FLG_CZECHOSLOVAK;
	}
      else if (cstring_equalLit (cflag, "globunspec")
	       || cstring_equalLit (cflag, "globuncon"))
	{
	  res = FLG_GLOBUNSPEC;
	}
      else if (cstring_equalLit (cflag, "modglobsunspec")
	       || cstring_equalLit (cflag, "modglobsuncon")
	       || cstring_equalLit (cflag, "modglobsnomods"))
	{
	  res = FLG_MODGLOBSUNSPEC;
	}
      else if (cstring_equalLit (cflag, "export"))
	{
	  res = FLG_EXPORTANY;
	}
      else if (cstring_equalLit (cflag, "macrospec"))
	{
	  res = FLG_MACRODECL;
	}
      else if (cstring_equalLit (cflag, "ansireservedlocal"))
	{
	  res = FLG_ISORESERVEDLOCAL;
	}
      else if (cstring_equalLit (cflag, "warnposix"))
	{
	  res = FLG_WARNPOSIX;
	}
      else if (cstring_equalLit (cflag, "defuse"))
	{
	  res = FLG_USEDEF;
	}
      else if (cstring_equalLit (cflag, "macroundef"))
	{
	  res = FLG_MACROUNDEF;
	}
      else if (cstring_equalLit (cflag, "showcol"))
	{
	  res = FLG_SHOWCOL;
	}
      else if (cstring_equalLit (cflag, "intbool"))
	{
	  res = FLG_BOOLINT;
	}
      else if (cstring_equalLit (cflag, "intchar"))
	{
	  res = FLG_CHARINT;
	}
      else if (cstring_equalLit (cflag, "intenum"))
	{
	  res = FLG_ENUMINT;
	}
      else if (cstring_equalLit (cflag, "intlong"))
	{
	  res = FLG_LONGINT;
	}
      else if (cstring_equalLit (cflag, "intshort"))
	{
	  res = FLG_SHORTINT;
	}
      /*
      ** Backwards compatibility for our American friends...
      */
      
      else if (cstring_equalLit (cflag, "ansilib"))
	{
	  res = FLG_ANSILIB;
	}
      else if (cstring_equalLit (cflag, "ansistrictlib"))
	{
	  res = FLG_STRICTLIB;
	}
      else if (cstring_equalLit (cflag, "skipansiheaders"))
	{
	  res = FLG_SKIPISOHEADERS;
	}
      else if (cstring_equalLit (cflag, "ansireserved"))
	{
	  res = FLG_ISORESERVED;
	}
      else if (cstring_equalLit (cflag, "ansireservedinternal"))
	{
	  res = FLG_ISORESERVEDLOCAL;
	}
      
      /*
      ** Obsolete Flags
      */
      
      else if (cstring_equalLit (cflag, "accessunspec"))
	{
	  if (!quiet) 
	    {
	      llerror_flagWarning 
		(cstring_makeLiteral
		 ("accessunspec flag is no longer supported.  It has been replaced by accessmodule, accessfile and "
		  "accessfunction to provide more precise control of accessibility "
		  "of representations.  For more information, "
		  "see splint -help accessmodule"));
	    }
	  
	  res = SKIP_FLAG;
	}
      else if (cstring_equalLit (cflag, "ansilimits"))
	{
	  llerror_flagWarning 
	    (cstring_makeLiteral
	     ("ansilimits flag is no longer supported.  It has been replaced by ansi89limits and "
	      "iso99limits to select either the lower translation limits imposed by the ANSI89 "
	      "standard or the typically higher limits prescribed by ISO C99."));
	  
	  res = SKIP_FLAG;
	}
      else if (cstring_equalLit (cflag, "staticmods"))
	{
	  if (!quiet) 
	    {
	      llerror_flagWarning 
		(cstring_makeLiteral
		 ("staticmods flag is obsolete.  You probably "
		  "want impcheckmodstatics.  For more information, "
		  "see splint -help impcheckmodstatics"));
	    }
	  
	  res = SKIP_FLAG;
	}
      else if (cstring_equalLit (cflag, "bool"))
	{
	  if (!quiet) 
	    {
	      llerror_flagWarning
		(cstring_makeLiteral ("bool flag is obsolete.  It never really "
				      "made sense in the first place."));
	    }
	  
	  res = SKIP_FLAG;
	}
      else if (cstring_equalLit (cflag, "shiftsigned"))
	{
	  if (!quiet) 
	    {
	      llerror_flagWarning
		(cstring_makeLiteral ("shiftsigned flag is obsolete.  You probably "
				      "want bitwisesigned, shiftnegative or shiftimplementation."));
	    }
	  
	  res = SKIP_FLAG;
	}
      else if (cstring_equalLit (cflag, "ansi"))
	{
	  if (!quiet) 
	    {
	      llerror_flagWarning
		(cstring_makeLiteral ("ansi flag is obsolete.  You probably "
				      "want noparams and/or oldstyle."));
	    }
	  
	  res = SKIP_FLAG;
	}
      else if (cstring_equalLit (cflag, "usestderr"))
	{
	  if (!quiet)
	    {
	      llerror_flagWarning 
		(cstring_makeLiteral
		 ("usestderr flag is obsolete. This has been replaced "
		  "by more precise flags for controlling the warning, "
		  "status message and fatal error streams independently: message-stream-stdout, "
		  "message-stream-stderr, message-stream <file>, "
		  "warning-stream-stdout, warning-stream-stderr, warning-stream <file>, "
		  "error-stream-stdout, error-stream-stderr, error-stream <file>."));
	    }
	  
	  res = SKIP_FLAG;
	}

      else if (cstring_equalLit (cflag, "stdio"))
	{
	  if (!quiet) 
	    {
	      llerror_flagWarning 
		(cstring_makeLiteral
		 ("stdio flag is obsolete.  You may "
		  "want strictlib or one of the gloabls "
		  "checking flags.  For more information, "
		  "see splint -help strictlib or splint -help flags globals"));
	    }
	  
	  res = SKIP_FLAG;
	}
      else if (flags_isModeName (cflag))
	{
	  res = MODENAME_FLAG;
	}
      else
	{
	  res = INVALID_FLAG;
	}
    }

  cstring_free (cflag);
  return res;
}

void flags_setValueFlag (flagcode opt, cstring arg)
{
  switch (opt)
    {
    case FLG_EXPECT:
    case FLG_LCLEXPECT:
    case FLG_LIMIT:  
    case FLG_LINELEN:
    case FLG_INDENTSPACES:
    case FLG_LOCINDENTSPACES:
    case FLG_BUGSLIMIT:
    case FLG_EXTERNALNAMELEN:
    case FLG_INTERNALNAMELEN:
    case FLG_CONTROLNESTDEPTH:
    case FLG_STRINGLITERALLEN:
    case FLG_NUMSTRUCTFIELDS:
    case FLG_NUMENUMMEMBERS:
    case FLG_INCLUDENEST:
      {
	int val = cstring_toPosInt (arg);

	if (val < 0)
	  {
	    llerror 
	      (FLG_BADFLAG,
	       message 
	       ("Flag %s must be followed by a positive number number.  "
		"Followed by %s",
		flagcode_unparse (opt), arg));
	  }
	else
	  {
	    context_setValueAndFlag (opt, val);
	  }
      }
      break;
    case FLG_COMMENTCHAR:
      {
	if (cstring_length (arg) != 1)
	  {
	    llfatalerrorLoc
	      (message
	       ("Flag %s should be followed by a single character.  Followed by %s",
		flagcode_unparse (opt), arg));
	  }
	else
	  {
	    context_setCommentMarkerChar (cstring_firstChar (arg));
	  }
      }
      break;
      BADDEFAULT;
    }
}

void flags_setStringFlag (flagcode opt, /*@only@*/ cstring arg)
{
  switch (opt)
    {
    case FLG_TMPDIR:
      {
	if (cstring_lastChar (arg) == CONNECTCHAR)
	  {
	    context_setString (opt, arg);
	  }
	else
	  {
	    context_setString (opt, cstring_appendChar (arg, CONNECTCHAR));
	  }
	break;
      }
    default:
      {
	context_setString (opt, arg);
	break;
      }
    }
}

cstring
describeModes ()
{
  cstring s = cstring_makeLiteral ("Flag                    ");
  cstringSList sflags = sortedFlags ();

  allModes (modename)
    {
      s = message ("%q%9s", s, cstring_fromChars (modename));
    } end_allModes;
  
  s = message ("%q\n", s);

  cstringSList_elements (sflags, flagname)
    {
      flagcode code = flags_identifyFlag (flagname);
      fflag currentflag = flags[code];
      
      if (mstring_isDefined (currentflag.desc) && flagcode_isModeFlag (code))
	{
	  s = message ("%q\n%27s", s, 
		       cstring_fromChars (currentflag.flag));
	  
	  allModes (modename)
	    {
	      context_setMode (cstring_fromChars (modename));
	      
	      if (context_getFlag (code))
		{
		  s = message ("%q%9s", s, cstring_makeLiteralTemp ("+"));
		}
	      else
		{
		  s = message ("%q%9s", s, cstring_makeLiteralTemp (" "));
		}

	      context_resetModeFlags ();
	    } end_allModes;
	}
    } end_cstringSList_elements;
  
  cstringSList_free (sflags);

  s = cstring_appendChar (s, '\n');

  return (s);
}

# if 0
static /*@unused@*/ cstring
listModes (void)
{
  cstring s = cstring_makeLiteral ("\t");
  int i = 0;

  allModes (modename)
    {
      if (i != 0 && (i % 4 == 0))
	{
	  s = message ("%q\n\t%15s", s, cstring_fromChars (modename));
	}
      else
	{
	  s = message ("%q%15s", s, cstring_fromChars (modename));
	}
      i++;
    } end_allModes;

  return s;
}
# endif

bool
flags_isModeName (cstring s)
{
  allModes (modename)
    {
      if (mstring_isDefined (modename))
	{
	  if (cstring_equalLit (s, modename))
	    {
	      return TRUE;
	    }
	}
     } end_allModes;

  return FALSE;
}

extern bool flagcode_hasArgument (flagcode f)
{
  return (flags[f].argtype != ARG_NONE);
}

extern bool flagcode_hasNumber (flagcode f)
{
  return (flags[f].argtype == ARG_NUMBER);
}

extern bool flagcode_hasChar (flagcode f)
{
  return (flags[f].argtype == ARG_CHAR);
}

extern bool flagcode_hasString (flagcode f)
{
  return (flags[f].argtype == ARG_STRING
	  || flags[f].argtype == ARG_FILE
	  || flags[f].argtype == ARG_DIRECTORY
	  || flags[f].argtype == ARG_PATH);
}

extern int flagcode_valueIndex (flagcode f)
{
  /*@unchecked@*/ static bool initialized = FALSE;
  int i;
  /*@unchecked@*/ static flagcode valueFlags[NUMVALUEFLAGS];
  
  if (!initialized)
    {
      int nv = 0;

      allFlagCodes (code)
	{
	  if (flagcode_hasNumber (code) || flagcode_hasChar (code))
	    {
	      llassert (nv < NUMVALUEFLAGS);
	      DPRINTF (("Value flag: %s [%d]", flagcode_unparse (code), (int) code));
	      valueFlags[nv] = code;
	      nv++;
	    }
	} end_allFlagCodes;

      llassertprint (nv == NUMVALUEFLAGS,
		     ("Number of value flags: %d (expected %d)",
		      nv, (int) NUMVALUEFLAGS));
      initialized = TRUE;
    }

  for (i = 0; i < NUMVALUEFLAGS; i++)
    {
      /* static valueFlags must be defined */
      /*@-usedef@*/
      if (f == valueFlags[i]) /*@=usedef@*/
	{
	  return i;
	}
    }

  fprintf (stderr, "Cannot find value flag: %d", (int) f);
  exit (EXIT_FAILURE);
  /* Cannot do this...might call recursively...
  llfatalbug (message ("Cannot fine value flag: %d", (int) f));
  BADEXIT;
  */
}

extern int flagcode_stringIndex (flagcode f)
{
  /*@unchecked@*/ static bool initialized = FALSE;
  /*@unchecked@*/ static flagcode stringFlags[NUMSTRINGFLAGS];
  int i;


  if (!initialized)
    {
      int nv = 0;

      allFlagCodes (code)
	{
	  if (flagcode_hasString (code))
	    {
	      llassertprint (nv < NUMSTRINGFLAGS, ("Incorrect number of string flags: %d (need at least %d)", NUMSTRINGFLAGS, nv));
	      stringFlags[nv] = code;
	      nv++;
	    }
	} end_allFlagCodes;

      llassertprint (nv == NUMSTRINGFLAGS,
		     ("number of string flags: %d (expected %d)",
		      nv, NUMSTRINGFLAGS));
      initialized = TRUE;
    }

  for (i = 0; i < NUMSTRINGFLAGS; i++)
    {
      /*@-usedef@*/ if (f == stringFlags[i]) /*@=usedef@*/
	{
	  return i;
	}
    }

  llbug (message ("Bad string flag: %s", flagcode_unparse (f)));
  BADEXIT;
}

bool flagcode_isNamePrefixFlag (flagcode f)
{
  switch (f)
    {
    case FLG_MACROVARPREFIX:
    case FLG_TAGPREFIX:
    case FLG_ENUMPREFIX:
    case FLG_FILESTATICPREFIX:
    case FLG_GLOBPREFIX:
    case FLG_TYPEPREFIX:
    case FLG_EXTERNALPREFIX:
    case FLG_LOCALPREFIX:
    case FLG_UNCHECKEDMACROPREFIX:
    case FLG_CONSTPREFIX:
    case FLG_ITERPREFIX:
    case FLG_DECLPARAMPREFIX:
      return TRUE;
    default:
      return FALSE;
    }
}

static cstring findLarchPathFile (/*@temp@*/ cstring s)
{
  cstring pathName;
  filestatus status;
  
  status = osd_getPath (context_getLarchPath (), s, &pathName);
  
  if (status == OSD_FILEFOUND)
    {
      return pathName;
    }
  else if (status == OSD_FILENOTFOUND)
    {
      showHerald ();
      lldiagmsg	(message ("Cannot find file on LARCH_PATH: %s", s));
    }
  else if (status == OSD_PATHTOOLONG)
    {
      /* Directory and filename are too long.  Report error. */
      llbuglit ("soure_getPath: Filename plus directory from search path too long");
    }
  else
    {
      BADBRANCH;
    }

  return cstring_undefined;
}

static void addLarchPathFile (fileIdList files, /*@temp@*/ cstring s)
{
  cstring pathName = findLarchPathFile (s);

  if (cstring_isDefined (pathName))
    {
      if (fileTable_exists (context_fileTable (), pathName))
	{
	  showHerald ();
	  lldiagmsg (message ("File listed multiple times: %s", pathName));
	  cstring_free (pathName);
	}
      else
	{
	  fileIdList_add (files, fileTable_addFileOnly (context_fileTable (), pathName));
	}
    }
}


static void addFile (fileIdList files, /*@only@*/ cstring s)
{
  if (fileTable_exists (context_fileTable (), s))
    {
      showHerald ();
      lldiagmsg (message ("File listed multiple times: %s", s));
      cstring_free (s);
    }
  else
    {
      fileIdList_add (files, fileTable_addFileOnly (context_fileTable (), s));
    }
}

static void addXHFile (fileIdList files, /*@temp@*/ cstring s)
{
  cstring pathName = findLarchPathFile (s);

  if (cstring_isDefined (pathName))
    {
      if (fileTable_exists (context_fileTable (), pathName))
	{
	  showHerald ();
	  lldiagmsg (message ("File listed multiple times: %s", s));
	}
      else
	{
	  fileIdList_add (files, fileTable_addXHFile (context_fileTable (), pathName));
	}
    }

  cstring_free (pathName);
}

void
flags_processFlags (bool inCommandLine, 
		    fileIdList xfiles,
		    fileIdList cfiles,
		    fileIdList lclfiles,
		    fileIdList mtfiles,
		    cstringList *passThroughArgs,
		    int argc, char **argv)
{
  int i;
  cstringSList fl = cstringSList_undefined;
    
  for (i = 0; i < argc; i++)
    {
      char *thisarg;

      llassert (argv != NULL);
      thisarg = argv[i];
      
      DPRINTF (("process thisarg [%d]: %s", i, thisarg));

      if (*thisarg == '-' || *thisarg == '+')
	{
	  bool set = (*thisarg == '+');
	  cstring flagname;
	  flagcode opt;

	  if (*(thisarg + 1) == '-') { /* allow -- before flags */
	    flagname = cstring_fromChars (thisarg + 2);
	  } else {
	    flagname = cstring_fromChars (thisarg + 1);
	  }

	  opt = flags_identifyFlag (flagname);
	  DPRINTF (("Flag [%s]: %s", flagname, flagcode_unparse (opt)));
	  
	  if (flagcode_isInvalid (opt))
	    {
	      DPRINTF (("Error!"));
	      voptgenerror (FLG_BADFLAG,
			    message ("Unrecognized option: %s", 
				     cstring_fromChars (thisarg)),
			    g_currentloc);
	    }
	  else if (flagcode_isHelpFlag (opt))
	    {
	      if (inCommandLine)
		{
		  voptgenerror (FLG_BADFLAG,
				message ("Help flag must be first on the command line: %s", 
					 cstring_fromChars (thisarg)),
				g_currentloc);
		}
	      else
		{
		  voptgenerror (FLG_BADFLAG,
				message ("Help flags can only be used on the command line: %s", 
					 cstring_fromChars (thisarg)),
				g_currentloc);
		}
	    }
	  else if (flagcode_isPassThrough (opt)) /* preprocessor flag: -D or -U */
	    { 
	      /*
	      ** Following space is optional, don't include the -
	      */
	      
	      *passThroughArgs = cstringList_add (*passThroughArgs, 
						  cstring_fromCharsNew (thisarg + 1));
	    }
	  else if (opt == FLG_INCLUDEPATH || opt == FLG_SPECPATH)
	    {
	      if (mstring_length (thisarg) < 2) {
		BADBRANCH;
	      } else {
		if (mstring_equal (thisarg, "-I-")) {
		  cppAddIncludeDir (cstring_fromChars (thisarg)); /* Need to handle this specially. */
		} else {
		  cstring dir = cstring_suffix (cstring_fromChars (thisarg), 2); /* skip over -I */
		  
		  DPRINTF (("Length of thisarg [%s] %d", thisarg, cstring_length (thisarg)));
		  
		  if (cstring_length (dir) == 0) {
		    DPRINTF (("space after directory: "));
		    if (++i < argc) {
		      dir = cstring_fromChars (argv[i]);
		    } else {
		      voptgenerror
			(FLG_BADFLAG,
			 message
			 ("Flag %s must be followed by a directory name",
			  flagcode_unparse (opt)),
			 g_currentloc);
		    }
		  } 
		  
		  DPRINTF (("Got directory: [%s]", dir));
		  
		  switch (opt)
		    {
		    case FLG_INCLUDEPATH:
		      cppAddIncludeDir (dir);
		      /*@switchbreak@*/ break;
		    case FLG_SPECPATH:
		      /*@-mustfree@*/
		      g_localSpecPath = cstring_toCharsSafe
			(message ("%s%h%s", 
				  cstring_fromChars (g_localSpecPath), 
				  PATH_SEPARATOR,
				  dir));
		      /*@=mustfree@*/
		      /*@switchbreak@*/ break;
		      BADDEFAULT;
		    }
		}
	      }
	    }
	  else if (flagcode_isModeName (opt))
	    {
	      context_setMode (flagname);
	    }
	  else if (inCommandLine && flagcode_isMessageControlFlag (opt))
	    {
	      /*
	      ** Processed on first pass
	      */
	      
	      if (flagcode_hasArgument (opt))
		{
		  ++i;
		}
	    }
	  else
	    {
	      /*
	      ** A normal control flag
	      */

	      context_userSetFlag (opt, set);
	      
	      if (flagcode_hasArgument (opt))
		{
		  if (flagcode_hasNumber (opt))
		    {
		      if (++i < argc)
			{
			  flags_setValueFlag (opt, cstring_fromCharsNew (argv[i]));
			}
		      else
			{
			  voptgenerror
			    (FLG_BADFLAG,
			     message
			     ("Flag %s must be followed by a number",
			      flagcode_unparse (opt)),
			     g_currentloc);
			}
		    } 
		  else if (flagcode_hasChar (opt))
		    {
		      if (++i < argc)
			{
			  flags_setValueFlag (opt, cstring_fromCharsNew (argv[i]));
			}
		      else
			{
			  voptgenerror
			    (FLG_BADFLAG,
			     message
			     ("Flag %s must be followed by a character",
			      flagcode_unparse (opt)),
			     g_currentloc);
			}
		    } 
		  else if (flagcode_hasString (opt)
			   || opt == FLG_INIT || opt == FLG_OPTF)
		    {
		      if (++i < argc)
			{
			  /*drl 10/21/2002
			    Changed this because arg can be freed when it's passed to
			    lslinit_setInitFile and freeing argv[i] causes a seg fault
			  */
			  cstring arg =  cstring_fromCharsNew (argv[i]);
			  
			  if (opt == FLG_OPTF)
			    {
			      if (inCommandLine)
				{
				  ; /* -f already processed */
				}
			      else
				{
				  (void) rcfiles_read (arg, passThroughArgs, TRUE);
				}
			    }
			  else if (opt == FLG_INIT)
			    {
			      lslinit_setInitFile (inputStream_create 
						   (arg, 
						    cstring_makeLiteralTemp (LCLINIT_SUFFIX),
						    FALSE));
			      break;
			    }
			  else
			    {
			      DPRINTF (("String flag: %s / %s",
					flagcode_unparse (opt), arg));
			      if (opt == FLG_MTSFILE)
				{
				  /*
				  ** arg identifies mts files
				  */
				  cstring tmp =  message ("%s%s", arg, MTS_EXTENSION);
				  addLarchPathFile (mtfiles, tmp);
				  cstring_free (tmp);
				  tmp = message ("%s%s", arg, XH_EXTENSION);
				  addXHFile (xfiles, tmp);
				  cstring_free (tmp);
				}
			      else
				{
				  flags_setStringFlag (opt, cstring_copy (arg));
				}
			    }
			}
		      else
			{
			  voptgenerror
			    (FLG_BADFLAG,
			     message
			     ("Flag %s must be followed by a string",
			      flagcode_unparse (opt)),
			     g_currentloc);
			}
		    }
		  else
		    {
		      /* no argument */
		    }
		}
	    }
	}
      else /* its a filename */
	{
	  DPRINTF (("Adding filename: %s", thisarg));
	  fl = cstringSList_add (fl, cstring_fromChars (thisarg));
	}
    }
  
  /*
  ** create lists of C and LCL files
  */
  
  if (inCommandLine)
    {
      cstringSList_elements (fl, current)
	{
	  cstring ext = fileLib_getExtension (current);
	  
	  if (cstring_isUndefined (ext))
	    {
	      /* no extension --- both C and LCL with default extensions */
	      
	      addFile (cfiles, message ("%s%s", current, C_EXTENSION));
	      addFile (lclfiles, message ("%s%s", current, LCL_EXTENSION));
	    }
	  else if (cstring_equal (ext, XH_EXTENSION))
	    {
	      addXHFile (xfiles, current);
	    }
	  else if (cstring_equal (ext, PP_EXTENSION))
	    {
	      if (!context_getFlag (FLG_NOPP))
		{
		  voptgenerror 
		    (FLG_FILEEXTENSIONS,
		     message ("File extension %s used without +nopp flag (will be processed as C source code): %s", 
			      ext, current),
		     g_currentloc);
		}
	      
	      addFile (cfiles, cstring_copy (current));
	    }
	  else if (cstring_equal (ext, LCL_EXTENSION)) 
	    {
	      addFile (lclfiles, cstring_copy (current));
	    }
	  else if (fileLib_isCExtension (ext))
	    {
	      addFile (cfiles, cstring_copy (current));
	    }
	  else if (cstring_equal (ext, MTS_EXTENSION))
	    {
	      addLarchPathFile (mtfiles, current);
	    }
	  else 
	    {
	      voptgenerror 
		(FLG_FILEEXTENSIONS,
		 message ("Unrecognized file extension: %s (assuming %s is C source code)", 
			  current, ext),
		 g_currentloc);
	      
	      addFile (cfiles, cstring_copy (current));
	    }
	} end_cstringSList_elements;
    }
  else
    {
      if (cstringSList_size (fl) != 0)
	{
	  /* Cannot list files in .splintrc files */
	  voptgenerror (FLG_BADFLAG, 
			message ("Cannot list files in .splintrc files: %s (probable missing + or -)",
				 cstringSList_unparse (fl)),
			g_currentloc);
	}
    }

  cstringSList_free (fl); /* evans 2002-07-12: why wasn't this reported!?? */
}

int flagcode_priority (/*@unused@*/ flagcode code)
{
  /*
  ** For now, we do a really simple prioritization: all are 1
  */

  return 1;
}
