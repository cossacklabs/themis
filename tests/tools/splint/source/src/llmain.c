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
** llmain.c
**
** Main module for Splint annotation-assisted program checker
*/

# include <signal.h>
# include <time.h>
/*
** Ensure that WIN32 and _WIN32 are both defined or both undefined.
*/

# ifdef WIN32
# ifndef _WIN32
# error "Inconsistent definitions."
# endif
# else
# ifdef _WIN32
# error "Inconsistent definitions."
# endif
# endif

# ifdef WIN32
# include <windows.h>
# include <process.h>
# endif

# include "splintMacros.nf"
# include "basic.h"
# include "osd.h"
# include "help.h"
# include "gram.h"
# include "lclscan.h"
# include "scanline.h"
# include "lclscanline.h"
# include "lclsyntable.h"
# include "lcltokentable.h"
# include "lslparse.h"
# include "scan.h"
# include "syntable.h"
# include "tokentable.h"
# include "lslinit.h"
# include "lclinit.h"
# include "lh.h"
# include "imports.h"

# include "Headers/version.h" /* Visual C++ finds the wrong version.h */
# include "lcllib.h"
# include "cgrammar.h"
# include "rcfiles.h"
# include "llmain.h"

extern /*@external@*/ int yydebug;
static void cleanupFiles (void);
/*
** evans 2002-07-03: renamed from interrupt to avoid conflict with WATCOM compiler keyword
**    (Suggested by Adam Clarke)
*/

static void llinterrupt (int p_i);

static void describeVars (void);
static bool specialFlagsHelp (char *p_next);
static bool hasShownHerald = FALSE;
static char *specFullName (char *p_specfile, /*@out@*/ char **p_inpath) 
     /*@modifies *p_inpath@*/ ;

static bool anylcl = FALSE;
static clock_t inittime;

static fileIdList preprocessFiles (fileIdList, bool)
  /*@modifies fileSystem@*/ ;

static void warnSysFiles(fileIdList p_files) /*@modifies fileSystem@*/;

static
void lslCleanup (void)
   /*@globals killed g_symtab@*/
   /*@modifies internalState, g_symtab@*/
{
  /*
  ** Cleanup all the LCL/LSL.
  */

  static bool didCleanup = FALSE;

  llassert (!didCleanup);
  llassert (anylcl);

  didCleanup = TRUE;

  lsymbol_destroyMod ();
  LCLSynTableCleanup ();
  LCLTokenTableCleanup ();
  LCLScanLineCleanup ();
  LCLScanCleanup ();

  /* clean up LSL parsing */

  lsynTableCleanup ();
  ltokenTableCleanup ();
  lscanLineCleanup ();
  LSLScanCleanup ();

  symtable_free (g_symtab);
  sort_destroyMod (); 
}

static void
lslProcess (fileIdList lclfiles)
   /*@globals undef g_currentSpec, undef g_currentSpecName, g_currentloc,
              undef killed g_symtab; @*/
   /*@modifies g_currentSpec, g_currentSpecName, g_currentloc, internalState, fileSystem; @*/
{
  char *path = NULL;
  bool parser_status = FALSE;
  bool overallStatus = FALSE;
  
  lslinit_process ();
  inittime = clock ();
    
  context_resetSpecLines ();

  fileIdList_elements (lclfiles, fid)
    {
      cstring actualName = cstring_undefined;
      cstring fname = fileTable_fileName (fid);
      
      if (osd_getPath (cstring_fromChars (g_localSpecPath), 
		       fname, &actualName) == OSD_FILENOTFOUND)
	{
	  if (mstring_equal (g_localSpecPath, "."))
	    {
	      lldiagmsg (message ("Spec file not found: %q", osd_outputPath (fname)));
	    }
	  else
	    {
	      lldiagmsg (message ("Spec file not found: %q (on %s)", 
				  osd_outputPath (fname), 
				  cstring_fromChars (g_localSpecPath)));
	    }
	}
      else
	{
	  inputStream specFile;
	  /*@access cstring@*/
	  char *namePtr = actualName;

	  while (*namePtr == '.' && *(namePtr + 1) == CONNECTCHAR) 
	    {
	      namePtr += 2;
	    }
	  /*@noaccess cstring@*/

	  g_currentSpec = cstring_fromCharsNew (namePtr);

	  specFile = inputStream_create (cstring_copy (g_currentSpec),
					 LCL_EXTENSION, TRUE);
	  
	  llassert (inputStream_isDefined (specFile));
	  
	  g_currentSpecName = specFullName 
	    (cstring_toCharsSafe (g_currentSpec),
	     &path);

	  setSpecFileId (fid);
	  	  
	  displayScan (message ("reading spec %s", g_currentSpec));
	  
	  /* Open the source file */
	  
	  if (!inputStream_open (specFile))
	    {
	      lldiagmsg (message ("Cannot open file: %q",
				  osd_outputPath (inputStream_fileName (specFile))));
	      inputStream_free (specFile);
	    }
	  else
	    {
	      scopeInfo dummy_scope = (scopeInfo) dmalloc (sizeof (*dummy_scope));
	      dummy_scope->kind = SPE_INVALID;
	      
	      lhInit (specFile);
	      LCLScanReset (specFile);
	      
	      /* 
              ** Minor hacks to allow more than one LCL file to
	      ** be scanned, while keeping initializations
	      */
	      
	      symtable_enterScope (g_symtab, dummy_scope);
	      resetImports (cstring_fromChars (g_currentSpecName));
	      context_enterLCLfile ();
	      (void) lclHadNewError ();
	      
	      parser_status = (ylparse () != 0);
	      context_exitLCLfile ();
	      lhCleanup ();
	      overallStatus = parser_status || lclHadNewError (); 

	      if (context_getFlag (FLG_DOLCS))
		{
		  if (overallStatus)
		    {
 		      outputLCSFile (path, "%FAILED Output from ",
				     g_currentSpecName);
		    }
		  else
		    {
		      outputLCSFile (path, "%PASSED Output from ", 
				     g_currentSpecName);
		    }
		}

	      (void) inputStream_close (specFile);
	      inputStream_free (specFile);

	      symtable_exitScope (g_symtab);
	    }      
	}
      cstring_free (actualName);
    } end_fileIdList_elements; 
    
    /* Can cleanup lsl stuff right away */
    
    lslCleanup ();
    
    g_currentSpec = cstring_undefined;
    g_currentSpecName = NULL;
}

