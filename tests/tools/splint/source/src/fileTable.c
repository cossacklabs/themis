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
** fileTable.c
**
** replaces filenamemap.c
** based (loosely) on typeTable.c
**
** entries in the fileTable are:
**
**        name - name of the file
**        type - kind of file (a temp file to be deleted?)
**        link - derived from this file
**
*/
/*
 * Herbert 04/1997:
 * - Added conditional stuff (macros OS2 and MSDOS) to make names of temporary 
 *   files under Windows or OS/2 not larger than 8+3 characters to avoid 
 *   trouble with FAT file systems or Novell Netware volumes.
 * - Added include of new header file portab.h containing OS dependent stuff.
 * - Changed occurance of '/' as path delimiter to a macro.
 * - Added conditional stuff (#define and #include) for IBM's compiler.
 */

# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include "splintMacros.nf"
# include "basic.h"
# include "osd.h"
# include "llmain.h"

# ifdef WIN32
# include <io.h>
# else
# if defined(__IBMC__) && defined(OS2)
# include <process.h>
# include <io.h>
# define getpid _getpid
# define S_IRUSR S_IREAD
# define S_IWUSR S_IWRITE 
# define S_IXUSR S_IEXEC
# endif
# endif

/*@access fileId*/

static void 
fileTable_addOpen (fileTable p_ft, /*@observer@*/ FILE *p_f, /*@only@*/ cstring p_fname) 
  /*@modifies p_ft@*/ ;

static bool fileTable_inRange (fileTable ft, fileId fid) /*@*/ 
{
  return (fileTable_isDefined (ft) && (fid >= 0) && (fid < ft->nentries));
}

static fileId fileTable_internAddEntry (fileTable p_ft, /*@only@*/ ftentry p_e) 
   /*@modifies p_ft@*/ ;
static /*@only@*/ cstring makeTempName (cstring p_dir, cstring p_pre, cstring p_suf);

static /*@only@*/ cstring
fileType_unparse (fileType ft)
{
  switch (ft)
    {
    case FILE_NORMAL:  return cstring_makeLiteral ("normal");
    case FILE_NODELETE:  return cstring_makeLiteral ("normal");
    case FILE_LSLTEMP: return cstring_makeLiteral ("ltemp");
    case FILE_HEADER:  return cstring_makeLiteral ("header");
    case FILE_XH:  return cstring_makeLiteral ("xh");
    case FILE_MACROS:  return cstring_makeLiteral ("macros");
    case FILE_METASTATE:  return cstring_makeLiteral ("metastate");
    }

  BADEXIT;
}

static int
fileTable_getIndex (fileTable ft, cstring s)
{
  int res;
  cstring abspath;
  if (ft == NULL) return NOT_FOUND;
  abspath = osd_absolutePath (cstring_undefined, s);
  
  if (context_getFlag (FLG_CASEINSENSITIVEFILENAMES))
    {
      abspath = cstring_downcase (abspath);
    }

  DPRINTF (("Absolute path: %s: %s", s, abspath));
  res = cstringTable_lookup (ft->htable, abspath);
  cstring_free (abspath);
  return res;
}

static cstring ftentry_unparse (fileTable ft, ftentry fte)
{
  if (fileId_isValid (fte->fder))
    {
      llassert (fileTable_isDefined (ft));

      return message ("%s %q %d (%s)", 
		      fte->fname, 
		      fileType_unparse (fte->ftype),
		      fte->fder,
		      ft->elements[fte->fder]->fname);
    }
  else
    {
      return message ("%s %q", fte->fname,
		      fileType_unparse (fte->ftype));
    }
}

/*@only@*/ cstring
fileTable_unparse (fileTable ft)
{
  cstring s = cstring_undefined;
  int i;

  if (fileTable_isUndefined (ft))
    {
      return (cstring_makeLiteral ("<fileTable undefined>"));
    }

  for (i = 0; i < ft->nentries; i++)
    {
      s = message ("%s\n[%d] %q", s, i, ftentry_unparse (ft, ft->elements[i]));
    }

  return s;
}

