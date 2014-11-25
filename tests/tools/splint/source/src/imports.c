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
** imports.c
**
** module for importing LCL specs.
**
**  AUTHOR:
**	Yang Meng Tan,
**         Massachusetts Institute of Technology
*/

# include "splintMacros.nf"
# include "basic.h" 
# include "osd.h"
# include "llgrammar.h" /* need simpleOp, MULOP and logicalOp in makeInfixTermNode */
# include "lclscan.h"
# include "checking.h"
# include "imports.h"
# include "lslparse.h"
# include "lh.h"
# include "llmain.h"

void
outputLCSFile (char *path, char *msg, char *specname)
{
  static bool haserror = FALSE;
  char *sfile = mstring_concat (specname, ".lcs");
  char *outfile = mstring_concat (path, sfile);
  char *s;
  FILE *outfptr = fileTable_openWriteFile (context_fileTable (), cstring_fromChars (outfile));
  sfree (sfile);

  DPRINTF (("Output lcl file: %s / %s / %s", path, specname, outfile));
  
  /* check write permission */
  
  if (outfptr == NULL)	/* Cannot open file */
    {			
      if (!haserror)
	{
	  lclplainerror (message ("Cannot write to output file: %s", 
				  cstring_fromChars (outfile)));
	  haserror = TRUE;
	}
      sfree (outfile);
      return;
    }

  fprintf (outfptr, "%s", msg);
  fprintf (outfptr, "%s\n", LCL_PARSE_VERSION);
  
  /* output line %LCLimports foo bar ... */
  fprintf (outfptr, "%%LCLimports ");

  lsymbolSet_elements (g_currentImports, sym)
    {
      s = lsymbol_toChars (sym);

      if (s != NULL && !mstring_equal (s, specname))
	{
	  fprintf (outfptr, "%s ", s);
	}
    } end_lsymbolSet_elements;
  
  fprintf (outfptr, "\n");
  
  sort_dump (outfptr, TRUE);
  symtable_dump (g_symtab, outfptr, TRUE);

  check (fileTable_closeFile (context_fileTable (), outfptr));  
  sfree (outfile);  
}

void
importCTrait (void)
{
  cstring infile = cstring_undefined;
  filestatus status = osd_findOnLarchPath (cstring_makeLiteralTemp (CTRAITSYMSNAME), 
					   &infile);

  switch (status)
    {
    case OSD_FILEFOUND:
      /*
      ** This line was missing before version 2.3f.  Bug fix by Mike Smith.
      **    This looks like a bug - infile is already fully qualified path  
      **    parseSignatures() adds another path to the front and fails to   
      **    open the file.                                                  
      */
	   
      (void) parseSignatures (cstring_fromCharsNew (CTRAITSYMSNAME));
      (void) parseSignatures (infile);
      break;
    case OSD_FILENOTFOUND:
      /* try spec name */
      status = osd_findOnLarchPath (cstring_makeLiteralTemp (CTRAITSPECNAME),
				    &infile);

      if (status == OSD_FILEFOUND)
	{
	  callLSL (cstring_makeLiteralTemp (CTRAITSPECNAME),
		   message ("includes %s (%s for String)",
			    cstring_fromChars (CTRAITFILENAMEN), 
			    cstring_fromChars (sort_getName (g_sortCstring))));
	  cstring_free (infile);
	  break;
	}
      else
	{
	  lldiagmsg 
	    (message ("Unable to find %s or %s.  Check LARCH_PATH environment variable.",
		      cstring_fromChars (CTRAITSYMSNAME), 
		      cstring_fromChars (CTRAITSPECNAME)));
	  cstring_free (infile);
      	  llexit (LLFAILURE);
	}
    case OSD_PATHTOOLONG:
      lclbug (message ("importCTrait: the concatenated directory and file "
		       "name are too long: %s: "
		       "continuing without it", 
		       cstring_fromChars (CTRAITSPECNAME)));
      cstring_free (infile);
      break;
    }
}

/*
** processImport --- load imports from file
**
**    impkind: IMPPLAIN  file on SPEC_PATH
**                       # include "./file.h" if it exists,
**			 # include "<directory where spec was found>/file.h" if not.
**			   (warn if neither exists)
**            IMPBRACKET file in default LCL imports directory
**                       # include <file.h>
**            IMPQUOTE   file directly
**                       # include "file.h"
*/

