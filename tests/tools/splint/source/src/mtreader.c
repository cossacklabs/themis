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
** mtreader.c
**
** Controls reading of .mts files.
*/

# include "splintMacros.nf"
# include "basic.h"
# include "mtgrammar.h"
# include "mtscanner.h"

void mtreader_readFile (cstring infile)
{
  inputStream sourceFile;
  fileId fid;
  cstring fname;

  sourceFile = inputStream_create (infile, cstring_undefined, FALSE);
  
  if (!inputStream_getPath (context_getLarchPath (), sourceFile))
    {
      lldiagmsg
	(message ("Cannot find metastate file: %s", inputStream_fileName (sourceFile)));
      inputStream_free (sourceFile);
      return;
    }
  
  if (!inputStream_open (sourceFile))
    {
      lldiagmsg 
	(message ("Cannot open metastate file: %s",
		  inputStream_fileName (sourceFile)));
      inputStream_free (sourceFile);

      return;
    }


  fname = inputStream_fileName (sourceFile);

  if (fileTable_exists (context_fileTable (), fname))
    {
      fid = fileTable_lookup (context_fileTable (), fname);
    }
  else
    {
      fid = fileTable_addMetastateFile (context_fileTable (), fname);
    }

  context_setFileId (fid);
  displayScan (message ("reading metastate %s", fname));
  
  mtscanner_reset (sourceFile);
  context_enterMTfile ();
  (void) mtparse ();
  context_exitMTfile ();

  check (inputStream_close (sourceFile));
  inputStream_free (sourceFile);
}

void mtreader_processDeclaration (mtDeclarationNode decl)
{
  DPRINTF (("Processing state %s", mtDeclarationNode_unparse (decl)));
  mtDeclarationNode_process (decl, FALSE);
  mtDeclarationNode_free (decl);
}

void mtreader_processGlobalDeclaration (mtDeclarationNode decl)
{
  DPRINTF (("Processing state %s", mtDeclarationNode_unparse (decl)));
  mtDeclarationNode_process (decl, TRUE);
  mtDeclarationNode_free (decl);
}
