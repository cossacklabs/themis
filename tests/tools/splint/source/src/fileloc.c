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
** fileloc.c
*/
/*
 * Modified by Herbert 04/19/97:
 * - added new include file portab.h (now in osd.h)
 * - added new private function fileloc_filenameForCpp() to handle
 *   filenames belonging to "#line" statements for OS/2 and MSDOS. It
 *   gets called by fileloc_lineMarker() and fileloc_previousLineMarker()
 *   instead of fileloc_unparseFilename().
 */

# include "splintMacros.nf"
# include "basic.h"
# include "osd.h"

static /*@only@*/ fileloc fileloc_createPrim (flkind p_kind, fileId p_fid, int p_line, int p_col) /*@*/ ;

/*
** builtin locs are never free'd
*/

static /*@owned@*/ fileloc s_builtinLoc = fileloc_undefined;
static /*@owned@*/ fileloc s_externalLoc = fileloc_undefined;

void fileloc_destroyMod ()
{
  if (fileloc_isDefined (s_builtinLoc))
    {
      sfree (s_builtinLoc);
      s_builtinLoc = fileloc_undefined;
    }

  if (fileloc_isDefined (s_externalLoc))
    {
      sfree (s_externalLoc);
      s_externalLoc = fileloc_undefined;
    }
}

static flkind fileId_kind (fileId s)
{
  cstring fname = fileTable_rootFileName (s);
  
  if (fileLib_isLCLFile (fname))
    {
      return (FL_SPEC);
    }
  else if (cstring_equalPrefix (fname, cstring_makeLiteralTemp (SYSTEM_LIBDIR)))
    {
      return (FL_STDHDR); 
    }
  else
    {
      return (FL_NORMAL);
    }
}  

fileloc
fileloc_decColumn (fileloc f, int x)
{
  fileloc ret = fileloc_copy (f);

  llassert (x >= 0);

  if (x > 0 && fileloc_isDefined (ret))
    {
      llassertprint (ret->column > x, ("decColumn %s: %d", fileloc_unparse (f), x));
      ret->column -= x;
    }

  return ret;
}

fileloc
fileloc_noColumn (fileloc f)
{
  if (fileloc_isDefined (f))
    {
      fileloc ret = fileloc_copy (f);

      if (fileloc_isDefined (ret))
	{
	  ret->column = 0;
	}

      return ret;
    }
  else
    {
      return fileloc_undefined;
    }
}

void
fileloc_subColumn (fileloc f, int x)
{
  if (x > 0 && fileloc_isDefined (f))
    {
      llassert (f->column > x);
      f->column -= x;
    }
}

fileloc fileloc_copy (fileloc f)
{
  if (fileloc_isDefined (f))
    {
      if (fileloc_isBuiltin (f) || fileloc_isExternal (f))
	{
	  /*
	  ** Legitimate (spurious) errors reported since no copy
	  ** is made.
	  */

	  /*@i3@*/ return f; /* No copy is necessary. */
	}
      else
	{
	  return (fileloc_createPrim (f->kind, f->fid, f->lineno, f->column));
	}
    }
  else
    {
      return fileloc_undefined;
    }
}

fileloc fileloc_update (/*@only@*/ fileloc old, fileloc fnew)
{
  if (fileloc_isUndefined (fnew))
    {
      fileloc_free (old);
      return fileloc_undefined;
    }
  else if (fileloc_isUndefined (old) || fileloc_isBuiltin (old) || fileloc_isExternal (old))
    {
      return (fileloc_copy (fnew));
    }
  else
    {
      old->kind   = fnew->kind;
      old->fid    = fnew->fid; 
      old->lineno = fnew->lineno;
      old->column = fnew->column;

      return old;
    }
}

fileloc fileloc_updateFileId (/*@only@*/ fileloc old, fileId s)
{
  if (fileloc_isUndefined (old) || fileloc_isBuiltin (old) || fileloc_isExternal (old))
    {
      return (fileloc_create (s, 1, 1));
    }
  else
    {
      old->kind   = fileId_kind (s);
      old->fid    = s; 
      old->lineno = 1;
      old->column = 1;

      return old;
    }
}