void
processImport (lsymbol importSymbol, ltoken tok, impkind kind)
{
  bool readableP, oldexporting;
  bool compressedFormat = FALSE;
  inputStream imported, imported2, lclsource;
  char *bufptr;
  char *tmpbufptr;
  char *cptr;
  cstring name;
  lsymbol sym;
  char importName[MAX_NAME_LENGTH + 1];
  cstring importFileName;
  cstring path = cstring_undefined;
  cstring fpath, fpath2;
  mapping map;
  filestatus ret;

  importFileName = lsymbol_toString (importSymbol);
  name = cstring_concat (importFileName, cstring_makeLiteralTemp (IO_SUFFIX));

  /*
  ** find .lcs file
  */
  
  switch (kind)
    {
    case IMPPLAIN:
      path = message ("%s%c%s", cstring_fromChars (g_localSpecPath), PATH_SEPARATOR, 
		      context_getLarchPath ());

      break;
    case IMPBRACKET:
      path = cstring_copy (context_getLCLImportDir ());
      break;
    case IMPQUOTE:
      path = cstring_fromCharsNew (g_localSpecPath);
      break;
    default:
      llbuglit ("bad imports case\n");
    }

  if ((ret = osd_getPath (path, name, &fpath)) != OSD_FILEFOUND)
    {
      cstring fname2;
      
      if (ret == OSD_PATHTOOLONG)
	{
	  llfatalerrorLoc (cstring_makeLiteral ("Path too long"));
	}
      
      imported2 = inputStream_create (cstring_copy (importFileName),
				      LCL_EXTENSION, FALSE);
      fname2 = inputStream_fileName (imported2);

      if (osd_getPath (path, fname2, &fpath2) == OSD_FILEFOUND)
	{
	  llfatalerrorLoc
	    (message ("Specs must be processed before it can be imported: %s", 
		      fpath2));
	}
      else
	{
	  if (kind == IMPPLAIN || kind == IMPQUOTE)
	    {
	      llfatalerrorLoc (message ("Cannot find file to import: %s", name));
	    }
	  else
	    {
	      llfatalerrorLoc (message ("Cannot find standard import file: %s", name));
	    }
	}
    }

  
  imported = inputStream_create (fpath, cstring_makeLiteralTemp (IO_SUFFIX), FALSE);
    
  readableP = inputStream_open (imported);
    
  if (!readableP)
    {			/* can't read ? */
      llfatalerrorLoc (message ("Cannot open import file for reading: %s",
				inputStream_fileName (imported)));
    }

  bufptr = inputStream_nextLine (imported);

  if (bufptr == 0)
    {
      llerror (FLG_SYNTAX, message ("Import file is empty: %s", 
				    inputStream_fileName (imported)));
      cstring_free (name);
      (void) inputStream_close (imported);
      inputStream_free (imported);

      cstring_free (path);
      return;
    }

  /* was it processed successfully ? */
  if (firstWord (bufptr, "%FAILED"))
    {
      llfatalerrorLoc
	(message ("Imported file was not checked successfully: %s.", name));
    }
  
  /*
  ** Is it generated by the right version of the checker? 
  **
  ** Uncompressed  .lcs files start with %PASSED
  ** Compressed files start with %LCS 
  */
  
  if (firstWord (bufptr, "%PASSED"))
    {
      /* %PASSED Output from LCP Version 2.* and 3.* */
      /*                     1234567890123*/
      /*                                 +*/

      cptr = strstr (bufptr, "LCP Version");
      
      if (cptr != NULL)
	{
	  /* 
	  ** Only really old files start this way!
	  */

	  cptr += 12;
	  if (*cptr != '2' && *cptr != '3')
	    {
	      llfatalerrorLoc (message ("Imported file %s is obsolete: %s.",
					inputStream_fileName (imported),
					cstring_fromChars (bufptr)));
	    }
	}

      compressedFormat = FALSE;
    }
  else 
    {
      if (!firstWord (bufptr, "%LCS"))
	{
	  llfatalerrorLoc (message ("Imported file %s is not in correct format: %s",
				    inputStream_fileName (imported),
				    cstring_fromChars (bufptr)));
	}
      
      compressedFormat = TRUE;
    }
  
  /* push the imported LCL spec onto g_currentImports */

  context_enterImport ();
  
  bufptr = inputStream_nextLine (imported);
  llassert (bufptr != NULL);

  tmpbufptr = bufptr;

    /* expect %LCLimports foo bar ... */
  if (firstWord (bufptr, "%LCLimports "))
    {
      bufptr = bufptr + strlen ("%LCLimports ");
      while (sscanf (bufptr, "%s", importName) == 1)
	{
	  bufptr = bufptr + strlen (importName) + 1;	/* 1 for space */
	  sym = lsymbol_fromChars (importName);
	  if (sym == importSymbol || 
	      lsymbolSet_member (g_currentImports, sym))
	    {
	      /* ensure that the import list does not contain itself: an
		 invariant useful for checking imports cycles. */
	      lclsource = LCLScanSource ();
	      lclfatalerror (tok, 
			     message ("Imports cycle: %s%s imports %s",
				      importFileName,
				      LCL_EXTENSION,
				      cstring_fromChars (importName)));
	    }	  
	  /* push them onto g_currentImports */
	  /* evs - 94 Apr 3:  I don't think it should do this! */
	  /* (void) lsymbolSet_insert (g_currentImports, sym); */
	}
    }
  else
    {
      lclsource = LCLScanSource ();
      lclfatalerror (tok, message ("Unexpected line in imported file %s: %s", 
				   name, 
				   cstring_fromChars (bufptr)));
    }
	  
  /* read in the imported info */
  oldexporting = sort_setExporting (TRUE);

  map = mapping_create ();

  /* tok for error line numbering */

  if (!compressedFormat)
    {
      sort_import (imported, tok, map);	
    }
  
  (void) sort_setExporting (oldexporting);
  
  /* sort_import updates a mapping of old anonymous sorts to new
     anonymous sort that is needed in symtable_import */
  /* mapping_print (map); */
  
  if (!compressedFormat)
    {
      symtable_import (imported, tok, map);
    }
  else
    {
      /* symtable_loadImport (imported, tok, map); */
    }
  
  check (inputStream_close (imported));
  inputStream_free (imported);
  
  mapping_free (map);
  cstring_free (name);
  cstring_free (path);

  context_leaveImport ();  
}