void fileTable_printTemps (fileTable ft)
{
  if (fileTable_isDefined (ft))
    {
      int i;

      for (i = 0; i < ft->nentries; i++)
	{
	  if (ft->elements[i]->ftemp)
	    {
	      if (fileId_isValid (ft->elements[i]->fder))
		{
		  fprintf (stderr, "  %s:1\n\t%s:1\n", 
			   cstring_toCharsSafe (ft->elements[ft->elements[i]->fder]->fname),
			   cstring_toCharsSafe (ft->elements[i]->fname));
		}
	      else
		{
		  fprintf (stderr, "[no file]\n\t%s:1\n",
			   cstring_toCharsSafe (ft->elements[i]->fname));
		}
	    }
	}
    }
}

/*
** loads in fileTable from fileTable_dump
*/

static /*@notnull@*/ ftentry
ftentry_create (/*@keep@*/ cstring tn, bool temp, fileType typ, fileId der)
{
  ftentry t = (ftentry) dmalloc (sizeof (*t));
  
  if (cstring_isUndefined (tn))
    {
      llbug (cstring_makeLiteral ("Undefined filename!"));
    }
  
  t->fname = tn;
  t->basename = cstring_undefined;
  t->ftemp = temp;
  t->ftype = typ;
  t->fder  = der;

  /* Don't set these until the basename is needed. */
  t->fsystem = FALSE;
  t->fspecial = FALSE;

  return t;
}

static void
ftentry_free (/*@only@*/ ftentry t)
{
  cstring_free (t->fname);
  cstring_free (t->basename);
  sfree (t);
}

/*@only@*/ /*@notnull@*/ fileTable
fileTable_create ()
{
  fileTable ft = (fileTable) dmalloc (sizeof (*ft));
  
  ft->nentries = 0;
  ft->nspace = FTBASESIZE;
  ft->elements = (ftentry *) dmalloc (FTBASESIZE * sizeof (*ft->elements));
  ft->htable = cstringTable_create (FTHASHSIZE);

  ft->nopen = 0;
  ft->nopenspace = FTBASESIZE;
  ft->openelements = (foentry *) dmalloc (FTBASESIZE * sizeof (*ft->openelements));

  return (ft);
}

/*@-bounds@*/
static void
fileTable_grow (fileTable ft)
{
  int i;
  ftentry *newent;

  llassert (fileTable_isDefined (ft));

  ft->nspace = FTBASESIZE;

  newent = (ftentry *) dmalloc ((ft->nentries + ft->nspace) * sizeof (*newent));

  for (i = 0; i < ft->nentries; i++)
    {
      newent[i] = ft->elements[i];
    }

  sfree (ft->elements);
  ft->elements = newent;
}
/*@=bounds@*/
static void
fileTable_growOpen (fileTable ft)
{
  int i;
  foentry *newent;

  llassert (fileTable_isDefined (ft));

  ft->nopenspace = FTBASESIZE;

  newent = (foentry *) dmalloc ((ft->nopen + ft->nopenspace) * sizeof (*newent));
  
  for (i = 0; i < ft->nopen; i++)
    {
      newent[i] = ft->openelements[i];
    }

  sfree (ft->openelements);
  ft->openelements = newent;
}

static fileId
fileTable_internAddEntry (fileTable ft, /*@only@*/ ftentry e)
{
  llassert (fileTable_isDefined (ft));

  if (ft->nspace <= 0)
    fileTable_grow (ft);

  ft->nspace--;

  DPRINTF (("Adding: %s", e->fname));

  if (context_getFlag (FLG_CASEINSENSITIVEFILENAMES))
    {
      cstring sd = cstring_downcase (e->fname);
      cstringTable_insert (ft->htable, sd, ft->nentries);
    }
  else
    {
      cstringTable_insert (ft->htable, cstring_copy (e->fname), ft->nentries); 
    }

  /* evans 2002-07-12:
     Before, there was no cstring_copy above, and e->fname was free'd in the if branch.
     Splint should have caught this, and produced a warning for this assignment.
     Why not?
  */
  ft->elements[ft->nentries] = e;

  ft->nentries++;
  return (ft->nentries - 1);
}