void
fileloc_free (/*@only@*/ fileloc f)
{
  if (fileloc_isDefined (f))
    {
      if (f != g_currentloc)
	{
	  if (fileloc_isBuiltin (f) || fileloc_isExternal (f))
	    {
	      ; /* don't free */
	    }
	  else
	    {
	      sfree (f);  
	      /*@-branchstate@*/ 
	    } 
	}
      else
	{
	  ; /* Don't free g_currentloc ever! */
	}
      /*@=branchstate@*/
    }
}

void
fileloc_reallyFree (/*@only@*/ fileloc f)
{
  if (fileloc_isDefined (f))
    {
      if (fileloc_isBuiltin (f) || fileloc_isExternal (f))
	{
	  ; /* don't free */
	}
      else
	{
	  sfree (f);  
	/*@-branchstate@*/ } /*@=branchstate@*/
    }
}

cstring fileloc_getBase (fileloc f)
{
  llassert (fileloc_isDefined (f));

  return (fileTable_fileNameBase (f->fid));
}

bool
fileloc_equal (fileloc f1, fileloc f2)
{
  return ((f1 == f2)
	  || (fileloc_isDefined (f1) && fileloc_isDefined (f2) 
	      && ((f1->column == f2->column) && 
		  (f1->lineno == f2->lineno) && fileloc_sameFile (f1, f2))));
}

int
fileloc_compare (fileloc f1, fileloc f2)
{
  if (fileloc_isUndefined (f1))
    {
      if (fileloc_isUndefined (f2)) return 0;
      return -1;
    }
  
  if (fileloc_isUndefined (f2))
    return 1;

  /*@access fileId@*/ 
  INTCOMPARERETURN (f1->fid, f2->fid); 
  /*@noaccess fileId@*/


  /* drl 8-11-01 fix what I think is a bug
     lineno should more important than column number*/

  INTCOMPARERETURN (f1->lineno, f2->lineno);   
  INTCOMPARERETURN (f1->column, f2->column);
  
  return 0;
}

bool
fileloc_withinLines (fileloc f1, fileloc f2, int n)
{
  
  return (fileloc_isDefined (f1) && 
	  fileloc_isDefined (f2) &&
	  ((f2->lineno <= f1->lineno + n) 
	   && (f2->lineno >= f1->lineno)
	   && fileloc_sameFile (f1, f2)));
}

bool
fileloc_lessthan (fileloc f1, fileloc f2)
{
  /*@access fileId*/
  return ((fileloc_isDefined (f1) && fileloc_isDefined (f2))
	  && ((f1->fid < f2->fid)
	      || ((f1->fid == f2->fid)
		  && ((f1->lineno < f2->lineno)
		      || ((f1->lineno == f2->lineno)
			  && (f1->column < f2->column))))));
  /*@noaccess fileId*/
}

/*
** returns true if f1 and f2 are different files,
** or f1 is before f2 in same file
*/

bool
fileloc_notAfter (fileloc f1, fileloc f2)
{
  /*@access fileId*/
  return ((fileloc_isDefined (f1) && fileloc_isDefined (f2))
	  && ((f1->fid != f2->fid)
	      || ((f1->lineno < f2->lineno)
		  || ((f1->lineno == f2->lineno)
		      && (f1->column <= f2->column)))));
  /*@noaccess fileId@*/
}

bool
fileloc_isStandardLibrary (fileloc f)
{
  cstring s = fileloc_getBase (f);

  return (cstring_equalLit (s, LLSTDLIBS_NAME)
	  || cstring_equalLit (s, LLSTRICTLIBS_NAME)
	  || cstring_equalLit (s, LLUNIXLIBS_NAME)
	  || cstring_equalLit (s, LLUNIXSTRICTLIBS_NAME)
	  || cstring_equalLit (s, LLPOSIXSTRICTLIBS_NAME)
	  || cstring_equalLit (s, LLPOSIXLIBS_NAME));
}

