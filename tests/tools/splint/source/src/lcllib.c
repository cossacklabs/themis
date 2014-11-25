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
** lcllib.c
**
** routines for loading and creating library files
**
** this is a brute-force implementation, a more efficient
** representation should be designed.
**
*/

/*
 * Herbert 02/17/2002:
 * - fixed the recognition of Posix headers for OS/2
 */

# include "splintMacros.nf"
# include "basic.h"
# include "osd.h"
# include "version.h"

# include "gram.h"
# include "lclscan.h"

# include "lcllib.h"
# include "llmain.h"

/*@-incondefs@*/ /*@-redecl@*/
extern /*:open:*/ /*@dependent@*/ FILE *yyin;
/*@=incondefs@*/ /*@=redecl@*/

/*@constant int NUMLIBS; @*/
# define NUMLIBS 25

/*@constant int NUMPOSIXLIBS; @*/
# define NUMPOSIXLIBS 18

static ob_mstring posixlibs[NUMPOSIXLIBS] = 
{
  "dirent",
  "fcntl",
  "grp",
  "pwd",
  "regex",
  "sys/stat",
  "sys/times",
  "sys/types", 
  "netdb", /* unix */
  "netinet/in", /* unix */
  "sys/resource", /* unix */
  "sys/socket", /* not posix */
  "sys/syslog", /* not posix */
  "sys/utsname",
  "sys/wait",
  "termios",
  "unistd",
  "utime"
} ;

static ob_mstring stdlibs[NUMLIBS] =
{
  "assert", 
  "complex"
  "ctype",
  "errno",
  "fenv",
  "float",
  "inttypes",
  "iso646",
  "limits",
  "locale",
  "math", 
  "setjmp",
  "signal",
  "stdarg",
  "stdbool",
  "stddef",
  "stdint",
  "stdio",
  "stdlib",
  "string",
  "strings", /* some systems use this...they shouldn't */
  "tgmath",
  "time",
  "wchar",
  "wctype"
} ;

static bool loadLCDFile (FILE * p_f, cstring p_name);

bool
lcllib_isSkipHeader (cstring sname)
{
  int i;
  bool posixlib = FALSE;
  char *libname;
  char *matchname;
  cstring xname;

  llassert (cstring_isDefined (sname));
  xname = fileLib_withoutExtension (sname, cstring_makeLiteralTemp (".h"));

  DPRINTF (("Include? %s", sname));

  /*@access cstring@*/
  llassert (cstring_isDefined (xname));
# if defined (OS2)
  {
    /* Posixlibs use forward slashes, so we use them here, too */
    cstring_replaceAll (xname, '\\', '/');
    libname = strrchr (xname, '/');
    DPRINTF (("libname: %s", libname));
  }
# else
  libname = strrchr (xname, CONNECTCHAR);
# endif
  matchname = libname;

  if (libname == NULL) 
    {
      libname = xname;
    }
  else
    {
      libname++;
      /*@-branchstate@*/
    }
  /*@=branchstate@*/

  if (mstring_equal (libname, "varargs"))
    {
      fileloc tmp = fileloc_makePreprocPrevious (g_currentloc);
      
      voptgenerror 
	(FLG_USEVARARGS,
	 message ("Include file <%s.h> is inconsistent with "
		  "ANSI library (should use <stdarg.h>)",
		  cstring_fromChars (libname)),
	 tmp);
      
      fileloc_free (tmp);
      cstring_free (xname);
      return TRUE;
    }

  if (context_getFlag (FLG_SKIPISOHEADERS)
      && context_usingAnsiLibrary ())
    {
      for (i = 0; i < NUMLIBS; i++)
	{
	  if (mstring_equal (libname, stdlibs[i]))
	    {
	      sfree (xname);
	      return TRUE;
	    }
	}
    }

  for (i = 0; i < NUMPOSIXLIBS; i++)
    {
      if (strchr (posixlibs[i], CONNECTCHAR) != NULL
# if defined (OS2)
	  || strchr (posixlibs[i], ALTCONNECTCHAR) != NULL
# endif
	  )
	{
	  char *ptr;
	  
	  DPRINTF (("xname: %s, posix: %s", xname, posixlibs[i]));
	  if ((ptr = strstr (xname, posixlibs[i])) != NULL) 
	    {
	      if (ptr[strlen (posixlibs[i])] == '\0')
		{
		  posixlib = TRUE;
		  matchname = ptr;
		  break;
		}
	      else
		{
		  ; /* no match */
		}
	    }
	}
      else
	{
	  if (mstring_equal (libname, posixlibs[i]))
	    {
	      posixlib = TRUE;
	      matchname = libname;
	      break;
	    }
	  /*@-branchstate@*/ 
	}
    } /*@=branchstate@*/
  
  if (posixlib)
    {
      if (context_usingPosixLibrary ())
	{
	  if (context_getFlag (FLG_SKIPPOSIXHEADERS))
	    {
	      cstring_free (xname);
	      /*@-nullstate@*/ 
	      return TRUE; 
	      /*@=nullstate@*/

	      /* evans 2002-03-02: 
		   the returned reference is possibly null,
  		   but this should not change the null state of the parameter
	      */
	    }
	}
      else
	{	
	  fileloc tmp = fileloc_makePreprocPrevious (g_currentloc);	      
	  
	  voptgenerror 
	    (FLG_WARNPOSIX,
	     message ("Include file <%s.h> matches the name of a "
		      "POSIX library, but the POSIX library is "
		      "not being used.  Consider using +posixlib "
		      "or +posixstrictlib to select the POSIX "
		      "library, or -warnposix "
		      "to suppress this message.",
		      cstring_fromChars (matchname)),
	     tmp);
	  
	  fileloc_free (tmp);
	}
    }

  cstring_free (xname);
  /*@noaccess cstring@*/
  /*@-nullstate@*/ /* same problem as above */
  return FALSE;
  /*@=nullstate@*/
}

