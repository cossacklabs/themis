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
** mtDeclarationPiece.c
*/

# include "splintMacros.nf"
# include "basic.h"

static mtDeclarationPiece 
mtDeclarationPiece_create (mtPieceKind kind, /*@null@*/ /*@only@*/ void *node)
{
  mtDeclarationPiece res = (mtDeclarationPiece) dmalloc (sizeof (*res));

  res->kind = kind;
  res->node = node;

  return res;
}

extern mtDeclarationPiece mtDeclarationPiece_createContext (mtContextNode node) /*@*/ 
{ 
  return mtDeclarationPiece_create (MTP_CONTEXT, (void *) node);
}

extern mtDeclarationPiece mtDeclarationPiece_createValues (mtValuesNode node) /*@*/ 
{
  return mtDeclarationPiece_create (MTP_VALUES, (void *) node);
}

extern mtDeclarationPiece mtDeclarationPiece_createDefaults (mtDefaultsNode node) /*@*/ 
{
  return mtDeclarationPiece_create (MTP_DEFAULTS, (void *) node);
}

extern mtDeclarationPiece mtDeclarationPiece_createValueDefault (mttok node) /*@*/ 
{
  llassert (mttok_isIdentifier (node));
  return mtDeclarationPiece_create (MTP_DEFAULTVALUE, (void *) node);
}

extern mtDeclarationPiece mtDeclarationPiece_createAnnotations (mtAnnotationsNode node) /*@*/ 
{
  return mtDeclarationPiece_create (MTP_ANNOTATIONS, (void *) node);
}

extern mtDeclarationPiece mtDeclarationPiece_createMerge (mtMergeNode node) /*@*/ 
{
  return mtDeclarationPiece_create (MTP_MERGE, (void *) node);
}

extern mtDeclarationPiece mtDeclarationPiece_createTransfers (mtTransferClauseList node) /*@*/ 
{
  return mtDeclarationPiece_create (MTP_TRANSFERS, (void *) node);
}

extern mtDeclarationPiece mtDeclarationPiece_createPreconditions (mtTransferClauseList node) /*@*/ 
{
  return mtDeclarationPiece_create (MTP_PRECONDITIONS, (void *) node);
}

mtDeclarationPiece mtDeclarationPiece_createPostconditions (mtTransferClauseList node) /*@*/ 
{
  return mtDeclarationPiece_create (MTP_POSTCONDITIONS, (void *) node);
}

mtDeclarationPiece mtDeclarationPiece_createLosers (mtLoseReferenceList node) /*@*/ 
{
  return mtDeclarationPiece_create (MTP_LOSERS, (void *) node);
}

/*@only@*/ cstring mtDeclarationPiece_unparse (mtDeclarationPiece p)
{
  if (mtDeclarationPiece_isUndefined (p))
    {
      return cstring_undefined;
    }

  switch (p->kind)
    {
    case MTP_CONTEXT:
      /*@access mtContextNode@*/
      return mtContextNode_unparse ((mtContextNode) p->node);
      /*@noaccess mtContextNode@*/
    case MTP_VALUES:
      /*@access mtValuesNode@*/
      return mtValuesNode_unparse ((mtValuesNode) p->node);
      /*@noaccess mtValuesNode@*/
    case MTP_DEFAULTS:
      /*@access mtDefaultsNode@*/
      return mtDefaultsNode_unparse ((mtDefaultsNode) p->node);
      /*@noaccess mtDefaultsNode@*/
    case MTP_DEFAULTVALUE:
      /*@access mttok@*/
      return message ("default %q", mttok_getText ((mttok) p->node));
      /*@noaccess mttok@*/
    case MTP_ANNOTATIONS:
      /*@access mtAnnotationsNode@*/
      return mtAnnotationsNode_unparse ((mtAnnotationsNode) p->node);
      /*@noaccess mtAnnotationsNode@*/
    case MTP_MERGE:
      /*@access mtMergeNode@*/
      return mtMergeNode_unparse ((mtMergeNode) p->node);
      /*@noaccess mtMergeNode@*/
    case MTP_TRANSFERS:
    case MTP_PRECONDITIONS:
    case MTP_POSTCONDITIONS:
      /*@access mtTransferClauseList@*/
      return mtTransferClauseList_unparse ((mtTransferClauseList) p->node);
      /*@noaccess mtTransferClauseList@*/
    case MTP_LOSERS:
      /*@access mtLoseReferenceList@*/
      return mtLoseReferenceList_unparse ((mtLoseReferenceList) p->node);
      /*@noaccess mtLoseReferenceList@*/
    case MTP_DEAD:
      return cstring_makeLiteral ("Dead Piece");
    }

  BADBRANCHRET (cstring_undefined);
}

extern bool mtDeclarationPiece_matchKind (mtDeclarationPiece p, mtPieceKind kind) /*@*/
{
  if (mtDeclarationPiece_isDefined (p))
    {
      return (p->kind == kind);
    }
  else 
    {
      return FALSE;
    }
}

extern mtContextNode mtDeclarationPiece_getContext (mtDeclarationPiece node)
{
  llassert (mtDeclarationPiece_isDefined (node));
  llassert (node->kind == MTP_CONTEXT);

  /*@-abstract@*/
  return (mtContextNode) node->node;
  /*@=abstract@*/
}

