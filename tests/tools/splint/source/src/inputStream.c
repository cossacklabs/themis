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
** source.c
**
** Interface to source file abstraction
**
**	NOTE:	    This module is almost identical to the one for LCL.  The
**		    only difference is that a couple of source lines have been
**		    commented out.
**
**		    This module has too many dependencies to be in the common
**		    source area.  Any of the solutions that would allow this
**		    module to be common had its own set of compromises.  It
**		    seemed best and most straightforward to just keep separte
**		    copies for LSL and LCL.  We should examine this again if we
**		    ever reorganize the module structure.
**
**  AUTHORS:
**
**     Steve Garland,
**         Massachusetts Institute of Technology
**     Joe Wild, Technical Languages and Environments, DECspec project
*/

# include "splintMacros.nf"
# include "basic.h"
# include "osd.h"

extern bool
inputStream_close (inputStream s)
{
  llassert (inputStream_isDefined (s));

  if (s->file != NULL)
    {
      check (fileTable_closeFile (context_fileTable (), s->file));
      s->file = NULL;
      return TRUE;
    }

  return FALSE;
}

extern void
inputStream_free (/*@null@*/ /*@only@*/ inputStream s)
{
  if (inputStream_isDefined (s))
    {
      cstring_free (s->name);
      cstring_free (s->stringSource);
      sfree (s);
    }
}

extern /*@only@*/ inputStream 
inputStream_create (cstring name, cstring suffix, bool echo)
{
  char *ps;
  inputStream s = (inputStream) dmalloc (sizeof (*s));

  s->name = name;
  s->file = NULL;

  /*@access cstring@*/
  llassert (cstring_isDefined (s->name));
  ps = strrchr (s->name, CONNECTCHAR);

  if (ps == NULL)
    {
      ps = s->name;
    }
  /*@noaccess cstring@*/

  if (strchr (ps, '.') == NULL)
    {
      s->name = cstring_concatFree1 (s->name, suffix);
    }

  s->name = fileLib_cleanName (s->name);

  s->lineNo = 0;
  s->charNo = 0;
  s->curLine = NULL;
  s->echo = echo;
  s->fromString = FALSE;
  s->stringSource = NULL;
  s->stringSourceTail = NULL;
  s->buffer[0] = '\0';

  return s;
}

extern /*@only@*/ inputStream 
inputStream_fromString (cstring name, cstring str)
{
  inputStream s = (inputStream) dmalloc (sizeof (*s));

  s->name = cstring_copy (name);
  s->stringSource = cstring_copy (str);
  s->stringSourceTail = s->stringSource;
  s->file = 0;
  s->echo = FALSE;
  s->fromString = TRUE;
  s->lineNo = 0;
  s->charNo = 0;
  s->curLine = NULL;
  s->buffer[0] = '\0';

  return s;
}

extern int inputStream_nextChar (inputStream s)
{
  int res;

  llassert (inputStream_isDefined (s));
  res = inputStream_peekChar (s);

  if (res != EOF) 
    {
      if (res == (int) '\n')
	{
	  s->curLine = NULL;
	  s->charNo = 0;
	  incLine ();
	}
      else
	{
	  s->charNo++;
	  incColumn ();
	}
    }

  DPRINTF (("Next char: %c [%d]", (char) res, res));
  return res;
}

extern int inputStream_peekNChar (inputStream s, int n)
     /* Doesn't work across lines! */
{
  llassert (inputStream_isDefined (s));
  llassert (s->curLine != NULL);
  llassert (s->charNo + n < strlen (s->curLine));
  return ((int) s->curLine [s->charNo + n]);
}

extern int inputStream_peekChar (inputStream s)
{  
  llassert (inputStream_isDefined (s));

  if (s->curLine == NULL)
    {
      char *cur;
      s->curLine = NULL;
      cur = inputStream_nextLine (s);
      s->curLine = cur; /* split this to avoid possible undefined behavior */
      s->charNo = 0;
    }

  if (s->curLine == NULL)  
    {
      return EOF;
    }
 
  llassert (s->charNo <= strlen (s->curLine));

  if (s->curLine[s->charNo] == '\0') 
    {
      return (int) '\n';
    }
 
  return ((int) s->curLine [s->charNo]);
} 