void fileTable_noDelete (fileTable ft, cstring name)
{
  fileId fid = fileTable_lookup (ft, name);

  if (fileId_isValid (fid)) 
    {
      llassert (fileTable_isDefined (ft));
      ft->elements[fid]->ftype = FILE_NODELETE;
    }
  else
    {
      DPRINTF (("Invalid no delete: %s", name));
    }
}

static fileId
fileTable_addFilePrim (fileTable ft, /*@temp@*/ cstring name, 
		       bool temp, fileType typ, fileId der)
   /*@modifies ft@*/
{
  cstring absname = osd_absolutePath (NULL, name);
  int tindex = fileTable_getIndex (ft, absname);
  
  llassert (ft != fileTable_undefined);

  if (tindex != NOT_FOUND)
    {
      llcontbug (message ("fileTable_addFilePrim: duplicate entry: %q", absname));
      return tindex;
    }
  else
    {
      ftentry e = ftentry_create (absname, temp, typ, der);

      if (der == fileId_invalid)
	{
	  llassert (cstring_isUndefined (e->basename));

	  e->basename = fileLib_removePathFree (fileLib_removeAnyExtension (absname));
	  e->fsystem = context_isSystemDir (absname);

	  /*
	  ** evans 2002-03-15: change suggested by Jim Zelenka
	  **                   support relative paths for system directories
	  */

	  if (!e->fsystem)
	    {
	      e->fsystem = context_isSystemDir (name);
	    }

	  e->fspecial = context_isSpecialFile (absname);

	  if (e->fspecial)
	    {
	      cstring srcname = cstring_concatFree1 (fileLib_removeAnyExtension (absname), 
						     C_EXTENSION);
	      fileId fid = fileTable_lookup (ft, srcname);
	      cstring_free (srcname);

	      if (fileId_isValid (fid))
		{
		  fileId derid = ft->elements[fid]->fder;

		  ft->elements[fid]->fspecial = TRUE;

		  if (fileId_isValid (derid))
		    {
		      ft->elements[derid]->fspecial = TRUE;
		    }
		}
	    }
	}
      else
	{
	  ftentry de = ft->elements[der];

	  llassert (cstring_isUndefined (e->basename));
	  e->basename = cstring_copy (de->basename);
	  e->fsystem = de->fsystem;
	  e->fspecial = de->fspecial;
	}

      return (fileTable_internAddEntry (ft, e));
    }
}

fileId
fileTable_addFile (fileTable ft, cstring name)
{
  return (fileTable_addFilePrim (ft, name, FALSE, FILE_NORMAL, fileId_invalid));
}

fileId
fileTable_addFileOnly (fileTable ft, /*@only@*/ cstring name)
{
  fileId res = fileTable_addFilePrim (ft, name, FALSE, FILE_NORMAL, fileId_invalid);
  cstring_free (name);
  return res;
}

fileId
fileTable_addHeaderFile (fileTable ft, cstring name)
{
  fileId res;
  res = fileTable_addFilePrim (ft, name, FALSE, FILE_HEADER, fileId_invalid);
  return res;

}

void
fileTable_addStreamFile (fileTable ft, FILE *fstream, cstring name)
{
  fileTable_addOpen (ft, fstream, cstring_copy (name));
}

bool
fileTable_isHeader (fileTable ft, fileId fid)
{
  if (fileId_isInvalid (fid))
    {
      return FALSE;
    }

  llassert (fileTable_isDefined (ft) && fileTable_inRange (ft, fid));
  return (ft->elements[fid]->ftype == FILE_HEADER);
}