bool
fileloc_sameFileAndLine (fileloc f1, fileloc f2)
{
  return (fileloc_sameFile (f1, f2)
	  && (fileloc_isDefined (f1) && fileloc_isDefined (f2)
	      && f1->lineno == f2->lineno));
}

bool
fileloc_sameFile (fileloc f1, fileloc f2)
{
  if ((fileloc_isUndefined (f1) || (fileloc_isUndefined (f2)) 
       || (fileloc_isLib (f1)) || (fileloc_isLib (f2))))
    {
      return FALSE;
    }
  else
    {
      return (fileId_equal (f1->fid, f2->fid));
    }
}

bool
fileloc_sameModule (fileloc f1, fileloc f2)
{
  if (fileloc_isUndefined (f1))
    {
      return (fileloc_isUndefined (f2));
    }
  else if (fileloc_isUndefined (f2))
    {
      return (FALSE);
    }
  else
    {
      if (fileloc_isBuiltin (f1) || fileloc_isBuiltin (f2)
	  || fileloc_isExternal (f1) || fileloc_isExternal (f2))
	{
	  return fileloc_sameFile (f1, f2);
	}
      else
	{
	  cstring s1 = fileloc_getBase (f1);
	  cstring s2 = fileloc_getBase (f2);
	  
	  return (cstring_equal (s1, s2)); 
	}
    }
}

bool
fileloc_sameBaseFile (fileloc f1, fileloc f2)
{
  if (fileloc_isUndefined (f1))
    {
      return (fileloc_isUndefined (f2));
    }
  else if (fileloc_isUndefined (f2))
    {
      return (FALSE);
    }
  else
    {
      return (fileId_baseEqual (f1->fid, f2->fid));
    }
}

bool fileloc_isSystemFile (fileloc f1)
{
  if (fileloc_isDefined (f1)
      && !fileloc_isBuiltin (f1)
      && !fileloc_isExternal (f1))
    {
      return (fileTable_isSystemFile (context_fileTable (), f1->fid));
    }

  return FALSE;
}

bool fileloc_isXHFile (fileloc f1)
{
  if (fileloc_isDefined (f1)
      && !fileloc_isBuiltin (f1)
      && !fileloc_isExternal (f1))
    {
      DPRINTF (("Fileloc is XH: [%p] %s", f1, fileloc_unparse (f1)));
      return (fileTable_isXHFile (context_fileTable (), f1->fid));
    }
  
  return FALSE;
}

bool
fileloc_almostSameFile (fileloc f1, fileloc f2)
{
  if ((fileloc_isUndefined (f1) || (fileloc_isUndefined (f2)) 
       || (fileloc_isLib (f1)) || (fileloc_isLib (f2))))
    {
      return FALSE;
    }
  else
    {
      if (fileId_baseEqual (f1->fid, f2->fid))
	{
	  return TRUE;
	}
      else if (fileTable_isSystemFile (context_fileTable (), f1->fid)
	       || fileTable_isSystemFile (context_fileTable (), f2->fid))
	{
	  return TRUE;
	}
      else if (fileTable_isSpecialFile (context_fileTable (), f1->fid)
	       || fileTable_isSpecialFile (context_fileTable (), f2->fid))
	{
	  return (cstring_equal (fileloc_getBase (f1), 
				 fileloc_getBase (f2)));
	}
      else 
	{
	  return FALSE;
	}
    }
}

/*@only@*/ fileloc
fileloc_fromTok (ltoken t) 
{
  cstring fname = ltoken_fileName (t);
  fileId fid = fileTable_lookup (context_fileTable (), fname);
  fileloc fl;

  if (!fileId_isValid (fid))
    {
      fid = fileTable_addLCLFile (context_fileTable (), fname);
    }
  
  fl = fileloc_create (fid, (int) ltoken_getLine (t), (int) ltoken_getCol (t));
  
  return (fl);
}