extern /*@dependent@*/ /*@null@*/ 
char *inputStream_nextLine (inputStream s)
{
  char *currentLine;
  size_t len;

  llassert (inputStream_isDefined (s));
  llassert (s->curLine == NULL);
  s->charNo = 0;

  if (s->fromString)
    {
      if (cstring_isEmpty (s->stringSourceTail))
	{
	  currentLine = 0;
	}
      else
	{
	  /*@access cstring@*/
	  char *c = strchr (s->stringSourceTail, '\n');
	  
	  /* in case line is terminated not by newline */ 
	  if (c == 0)
	    {
	      c = strchr (s->stringSourceTail, '\0');
	    }

	  len = size_fromInt (c - s->stringSourceTail + 1);

	  if (len > size_fromInt (STUBMAXRECORDSIZE - 2))
	    {
	      len = size_fromInt (STUBMAXRECORDSIZE - 2);
	    }

	  currentLine = &(s->buffer)[0];
	  strncpy (currentLine, s->stringSourceTail, len);
	  currentLine[len] = '\0';
	  s->stringSourceTail += len;
	  /*@noaccess cstring@*/
	}
      
    }
  else
    {
      llassert (s->file != NULL);
      currentLine = fgets (&(s->buffer)[0], STUBMAXRECORDSIZE, s->file);
    }
  if (currentLine == 0)
    {
      strcpy (s->buffer, "*** End of File ***");
    }
  else
    {
      s->lineNo++;
      len = strlen (currentLine) - 1;
      if (s->buffer[len] == '\n')
	{
	  s->buffer[len] = '\0';
	}
      else 
	{
	  if (len >= size_fromInt (STUBMAXRECORDSIZE - 2))
	    {
	      lldiagmsg (message ("Input line too long: %s",
				  cstring_fromChars (currentLine)));
	    }
	}
    }

  /* if (s->echo) slo_echoLine (currentLine);		only needed in LCL */
  return currentLine;
}

extern bool
inputStream_open (inputStream s)
{
  llassert (inputStream_isDefined (s));
  if (s->fromString)
    {
      /* not an error: tail is dependent */
      s->stringSourceTail = s->stringSource; 
      return TRUE;
    }

  DPRINTF (("Opening: %s", s->name));
  s->file = fileTable_openReadFile (context_fileTable (), s->name);
  return (s->file != 0 || s->fromString);
}

/*
** requires
**  path != NULL \and
**  s != NULL \and
**  *s.name == filename (*s.name) || filetype (*s.name)
**      *s.name consists of a file name and type only ("<filename>.<type>)
**	No path name is included
**
** ensures
**  if filefound (*path, *s) then
**	result = true \and *s.name = filespec_where_file_found (*path, *s)
**  else
**	result = false
*/

extern bool inputStream_getPath (cstring path, inputStream s)
{
  cstring returnPath;
  filestatus status;		/* return status of osd_getEnvPath.*/
  bool rVal;			/* return value of this procedure. */
  
  llassert (cstring_isDefined (path));
  llassert (inputStream_isDefined (s));
  llassert (cstring_isDefined (s->name));

  status = osd_getPath (path, s->name, &returnPath);

  if (status == OSD_FILEFOUND)
    {				/* Should be majority of cases. */
      rVal = TRUE;
      
      cstring_free (s->name);
      s->name = fileLib_cleanName (returnPath);
    }
  else if (status == OSD_FILENOTFOUND)
    {
      rVal = FALSE;
    }
  else if (status == OSD_PATHTOOLONG)
    {
      rVal = FALSE;
     /* Directory and filename are too long.  Report error. */
     llbuglit ("soure_getPath: Filename plus directory from search path too long");
 }
  else
    {
      rVal = FALSE;
      llbuglit ("inputStream_getPath: invalid return status");
    }

  return rVal;
}

/*:open:*/ FILE *inputStream_getFile (inputStream s)
{
  llassert (inputStream_isDefined (s));
  llassert (s->file != NULL);
  return s->file;
}

cstring inputStream_fileName (inputStream s)
{
  llassert (inputStream_isDefined (s));
  return s->name;
}

bool inputStream_isOpen (inputStream s)
{
  return (inputStream_isDefined (s) && (s->file != 0 || s->fromString));
}

int inputStream_thisLineNumber (inputStream s)
{
  llassert (inputStream_isDefined (s));
  return s->lineNo;
}