static void handlePassThroughFlag (char *arg)
{
  char *curarg = arg;
  char *quotechar = strchr (curarg, '\"');
  int offset = 0;
  bool open = FALSE;
  char *freearg = NULL;

  while (quotechar != NULL)
    {
      if (*(quotechar - 1) == '\\')
	{
	  char *tp = quotechar - 2;
	  bool escape = TRUE;

	  while (*tp == '\\')
	    {
	      escape = !escape;
	      tp--;
	    }
	  
	  if (escape)
	    {
	      curarg = quotechar + 1;
	      quotechar = strchr (curarg, '\"');
	      continue;
	    }
	}
      
      llassert (quotechar != NULL);
      *quotechar = '\0';
      offset = (quotechar - arg) + 2;
      
      if (open)
	{
	  arg = cstring_toCharsSafe
	    (message ("%s\"\'%s", 
		      cstring_fromChars (arg), 
		      cstring_fromChars (quotechar + 1))); 
	  freearg = arg;
	  open = FALSE;
	}
      else
	{
	  arg = cstring_toCharsSafe
	    (message ("%s\'\"%s", 
		      cstring_fromChars (arg), 
		      cstring_fromChars (quotechar + 1)));
	  freearg = arg;
	  open = TRUE;
	}
      
      curarg = arg + offset;
      quotechar = strchr (curarg, '\"');
    }

  if (open)
    {
      showHerald ();
      voptgenerror (FLG_BADFLAG,
		    message ("Unclosed quote in flag: %s",
			     cstring_fromChars (arg)),
		    g_currentloc);
    }
  else
    {
      if (arg[0] == 'D') {
	cstring def;
	
	/* 
	** If the value is surrounded by single quotes ('), remove
	** them.  This is an artifact of UNIX command line?
	*/

	def = osd_fixDefine (cstring_fromChars (arg + 1));
	DPRINTF (("Do define: %s", def));
	cppDoDefine (def);
	DPRINTF (("After define"));
	cstring_free (def);
      } else if (arg[0] == 'U') {
	cppDoUndefine (cstring_fromChars (arg + 1));
      } else {
	BADBRANCH;
      }
    }
  
  sfree (freearg);
}

void showHerald (void)
{
  if (hasShownHerald || context_getFlag (FLG_QUIET)) 
    {
      return;
    }
  else
    {
      fprintf (g_messagestream, "%s\n\n", SPLINT_VERSION);
      hasShownHerald = TRUE;
      llflush ();
    }
}

/*
** Disable MSVC++ warning about return value.  Methinks humbly splint control
** comments are a mite more legible.
*/

# ifdef WIN32
# pragma warning (disable:4035) 
# endif