extern mtContextNode mtDeclarationPiece_stealContext (mtDeclarationPiece node)
{
  mtContextNode res;

  llassert (mtDeclarationPiece_isDefined (node));
  llassert (node->kind == MTP_CONTEXT);

  /*@-abstract@*/
  res = (mtContextNode) node->node;
  /*@=abstract@*/
  node->kind = MTP_DEAD;
  node->node =  NULL;
  return res;  
}

extern mtDefaultsNode mtDeclarationPiece_getDefaults (mtDeclarationPiece node)
{
  llassert (mtDeclarationPiece_isDefined (node));
  llassert (node->kind == MTP_DEFAULTS);

  /*@-abstract@*/
  return (mtDefaultsNode) node->node;
  /*@=abstract@*/
}

extern cstring mtDeclarationPiece_getDefaultValue (mtDeclarationPiece node)
{
  llassert (mtDeclarationPiece_isDefined (node));
  llassert (node->kind == MTP_DEFAULTVALUE);

  /*@-abstract@*/
  return mttok_observeText ((mttok) node->node);
  /*@=abstract@*/
}

extern mtAnnotationsNode mtDeclarationPiece_getAnnotations (mtDeclarationPiece node)
{
  llassert (mtDeclarationPiece_isDefined (node));
  llassert (node->kind == MTP_ANNOTATIONS);

  /*@-abstract@*/
  return (mtAnnotationsNode) node->node;
  /*@=abstract@*/
}

extern mtMergeNode mtDeclarationPiece_getMerge (mtDeclarationPiece node)
{
  llassert (mtDeclarationPiece_isDefined (node));
  llassert (node->kind == MTP_MERGE);

  /*@-abstract@*/
  return (mtMergeNode) node->node;
  /*@=abstract@*/
}

extern mtTransferClauseList mtDeclarationPiece_getTransfers (mtDeclarationPiece node)
{
  llassert (mtDeclarationPiece_isDefined (node));
  llassert (node->kind == MTP_TRANSFERS);

  /*@-abstract@*/
  return (mtTransferClauseList) node->node;
  /*@=abstract@*/
}

extern mtTransferClauseList mtDeclarationPiece_getPreconditions (mtDeclarationPiece node)
{
  llassert (mtDeclarationPiece_isDefined (node));
  llassert (node->kind == MTP_PRECONDITIONS);

  /*@-abstract@*/
  return (mtTransferClauseList) node->node;
  /*@=abstract@*/
}

extern mtTransferClauseList mtDeclarationPiece_getPostconditions (mtDeclarationPiece node)
{
  llassert (mtDeclarationPiece_isDefined (node));
  llassert (node->kind == MTP_POSTCONDITIONS);

  /*@-abstract@*/
  return (mtTransferClauseList) node->node;
  /*@=abstract@*/
}

extern mtLoseReferenceList mtDeclarationPiece_getLosers (mtDeclarationPiece node)
{
  llassert (mtDeclarationPiece_isDefined (node));
  llassert (node->kind == MTP_LOSERS);

  /*@-abstract@*/
  return (mtLoseReferenceList) node->node;
  /*@=abstract@*/
}

extern mtValuesNode mtDeclarationPiece_getValues (mtDeclarationPiece node)
{
  llassert (mtDeclarationPiece_isDefined (node));
  llassert (node->kind == MTP_VALUES);

  /*@-abstract@*/
  return (mtValuesNode) node->node;
  /*@=abstract@*/
}

extern void mtDeclarationPiece_free (/*@only@*/ mtDeclarationPiece node) 
{
  if (node != NULL)
    {
      switch (node->kind)
	{
	case MTP_DEAD:
	  llassert (node->node == NULL);
	  break;

	case MTP_CONTEXT:
	  /*@access mtContextNode@*/
	  mtContextNode_free ((mtContextNode) node->node);
	  break;
	  /*@noaccess mtContextNode@*/
	case MTP_VALUES:
	  /*@access mtValuesNode@*/
	  mtValuesNode_free ((mtValuesNode) node->node);
	  break;
	  /*@noaccess mtValuesNode@*/
	case MTP_DEFAULTS:
	  /*@access mtDefaultsNode@*/
	  mtDefaultsNode_free ((mtDefaultsNode) node->node);
	  break;
	  /*@noaccess mtDefaultsNode@*/
	case MTP_DEFAULTVALUE:
	  /*@access mttok@*/
	  mttok_free ((mttok) node->node);
	  break;
	  /*@noaccess mttok@*/
	case MTP_ANNOTATIONS:
	  /*@access mtAnnotationsNode@*/
	  mtAnnotationsNode_free ((mtAnnotationsNode) node->node);
	  break;
	  /*@noaccess mtAnnotationsNode@*/
	case MTP_MERGE:
	  /*@access mtMergeNode@*/
	  mtMergeNode_free ((mtMergeNode) node->node);
	  break;
	  /*@noaccess mtMergeNode@*/
	case MTP_TRANSFERS:
	case MTP_PRECONDITIONS:
	case MTP_POSTCONDITIONS:
	  /*@access mtTransferClauseList@*/
	  mtTransferClauseList_free ((mtTransferClauseList) node->node);
	  break;
	  /*@noaccess mtTransferClauseList@*/
	case MTP_LOSERS:
	  /*@access mtLoseReferenceList@*/
	  mtLoseReferenceList_free ((mtLoseReferenceList) node->node);
	  break;
	  /*@noaccess mtLoseReferenceList@*/
	}
      
      sfree (node);
    }
}