/*@only@*/ fileloc
fileloc_createLib (cstring ln)
{
  flkind fk = FL_LIB;
  fileId fid = fileTable_lookup (context_fileTable (), ln);

  if (!fileId_isValid (fid))
    {
      fid = fileTable_addLibraryFile (context_fileTable (), ln);
    }

  if (cstring_equalPrefix (ln, cstring_makeLiteralTemp (SYSTEM_LIBDIR)))
    {
      fk = FL_STDLIB;
    }

  return (fileloc_createPrim (fk, fid, 0, 0));
}

fileloc fileloc_createRc (cstring name)
{
  fileId fid = fileTable_addFile (context_fileTable (), name);

  return (fileloc_createPrim (FL_RC, fid, 0, 0));
}

fileloc fileloc_createExternal (void)
{
  /*@i@*/ return (fileloc_getExternal ()); 
}

fileloc fileloc_getExternal (void)
{
  if (s_externalLoc == fileloc_undefined) 
    {
      s_externalLoc = fileloc_createPrim (FL_EXTERNAL, fileId_invalid, 0, 0);
    }

  return s_externalLoc;
}

fileloc fileloc_observeBuiltin ()
{
  /*@-onlytrans@*/ return (fileloc_getBuiltin ()); /*@=onlytrans@*/
}

fileloc fileloc_getBuiltin ()
{
  static /*@owned@*/ fileloc res = fileloc_undefined;

  if (res == fileloc_undefined)
    {
      res = fileloc_createPrim (FL_BUILTIN, fileId_invalid, 0, 0); 
    }

  return res;
}

fileloc
fileloc_makePreproc (fileloc loc)
{
  if (fileloc_isDefined (loc))
    {
      return (fileloc_createPrim (FL_PREPROC, loc->fid, 
				  loc->lineno, loc->column));
    }

  return (fileloc_createPrim (FL_PREPROC, fileId_invalid, 0, 0));
}

fileloc
fileloc_makePreprocPrevious (fileloc loc)
{
  if (fileloc_isDefined (loc))
    {
      if (loc->lineno > 1)
	{
	  return (fileloc_createPrim (FL_PREPROC, loc->fid,
				      loc->lineno - 1, 0));
	}
      else
	{
	  return (fileloc_createPrim (FL_PREPROC, loc->fid,
				      loc->lineno, 0));
	}
    }

  return (fileloc_createPrim (FL_PREPROC, fileId_invalid, 0, 0));
}

/* We pretend the result is only, because fileloc_free doesn't free it! */
/*@only@*/ fileloc
fileloc_createBuiltin ()
{
  if (fileloc_isUndefined (s_builtinLoc))
    {
      s_builtinLoc = fileloc_createPrim (FL_BUILTIN, fileId_invalid, 0, 0); 
    }

  /*@-globstate@*/ /*@-retalias@*/ 
  return s_builtinLoc;
  /*@=globstate@*/ /*@=retalias@*/ 
}

/*@only@*/ fileloc
fileloc_createImport (cstring fname, int lineno)
{
  fileId fid = fileTable_lookup (context_fileTable (), fname);

  if (!fileId_isValid (fid))
    {
      fid = fileTable_addImportFile (context_fileTable (), fname);
    }

  return (fileloc_createPrim (FL_IMPORT, fid, lineno, 0));
}

static /*@only@*/ fileloc
fileloc_createPrim (flkind kind, fileId fid, int line, int col)
{
  fileloc f = (fileloc) dmalloc (sizeof (*f));
  
  f->kind   = kind;
  f->fid    = fid; 
  f->lineno = line;
  f->column = col;

  DPRINTF (("Fileloc create: [%p] %s", f, fileloc_unparse (f)));
  return (f);
}

/*@only@*/ fileloc
fileloc_createSpec (fileId fid, int line, int col)
{
  return (fileloc_createPrim (FL_SPEC, fid, line, col));
}