bool
fileTable_isSystemFile (fileTable ft, fileId fid)
{
  if (fileId_isInvalid (fid))
    {
      return FALSE;
    }

  llassert (fileTable_isDefined (ft) && fileTable_inRange (ft, fid));
  return (ft->elements[fid]->fsystem);
}

bool
fileTable_isXHFile (fileTable ft, fileId fid)
{
  if (fileId_isInvalid (fid))
    {
      return FALSE;
    }

  if (!(fileTable_isDefined (ft) && fileTable_inRange (ft, fid)))
    {
      llcontbug (message ("Bad file table or id: %s %d", bool_unparse (fileTable_isDefined (ft)), fid));
      return FALSE;
    }
  else
    {
      return (ft->elements[fid]->ftype == FILE_XH);
    }
}

bool
fileTable_isSpecialFile (fileTable ft, fileId fid)
{
  if (fileId_isInvalid (fid))
    {
      return FALSE;
    }
  
  llassert (fileTable_isDefined (ft) && fileTable_inRange (ft, fid));
  return (ft->elements[fid]->fspecial);
}

fileId
fileTable_addLibraryFile (fileTable ft, cstring name)
{
  return (fileTable_addFilePrim (ft, name, FALSE, FILE_HEADER, fileId_invalid));
}

fileId
fileTable_addXHFile (fileTable ft, cstring name)
{
  return (fileTable_addFilePrim (ft, name, FALSE, FILE_XH, fileId_invalid));
}

fileId
fileTable_addImportFile (fileTable ft, cstring name)
{
  return (fileTable_addFilePrim (ft, name, FALSE, FILE_HEADER, fileId_invalid));
}

fileId
fileTable_addLCLFile (fileTable ft, cstring name)
{
  return (fileTable_addFilePrim (ft, name, FALSE, FILE_HEADER, fileId_invalid));
}

static int tmpcounter = 0;

fileId
fileTable_addMacrosFile (fileTable ft)
{
  cstring newname =
    makeTempName (context_tmpdir (), cstring_makeLiteralTemp ("lmx"),
		  cstring_makeLiteralTemp (".llm"));
  fileId res = fileTable_addFilePrim (ft, newname, TRUE, FILE_MACROS, fileId_invalid);
  cstring_free (newname);
  return res;
}

fileId
fileTable_addMetastateFile (fileTable ft, cstring name)
{
  return (fileTable_addFilePrim (ft, name, FALSE, FILE_METASTATE, fileId_invalid));
}

fileId
fileTable_addCTempFile (fileTable ft, fileId fid)
{
  cstring newname =
    makeTempName (context_tmpdir (), cstring_makeLiteralTemp ("cl"), 
		  C_EXTENSION);
  fileId res;

  DPRINTF (("tmp dir: %s", context_tmpdir ()));
  DPRINTF (("new name: %s", newname));

  llassert (fileTable_isDefined (ft));

  if (!fileId_isValid (ft->elements[fid]->fder))
    {
      if (fileTable_isXHFile (ft, fid))
	{
	  res = fileTable_addFilePrim (ft, newname, TRUE, FILE_XH, fid);
	}
      else
	{
	  res = fileTable_addFilePrim (ft, newname, TRUE, FILE_NORMAL, fid);
	}
    }
  else 
    {
      if (fileTable_isXHFile (ft, fid))
	{
	  res = fileTable_addFilePrim (ft, newname, TRUE, FILE_XH,
				       ft->elements[fid]->fder);
	}
      else
	{
	  res = fileTable_addFilePrim (ft, newname, TRUE, FILE_NORMAL,
				       ft->elements[fid]->fder);
	}
    }

  DPRINTF (("Added file: %s", fileTable_fileName (res)));
  cstring_free (newname);
  return res;
}

