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
** mtDeclarationPieces.c
*/

# include "splintMacros.nf"
# include "basic.h"

extern mtDeclarationPieces mtDeclarationPieces_create (void) /*@*/ 
{
  return mtDeclarationPieces_undefined;
}

extern mtDeclarationPieces mtDeclarationPieces_append (mtDeclarationPieces node,
						       /*@only@*/ mtDeclarationPiece piece)
     /*@modifies node*/ 
{
  mtDeclarationPieces tnode = node;
  mtDeclarationPieces res = (mtDeclarationPieces) dmalloc (sizeof (*node));

  res->thisPiece = piece;
  res->rest = mtDeclarationPieces_undefined;

  if (mtDeclarationPieces_isUndefined (node)) {
    return res;
  }
  
  while (mtDeclarationPieces_isDefined (tnode->rest)) 
    {
      tnode = tnode->rest;
    }
  
  tnode->rest = res;
  return node;
}

extern cstring mtDeclarationPieces_unparse (mtDeclarationPieces node) /*@*/ 
{
  cstring res = cstring_newEmpty ();

  while (mtDeclarationPieces_isDefined (node)) 
    {
      res = message ("%q%q; ", res, mtDeclarationPiece_unparse (node->thisPiece));
      node = node->rest;
    }

  return res;
}

mtDeclarationPiece
mtDeclarationPieces_findPiece (mtDeclarationPieces pieces, mtPieceKind kind)
{
  bool foundone = FALSE;
  mtDeclarationPiece res = mtDeclarationPiece_undefined;

  while (mtDeclarationPieces_isDefined (pieces)) 
    {
      if (mtDeclarationPiece_matchKind (pieces->thisPiece, kind)) 
	{
	  if (foundone) 
	    {
	      llassert (mtDeclarationPiece_isDefined (res));
	      voptgenerror 
		(FLG_SYNTAX,
		 message ("Metastate declaration has duplicate pieces: %q / %q",
			  mtDeclarationPiece_unparse (res),
			  mtDeclarationPiece_unparse (pieces->thisPiece)),
		 g_currentloc);
	    } 
	  else 
	    {
	      foundone = TRUE;
	      llassert (mtDeclarationPiece_isUndefined (res));
	      res = pieces->thisPiece;
	    }
	}
      
      pieces = pieces->rest;
    }
  
  return res;
}

extern void mtDeclarationPieces_free (/*@only@*/ mtDeclarationPieces node)
{
  if (mtDeclarationPieces_isDefined (node))
    {
      mtDeclarationPiece_free (node->thisPiece);
      mtDeclarationPieces_free (node->rest);
    }
 
  sfree (node);
}