fileloc fileloc_create (fileId fid, int line, int col)
{
  return (fileloc_createPrim (fileId_kind (fid), fid, line, col));
}

/*@observer@*/ cstring
fileloc_filename (fileloc f)
{
  return (fileloc_isDefined (f) ? fileTable_rootFileName (f->fid) : cstring_makeLiteralTemp ("<unknown>"));
}

/*@only@*/ cstring fileloc_outputFilename (fileloc f)
{
  if (fileloc_isDefined (f))
    {
      if (fileId_isValid (f->fid))
	{
	  return osd_outputPath (fileTable_rootFileName (f->fid));
	}
      else
	{
	  return cstring_makeLiteral ("<invalid>");
	}
    }
  else
    {
      return cstring_makeLiteral ("<unknown>");
    }
}

cstring
fileloc_unparseFilename (fileloc f)
{
  if (fileloc_isDefined (f))
    {
      switch (f->kind)
	{
	case FL_LIB:
	  return (message ("load file %q", fileloc_outputFilename (f)));
	case FL_BUILTIN:
	  return (cstring_makeLiteral ("# builtin #"));
	case FL_IMPORT:
	  return (message ("import file %q", fileloc_outputFilename (f)));
	case FL_EXTERNAL:
	  return (cstring_makeLiteral ("<external>"));
	default: 
	  return (fileloc_outputFilename (f));
	}
    }
  return cstring_undefined;
}

int
fileloc_lineno (fileloc f)
{
  return (fileloc_isDefined (f) ? f->lineno : -1);
}

int
fileloc_column (fileloc f)
{
  return (fileloc_isDefined (f) ? f->column : -1);
}

/*@only@*/ cstring
fileloc_unparse (fileloc f)
{
  static bool in_funparse = FALSE;
  bool parenFormat = context_getFlag (FLG_PARENFILEFORMAT); 
  bool htmlFormat = context_getFlag (FLG_HTMLFILEFORMAT);
  cstring res = cstring_undefined;

  /* watch out for recursive calls when debugging... */
  llassert (!in_funparse);
  in_funparse = TRUE;

  if (fileloc_isDefined (f))
    {
       switch (f->kind)
	{
	case FL_BUILTIN:
	  {
	    res = cstring_makeLiteral ("Command Line");
	    break;
	  }
	case FL_IMPORT:
	  if (parenFormat)
	    {
	      res = message ("import file %q(%d)", fileloc_outputFilename (f), f->lineno);
	    }
	  else
	    {
	      res = message ("import file %q:%d", fileloc_outputFilename (f), f->lineno);
	    }
	  break;
	case FL_PREPROC:
	  {
	    if (parenFormat)
	      {
		res = message ("%q(%d)", fileloc_outputFilename (f), f->lineno);
	      }
	    else
	      {
		res = message ("%q:%d", fileloc_outputFilename (f), f->lineno);
	      }
	    
	    break;
	  }
	case FL_EXTERNAL:
	  res = cstring_makeLiteral ("<external>");
	  break;
	default:
	  {
	    cstring fname;
	    
	    if (f->kind == FL_LIB)
	      {
		fname = message ("load file %q", fileloc_outputFilename (f));

		if (!context_getFlag (FLG_SHOWLOADLOC))
		  {
		    res = fname;
		    break;
		  }
	      }
	    else
	      {
		fname = fileloc_outputFilename (f);
	      }

	    if (context_getFlag (FLG_SHOWCOL))
	      {
		if (fileloc_linenoDefined (f))
		  {
		    if (fileloc_columnDefined (f))
		      {
			if (parenFormat)
			  {
			    res = message ("%q(%d,%d)", fname, f->lineno, f->column);
			  }
			else
			  {
			    res = message ("%q:%d:%d", fname, f->lineno, f->column);
			  }
		      }
		    else
		      {
			if (parenFormat)
			  {
			    res = message ("%q(%d)", fname, f->lineno);
			  }
			else
			  {
			    res = message ("%q:%d", fname, f->lineno);
			  }
		      }
		  }
		else
		  {
		    res = fname;
		    /*@-branchstate@*/ /* spurious warnings reporteded because of break above */
		  }
	      }
	    else if (fileloc_linenoDefined (f))
	      {
		if (parenFormat)
		  {
		    res = message ("%q(%d)", fname, f->lineno);
		  }
		else
		  {
		    res = message ("%q:%d", fname, f->lineno);
		  }
	      }
	    else
	      {
		res = fname;
	      }
	  }
	}

       if (htmlFormat && fileloc_linenoDefined (f))
	 {
	   res = message ("<a href=\"#line%d\">%s</a>", f->lineno, res);
	 }
    }
  else
    {
      res = cstring_makeLiteral ("< Location unknown >");
    }
  /*@=branchstate@*/ /* this is a spurious warning because of the break */
  
  in_funparse = FALSE;
  return res;
}

