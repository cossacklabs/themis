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
** mtDeclarationNode.c
*/

# include "splintMacros.nf"
# include "basic.h"

extern mtDeclarationNode mtDeclarationNode_create (mttok name, mtDeclarationPieces pieces) /*@*/ 
{
  mtDeclarationNode res = (mtDeclarationNode) dmalloc (sizeof (*res));

  res->name = mttok_getText (name);
  res->loc = mttok_stealLoc (name);
  res->pieces = pieces;

  mttok_free (name);
  return res;
}

extern cstring mtDeclarationNode_unparse (mtDeclarationNode node) /*@*/ 
{
  return message ("state %s %q",
		  node->name,
		  mtDeclarationPieces_unparse (node->pieces));
}

extern void mtDeclarationNode_process (mtDeclarationNode node, bool isglobal)
{
  int i;
  int j;

  mtDeclarationPieces pieces;
  mtDeclarationPiece mtp;
  mtContextNode mtcontext;
  stateCombinationTable tsc;
  stateCombinationTable tmerge;
  cstringList mvals;
  metaStateInfo msinfo;
  int nvalues;

  cstring defaultMergeMessage = 
    cstring_makeLiteralTemp ("Incompatible state merge (default behavior)");

  pieces = node->pieces;

  /*
  ** First, we need to find the values piece.
  */

  mtp = mtDeclarationPieces_findPiece (pieces, MTP_VALUES);

  if (mtDeclarationPiece_isUndefined (mtp)) 
    {
      voptgenerror (FLG_SYNTAX,
		    message ("Metastate declaration missing values clause: %s",
			     mtDeclarationNode_getName (node)),
		    mtDeclarationNode_getLoc (node));
      return;
    }
  else 
    {
      mtValuesNode  mtv = mtDeclarationPiece_getValues (mtp);
      mvals = mtValuesNode_getValues (mtv);
    }

  /*@-usedef@*/ /* splint should figure this out... */
  nvalues = cstringList_size (mvals);
  /*@=usedef@*/

  mtp = mtDeclarationPieces_findPiece (pieces, MTP_CONTEXT);

  if (mtDeclarationPiece_isUndefined (mtp)) 
    {
      ; /* No context, assume anywhere is okay. */
      mtcontext = mtContextNode_createAny ();
    }
  else 
    {
      mtcontext = mtDeclarationPiece_stealContext (mtp);
    }

  if (isglobal)
    {
      /*
      ** For global state, instead of a transfers piece, we have constraints.
      */

      mtp = mtDeclarationPieces_findPiece (pieces, MTP_TRANSFERS);

      if (!mtDeclarationPiece_isUndefined (mtp)) 
	{
	  voptgenerror (FLG_SYNTAX,
			message ("Global state declaration uses transfers clause.  Should use preconditions and postconsitions clauses instead: %s",
				 mtDeclarationNode_getName (node)),
			mtDeclarationNode_getLoc (node));
	  mtContextNode_free (mtcontext);
	  return;
	} 

      mtp = mtDeclarationPieces_findPiece (pieces, MTP_PRECONDITIONS);
      
      if (mtDeclarationPiece_isUndefined (mtp)) 
	{
	  voptgenerror (FLG_SYNTAX,
			message ("Metastate declaration missing preconditions clause: %s",
				 mtDeclarationNode_getName (node)),
			mtDeclarationNode_getLoc (node));
	  mtContextNode_free (mtcontext);
	  return;
	} 
      else 
	{
	  mtTransferClauseList mtransfers = mtDeclarationPiece_getPreconditions (mtp);
	  tsc = stateCombinationTable_create (nvalues);
	  
	  mtTransferClauseList_elements (mtransfers, transfer)
	    {
	      cstring tfrom = mtTransferClause_getFrom (transfer);
	      cstring tto = mtTransferClause_getTo (transfer);
	      mtTransferAction taction = mtTransferClause_getAction (transfer);
	      cstring vname = mtTransferAction_getValue (taction);
	      
	      int fromindex;
	      int toindex;
	      int vindex;
	      
	      DPRINTF (("Transfer: %s", mtTransferClause_unparse (transfer)));
	      
	      if (cstringList_contains (mvals, tfrom)) 
		{
		  fromindex = cstringList_getIndex (mvals, tfrom);
		}
	      else
		{
		  voptgenerror
		    (FLG_SYNTAX,
		     message ("Precondition clause uses unrecognized caller value %s: %q",
			      tfrom, mtTransferClause_unparse (transfer)),
		     mtTransferClause_getLoc (transfer));
		  continue;
		}
	      
	      if (cstringList_contains (mvals, tto)) 
		{
		  toindex = cstringList_getIndex (mvals, tto);
		}
	      else
		{
		  voptgenerror
		    (FLG_SYNTAX,
		     message ("Precondition clause uses unrecognized constraint value %s: %q",
			      tto, mtTransferClause_unparse (transfer)),
		     mtTransferClause_getLoc (transfer));
		  continue;
		}
	      
	      if (mtTransferAction_isError (taction))
		{
		  vindex = metaState_error;
		}
	      else 
		{
		  if (cstringList_contains (mvals, vname)) 
		    {
		      vindex = cstringList_getIndex (mvals, vname);
		    }
		  else
		    {
		      voptgenerror
			(FLG_SYNTAX,
			 message ("Precondition clause uses unrecognized result state %s: %q",
				  vname, mtTransferClause_unparse (transfer)),
			 mtTransferClause_getLoc (transfer));
		      continue;
		    }
		}
	      
	      if (mtTransferAction_isError (taction))
		{
		  stateCombinationTable_set 
		    (tsc, fromindex, toindex,
		     vindex,
		     cstring_copy (mtTransferAction_getMessage (taction)));
		}
	      else
		{
		  stateCombinationTable_set (tsc, fromindex, toindex,
					     vindex,
					     cstring_undefined);
		}
	    } end_mtTransferClauseList_elements ;
	}
    }
  else
    {
      mtp = mtDeclarationPieces_findPiece (pieces, MTP_PRECONDITIONS);

      if (!mtDeclarationPiece_isUndefined (mtp)) 
	{
	  voptgenerror 
	    (FLG_SYNTAX,
	     message ("Non-global state declaration uses preconditions clause. "
		      "Should use transfers clause instead: %s",
		      mtDeclarationNode_getName (node)),
	     mtDeclarationNode_getLoc (node));
	  mtContextNode_free (mtcontext);
	  return;
	} 

      mtp = mtDeclarationPieces_findPiece (pieces, MTP_POSTCONDITIONS);

      if (!mtDeclarationPiece_isUndefined (mtp)) 
	{
	  voptgenerror 
	    (FLG_SYNTAX,
	     message ("Non-global state declaration uses postconditions clause. "
		      "Should use transfers clause instead: %s",
		      mtDeclarationNode_getName (node)),
	     mtDeclarationNode_getLoc (node));
	  mtContextNode_free (mtcontext);
	  return;
	} 

      mtp = mtDeclarationPieces_findPiece (pieces, MTP_TRANSFERS);
      
      if (mtDeclarationPiece_isUndefined (mtp)) 
	{
	  voptgenerror (FLG_SYNTAX,
			message ("Metastate declaration missing transfers clause: %s",
				 mtDeclarationNode_getName (node)),
			mtDeclarationNode_getLoc (node));
	  mtContextNode_free (mtcontext);
	  return;
	} 
      else 
	{
	  mtTransferClauseList mtransfers = mtDeclarationPiece_getTransfers (mtp);
	  tsc = stateCombinationTable_create (nvalues);
	  
	  mtTransferClauseList_elements (mtransfers, transfer)
	    {
	      cstring tfrom = mtTransferClause_getFrom (transfer);
	      cstring tto = mtTransferClause_getTo (transfer);
	      mtTransferAction taction = mtTransferClause_getAction (transfer);
	      cstring vname = mtTransferAction_getValue (taction);
	      
	      int fromindex;
	      int toindex;
	      int vindex;
	      
	      DPRINTF (("Transfer: %s", mtTransferClause_unparse (transfer)));
	      
	      if (cstringList_contains (mvals, tfrom)) 
		{
		  fromindex = cstringList_getIndex (mvals, tfrom);
		}
	      else
		{
		  voptgenerror
		    (FLG_SYNTAX,
		     message ("Transfer clause uses unrecognized from value %s: %q",
			      tfrom, mtTransferClause_unparse (transfer)),
		     mtTransferClause_getLoc (transfer));
		  continue;
		}
	      
	      if (cstringList_contains (mvals, tto)) 
		{
		  toindex = cstringList_getIndex (mvals, tto);
		}
	      else
		{
		  voptgenerror
		    (FLG_SYNTAX,
		     message ("Transfer clause uses unrecognized to value %s: %q",
			      tto, mtTransferClause_unparse (transfer)),
		     mtTransferClause_getLoc (transfer));
		  continue;
		}
	      
	      if (mtTransferAction_isError (taction))
		{
		  vindex = metaState_error;
		}
	      else 
		{
		  if (cstringList_contains (mvals, vname)) 
		    {
		      vindex = cstringList_getIndex (mvals, vname);
		    }
		  else
		    {
		      voptgenerror
			(FLG_SYNTAX,
			 message ("Transfer clause uses unrecognized result state %s: %q",
				  vname, mtTransferClause_unparse (transfer)),
			 mtTransferClause_getLoc (transfer));
		      continue;
		    }
		}
	      
	      if (mtTransferAction_isError (taction))
		{
		  stateCombinationTable_set 
		    (tsc, fromindex, toindex,
		     vindex,
		     cstring_copy (mtTransferAction_getMessage (taction)));
		}
	      else
		{
		  stateCombinationTable_set (tsc, fromindex, toindex,
					     vindex,
					     cstring_undefined);
		}
	    } end_mtTransferClauseList_elements ;
	}
    }

  mtp = mtDeclarationPieces_findPiece (pieces, MTP_LOSERS);

  if (mtDeclarationPiece_isDefined (mtp))
    {
      mtLoseReferenceList mlosers = mtDeclarationPiece_getLosers (mtp);
      
      mtLoseReferenceList_elements (mlosers, loseref)
	{
	  cstring tfrom = mtLoseReference_getFrom (loseref);
	  mtTransferAction taction = mtLoseReference_getAction (loseref);
	  int fromindex;
	  /* Losing reference is represented by transfer to nvalues */
	  int toindex = nvalues; 
	  int vindex = metaState_error;

	  llassert (mtTransferAction_isError (taction));

	  if (cstringList_contains (mvals, tfrom)) 
	    {
	      fromindex = cstringList_getIndex (mvals, tfrom);
	    }
	  else
	    {
	      voptgenerror
		(FLG_SYNTAX,
		 message ("Lose reference uses unrecognized from value %s: %q",
			  tfrom, mtLoseReference_unparse (loseref)),
		 mtLoseReference_getLoc (loseref));
	      continue;
	    }

	  /*@-usedef@*/
	  stateCombinationTable_set 
	    (tsc, fromindex, toindex, vindex,
	     cstring_copy (mtTransferAction_getMessage (taction)));
	  /*@=usedef@*/
	} end_mtLoseReferenceList_elements ;
    }
  
  tmerge = stateCombinationTable_create (nvalues);

  /* Default merge is to make all incompatible mergers errors. */
  
  for (i = 0; i < nvalues; i++) 
    {
      for (j = 0; j < nvalues; j++)
	{
	  if (i != j) 
	    {
	      stateCombinationTable_set 
		(tmerge, i, j, metaState_error, cstring_copy (defaultMergeMessage));
	    }
	}
    }

  mtp = mtDeclarationPieces_findPiece (pieces, MTP_MERGE);
  
  if (mtDeclarationPiece_isDefined (mtp))
    {
      mtMergeNode mtmerge = mtDeclarationPiece_getMerge (mtp);
      mtMergeClauseList mclauses = mtMergeNode_getClauses (mtmerge);

      DPRINTF (("Merge node: %s", mtMergeNode_unparse (mtmerge)));

      mtMergeClauseList_elements (mclauses, merge)
	{
	  mtMergeItem item1 = mtMergeClause_getItem1 (merge);
	  mtMergeItem item2 = mtMergeClause_getItem2 (merge);
	  mtTransferAction taction = mtMergeClause_getAction (merge);
	  int low1index, high1index;
	  int low2index, high2index;
	  int vindex;

	  DPRINTF (("Merge %s X %s => %s",
		    mtMergeItem_unparse (item1),
		    mtMergeItem_unparse (item2),
		    mtTransferAction_unparse (taction)));
	  
	  if (!mtMergeItem_isStar (item1))
	    {
	      if (cstringList_contains (mvals, mtMergeItem_getValue (item1)))
		{
		  low1index = cstringList_getIndex (mvals, mtMergeItem_getValue (item1));
		  high1index = low1index;
		}
	      else
		{
		  voptgenerror
		    (FLG_SYNTAX,
		     message ("Merge clause uses unrecognized first value %s: %q",
			      mtMergeItem_getValue (item1), 
			      mtMergeClause_unparse (merge)),
		     mtMergeClause_getLoc (merge));
		  continue;
		}
	    }
	  else
	    {
	      low1index = 0;
	      high1index = nvalues - 1;
	    }

	  if (!mtMergeItem_isStar (item2))
	    {
	      if (cstringList_contains (mvals, mtMergeItem_getValue (item2))) 
		{
		  low2index = cstringList_getIndex (mvals, mtMergeItem_getValue (item2));
		  high2index = low2index;
		}
	      else
		{
		  voptgenerror
		    (FLG_SYNTAX,
		     message ("Merge clause uses unrecognized second value %s: %q",
			      mtMergeItem_getValue (item2), 
			      mtMergeClause_unparse (merge)),
		     mtMergeItem_getLoc (item2));
		  continue;
		}
	    }
	  else
	    {
	      low2index = 0;
	      high2index = nvalues - 1;
	    }
	  
	  if (mtTransferAction_isError (taction))
	    {
	      vindex = metaState_error;
	    }
	  else 
	    {
	      cstring vname = mtTransferAction_getValue (taction);

	      if (cstringList_contains (mvals, vname)) 
		{
		  vindex = cstringList_getIndex (mvals, vname);
		}
	      else
		{
		  voptgenerror
		    (FLG_SYNTAX,
		     message ("Merge clause uses unrecognized result state %s: %q",
			      vname, mtMergeClause_unparse (merge)),
		     mtTransferAction_getLoc (taction));
		  continue;
		}
	    }

	  for (i = low1index; i <= high1index; i++)
	    {
	      for (j = low2index; j <= high2index; j++)
		{
		  /* Need to add checks for multiple definitions! */
		  
		  if (mtTransferAction_isError (taction))
		    {
		      stateCombinationTable_update
			(tmerge, 
			 i, j, 
			 vindex,
			 cstring_copy (mtTransferAction_getMessage (taction)));
		    }
		  else
		    {
		      stateCombinationTable_update
			(tmerge, 
			 i, j, 
			 vindex,
			 cstring_undefined);
		    }
		}
	    }

	  /*
	  ** Unless otherwise indicated, merging is symmetric:
	  */

	  for (i = low1index; i <= high1index; i++)
	    {
	      for (j = low2index; j <= high2index; j++)
		{
		  cstring msg;

		  if (stateCombinationTable_lookup (tmerge, j, i, &msg) == metaState_error)
		    {
		      if (cstring_equal (msg, defaultMergeMessage))
			{
			  /* Override the default action */
			  if (mtTransferAction_isError (taction))
			    {
			      stateCombinationTable_update
				(tmerge, 
				 j, i,
				 vindex,
				 cstring_copy (mtTransferAction_getMessage (taction)));
			    }
			  else
			    {
			      stateCombinationTable_update
				(tmerge, 
				 j, i, 
				 vindex,
				 cstring_undefined);
			    }
			}
		    }
		}
	    }
	} end_mtMergeClauseList_elements ;  
    }

  msinfo = metaStateInfo_create (cstring_copy (mtDeclarationNode_getName (node)),
				 cstringList_copy (mvals),
				 mtcontext,
				 /*@-usedef@*/ tsc, /*@=usedef@*/
				 tmerge,
				 fileloc_copy (mtDeclarationNode_getLoc (node)));

  mtp = mtDeclarationPieces_findPiece (pieces, MTP_ANNOTATIONS);

  if (mtDeclarationPiece_isDefined (mtp))
    {
      mtAnnotationsNode mtannots = mtDeclarationPiece_getAnnotations (mtp);
      mtAnnotationList mtalist = mtAnnotationsNode_getAnnotations (mtannots);

      DPRINTF (("Has annotations: %s", mtAnnotationList_unparse (mtalist)));      

      mtAnnotationList_elements (mtalist, annot) 
	{
	  cstring aname = mtAnnotationDecl_getName (annot);
	  cstring avalue = mtAnnotationDecl_getValue (annot);

	  DPRINTF (("Process annotation: %s", mtAnnotationDecl_unparse (annot)));
	  
	  if (cstringList_contains (mvals, avalue)) 
	    {
	      int vindex = cstringList_getIndex (mvals, avalue);
	      mtContextNode acontext = mtAnnotationDecl_stealContext (annot);
	      
	      context_addAnnotation 
		(annotationInfo_create (cstring_copy (aname), msinfo, 
					acontext, vindex,
					fileloc_copy (mtAnnotationDecl_getLoc (annot))));
	    }
	  else
	    {
	      voptgenerror
		(FLG_SYNTAX,
		 message ("Annotation declaration uses unrecognized value name %s: %q",
			  avalue, mtAnnotationDecl_unparse (annot)),
		 mtAnnotationDecl_getLoc (annot));
	    }

	} end_mtAnnotationList_elements ;
    }

  mtp = mtDeclarationPieces_findPiece (pieces, MTP_DEFAULTS);

  if (mtDeclarationPiece_isDefined (mtp))
    {
      mtDefaultsNode mdn = mtDeclarationPiece_getDefaults (mtp);
      mtDefaultsDeclList mdecls = mtDefaultsNode_getDecls (mdn);
      
      llassert (!isglobal);

      mtDefaultsDeclList_elements (mdecls, mdecl)
	{
	  mtContextNode mcontext = mtDefaultsDecl_getContext (mdecl);
	  cstring mvalue = mtDefaultsDecl_getValue (mdecl);

	  if (cstringList_contains (mvals, mvalue)) 
	    {
	      int vindex = cstringList_getIndex (mvals, mvalue);
	      mtContextKind mkind;

	      if (mtContextNode_isReference (mcontext))
		{
		  mkind = MTC_REFERENCE;
		}
	      else if (mtContextNode_isParameter (mcontext))
		{
		  mkind = MTC_PARAM;
		}
	      else if (mtContextNode_isResult (mcontext))
		{
		  mkind = MTC_RESULT;
		}
	      else if (mtContextNode_isLiteral (mcontext))
		{
		  mkind = MTC_LITERAL;
		}
	      else if (mtContextNode_isNull (mcontext))
		{
		  mkind = MTC_NULL;
		}
	      else
		{
		  DPRINTF (("Bad: %s", mtContextNode_unparse (mcontext)));
		  BADBRANCH;
		}

	      if (metaStateInfo_getDefaultValueContext (msinfo, mkind) != stateValue_error)
		{
		  voptgenerror
		    (FLG_SYNTAX,
		     message ("Duplicate defaults declaration for context %q: %q",
			      mtContextNode_unparse (mcontext), 
			      mtDefaultsDecl_unparse (mdecl)),
		     mtDefaultsDecl_getLoc (mdecl));
		}
	      else
		{
		  metaStateInfo_setDefaultValueContext (msinfo, mkind, vindex);
		}
	    }
	  else
	    {
	      voptgenerror
		(FLG_SYNTAX,
		 message ("Defaults declaration uses unrecognized value name %s: %q",
			  mvalue, mtDefaultsDecl_unparse (mdecl)),
		 mtDefaultsDecl_getLoc (mdecl));
	    }
	} end_mtDefaultsDeclList_elements ;
    }

  mtp = mtDeclarationPieces_findPiece (pieces, MTP_DEFAULTVALUE);

  if (mtDeclarationPiece_isDefined (mtp))
    {
      cstring mvalue = mtDeclarationPiece_getDefaultValue (mtp);
      llassert (isglobal);
      
      if (cstringList_contains (mvals, mvalue)) 
	{
	  int vindex = cstringList_getIndex (mvals, mvalue);
	  
	  if (metaStateInfo_getDefaultRefValue (msinfo) != stateValue_error)
	    {
	      voptgenerror
		(FLG_SYNTAX,
		 message ("Duplicate default value declaration for global state: %s",
			  mvalue),
		 mtDeclarationNode_getLoc (node));
	    }
	  else
	    {
	      metaStateInfo_setDefaultRefValue (msinfo, vindex);
	    }
	}
      else
	{
	  voptgenerror
	    (FLG_SYNTAX,
	     message ("Default value declaration uses unrecognized value name: %s",
		      mvalue),
	     mtDeclarationNode_getLoc (node));
	}
    }
  
  context_addMetaState (cstring_copy (mtDeclarationNode_getName (node)),
			msinfo);
}

extern void mtDeclarationNode_free (/*@only@*/ mtDeclarationNode node)
{
  mtDeclarationPieces_free (node->pieces);
  cstring_free (node->name);
  fileloc_free (node->loc);
  sfree (node);
}

extern fileloc mtDeclarationNode_getLoc (mtDeclarationNode node) 
{
  return node->loc;
}

extern /*@observer@*/ cstring mtDeclarationNode_getName (mtDeclarationNode node)
{
  return node->name;
}
