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
** rcfiles.c
*/

# include "splintMacros.nf"
# include "basic.h"
# include "rcfiles.h"


static void rcfiles_loadFile (FILE *p_rcfile, cstringList *p_passThroughArgs)
   /*@modifies *p_passThroughArgs, p_rcfile@*/
   /*@ensures closed p_rcfile@*/ ;

bool rcfiles_read (cstring fname, cstringList *passThroughArgs, bool report)
{
  bool res = FALSE;

  if (fileTable_exists (context_fileTable (), fname))
    {
      if (report)
	{
	  voptgenerror
	    (FLG_WARNRC, 
	     message ("Multiple attempts to read options file: %s", fname),
	     g_currentloc);
	}
    }
  else
    {
      FILE *innerf = fileTable_openReadFile (context_fileTable (), fname);
      
      if (innerf != NULL)
	{
	  fileloc fc = g_currentloc;
	  g_currentloc = fileloc_createRc (fname);

	  displayScan (message ("reading options from %q", 
				fileloc_outputFilename (g_currentloc)));
	  
	  rcfiles_loadFile (innerf, passThroughArgs);
	  fileloc_reallyFree (g_currentloc);
	  g_currentloc = fc;
	  res = TRUE;
	}
      else 
	{
	  if (report)
	    {
	      voptgenerror
		(FLG_WARNRC, 
		 message ("Cannot open options file: %s", fname),
		 g_currentloc);
	    }
	}
    }

  return res;
}

static void rcfiles_loadFile (/*:open:*/ FILE *rcfile, cstringList *passThroughArgs)
   /*@modifies rcfile@*/
   /*@ensures closed rcfile@*/
{
  char *s = mstring_create (MAX_LINE_LENGTH);
  char *os = s;
  cstringList args = cstringList_new ();

  DPRINTF (("Loading rc file..."));
  
  s = os;

  while (reader_readLine (rcfile, s, MAX_LINE_LENGTH) != NULL)
    {
      char c;

      DPRINTF (("Line: %s", s));
      DPRINTF (("args: %s", cstringList_unparse (args)));

      while (*s == ' ' || *s == '\t')
	{
	  s++;
	  incColumn ();
	}
      
      while (*s != '\0')
	{
	  char *thisflag;
	  bool escaped = FALSE;
	  bool quoted = FALSE;
	  c = *s;

	  /* comment characters */
	  if (c == '#' || c == ';' || c == '\n') 
	    {
	      /*@innerbreak@*/
	      break;
	    }
	  
	  thisflag = s;
	  
	  while ((c = *s) != '\0')
	    { /* remember to handle spaces and quotes in -D and -U ... */
	      if (escaped)
		{
		  escaped = FALSE;
		}
	      else if (quoted)
		{
		  if (c == '\\')
		    {
		      escaped = TRUE;
		    }
		  else if (c == '\"')
		    {
		      quoted = FALSE;
		    }
		  else
		    {
		      ;
		    }
		}
	      else if (c == '\"')
		{
		  quoted = TRUE;
		}
	      else
		{
		 if (c == ' ' || c == '\t' || c == '\n')
		   {
		     /*@innerbreak@*/ break;
		   }
	       }
		  
	      s++; 
	      incColumn ();
	    }

	  DPRINTF (("Nulling: %c", *s));
	  *s = '\0';

	  if (mstring_isEmpty (thisflag))
	    {
	      llfatalerror (message ("Missing flag: %s",
				     cstring_fromChars (os)));
	    }
	  else
	    {
	      args = cstringList_add (args, cstring_fromCharsNew (thisflag));
	      DPRINTF (("args: %s", cstringList_unparse (args)));
	    }
	  
	  *s = c;
	  DPRINTF (("Pass through: %s", cstringList_unparse (*passThroughArgs)));
	  
	  while ((c == ' ') || (c == '\t'))
	    {
	      c = *(++s);
	      incColumn ();
	    } 
	}
      
      s = os;
    }

  sfree (os); 

  DPRINTF (("args: %s", cstringList_unparse (args)));
  flags_processFlags (FALSE, 
		      fileIdList_undefined,
		      fileIdList_undefined,
		      fileIdList_undefined,
		      fileIdList_undefined,
		      passThroughArgs,
		      cstringList_size (args),
		      /*@-nullstate@*/ /*@-type@*/ /* exposes cstring type */ 
		      cstringList_getElements (args)
		      /*@=nullstate@*/ /*@=type@*/
		      );
  cstringList_free (args);
  check (fileTable_closeFile (context_fileTable (), rcfile));
}