fileId
fileTable_addltemp (fileTable ft)
{
  cstring newname = makeTempName (context_tmpdir (),
				  cstring_makeLiteralTemp ("ls"), 
				  cstring_makeLiteralTemp (".lsl"));
  fileId ret;
  
  if (cstring_hasNonAlphaNumBar (newname))
    {
      char *lastpath = (char *)NULL;

      if (tmpcounter == 0)
	{
	  lldiagmsg
	    (message
	     ("Operating system generates tmp filename containing invalid charater: %s",
	      newname));
	  lldiagmsg (cstring_makeLiteral 
		     ("Try cleaning up the tmp directory.  Attempting to continue."));
	}
      
      /*@access cstring@*/
      llassert (cstring_isDefined (newname));
      lastpath = strrchr (newname, CONNECTCHAR); /* get the directory */
      llassert (lastpath != NULL);
      *lastpath = '\0';

      newname = message ("%q%hlsl%d.lsl", 
			 newname,
			 CONNECTCHAR,
			 tmpcounter);
      /*@noaccess cstring@*/
      tmpcounter++;
    }
  
  /*
  ** this is kind of yucky...need to make the result of cstring_fromChars
  ** refer to the same storage as its argument.  Of course, this loses,
  ** since cstring is abstract.  Should make it an only?
  */

  ret = fileTable_addFilePrim (ft, newname, TRUE, FILE_LSLTEMP, fileId_invalid);
  cstring_free (newname);
  return (ret);
}

bool
fileTable_exists (fileTable ft, cstring s)
{
  int tindex = fileTable_getIndex (ft, s);

  if (tindex == NOT_FOUND)
    {
      DPRINTF (("Not found: %s", s));
      return FALSE;
    }
  else
    {
      return TRUE;
    }
}

fileId
fileTable_lookup (fileTable ft, cstring s)
{
  int tindex = fileTable_getIndex (ft, s);

  if (tindex == NOT_FOUND)
    {
      return fileId_invalid;
    }
  else
    {
      return tindex;
    }
}

/*
** This is pretty awkward --- when we find the real path of 
** a .xh file, we may need to change the recorded name.  [Sigh]
*/

void
fileTable_setFilePath (fileTable ft, fileId fid, cstring path)
{
  llassert (fileId_isValid (fid));
  llassert (fileTable_isDefined (ft));
  /* Need to put new string in hash table */
  cstringTable_insert (ft->htable, cstring_copy (path), fid);
  ft->elements[fid]->fname = cstring_copy (path);
}

fileId
fileTable_lookupBase (fileTable ft, cstring base)
{
  int tindex;

  if (context_getFlag (FLG_CASEINSENSITIVEFILENAMES))
    {
      cstring dbase = cstring_downcase (base);
      tindex = fileTable_getIndex (ft, dbase);
      cstring_free (dbase);
    }
  else
    {
      tindex = fileTable_getIndex (ft, base);
    }

  if (tindex == NOT_FOUND)
    {
      return fileId_invalid;
    }
  else
    {
      fileId der;

      llassert (fileTable_isDefined (ft));

      der = ft->elements[tindex]->fder;
      
      if (!fileId_isValid (der))
	{
	  der = tindex;
	}

      return der; 
    }
}

cstring
fileTable_getName (fileTable ft, fileId fid)
{
  if (!fileId_isValid (fid))
    {
      llcontbug 
	(message ("fileTable_getName: called with invalid type id: %d", fid));
      return cstring_makeLiteralTemp ("<invalid>");
    }

  llassert (fileTable_isDefined (ft));
  return (ft->elements[fid]->fname);
}

cstring
fileTable_getRootName (fileTable ft, fileId fid)
{
  fileId fder;

  if (!fileId_isValid (fid))
    {
      llcontbug (message ("fileTable_getName: called with invalid id: %d", fid));
      return cstring_makeLiteralTemp ("<invalid>");
    }

  if (!fileTable_isDefined (ft))
    {
      return cstring_makeLiteralTemp ("<no file table>");
    }

  fder = ft->elements[fid]->fder;

  if (fileId_isValid (fder))
    {
      return (ft->elements[fder]->fname);
    }
  else
    {
      return (ft->elements[fid]->fname);
    }
}