static void printDot (void)
{
  displayScanContinue (cstring_makeLiteralTemp ("."));
}

void
dumpState (cstring cfname)
{
  FILE *f;
  cstring fname = fileLib_addExtension (cfname, cstring_makeLiteralTemp (DUMP_SUFFIX));
  
  f = fileTable_openWriteFile (context_fileTable (), fname);

  displayScanOpen (message ("Dumping to %s ", fname)); 
  
  if (f == NULL)
    {
      lldiagmsg (message ("Cannot open dump file for writing: %s", fname));
    }
  else
    {
      /*
      ** sequence is convulted --- must call usymtab_prepareDump before
      **    dumping ctype table to convert type uid's
      */

      printDot ();

      /*
      DPRINTF (("Before prepare dump:"));
      ctype_printTable ();
      DPRINTF (("Preparing dump..."));
      */

      usymtab_prepareDump ();

      /*
      ** Be careful, these lines must match loadLCDFile checking.
      */

      fprintf (f, "%s %s\n", LIBRARY_MARKER, cstring_toCharsSafe (fname));
      fprintf (f, ";;Splint %f\n", SPLINT_LIBVERSION);
      fprintf (f, ";;lib:%d\n", (int) context_getLibrary ());
      fprintf (f, ";;ctTable\n");
      
      DPRINTF (("Dumping types..."));
      printDot ();
      ctype_dumpTable (f);
      printDot ();
      
      DPRINTF (("Dumping type sets..."));
      fprintf (f, ";;tistable\n");
      typeIdSet_dumpTable (f);
      printDot ();
      
      DPRINTF (("Dumping usymtab..."));
      fprintf (f, ";;symTable\n");
      usymtab_dump (f);
      printDot ();

      DPRINTF (("Dumping modules..."));
      fprintf (f, ";; Modules access\n");
      context_dumpModuleAccess (f);
      fprintf (f, ";;End\n");
      check (fileTable_closeFile (context_fileTable (), f));
    }

  displayScanClose ();
  cstring_free (fname);
}

bool
loadStandardState ()
{
  cstring fpath;
  FILE *stdlib;
  bool result;
  cstring libname = fileLib_addExtension (context_selectedLibrary (), 
					  cstring_makeLiteralTemp (DUMP_SUFFIX));
  
  if (osd_findOnLarchPath (libname, &fpath) != OSD_FILEFOUND)
    {
      lldiagmsg (message ("Cannot find %sstandard library: %s", 
			  cstring_makeLiteralTemp 
			  (context_getFlag (FLG_STRICTLIB) ? "strict " 
			   : (context_getFlag (FLG_UNIXLIB) ? "unix " : "")),
			  libname));
      lldiagmsg (cstring_makeLiteral ("     Check LARCH_PATH environment variable."));
      result = FALSE;
    }
  else
    {
      stdlib = fileTable_openReadFile (context_fileTable (), fpath);

      if (stdlib == NULL)
	{
	  lldiagmsg (message ("Cannot read standard library: %s",
			      fpath));
	  lldiagmsg (cstring_makeLiteral ("     Check LARCH_PATH environment variable."));

	  result = FALSE;
	}
      else
	{
	  if (context_getFlag (FLG_WHICHLIB))
	    {
	      char *t = mstring_create (MAX_NAME_LENGTH);
	      char *ot = t;

	      if ((t = reader_readLine (stdlib, t, MAX_NAME_LENGTH)) == NULL)
		{
		  llfatalerror (cstring_makeLiteral ("Standard library format invalid"));
		}

	      if ((t = reader_readLine (stdlib, t, MAX_NAME_LENGTH)) != NULL)
		{
		  if (*t == ';' && *(t + 1) == ';') 
		    {
		      t += 2;
		    }
		}

	      if (t == NULL)
		{
		  lldiagmsg (message ("Standard library: %s <cannot read creation information>", 
				      fpath));
		}
	      else
		{
		  char *tt;

		  tt = strrchr (t, '\n');
		  if (tt != NULL)
		    *tt = '\0';

		  lldiagmsg (message ("Standard library: %s", fpath));
		  /* evans 2004-01-13: removed this (it is the libversion which is confusing) */
		  /*   lldiagmsg (message ("   (created using %s)", cstring_fromChars (t)));		    */
		}

	      sfree (ot);
	      
	      check (fileTable_closeFile (context_fileTable (), stdlib));
	      stdlib = fileTable_openReadFile (context_fileTable (), fpath);
	    }

	  llassert (stdlib != NULL);

	  fileloc_reallyFree (g_currentloc);
	  g_currentloc = fileloc_createLib (libname);

	  DPRINTF (("Loading: %s", fpath));

	  displayScanOpen (message ("loading standard library %s ", fpath));
	  result = loadLCDFile (stdlib, fpath);
	  displayScanClose ();

	  check (fileTable_closeFile (context_fileTable (), stdlib));
	}
    }

  cstring_free (libname);
  return result;
}