/*@only@*/ cstring
fileloc_unparseRaw (cstring fname, int lineno)
{
  if (!cstring_isEmpty (fname))
    {
      bool parenFormat = context_getFlag (FLG_PARENFILEFORMAT); 
      
      if (parenFormat)
	{
	  return (message ("%q(%d)", osd_outputPath (fname), lineno));
	}
      else
	{
	  return (message ("%q:%d", osd_outputPath (fname), lineno));
	}
    }
  else
    {
      return cstring_makeLiteral ("Command Line");
    }
}

/*@only@*/ cstring
fileloc_unparseRawCol (cstring fname, int lineno, int col)
{
  if (!cstring_isEmpty (fname))
    {
      if (context_getFlag (FLG_SHOWCOL)) 
	{
	  bool parenFormat = context_getFlag (FLG_PARENFILEFORMAT); 
	  
	  if (parenFormat)
	    {
	      return (message ("%q(%d,%d)", osd_outputPath (fname), lineno, col));
	    }
	  else
	    {
	      return (message ("%q:%d:%d", osd_outputPath (fname), lineno, col));
	    }
	}
      else
	{
	  return fileloc_unparseRaw (fname, lineno);
	}
    }
  else
    {
      return cstring_makeLiteral ("Command Line");
    }
}

bool fileloc_isSpecialFile (fileloc f)
{
  if (fileloc_isDefined (f) 
      && fileId_isValid (f->fid))
    {
      return (fileTable_isSpecialFile (context_fileTable (), f->fid));
    }
  else
    {
      return FALSE;
    }
}

bool fileloc_isHeader (fileloc f)
{
  /* returns true if is not a .c file */

  return (fileloc_isDefined (f) && fileId_isValid (f->fid)
	  && fileId_isHeader (f->fid));
}

bool fileloc_isSpec (fileloc f)
{
  return (fileloc_isDefined (f) && 
	  (f->kind == FL_LIB || f->kind == FL_STDLIB || f->kind == FL_SPEC));
}

bool fileloc_isRealSpec (fileloc f)
{
  return (fileloc_isDefined (f) && (f->kind == FL_SPEC));
}

bool fileloc_isLib (fileloc f)
{
  return (fileloc_isDefined (f) 
	  && (f->kind == FL_LIB || f->kind == FL_STDHDR || f->kind == FL_STDLIB));
}

bool fileloc_isStandardLib (fileloc f)
{
  return (fileloc_isDefined (f) && f->kind == FL_STDLIB);
}

/*@only@*/ cstring fileloc_unparseDirect (fileloc fl)
{
  if (fileloc_isDefined (fl))
    {
      return (message ("%d:%d:%d",
		       /*@access fileId@*/ (int) fl->fid, /*@noaccess fileId@*/
		       fl->lineno, fl->column));
    }
  else
    {
      return (cstring_makeLiteral ("<undef>"));
    }
}

bool fileloc_isUser (fileloc f)
{
  return (fileloc_isDefined (f) 
	  && f->kind == FL_NORMAL);
}