int main (int argc, char *argv[])
  /*@globals killed undef g_currentloc,
	     killed       g_localSpecPath,  
	     killed undef g_currentSpec,
	     killed undef g_currentSpecName,
	     killed undef yyin,
                    undef g_warningstream, g_messagestream, g_errorstream;
   @*/
  /*@modifies g_currentloc, g_localSpecPath, g_currentSpec, g_currentSpecName, 
              fileSystem, yyin; 
  @*/
{
  bool first_time = TRUE;
  bool expsuccess;
  inputStream sourceFile = inputStream_undefined;
 
  fileIdList dercfiles;
  cstringList passThroughArgs = cstringList_undefined;
  fileIdList cfiles, xfiles, lclfiles, mtfiles;
  clock_t before, lcltime, libtime, pptime, cptime, rstime;
  int i = 0;

# ifdef __EMX__
  _wildcard (&argc, &argv);
# endif

  g_warningstream = stdout;
  g_messagestream = stderr;
  g_errorstream = stderr;

  (void) signal (SIGINT, llinterrupt);
  (void) signal (SIGSEGV, llinterrupt); 

  flags_initMod ();
  qual_initMod ();
  clabstract_initMod ();
  typeIdSet_initMod ();
  osd_initMod ();
  cppReader_initMod ();

  setCodePoint ();
  
  g_currentloc = fileloc_createBuiltin ();
    
  before = clock ();
  context_initMod ();

  context_setInCommandLine ();

  if (argc <= 1)
    {
      help_showAvailableHelp ();
      llexit (LLSUCCESS);
    }
  
  /* -help must be the first flag to get help */
  if (flagcode_isHelpFlag (flags_identifyFlag (cstring_fromChars (argv[1]))))
    {
      /*
      ** Skip first flag and help flag
      */

      help_processFlags (argc - 2, argv + 2);
      llexit (LLSUCCESS);
    }

  setCodePoint ();
  yydebug = 0;

  /*
  ** Add include directories from environment.
  */

  {
    cstring incval = cstring_copy (osd_getEnvironmentVariable (INCLUDEPATH_VAR));
    cstring oincval = incval;

    if (cstring_isDefined (incval))
      {
	/*
	** Each directory on the include path is a system include directory.
	*/

	DPRINTF (("include: %s", incval));
	context_setString (FLG_SYSTEMDIRS, cstring_copy (incval));

	while (cstring_isDefined (incval))
	  {
	    /*@access cstring@*/
	    char *nextsep = strchr (incval, PATH_SEPARATOR);

	    if (nextsep != NULL)
	      {
		cstring dir;
		*nextsep = '\0';
		dir = cstring_copy (incval);

		if (cstring_length (dir) == 0
		    || !isalpha ((int) cstring_firstChar (dir)))
		  {
		    /* 
		    ** win32 environment values can have special values,
		    ** ignore them
		    */
		  }
		else
		  {
		    cppAddIncludeDir (dir);
		  }

		*nextsep = PATH_SEPARATOR;
		incval = cstring_fromChars (nextsep + 1);
		cstring_free (dir);
	      }
	    else
	      {
		break;
	      }

	    /*@noaccess cstring@*/
	  }
      }
    else /* 2001-09-09: herbert */
      {
	/* Put C_INCLUDE_PATH directories in sysdirs */
	cstring cincval = osd_getEnvironmentVariable (cstring_makeLiteralTemp ("C_INCLUDE_PATH"));

	if (cstring_isDefined (cincval))
	  {
	    context_setString (FLG_SYSTEMDIRS, cstring_copy (cincval));
	  }
      }
    /* /herbert */

    cstring_free (oincval);
  }

  /*
  ** check RCFILE for default flags
  */

  /*
  ** Process command line message formatting flags before reading rc file
  */

  {
    cstring home = osd_getHomeDir ();
    cstring fname  = cstring_undefined;
    bool defaultf = TRUE;
    bool nof = FALSE;

    for (i = 1; i < argc; i++)
      {
	char *thisarg;
	thisarg = argv[i];
	
	if (*thisarg == '-' || *thisarg == '+')
	  {
	    bool set = (*thisarg == '+');
	    flagcode opt;

	    thisarg++;

	    /*
	    ** Don't report warnings this time
	    */

	    opt = flags_identifyFlagQuiet (cstring_fromChars (thisarg));

	    if (opt == FLG_NOF)
	      {
		nof = TRUE;
	      }
	    else if (flagcode_isMessageControlFlag (opt))
	      {
		/*
		** Need to set it immediately, so rc file scan is displayed
		*/

		context_userSetFlag (opt, set);

		if (flagcode_hasArgument (opt))
		  {
		    llassert (flagcode_hasString (opt));
		    
		    if (++i < argc)
		      {
			fname = cstring_fromChars (argv[i]);
			flags_setStringFlag (opt, fname);
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
	      }
	    else if (opt == FLG_OPTF)
	      {
		if (++i < argc)
		  {
		    defaultf = FALSE;
		    fname = cstring_fromChars (argv[i]);
		    (void) rcfiles_read (fname, &passThroughArgs, TRUE);
		  }
		else
		  llfatalerror
		    (cstring_makeLiteral ("Flag f to select options file "
					  "requires an argument"));
	      }
	    else
	      {
		; /* wait to process later */
	      }
	  }
      }
        
    setCodePoint ();

    if (!nof && defaultf)
      {
	/*
	** No explicit rc file, first try reading ~/.splintrc
	*/

	if (cstring_isUndefined (fname))
	  {
	    if (!cstring_isEmpty (home)) 
	      {
		bool readhomerc, readaltrc;
		cstring homename, altname;

		homename = message ("%s%h%s", home, CONNECTCHAR,
				 cstring_fromChars (RCFILE));
		readhomerc = rcfiles_read (homename, &passThroughArgs, FALSE);
		
		/*
		** Try ~/.splintrc also for historical accuracy
		*/
		
		altname = message ("%s%h%s", home, CONNECTCHAR,
				 cstring_fromChars (ALTRCFILE));
		readaltrc = rcfiles_read (altname, &passThroughArgs, FALSE);

		if (readhomerc && readaltrc)
		  {

		    voptgenerror 
		      (FLG_WARNRC,
		       message ("Found both %s and %s files. Using both files, "
				"but recommend using only %s to avoid confusion.",
				homename, altname, homename),
		       g_currentloc);
		  }

		cstring_free (homename);
		cstring_free (altname);
	      }
	  }
	
	/*
	** Next, read .splintrc in the current working directory
	*/
	
	{
	  cstring rcname = message ("%s%s",osd_getCurrentDirectory (), cstring_fromChars (RCFILE));
	  cstring altname = message ("%s%s",osd_getCurrentDirectory (), cstring_fromChars (ALTRCFILE));
	  bool readrc, readaltrc;
	  
	  readrc = rcfiles_read (rcname, &passThroughArgs, FALSE);
	  readaltrc = rcfiles_read (altname, &passThroughArgs, FALSE);
	  
	  if (readrc && readaltrc)
	    {
	      voptgenerror (FLG_WARNRC,
			    message ("Found both %s and %s files. Using both files, "
				     "but recommend using only %s to avoid confusion.",
				     rcname, altname, rcname),
			    g_currentloc);
	      
	    }

	  cstring_free (rcname);
	  cstring_free (altname);
	}
      }
  }
  
  setCodePoint ();
  llassert (fileloc_isBuiltin (g_currentloc));

  cfiles = fileIdList_create ();
  xfiles = fileIdList_create ();
  lclfiles = fileIdList_create ();
  mtfiles = fileIdList_create ();

  /* argv[0] is the program name, don't pass it to flags_processFlags */
  flags_processFlags (TRUE, xfiles, cfiles,
		      lclfiles, mtfiles, 
		      &passThroughArgs,
		      argc - 1, argv + 1);

  showHerald (); 

  if (context_getFlag (FLG_CSV)) {
    cstring fname = context_getString (FLG_CSV);

    if (cstring_isDefined (fname)) {
      if (osd_fileExists (fname) && !context_getFlag (FLG_CSVOVERWRITE)) {
	lldiagmsg (message ("Specified CSV output file already exists (use +csvoverwrite to automatically overwrite): %s",
			    fname));
      } else {
	g_csvstream = fopen (cstring_toCharsSafe (fname), "w");
	
	DPRINTF (("Creating: %s", fname));
	if (g_csvstream == NULL) {
	  lldiagmsg (message ("Cannot open file for CSV output: %s", fname));
	} else {
	  displayScan (message ("Starting CSV output file: %s", context_getString (FLG_CSV)));
	  fprintf (g_csvstream, 
		   "Warning, Flag Code, Flag Name, Priority, File, Line, Column, Warning Text, Additional Text\n");
	}
      }
    }
  }

# ifdef DOANNOTS
  initAnnots ();
# endif

  inittime = clock ();

  context_resetErrors ();
  context_clearInCommandLine ();

  anylcl = !fileIdList_isEmpty (lclfiles);

  if (context_doMerge ())
    {
      cstring m = context_getMerge ();

      displayScanOpen (message ("< loading %s ", m));
      loadState (m);
      displayScanClose ();

      if (!usymtab_existsType (context_getBoolName ()))
	{
	  usymtab_initBool (); 
	}
    }
  else
    {
      if (!context_getFlag (FLG_NOLIB) && loadStandardState ())
	{
	  ;
	}
      else
	{
	  ctype_initTable ();
	}

      /* setup bool type and constants */
      usymtab_initBool (); 
    }

  fileloc_free (g_currentloc);
  g_currentloc = fileloc_createBuiltin ();

  /*
  ** Read metastate files (must happen before loading libraries) 
  */

  fileIdList_elements (mtfiles, mtfile)
    {
      context_setFileId (mtfile);
      displayScan (message ("processing %s", fileTable_rootFileName (mtfile)));
      mtreader_readFile (cstring_copy (fileTable_fileName (mtfile)));
    } end_fileIdList_elements;

  libtime = clock ();

  if (anylcl)
    {
      lslProcess (lclfiles);
    }

  usymtab_initGlobalMarker ();

  /*
  ** pre-processing
  **
  ** call the pre-preprocessor and /lib/cpp to generate appropriate
  ** files
  **
  */

  context_setInCommandLine ();
  
  DPRINTF (("Pass through: %s", cstringSList_unparse (passThroughArgs)));
  
  cstringList_elements (passThroughArgs, thisarg)
    {
      handlePassThroughFlag (cstring_toCharsSafe (thisarg));
    } 
  end_cstringList_elements;

  cstringList_free (passThroughArgs);

  cleanupMessages ();

  DPRINTF (("Initializing cpp reader!"));
  cppReader_initialize ();
  cppReader_saveDefinitions ();
  
  context_clearInCommandLine ();

  if (!context_getFlag (FLG_NOPP))
    {
      fileIdList tfiles;

      llflush ();

      displayScanOpen (cstring_makeLiteral ("preprocessing"));
      
      lcltime = clock ();

      context_setPreprocessing ();
      dercfiles = preprocessFiles (xfiles, TRUE);
      tfiles = preprocessFiles (cfiles, FALSE);
      warnSysFiles(cfiles);
      dercfiles = fileIdList_append (dercfiles, tfiles);
      fileIdList_free (tfiles);

      context_clearPreprocessing ();

      fileIdList_free (cfiles);

      displayScanClose ();
      pptime = clock ();
    }
  else
    {
      lcltime = clock ();
      dercfiles = fileIdList_append (cfiles, xfiles);
      pptime = clock ();
    }

  /*
  ** now, check all the corresponding C files
  **
  ** (for now these are just <file>.c, but after pre-processing
  **  will be <tmpprefix>.<file>.c)
  */

  /* Why was this here?  It is always a bug... */
# if 0
  {
# if defined (WIN32) || defined (OS2) && defined (__IBMC__)
    int nfiles = /*@-unrecog@*/ _fcloseall (); /*@=unrecog@*/

    if (nfiles != 0) 
      {
	llbug (message ("Files unclosed: %d", nfiles));
      }
# endif
  }
# endif

  DPRINTF (("Initializing..."));

  exprNode_initMod ();

  DPRINTF (("Okay..."));

  fileIdList_elements (dercfiles, fid)
    {
      sourceFile = inputStream_create (cstring_copy (fileTable_fileName (fid)), C_EXTENSION, TRUE);
      context_setFileId (fid);
      
      /* Open source file  */
      
      if (inputStream_isUndefined (sourceFile) || (!inputStream_open (sourceFile)))
	{
	  /* previously, this was ignored  ?! */
	  llbug (message ("Could not open temp file: %s", fileTable_fileName (fid)));
	}
      else
	{
	  yyin = inputStream_getFile (sourceFile); /*< shared <- only */
	
	  llassert (yyin != NULL);

	  displayScan (message ("checking %q", osd_outputPath (fileTable_rootFileName (fid))));
	  
	  /*
	  ** Every time, except the first time, through the loop,
	  ** need to call yyrestart to clean up the parse buffer.
	  */

	  if (!first_time)
	    {
	      (void) yyrestart (yyin);	
	    }
	  else
	    {
	      first_time = FALSE;
	    }
	  
	  DPRINTF (("Entering..."));
	  context_enterFile ();
	  (void) yyparse ();
	  context_exitCFile ();
		    
	  (void) inputStream_close (sourceFile);
	}      

      inputStream_free (sourceFile); /* evans 2002-07-12: why no warning without this?!! */
    } end_fileIdList_elements;

  fileIdList_free (dercfiles); /* evans 2002-07-12: why no warning without this?!! */
  cptime = clock ();
  
  /* process any leftover macros */

  context_processAllMacros ();
  
  /* check everything that was specified was defined */
  
  /* don't check if no c files were processed ?
  **   is this correct behaviour?
  */
  
  displayScan (cstring_makeLiteral ("global checks"));

  cleanupMessages ();
  
  if (context_getLinesProcessed () > 0)
    {
      usymtab_allDefined ();
    }

  if (context_maybeSet (FLG_TOPUNUSED))
    {
      uentry ue = usymtab_lookupSafe (cstring_makeLiteralTemp ("main"));

      if (uentry_isValid (ue))
	{
	  uentry_setUsed (ue, fileloc_observeBuiltin ());
	}

      usymtab_allUsed ();
    }

  if (context_maybeSet (FLG_EXPORTLOCAL))
    {
      usymtab_exportLocal ();
    }

  
  if (context_maybeSet (FLG_EXPORTHEADER))
    {
      usymtab_exportHeader ();
    }

  if (context_getFlag (FLG_SHOWUSES))
    {
      usymtab_displayAllUses ();
    }

  context_checkSuppressCounts ();

  if (context_doDump ())
    {
      cstring dump = context_getDump ();

      dumpState (dump);
    }

# ifdef DOANNOTS
  printAnnots ();
# endif

  cleanupFiles ();
  
  if (g_csvstream != NULL) {
    displayScan (message ("Closing CSV file: %s", context_getString (FLG_CSV)));
    check (fclose (g_csvstream) == 0);
  }
  
  if (context_getFlag (FLG_SHOWSUMMARY))
    {
      summarizeErrors (); 
    }
  
  {
    bool isQuiet = context_getFlag (FLG_QUIET);
    cstring specErrors = cstring_undefined;
    int nspecErrors = lclNumberErrors ();
    
    expsuccess = TRUE;

    if (context_neednl ())
      fprintf (g_warningstream, "\n");
    
    if (nspecErrors > 0)
      {
	if (nspecErrors == context_getLCLExpect ())
	  {
	    specErrors = 
	      message ("%d spec warning%&, as expected\n       ", 
		       nspecErrors);
	  }
	else
	  {
	    if (context_getLCLExpect () > 0)
	      {
		specErrors = 
		  message ("%d spec warning%&, expected %d\n       ", 
			   nspecErrors,
			   (int) context_getLCLExpect ());
	      }
	    else
	      {
		specErrors = message ("%d spec warning%&\n       ",
				      nspecErrors);
		expsuccess = FALSE;
	      }
	  }
      }
    else
	{
	  if (context_getLCLExpect () > 0)
	    {
	      specErrors = message ("No spec warnings, expected %d\n       ", 
				    (int) context_getLCLExpect ());
	      expsuccess = FALSE;
	    }
	}

      if (context_anyErrors ())
	{
	  if (context_numErrors () == context_getExpect ())
	    {
	      if (!isQuiet) {
		llmsg (message ("Finished checking --- "
				"%s%d code warning%&, as expected",
				specErrors, context_numErrors ()));
	      }
	    }
	  else
	    {
	      if (context_getExpect () > 0)
		{
		  if (!isQuiet) {
		    llmsg (message 
			   ("Finished checking --- "
			    "%s%d code warning%&, expected %d",
			    specErrors, context_numErrors (), 
			    (int) context_getExpect ()));
		  }

		  expsuccess = FALSE;
		}
	      else
		{
		  
		  if (!isQuiet)
		    {
		      llmsg (message ("Finished checking --- "
				      "%s%d code warning%&", 
				      specErrors, context_numErrors ()));
		    }

		  expsuccess = FALSE;
		}
	    }
	}
      else
	{
	  if (context_getExpect () > 0)
	    {
	      if (!isQuiet) {
		llmsg (message
		       ("Finished checking --- "
			"%sno code warnings, expected %d", 
			specErrors,
			(int) context_getExpect ()));
	      }

	      expsuccess = FALSE;
	    }
	  else
	    {
	      if (context_getLinesProcessed () > 0)
		{
		  if (cstring_isEmpty (specErrors))
		    {
		      if (!isQuiet) 
			{
			  llmsg (message ("Finished checking --- no warnings"));
			} 
		    }
		  else
		    {
		      if (!isQuiet) 
			{
			  llmsg (message ("Finished checking --- %sno code warnings",
					  specErrors));
			}
		    }
		}
	      else
		{
		  if (!isQuiet) {
		    llmsg (message ("Finished checking --- %sno code processed", 
				    specErrors));
		  }
		}
	    }
	}

      cstring_free (specErrors);
  
      if (context_numBugs () > 0) {
	expsuccess = FALSE;
	if (!isQuiet) {
	  llmsg (message ("   %d internal bugs reported", context_numBugs ()));
	}
      }
  }
  
  if (context_getFlag (FLG_STATS))
    {
      clock_t ttime = clock () - before;
      int specLines = context_getSpecLinesProcessed ();
      cstring specmsg = cstring_undefined;
      
      rstime = clock ();
      
      if (specLines > 0)
	{
	  specmsg = message ("%d spec, ", specLines);
	}
      
      /* The clock might wrap around, not platform-independent easy way to deal with this... */
      if (ttime > 0)
	{
# ifndef CLOCKS_PER_SEC
	  lldiagmsg (message ("%s%d source lines in %d time steps (steps/sec unknown)\n", 
			      specmsg,
			      context_getLinesProcessed (), 
			      (int) ttime));
# else
	  lldiagmsg (message ("%s%d source lines in %f s.\n", 
			      specmsg,
			      context_getLinesProcessed (), 
			      (double) ttime / CLOCKS_PER_SEC));
	  DPRINTF (("Time: %ld [%ld - %ld]", ttime, rstime, before));
# endif
	}
      else
	{
	  lldiagmsg (message ("%s%d source lines\n", 
			      specmsg,
			      context_getLinesProcessed ()));
	}

    }
  else
    {
      rstime = clock ();
    }
  
  if (context_getFlag (FLG_TIMEDIST))
    {
      clock_t ttime = clock () - before;
      
      if (ttime > 0)
	{
	  char *msg = (char *) dmalloc (256 * sizeof (*msg));
	  
	  if (anylcl)
	    {
	      /* Gack: really should figure out how to make configure find snprintf... */
# ifdef WIN32
	      (void) _snprintf (msg, 256,
# else
	      (void) snprintf (msg, 256,
# endif
			"Time distribution (percent): initialize %.2f / lcl %.2f / "
			"pre-process %.2f / c check %.2f / finalize %.2f \n", 
			(100.0 * (double) (libtime - before) / ttime),
			(100.0 * (double) (lcltime - libtime) / ttime),
			(100.0 * (double) (pptime - lcltime) / ttime),
			(100.0 * (double) (cptime - pptime) / ttime),
			(100.0 * (double) (rstime - cptime) / ttime));
	    }
	  else
	    {
# ifdef WIN32
	      (void) _snprintf (msg, 256,
# else
	      (void) snprintf (msg, 256,
# endif
			"Time distribution (percent): initialize %.2f / "
			"pre-process %.2f / c check %.2f / finalize %.2f \n", 
			(100.0 * (double) (libtime - before) / ttime),
			(100.0 * (double) (pptime - libtime) / ttime),
			(100.0 * (double) (cptime - pptime) / ttime),
			(100.0 * (double) (rstime - cptime) / ttime));
	    }
	  
	  llgenindentmsgnoloc (cstring_fromCharsO (msg));
	}
    }

  llexit (expsuccess ? LLSUCCESS : LLFAILURE);
  BADBRANCHRET (LLFAILURE);
}

# ifdef WIN32
/*
** Reenable return value warnings.
*/
# pragma warning (default:4035)
# endif 

void
llinterrupt (int i)
{
  switch (i)
    {
    case SIGINT:
      fprintf (g_errorstream, "*** Interrupt\n");
      llexit (LLINTERRUPT);
    case SIGSEGV:
      {
	cstring loc;
	
	/* Cheat when there are parse errors */
	checkParseError (); 
	
	fprintf (g_errorstream, "*** Segmentation Violation\n");
	
	/* Don't catch it if fileloc_unparse causes a signal */
	(void) signal (SIGSEGV, NULL);

	loc = fileloc_unparse (g_currentloc);
	
	fprintf (g_errorstream, "*** Location (not trusted): %s\n", 
		 cstring_toCharsSafe (loc));
	cstring_free (loc);
	printCodePoint ();
	fprintf (g_errorstream, "*** Please report bug to %s\n*** A useful bug report should include everything we need to reproduce the bug.\n", SPLINT_MAINTAINER);
	exit (LLGIVEUP);
      }
    default:
      fprintf (g_errorstream, "*** Signal: %d\n", i);
      /*@-mustfree@*/
      fprintf (g_errorstream, "*** Location (not trusted): %s\n", 
	       cstring_toCharsSafe (fileloc_unparse (g_currentloc)));
      /*@=mustfree@*/
      printCodePoint ();
      fprintf (g_errorstream, "*** Please report bug to %s\n*** A useful bug report should include everything we need to reproduce the bug.", SPLINT_MAINTAINER);
      exit (LLGIVEUP);
    }
}

void
cleanupFiles (void)
{
  static bool doneCleanup = FALSE;

  /* make sure this is only called once! */

  if (doneCleanup) return;

  setCodePoint ();

  /*
  ** Close all open files
  **    (There should only be open files, if we exited after a fatal error.)
  */

  fileTable_closeAll (context_fileTable ());

  if (context_getFlag (FLG_KEEP))
    {
      check (fputs ("Temporary files kept:\n", g_messagestream) != EOF);
      fileTable_printTemps (context_fileTable ());
    }
  else
    {
# if defined (WIN32) || defined (OS2) && defined (__IBMC__)
      int nfiles = /*@-unrecog@*/ _fcloseall (); /*@=unrecog@*/
      
      if (nfiles != 0) 
	{
	  llbug (message ("Files unclosed: %d", nfiles));
	}
# endif
      fileTable_cleanup (context_fileTable ());
    }

  doneCleanup = TRUE;
}

/*
** cleans up temp files (if necessary) and exits
*/

/*@noreturn@*/ void
llexit (int status)
{
  DPRINTF (("llexit: %d", status));

# if defined (WIN32) || defined (OS2) && defined (__IBMC__)
  if (status == LLFAILURE) 
    {
      _fcloseall ();
    }
# endif

  cleanupFiles ();

  if (status != LLFAILURE)
    {
      usymtab_destroyMod ();

      /*drl I'm commenting this line out
	        because it is causing Splint to crash when built with
		        2.95 I'm not sure if this is a compiler bug or if if has to do with bool
			        Any way if we're going to exist the program why do we bother freeing stuff...
      */
      /*      context_destroyMod (); */
      exprNode_destroyMod ();
      cppReader_destroyMod ();
      sRef_destroyMod ();
      uentry_destroyMod ();
      typeIdSet_destroyMod ();
      qual_destroyMod ();
      osd_destroyMod ();
      fileloc_destroyMod ();
# ifdef USEDMALLOC
      dmalloc_shutdown ();
# endif
    }

  exit ((status == LLSUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE);
}

static fileIdList preprocessFiles (fileIdList fl, bool xhfiles)
  /*@modifies fileSystem@*/
{
  bool msg = (context_getFlag (FLG_SHOWSCAN) && fileIdList_size (fl) > 10);
  int skip = (fileIdList_size (fl) / 5);
  int filesprocessed = 0;
  fileIdList dfiles = fileIdList_create ();

  fileloc_free (g_currentloc);
  g_currentloc = fileloc_createBuiltin ();

  fileIdList_elements (fl, fid)
    {
      cstring ppfname = fileTable_fileName (fid);

      if (!(osd_fileIsReadable (ppfname)))
	{
	  lldiagmsg (message ("Cannot open file: %q", osd_outputPath (ppfname)));
	  ppfname = cstring_undefined;
	}

      if (cstring_isDefined (ppfname))
	{
	  fileId dfile = fileTable_addCTempFile (context_fileTable (), fid);

	  if (xhfiles)
	    {
	      llassert (fileTable_isXHFile (context_fileTable (), dfile));
	    }

	  llassert (cstring_isNonEmpty (ppfname));
	  
	  if (msg)
	    {
	      if ((filesprocessed % skip) == 0) 
		{
		  if (filesprocessed == 0) {
		    displayScanContinue (cstring_makeLiteral (" "));
		  }
		  else {
		    displayScanContinue (cstring_makeLiteral ("."));
		  }
		}
	      filesprocessed++;
	    }

	  DPRINTF (("outfile: %s", fileTable_fileName (dfile)));

	  if (cppProcess (ppfname, fileTable_fileName (dfile)) != 0) 
	    {
	      llfatalerror (message ("Preprocessing error for file: %s", 
				     fileTable_rootFileName (fid)));
	    }
	  
	  fileIdList_add (dfiles, dfile);
	}
    } end_fileIdList_elements; 
  
  return dfiles;
}

/* This should be in an lclUtils.c file... */
char *specFullName (char *specfile, /*@out@*/ char **inpath)
{
  /* extract the path and the specname associated with the given file */
  char *specname = (char *) dmalloc (sizeof (*specname) 
				     * (strlen (specfile) + 9));
  char *ospecname = specname;
  char *path = (char *) dmalloc (sizeof (*path) * (strlen (specfile)));
  size_t size;
  long int i, j;
  
  /* initialized path to empty string or may have accidental garbage */
  *path = '\0';

  /*@-mayaliasunique@*/ 
  strcpy (specname, specfile);
  /*@=mayaliasunique@*/ 

  /* trim off pathnames in specfile */
  size = strlen (specname);

  for (i = size_toInt (size) - 1; i >= 0; i--)
    {
      if (specname[i] == CONNECTCHAR)
	{
	  /* strcpy (specname, (char *)specname+i+1); */
	  for (j = 0; j <= i; j++)	/* include '/'  */
	    {
	      path[j] = specname[j];
	    }

	  path[i + 1] = '\0';
	  specname += i + 1;
	  break;
	}
    }

  /* 
  ** also remove .lcl file extension, assume it's the last extension
  ** of the file name 
  */

  size = strlen (specname);

  for (i = size_toInt (size) - 1; i >= 0; i--)
    {
      if (specname[i] == '.')
	{
	  specname[i] = '\0';
	  break;
	}
    }
  
  *inpath = path;

  /*
  ** If specname no longer points to the original char,
  ** we need to allocate a new pointer and copy the string.
  */

  if (specname != ospecname) {
    char *rspecname = (char *) dmalloc (sizeof (*rspecname) * (strlen (specname) + 1));
    strcpy (rspecname, specname); /* evs 2000-05-16: Bug: was ospecname! */
    sfree (ospecname);
    return rspecname;
  } 

  return specname;
}

void warnSysFiles(fileIdList files)
{
  fileIdList_elements (files, file)
    {
      
      if (fileTable_isSystemFile (context_fileTable (), file) )
	{
	  if (!context_getFlag( FLG_SYSTEMDIRERRORS ) )
	    {
	      voptgenerror (FLG_WARNSYSFILES, message ("Warning %s is a considered a system file. No errors in this file will be reported.", fileTable_rootFileName (file) ), g_currentloc);
	    }
	}
    } 
  end_fileIdList_elements;
}