cstring
fileTable_getNameBase (fileTable ft, fileId fid)
{
  if (!fileId_isValid (fid))
    {
      llcontbug (message ("fileTable_getName: called with invalid id: %d", fid));
      return cstring_makeLiteralTemp ("<invalid>");
    }
  
  if (!fileTable_isDefined (ft))
    {
      return cstring_makeLiteralTemp ("<no file table>");
    }
  
  return (ft->elements[fid]->basename);
}

bool
fileTable_sameBase (fileTable ft, fileId f1, fileId f2)
{
  fileId fd1, fd2;

  if (!fileId_isValid (f1))
    {
      return FALSE;
    }

  if (!fileId_isValid (f2))
    {
      return FALSE;
    }

  llassert (fileTable_isDefined (ft));

  if (f1 == f2) 
    {
      return TRUE;
    }

  fd1 = ft->elements[f1]->fder;

  if (!fileId_isValid (fd1))
    {
      fd1 = f1;
    }

  fd2 = ft->elements[f2]->fder;


  if (!fileId_isValid (fd2))
    {
      fd2 = f2;
    }

  return (fd1 == fd2);
}

void
fileTable_cleanup (fileTable ft)
{
  int i;
  bool msg;
  int skip;
  
  llassert (fileTable_isDefined (ft));

  msg = ((ft->nentries > 40) && context_getFlag (FLG_SHOWSCAN));
  skip = ft->nentries / 10;

  if (msg)
    {
      (void) fflush (g_warningstream);
      displayScanOpen (cstring_makeLiteral ("cleaning"));
    }

  for (i = 0; i < ft->nentries; i++)
    {
      ftentry fe = ft->elements[i];

      if (fe->ftemp)
	{
	  /* let's be real careful now, hon! */
	  
	  /*
          ** Make sure it is really a derived file
	  */

	  
	  if (fe->ftype == FILE_LSLTEMP || fe->ftype == FILE_NODELETE)
	    {
	      ; /* already removed */ 
	    }
	  else if (fileId_isValid (fe->fder)) 
	    {
	      /* this should use close (fd) also... */
	      (void) osd_unlink (fe->fname);
	    }
	  else if (fe->ftype == FILE_MACROS)
	    {
	      (void) osd_unlink (fe->fname);
	    }
	  else
	    {
	      llbug (message ("Temporary file is not derivative: %s "
			      "(not deleted)", fe->fname));
	    }
	}
      else
	{
	  ;
	}

      if (msg && ((i % skip) == 0))
	{
	  displayScanContinue (cstring_makeLiteral (i == 0 ? " " : "."));
	}
    }

  if (msg)
    {
      displayScanClose ();
    }
}

void
fileTable_free (/*@only@*/ fileTable f)
{
  int i = 0;
  
  if (f == (fileTable)NULL) 
    {
      return;
    }

  while ( i < f->nentries ) 
    {
      ftentry_free (f->elements[i]);
      i++;
    }
  
  cstringTable_free (f->htable);
  sfree (f->elements);
  sfree (f->openelements); /*!! why didn't splint report this? */
  sfree (f);
}

/*
** unique temp filename are constructed from <dir><pre><pid><msg>.<suf>
** requires: <dir> must end in '/'
*/

static void nextMsg (char *msg)
{
  /*@+charint@*/
  if (msg[0] < 'Z') 
    {
      msg[0]++; 
    }
  else 
    {
      msg[0] = 'A';
      if (msg[1] < 'Z')
	{ 
	  msg[1]++; 
	}
      else
	{
	  msg[1] = 'A';
	  if (msg[2] < 'Z') 
	    {
	      msg[2]++;
	    }
	  else
	    {
	      msg[2] = 'A';
	      if (msg[3] < 'Z') 
		{
		  msg[3]++; 
		}
	      else
		{
		  llassertprint (FALSE, ("nextMsg: out of unique names!!!"));
		}
	    }
	}
    }
  /*@-charint@*/
}