/*@constant int BUFLEN;@*/
# define BUFLEN 128

static bool
loadLCDFile (FILE *f, cstring name)
{
  char buf[BUFLEN];
  
  /*
  ** Check version.  Should be >= SPLINT_LIBVERSION
  */

  if (reader_readLine (f, buf, BUFLEN) == NULL
      || !mstring_equalPrefix (buf, LIBRARY_MARKER))
    {
      loadllmsg (message ("Load library %s is not in Splint library format.  Attempting "
			  "to continue without library.", name));
      return FALSE;
    }
  
  if (reader_readLine (f, buf, BUFLEN) != NULL)
    {
      if (!mstring_equalPrefix (buf, ";;"))
	{
	  loadllmsg (message ("Load library %s is not in Splint library format.  Attempting "
			      "to continue without library.", name));
	  return FALSE;
	}
      else if (mstring_equalPrefix (buf, ";;ctTable"))
	{
	  loadllmsg (message ("Load library %s is in obsolete Splint library format.  Attempting "
			      "to continue anyway, but results may be incorrect.  Rebuild "
			      "the library with this version of splint.", 
			      name));
	}
      else 
	{
	  float version = 0.0;

	  if (sscanf (buf, ";;Splint %f", &version) != 1
	      && (sscanf (buf, ";;LCLint %f", &version) != 1))
	    {
	      loadllmsg (message ("Load library %s is not in Splint library format (missing version "
				  "number).  Attempting "
				  "to continue without library.", name));
	      return FALSE;
	    }
	  else
	    {
	      if ((SPLINT_LIBVERSION - version) >= FLT_EPSILON)
		{
		  cstring vname;
		  char *nl = strchr (buf, '\n');

		  *nl = '\0';

		  vname = cstring_fromChars (buf + 9);

		  loadllmsg (message ("Load library %s is in obsolete Splint library "
				      "format (version %f (%s), expecting version %f).  Attempting "
				      "to continue anyway, but results may be incorrect.  Rebuild "
				      "the library with this version of splint.", 
				      name, 
				      version,
				      vname, 
				      SPLINT_LIBVERSION));
		}
	      else
		{
		  if (reader_readLine (f, buf, BUFLEN) == NULL)
		    {
		      loadllmsg (message ("Load library %s is not in Splint library "
					  "format (missing library code). Attempting "
					  "to continue without library.", name));
		      return FALSE;
		    }
		  else 
		    {
		      int lib;
		      
		      if (sscanf (buf, ";;lib:%d", &lib) != 1)
			{
			  loadllmsg (message ("Load library %s is not in Splint library "
					      "format (missing library code). Attempting "
					      "to continue without library.", name));
			  return FALSE;
			}
		      else
			{
			  flagcode code = (flagcode) lib;

			  if (flagcode_isLibraryFlag (code))
			    {
			      if (context_doMerge ()) 
				{
				  context_setLibrary (code);
				}
			    }
			  else
			    {
			      loadllmsg (message ("Load library %s has invalid library code (%s).  "
						  "Attempting to continue without library.",
						  name,
						  flagcode_unparse (code)));
			      
			      return FALSE;
			    }
			}
		    }
		}
	    }
	}
    }
  else
    {
      loadllmsg (message ("Load library %s is not in Splint library format (missing lines).  "
			  "Attempting to continue without library.", name));
      return FALSE;
    }
  
  ctype_loadTable (f);
  printDot ();
  
  typeIdSet_loadTable (f);
  printDot ();
  
  usymtab_load (f);
  printDot ();
  
  context_loadModuleAccess (f);
  printDot ();
  
  return TRUE;
}

/*
** load state from file created by dumpState
*/

void
loadState (cstring cfname)
{
  FILE *f;
  cstring fname = fileLib_addExtension (cfname, cstring_makeLiteralTemp (DUMP_SUFFIX));

  f = fileTable_openReadFile (context_fileTable (), fname);

  if (f == NULL)
    {
      displayScanClose ();
      llfatalerror (message ("Cannot open dump file for loading: %s", 
			     fname));
    }
  else
    {
      fileloc_reallyFree (g_currentloc);
      g_currentloc = fileloc_createLib (cfname);

      if (!loadLCDFile (f, cfname)) 
	{
	  if (!loadStandardState ()) 
	    {
	      ctype_initTable ();
	    }
	}
      
      check (fileTable_closeFile (context_fileTable (), f));
    }

  /* usymtab_printAll (); */
  cstring_free (fname);
}