static /*@only@*/ cstring makeTempName (cstring dir, cstring pre, cstring suf)
{
  static int pid = 0; 
  static /*@owned@*/ char *msg = NULL; 
  static /*@only@*/ cstring pidname = NULL;
  size_t maxlen;
  cstring smsg;

  llassert (cstring_length (pre) <= 3);

  /*
  ** We limit the temp name to 8 characters:
  **   pre: 3 or less
  **   msg: 3
  **   pid: 2  (% 100)
  */

  if (msg == NULL)
    {
      msg = mstring_copy ("AAA"); /* there are 26^3 temp names */
    }

  if (pid == 0) 
    {
      /*@+matchanyintegral@*/
      pid = osd_getPid ();
      /*@=matchanyintegral@*/
    }

  if (cstring_isUndefined (pidname)) 
    {
      pidname = message ("%d", pid % 100);
    }
  
  maxlen = (cstring_length (dir) + cstring_length (pre) + mstring_length (msg) 
	    + cstring_length (pidname) + cstring_length (suf) + 2);

  DPRINTF (("Dir: %s / %s / %s / %s / %s",
	    dir, pre, pidname, msg, suf));

  smsg = message ("%s%s%s%s%s", dir, pre, pidname, cstring_fromChars (msg), suf);
  nextMsg (msg);

  DPRINTF (("Trying: %s", smsg));

  while (osd_fileExists (smsg))
    {
      cstring_free (smsg);
      smsg = message ("%s%s%s%s%s", dir, pre, pidname, cstring_fromChars (msg), suf);
      nextMsg (msg);
    }

  return smsg;
}

static foentry
foentry_create (/*@exposed@*/ FILE *f, /*@only@*/ cstring fname)
{
  foentry t = (foentry) dmalloc (sizeof (*t));
  t->f = f;
  t->fname = fname;
  return t;
}

static void 
foentry_free (/*@only@*/ foentry foe)
{
  cstring_free (foe->fname);
  sfree (foe);
}

static void 
fileTable_addOpen (fileTable ft, /*@observer@*/ FILE *f, /*@only@*/ cstring fname)
{
  llassert (fileTable_isDefined (ft));

  if (ft->nopenspace <= 0) 
    {
      fileTable_growOpen (ft);
    }

  ft->nopenspace--;
  ft->openelements[ft->nopen] = foentry_create (f, fname);
  ft->nopen++;
}

FILE *fileTable_createFile (fileTable ft, cstring fname)
{
# if defined (WIN32) && !defined (BCC32)
  int fdesc = _open (cstring_toCharsSafe (fname), 
		     O_WRONLY | O_CREAT | O_TRUNC | O_EXCL, 
		     _S_IWRITE | S_IREAD);
# else
   int fdesc = open (cstring_toCharsSafe (fname), 
		     O_WRONLY | O_CREAT | O_TRUNC | O_EXCL,
		     S_IRUSR | S_IWUSR);
# endif

  if (fdesc == -1)
    {
      osd_setTempError ();
      llfatalerror (message ("Temporary file for "
			     "pre-processor output already exists.  Trying to "
			     "open: %s.",
			     fname));

      /*@notreached@*/ return NULL;
    }
  else
    {
      FILE *res = fdopen (fdesc, "w");
  
      if (res != NULL) 
	{
	  fileTable_addOpen (ft, res, cstring_copy (fname));
	  DPRINTF (("Opening file: %s / %p", fname, res));
	}
      else
	{
	  DPRINTF (("Error opening: %s", fname));
	}

      return res;
    }
}

FILE *fileTable_createMacrosFile (fileTable ft, cstring fname)
{
# if defined (WIN32) && !defined (BCC32)
  int fdesc = _open (cstring_toCharsSafe (fname), 
		     O_RDWR | O_CREAT | O_TRUNC | O_EXCL,
		     _S_IREAD | _S_IWRITE);
# else
  int fdesc = open (cstring_toCharsSafe (fname), 
		    O_RDWR | O_CREAT | O_TRUNC | O_EXCL, 
		    S_IRUSR | S_IWUSR);
# endif

  if (fdesc == -1)
    {
      osd_setTempError ();
      llfatalerror (message ("Temporary file for "
			     "pre-processor output already exists.  Trying to "
			     "open: %s.",
			     fname));

      /*@notreached@*/ return NULL;
    }
  else
    {
      FILE *res = fdopen (fdesc, "w+");
  
      if (res != NULL) 
	{
	  fileTable_addOpen (ft, res, cstring_copy (fname));
	  DPRINTF (("Opening file: %s / %p", fname, res));
	}
      else
	{
	  DPRINTF (("Error opening: %s", fname));
	}

      return res;
    }
}

FILE *fileTable_openReadFile (fileTable ft, cstring fname)
{
  FILE *res = fopen (cstring_toCharsSafe (fname), "r");

  if (res != NULL) 
    {
      fileTable_addOpen (ft, res, cstring_copy (fname));
      DPRINTF (("Opening read file: %s / %p", fname, res));
    }
  else
    {
      DPRINTF (("Cannot open read file: %s", fname));
    }

  return res;
}

/*
** Allows overwriting
*/

FILE *fileTable_openWriteFile (fileTable ft, cstring fname)
{
  FILE *res = fopen (cstring_toCharsSafe (fname), "w");

  if (res != NULL) {
    fileTable_addOpen (ft, res, cstring_copy (fname));
    DPRINTF (("Opening file: %s / %p", fname, res));
  }

  return res;
}

FILE *fileTable_openWriteUpdateFile (fileTable ft, cstring fname)
{
  FILE *res = fopen (cstring_toCharsSafe (fname), "w+");

  if (res != NULL) {
    fileTable_addOpen (ft, res, cstring_copy (fname));
    DPRINTF (("Opening file: %s / %p", fname, res));
  }

  return res;
}

bool fileTable_closeFile (fileTable ft, FILE *f)
{
  bool foundit = FALSE;
  int i = 0;

  llassert (fileTable_isDefined (ft));

  DPRINTF (("Closing file: %p", f));

  for (i = 0; i < ft->nopen; i++) 
    {
      if (ft->openelements[i]->f == f)
	{
	  DPRINTF (("Closing file: %p = %s", f, ft->openelements[i]->fname));
	  
	  if (i == ft->nopen - 1)
	    {
	      foentry_free (ft->openelements[i]);
	      ft->openelements[i] = NULL;
	    }
	  else
	    {
	      foentry_free (ft->openelements[i]);
	      ft->openelements[i] = ft->openelements[ft->nopen - 1];
	      ft->openelements[ft->nopen - 1] = NULL;
	    }

	  ft->nopen--;
	  ft->nopenspace++;
	  foundit = TRUE;
	  break;
	}
    }
  
  llassert (foundit);
  return (fclose (f) == 0);
}

void fileTable_closeAll (fileTable ft)
{
  int i = 0;

  llassert (fileTable_isDefined (ft));

  for (i = 0; i < ft->nopen; i++) 
    {
      /* 
	 lldiagmsg (message ("Unclosed file at exit: %s", ft->openelements[i]->fname)); 
      */
      
      if (ft->openelements[i]->f != NULL)
	{
	  (void) fclose (ft->openelements[i]->f); /* No check - cleaning up after errors */
	}

      ft->openelements[i]->f = NULL;
      foentry_free (ft->openelements[i]);
      ft->openelements[i] = NULL;
    }
  
  ft->nopenspace += ft->nopen;
  ft->nopen = 0;
}

