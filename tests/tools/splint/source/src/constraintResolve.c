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
*
** constraintResolve.c
*/

/* #define DEBUGPRINT 1 */

# include <ctype.h> /* for isdigit */
# include "splintMacros.nf"
# include "basic.h"
# include "cgrammar.h"
# include "cgrammar_tokens.h"

# include "exprChecks.h"
# include "exprNodeSList.h"

/*@access constraint, exprNode @*/ /*!!! NO! Don't do this so recklessly - design your code more carefully so you don't need to! */

static constraint inequalitySubstitute (/*@returned@*/ constraint p_c, constraintList p_p);
static bool rangeCheck (arithType p_ar1, /*@observer@*/ constraintExpr p_expr1, arithType p_ar2, /*@observer@*/ constraintExpr p_expr2);

static constraint inequalitySubstituteUnsound (/*@returned@*/ constraint p_c, constraintList p_p);
static constraint inequalitySubstituteStrong (/*@returned@*/ constraint p_c, constraintList p_p);

static constraint constraint_searchandreplace (/*@returned@*/ constraint p_c, constraintExpr p_old, constraintExpr p_newExpr);

static constraint constraint_addOr (/*@returned@*/ constraint p_orig, /*@observer@*/ constraint p_orConstr);

static bool resolveOr (/*@temp@*/constraint p_c, /*@observer@*/ /*@temp@*/ constraintList p_list);

static /*@only@*/ constraintList reflectChangesEnsuresFree1 (/*@only@*/ constraintList p_pre2, constraintList p_post1);


/*@only@*/ constraintList constraintList_mergeEnsuresFreeFirst (constraintList list1, constraintList list2)
{
  constraintList ret;

  ret = constraintList_mergeEnsures (list1, list2);

  constraintList_free(list1);
  return ret;
}
					    
/*@only@*/ constraintList constraintList_mergeEnsures (constraintList list1, constraintList list2)
{
  constraintList ret;
  constraintList temp;

  llassert(constraintList_isDefined(list1) );
  llassert(constraintList_isDefined(list2) );

  DPRINTF(( message ("constraintList_mergeEnsures: list1 %s list2 %s",
		     constraintList_unparse(list1), constraintList_unparse(list2)
		     )));
  
  ret = constraintList_fixConflicts (list1, list2);
  ret = reflectChangesEnsuresFree1 (ret, list2);
  temp = constraintList_subsumeEnsures (ret, list2);
  constraintList_free(ret);
  ret = temp;

  temp = constraintList_subsumeEnsures (list2, ret);

  temp = constraintList_addList (temp, ret);
  constraintList_free(ret);
  
  DPRINTF(( message ("constraintList_mergeEnsures: returning %s ",
		     constraintList_unparse(temp) )
		     ));
  

  return temp;
}


/*@only@*/ constraintList constraintList_mergeRequiresFreeFirst (/*@only@*/ constraintList list1, constraintList list2)
{
  constraintList ret;

  ret = constraintList_mergeRequires(list1, list2);

  constraintList_free(list1);

  return ret;
}

/*@only@*/ constraintList constraintList_mergeRequires (constraintList list1, constraintList list2)
{
  constraintList ret;
  constraintList temp;

  DPRINTF((message ("constraintList_mergeRequires: merging  %s and %s ", constraintList_unparse (list1), constraintList_unparse(list2) ) ) );

  if (context_getFlag (FLG_REDUNDANTCONSTRAINTS) )
    {
      ret = constraintList_copy(list1);
      ret = constraintList_addList(ret, list2); 
      return ret;
    }
    
  /* get constraints in list1 not satified by list2 */
  temp = constraintList_reflectChanges(list1, list2);
  DPRINTF((message ("constraintList_mergeRequires: temp = %s", constraintList_unparse(temp) ) ) );

/*get constraints in list2 not satified by temp*/
  ret = constraintList_reflectChanges(list2, temp);
 
  DPRINTF((message ("constraintList_mergeRequires: ret =  %s", constraintList_unparse(ret) ) ) );
  
  ret = constraintList_addListFree (ret, temp);
  
  DPRINTF((message ("constraintList_mergeRequires: returning  %s", constraintList_unparse(ret) ) ) );

  return ret;
}

/* old name mergeResolve renamed for czech naming convention */
void exprNode_mergeResolve (exprNode parent, exprNode child1, exprNode child2)
{
  constraintList temp, temp2;

  DPRINTF((message ("magically merging constraint into parent:%s for", exprNode_unparse (parent) )) );

  DPRINTF((message (" children:  %s and %s", exprNode_unparse (child1), exprNode_unparse(child2) ) ) );

  
  if (exprNode_isUndefined(parent) )
    {
      llassert (exprNode_isDefined(parent) );
      return;
    }
  
  
  if (exprNode_isError (child1)  || exprNode_isError(child2) )
    {
      if (exprNode_isError (child1) && !exprNode_isError(child2) )
	 {
	   constraintList_free(parent->requiresConstraints);

	   parent->requiresConstraints = constraintList_copy (child2->requiresConstraints);
	   constraintList_free(parent->ensuresConstraints);

	   parent->ensuresConstraints = constraintList_copy (child2->ensuresConstraints);
	   DPRINTF((message ("Copied child constraints: pre: %s and post: %s",
			     constraintList_unparse( child2->requiresConstraints),
			     constraintList_unparse (child2->ensuresConstraints)
			     )
		    ));
	   return;
	 }
       else
	 {
	   llassert(exprNode_isError(child2) );
	   return;
	 }
     }

   llassert(!exprNode_isError (child1)  && ! exprNode_isError(child2) );
   
   DPRINTF((message ("Child constraints are %s %s and %s %s",
		     constraintList_unparse (child1->requiresConstraints),
		     constraintList_unparse (child1->ensuresConstraints),
		     constraintList_unparse (child2->requiresConstraints),
		     constraintList_unparse (child2->ensuresConstraints)
		     ) ) );
 
 
   constraintList_free(parent->requiresConstraints);

  parent->requiresConstraints = constraintList_copy (child1->requiresConstraints);

  if ( context_getFlag (FLG_ORCONSTRAINT) )
    temp = constraintList_reflectChangesOr (child2->requiresConstraints, child1->ensuresConstraints);
  else
    temp = constraintList_reflectChanges(child2->requiresConstraints, child1->ensuresConstraints);

  temp2 = constraintList_mergeRequires (parent->requiresConstraints, temp);
  constraintList_free(parent->requiresConstraints);
  constraintList_free(temp);
  
  parent->requiresConstraints = temp2;

  DPRINTF((message ("Parent requires constraints are %s  ",
		     constraintList_unparse (parent->requiresConstraints)
		     ) ) );

   constraintList_free(parent->ensuresConstraints);

  parent->ensuresConstraints = constraintList_mergeEnsures(child1->ensuresConstraints,
							   child2->ensuresConstraints);

  
  DPRINTF((message ("Parent constraints are %s and %s ",
		     constraintList_unparse (parent->requiresConstraints),
		     constraintList_unparse (parent->ensuresConstraints)
		     ) ) );
 
}
  
/*@only@*/ constraintList constraintList_subsumeEnsures (constraintList list1, constraintList list2)
{
  constraintList ret;
  ret = constraintList_makeNew();
  constraintList_elements (list1, el)
    {
      
      DPRINTF ((message ("Examining %s", constraint_unparse (el) ) ) );
      if (!constraintList_resolve (el, list2) )
	{
	  constraint temp;
	  temp = constraint_copy(el);
	  ret = constraintList_add (ret, temp);
	}
      else
	{
	  DPRINTF ((message ("Subsuming %s", constraint_unparse (el) ) ) );
	}
    } end_constraintList_elements;

    return ret;
}



/*used to be reflectChangesFreePre  renamed for Czech naming conventino*/
/* tries to resolve constraints in list pre2 using post1 */
/*@only@*/ constraintList constraintList_reflectChangesFreePre (/*@only@*/ constraintList pre2, /*@observer@*/ constraintList post1)
{
  constraintList ret;
  
  ret = constraintList_reflectChanges(pre2, post1);

  constraintList_free (pre2);
  
  return ret;
}

/* tries to resolve constraints in list pre2 using post1 */

static /*@only@*/ constraintList reflectChangesNoOr (/*@observer@*/ /*@temp@*/ constraintList pre2, /*@observer@*/ /*@temp@*/ constraintList post1)
{
  
  constraintList ret;
  constraint temp;
  constraint temp2;

  llassert  (! context_getFlag (FLG_ORCONSTRAINT) );

  ret = constraintList_makeNew();
  DPRINTF((message ("reflectChanges: lists %s and %s", constraintList_unparse(pre2), constraintList_unparse(post1) )));
  
  constraintList_elements (pre2, el)
    {
      if (!constraintList_resolve (el, post1) )
	{
	  temp = constraint_substitute (el, post1);
	  if (!constraintList_resolve (temp, post1) )
	    {
	      /* try inequality substitution
		 the inequality substitution may cause us to lose information
		 so we don't want to store the result but we do it anyway
	      */
	      temp2 = constraint_copy (temp);
	      temp2 = inequalitySubstitute (temp2, post1); 
	      if (!constraintList_resolve (temp2, post1) )
		{
		  temp2 = inequalitySubstituteUnsound (temp2, post1); 
		  if (!constraintList_resolve (temp2, post1) )
		    ret = constraintList_add (ret, temp2);
		  else
		    constraint_free(temp2);
		}
	      else
		{
		  constraint_free(temp2);
		}
	    }
	  constraint_free(temp);
	}
    } end_constraintList_elements;

    DPRINTF((message ("reflectChanges: returning %s", constraintList_unparse(ret) ) ) );
    return ret;
}

/* tries to resolve constraints in list pre2 using post1 */
/*@only@*/ constraintList constraintList_reflectChanges(/*@observer@*/ constraintList pre2, /*@observer@*/ constraintList post1)
{
  constraintList temp;
  
  if ( context_getFlag (FLG_ORCONSTRAINT) )
    
    temp = constraintList_reflectChangesOr (pre2, post1);
  else
    temp = reflectChangesNoOr(pre2, post1);

  return temp;				 
}

static constraint constraint_addOr (/*@returned@*/ constraint orig, /*@observer@*/ constraint orConstr)
{
  constraint c;

  llassert(constraint_isDefined(orig) );
  
  c = orig;

  DPRINTF((message("constraint_addor: oring %s onto %s", constraint_unparseOr(orConstr), constraint_unparseOr(orig) ) ));
  
  while (c->or != NULL)
    {
      c = c->or;
    }
  
  c->or = constraint_copy(orConstr);

  DPRINTF((message("constraint_addor: returning %s",constraint_unparseOr(orig) ) ));
  
  return orig;
}


static bool resolveOr ( /*@temp@*/ constraint c, /*@observer@*/ /*@temp@*/ constraintList list)
{
  constraint temp;

  int numberOr;

  numberOr = 0;

    llassert(constraint_isDefined(c) );

  DPRINTF(( message("resolveOr: constraint %s and list %s", constraint_unparseOr(c), constraintList_unparse(list) ) ));
  
  temp = c;

  do
    {
      if (constraintList_resolve (temp, list) )
	return TRUE;
      temp = temp->or;
      numberOr++;
      llassert(numberOr <= 10);
    }
  while (constraint_isDefined(temp));

  return FALSE;
}

/*This is a "helper" function for doResolveOr */

static /*@only@*/ constraint doResolve (/*@only@*/ constraint c, constraintList post1, bool * resolved)
{
  constraint temp;

  llassert(constraint_isDefined (c ) );

  DPRINTF((message("doResolve:: call on constraint c = : %q and constraintList %q",
		   constraint_unparseOr(c), constraintList_unparse(post1)
		   )
	   ));
  
  if (!resolveOr (c, post1) )
    {
      
      temp = constraint_substitute (c, post1);
      
      DPRINTF((message("doResolve:: after substitute temp is %q",
		   constraint_unparseOr(temp)
		       )
	       ));
  
      if (!resolveOr (temp, post1) )
	{
	  /* try inequality substitution */
	  constraint temp2;
	  
	  /* the inequality substitution may cause us to lose information
	     so we don't want to store the result but we do  anyway
	  */
	  temp2 = constraint_copy (c);
	  temp2 = inequalitySubstitute (temp2, post1);

	  if (!resolveOr (temp2, post1) )
	    {
	      constraint temp3;
	      temp3 = constraint_copy(temp2);
	      
	      temp3 = inequalitySubstituteStrong (temp3, post1);
	      if (!resolveOr (temp3, post1) )
		{
		  temp2 = inequalitySubstituteUnsound (temp2, post1); 
		  if (!resolveOr (temp2, post1) )
		    {
		      if (!constraint_same (temp, temp2) )
			{
			  /* drl added 8/28/2002*/
			  /*make sure that the information from
			    a post condition like i = i + 1 is transfered
			  */
			  constraint tempSub;
			  tempSub = constraint_substitute (temp2, post1);

			  DPRINTF((
				   message("doResolve: adding %s ",
					   constraint_unparseOr(tempSub)
					   )
				   ));
			  
			  DPRINTF((
				   message("doResolve: not adding %s ",
					   constraint_unparseOr(temp2)
					   )
				   ));
			  
			  temp = constraint_addOr (temp, tempSub);
			  constraint_free(tempSub);
			  
			}
		      if (!constraint_same (temp, temp3) && !constraint_same (temp3, temp2) )
			{
			 /* drl added 8/28/2002*/
			  /*make sure that the information from
			    a post condition like i = i + 1 is transfered
			  */
			  constraint tempSub;
			  
			  tempSub = constraint_substitute (temp3, post1);

			  DPRINTF((
				   message("doResolve: adding %s ",
					   constraint_unparseOr(tempSub)
					   )
				   ));

			  
			  DPRINTF((
				   message("doResolve: not adding %s ",
					   constraint_unparseOr(temp3)
					   )
				   ));

			  temp = constraint_addOr (temp, tempSub);

			  constraint_free(tempSub);
			}
		      *resolved = FALSE;
		      
		      constraint_free(temp2);
		      constraint_free(temp3);
		      constraint_free(c);
		      
		      return temp;
		    }
		  constraint_free(temp2);
		  constraint_free(temp3);
		}
	      else
		{
		  constraint_free(temp2);
		  constraint_free(temp3);
		}
	    }
	  else
	    {
	      constraint_free(temp2);
	    }		  
	  
	}
      constraint_free(temp);
    }
  constraint_free(c);
  
  *resolved = TRUE;
  return NULL;
}

static /*@only@*/ constraint doResolveOr (/*@observer@*/ /*@temp@*/ constraint c, constraintList post1, /*@out@*/bool * resolved)
{
  constraint ret;
  constraint next;
  constraint curr;
  
  DPRINTF(( message("doResolveOr: constraint %s and list %s", constraint_unparseOr(c), constraintList_unparse(post1) ) ));

  *resolved = FALSE;
  
  llassert(constraint_isDefined(c) );

  ret = constraint_copy(c);

  llassert(constraint_isDefined(ret) );

  if (constraintList_isEmpty(post1) )
    {
      return ret;
    }
  
  next = ret->or;
  ret->or = NULL;

  ret = doResolve (ret, post1, resolved);

  if (*resolved)
    {
      if (next != NULL)
	constraint_free(next);
      
      /*we don't need to free ret when resolved is false because ret is null*/
      llassert(ret == NULL);
      
      return NULL;
    }
  
  while (next != NULL)
    {
      curr = next;
      next = curr->or;
      curr->or = NULL;

      curr = doResolve (curr, post1, resolved);
      
       if (*resolved)
	{
	  /* curr is null so we don't try to free it*/
	  llassert(curr == NULL);
	  
	  if (next != NULL)
	    constraint_free(next);

	  constraint_free(ret);
	  return NULL;
	}
      ret = constraint_addOr (ret, curr);
      constraint_free(curr);
    }

  DPRINTF(( message("doResolveOr: returning ret = %s", constraint_unparseOr(ret) ) ));
  
  return ret;
}

/* tries to resolve constraints in list pr2 using post1 */
/*@only@*/ constraintList constraintList_reflectChangesOr (constraintList pre2, constraintList post1)
{
  bool resolved;
  constraintList ret;
  constraint temp;
  ret = constraintList_makeNew();
  DPRINTF((message ("constraintList_reflectChangesOr: lists %s and %s", constraintList_unparse(pre2), constraintList_unparse(post1) )));
  
  constraintList_elements (pre2, el)
    {
      temp = doResolveOr (el, post1, &resolved);

      if (!resolved)
	{
	  ret = constraintList_add(ret, temp);
	}
      else
	{
     /* we don't need to free temp when
	resolved is false because temp is null */
	  llassert(temp == NULL);
	}
      
    } end_constraintList_elements;

  DPRINTF((message ("constraintList_reflectChangesOr: returning %s", constraintList_unparse(ret) ) ) );
    return ret;
}

static /*@only@*/ constraintList reflectChangesEnsures (/*@observer@*/ constraintList pre2, constraintList post1)
{  
  constraintList ret;
  constraint temp;
  ret = constraintList_makeNew();
  constraintList_elements (pre2, el)
    {
      if (!constraintList_resolve (el, post1) )
	{
	  temp = constraint_substitute (el, post1);
	  llassert (temp != NULL);

	  if (!constraintList_resolve (temp, post1) )
	    ret = constraintList_add (ret, temp);
	  else
	    constraint_free(temp);  
	}
      else
	{
	  DPRINTF ((message ("Resolved away %s ", constraint_unparse(el) ) ) );
	}
    } end_constraintList_elements;

    return ret;
}


static /*@only@*/ constraintList reflectChangesEnsuresFree1 (/*@only@*/ constraintList pre2, constraintList post1)
{
  constraintList ret;

  ret = reflectChangesEnsures (pre2, post1);
  
  constraintList_free(pre2);

  return ret;
}


static bool constraint_conflict (constraint c1, constraint c2)
{
  if (!constraint_isDefined(c1) || !constraint_isDefined(c2))
    {
      return FALSE;
    }

  if (constraintExpr_similar (c1->lexpr, c2->lexpr))
    {
      if (c1->ar == EQ)
	if (c1->ar == c2->ar)
	  {
	    DPRINTF (("%s conflicts with %s", constraint_unparse (c1), constraint_unparse (c2)));
	    return TRUE;
	  }
    }  

  /* This is a slight kludge to prevent circular constraints like
     strlen(str) == maxRead(s) + strlen(str);
  */

  /*this code is functional but it may be worth cleaning up at some point. */
  
  if (c1->ar == EQ)
    if (c1->ar == c2->ar)
      {
	if (constraintExpr_search (c1->lexpr, c2->expr) )
	  if (constraintExpr_isTerm(c1->lexpr) )
	    {
	      constraintTerm term;
	      
	      term = constraintExpr_getTerm(c1->lexpr);

	      if (constraintTerm_isExprNode(term) )
		{
		  DPRINTF ((message ("%s conflicts with %s ", constraint_unparse (c1), constraint_unparse(c2) ) ) );
		  return TRUE;
		}
	    }
      }

  if (constraint_tooDeep(c1) || constraint_tooDeep(c2) )
    	{
	  DPRINTF ((message ("%s conflicts with %s (constraint is too deep", constraint_unparse (c1), constraint_unparse(c2) ) ) );
	  return TRUE;
	}
  
  DPRINTF ((message ("%s doesn't conflict with %s ", constraint_unparse (c1), constraint_unparse(c2) ) ) );

  return FALSE; 

}

static void constraint_fixConflict (/*@temp@*/ constraint good, /*@temp@*/ /*@observer@*/ constraint conflicting) /*@modifies good@*/
{
  llassert(constraint_isDefined(conflicting) );
  
  if (conflicting->ar == EQ)
    {
      llassert (constraint_isDefined(good));
      DPRINTF (("Replacing here!"));
      good->expr = constraintExpr_searchandreplace (good->expr, conflicting->lexpr, conflicting->expr);
      good = constraint_simplify (good);
    }


}

static bool conflict (constraint c, constraintList list)
{

  constraintList_elements (list, el)
    {
      if ( constraint_conflict(el, c) )
	{
	  constraint_fixConflict (el, c);
	  return TRUE;
	}
    } end_constraintList_elements;

    return FALSE;

}

/*
  check if constraint in list1 conflicts with constraints in List2.  If so we
  remove form list1 and change list2.
*/

constraintList constraintList_fixConflicts (constraintList list1, constraintList list2)
{
  constraintList ret;
  ret = constraintList_makeNew();
  llassert(constraintList_isDefined(list1) );
  constraintList_elements (list1, el)
    {
      if (! conflict (el, list2) )
	{
	  constraint temp;
	  temp = constraint_copy(el);
	  ret = constraintList_add (ret, temp);
	}
    } end_constraintList_elements;

    return ret;
}

/*returns true if constraint post satisfies cosntriant pre */

static bool constraintResolve_satisfies (constraint pre, constraint post)
{
  if (!constraint_isDefined (pre))
    {
      return TRUE;
    }

  if (!constraint_isDefined(post)) 
    {
      return FALSE;
    }

  if (constraint_isAlwaysTrue (pre))
    return TRUE;
  
  if (!constraintExpr_similar (pre->lexpr, post->lexpr) )
    {
      return FALSE;
    }
  
  if (constraintExpr_isUndefined(post->expr))
    {
      llassert(FALSE);
      return FALSE;
    }
  
  return rangeCheck (pre->ar, pre->expr, post->ar, post->expr);
}


bool constraintList_resolve (/*@temp@*/ /*@observer@*/ constraint c,
			     /*@temp@*/ /*@observer@*/ constraintList p)
{
  DPRINTF (("[resolve] Trying to resolve constraint: %s using %s",
	    constraint_unparse (c),
	    constraintList_unparse (p)));

  constraintList_elements (p, el)
    {
      if (constraintResolve_satisfies (c, el))
	{
	  DPRINTF (("constraintList_resolve: %s satifies %s", 
		    constraint_unparse (el), constraint_unparse (c)));
	  return TRUE;
	}
      
      DPRINTF (("constraintList_resolve: %s does not satify %s\n ", 
		constraint_unparse (el), constraint_unparse (c)));
    }
  end_constraintList_elements;

  DPRINTF (("No constraints satify: %s", constraint_unparse (c)));
  return FALSE;
}

static bool arithType_canResolve (arithType ar1, arithType ar2)
{
  switch (ar1)
    {
    case GTE:
    case GT:
      if ((ar2 == GT) || (ar2 == GTE) || (ar2 == EQ))
	{
	  return TRUE;
	}
      break;

    case EQ:
      if (ar2 == EQ)
	return TRUE;
      break;

    case LT:
    case LTE:
      if ((ar2 == LT) || (ar2 == LTE) || (ar2 == EQ))
	return TRUE;
      break;
    default:
      return FALSE;
    }
  return FALSE;	  
}

/*checks for the case expr2 == sizeof buf1  and buf1 is a fixed array*/
static bool sizeofBufComp(constraintExpr buf1, constraintExpr expr2)
{
  constraintTerm ct;
  exprNode e, t;
  sRef s1, s2;

  llassert(constraintExpr_isDefined(buf1) && constraintExpr_isDefined(expr2) );

  /*@access constraintExpr@*/
  
  if ((expr2->kind != term) && (buf1->kind != term) )
    return FALSE;

  
  ct = constraintExprData_termGetTerm(expr2->data);

  if (!constraintTerm_isExprNode(ct) )
    return FALSE;

  e = constraintTerm_getExprNode(ct);

  llassert (exprNode_isDefined(e));

  if (! (exprNode_isDefined(e)))
    return FALSE;
  
  if (e->kind != XPR_SIZEOF)
    return FALSE;
  
  t = exprData_getSingle (e->edata);
  s1 = exprNode_getSref (t);

  s2 = constraintTerm_getsRef(constraintExprData_termGetTerm(buf1->data) );

  /*drl this may be the wronge thing to test for but this
    seems to work correctly*/
  if (sRef_similarRelaxed(s1, s2)   || sRef_sameName (s1, s2) )
    {
      /* origly checked that ctype_isFixedArray(sRef_getType(s2)) but
	 removed that test */
	return TRUE;
    }
  return FALSE;
}

/* look for the special case of
   maxSet(buf) >= sizeof(buf) - 1
*/

/*drl eventually it would be good to check that
  buf is of type char.*/

static bool sizeOfMaxSet( /*@observer@*/ /*@temp@*/ constraint c)
{
  constraintExpr l, r, buf1, buf2, con;

  DPRINTF(( message("sizeOfMaxSet: checking %s ", constraint_unparse(c) )
	    ));

  llassert (constraint_isDefined(c) );
    
  l = c->lexpr;
  r = c->expr;

  if (!((c->ar == EQ) || (c->ar == GTE) || (c->ar == LTE) ) )
    return FALSE;

  llassert (constraintExpr_isDefined(l)  );
  llassert (constraintExpr_isDefined(r)  );

  /*check if the constraintExpr is MaxSet(buf) */
  if (l->kind == unaryExpr)
    {
      if (constraintExprData_unaryExprGetOp(l->data) == MAXSET)
	{
	  buf1 = constraintExprData_unaryExprGetExpr(l->data);
	}
      else
	return FALSE;
    }
  else
    return FALSE;

  
  if (r->kind != binaryexpr)
    return FALSE;
  
  buf2 = constraintExprData_binaryExprGetExpr1(r->data);
  con = constraintExprData_binaryExprGetExpr2(r->data);
  
  if (constraintExprData_binaryExprGetOp(r->data) == BINARYOP_MINUS)
    {
      if (constraintExpr_canGetValue(con) )
	{
	  long i;
	  
	  i = constraintExpr_getValue(con);
	  if (i != 1)
	    {
	      return FALSE;
	    }
	}
      else
	return FALSE;
    }

  if (constraintExprData_binaryExprGetOp(r->data) == BINARYOP_PLUS)
    {
      if (constraintExpr_canGetValue(con) )
	{
	  long i;
	  
	  i = constraintExpr_getValue(con);
	  if (i != -1)
	    {
	      return FALSE;
	    }
	}
      else
	return FALSE;
    }

  if (sizeofBufComp(buf1, buf2))
    {
      return TRUE;
    }
  else
    {
     return FALSE;
    } 
}
/*@noaccess constraintExpr@*/

/* We look for constraint which are tautologies */

bool constraint_isAlwaysTrue (/*@observer@*/ /*@temp@*/ constraint c)
{
  constraintExpr l, r;
  bool rHasConstant;
  int rConstant;

  
  llassert (constraint_isDefined(c) );  
  
  l = c->lexpr;
  r = c->expr;

  DPRINTF(( message("constraint_IsAlwaysTrue:examining %s", constraint_unparse(c) ) ));

  if (sizeOfMaxSet(c) )
    return TRUE;
  
  if (constraintExpr_canGetValue(l) && constraintExpr_canGetValue(r) )
    {
      int cmp;
      cmp = constraintExpr_compare (l, r);
      switch (c->ar)
	{
	case EQ:
	  return (cmp == 0);
	case GT:
	  return (cmp > 0);
	case GTE:
	  return (cmp >= 0);
	case LTE:
	  return (cmp <= 0);
	case LT:
	  return (cmp < 0);

	default:
	  BADEXIT;
	  /*@notreached@*/
	  break;
	}
    }

  if (constraintExpr_similar (l,r))
    {
      switch (c->ar)
	{
	case EQ:
	case GTE:
	case LTE:
	  return TRUE;
	  
	case GT:
	case LT:
	  break;
	default:
	  BADEXIT;
	  /*@notreached@*/
	  break;
	}
    }

  l = constraintExpr_copy (c->lexpr);
  r = constraintExpr_copy (c->expr);

  r = constraintExpr_propagateConstants (r, &rHasConstant, &rConstant);

  if (constraintExpr_similar (l,r) && (rHasConstant ) )
    {
      DPRINTF(( message("constraint_IsAlwaysTrue: after removing constants %s and %s are similar", constraintExpr_unparse(l), constraintExpr_unparse(r) ) ));
      DPRINTF(( message("constraint_IsAlwaysTrue: rconstant is %d", rConstant ) ));
      
      constraintExpr_free(l);
      constraintExpr_free(r);
      
      switch (c->ar)
	{
	case EQ:
	  return (rConstant == 0);
	case LT:
	  return (rConstant > 0);
	case LTE:
	  return (rConstant >= 0);
	case GTE:
	  return (rConstant <= 0);
	case GT:
	  return (rConstant < 0);
	  
	default:
	  BADEXIT;
	  /*@notreached@*/
	  break;
	}
    }  
      else
      {
	constraintExpr_free(l);
	constraintExpr_free(r);
	DPRINTF(( message("Constraint %s is not always true", constraint_unparse(c) ) ));
	return FALSE;
      }
  
  BADEXIT;
}

static bool rangeCheck (arithType ar1, /*@observer@*/ constraintExpr expr1, arithType ar2, /*@observer@*/ constraintExpr expr2)

{
  DPRINTF (("Doing range check %s and %s",
	    constraintExpr_unparse (expr1), constraintExpr_unparse (expr2)));

  if (!arithType_canResolve (ar1, ar2))
    return FALSE;
  
  switch (ar1)
    {
    case GTE:
      if (constraintExpr_similar (expr1, expr2) )
	return TRUE;
      /*@fallthrough@*/
    case GT:
      if (!  (constraintExpr_canGetValue (expr1) &&
	      constraintExpr_canGetValue (expr2) ) )
	{
	  constraintExpr e1, e2;
	  bool p1, p2;
	  int const1, const2;
	  
	  e1 = constraintExpr_copy(expr1);
	  e2 = constraintExpr_copy(expr2);
	  
	  e1 = constraintExpr_propagateConstants (e1, &p1, &const1);
	  e2 = constraintExpr_propagateConstants (e2, &p2, &const2);
	  
	  if (p1 || p2)
	    {
	      if (!p1)
		const1 = 0;
	      
	      if (!p2)
		const2 = 0;
	      
	      if (const1 <= const2)
		if (constraintExpr_similar (e1, e2) )
		  {
		    constraintExpr_free(e1);
		    constraintExpr_free(e2);
		    return TRUE;
		  }
	    }
	  DPRINTF(("Can't Get value"));
	  
	  constraintExpr_free(e1);
	  constraintExpr_free(e2);
	  return FALSE;
	}
      
      if (constraintExpr_compare (expr2, expr1) >= 0)
	return TRUE;
      
      return FALSE;
    case EQ:
      if (constraintExpr_similar (expr1, expr2) )
	return TRUE;
      
      return FALSE;
    case LTE:
      if (constraintExpr_similar (expr1, expr2) )
	return TRUE;
      /*@fallthrough@*/
    case LT:
      if (!  (constraintExpr_canGetValue (expr1) &&
	      constraintExpr_canGetValue (expr2) ) )
	{
	  constraintExpr e1, e2;
	  bool p1, p2;
	  int const1, const2;
	  
	  e1 = constraintExpr_copy(expr1);
	  e2 = constraintExpr_copy(expr2);
	  
	  e1 = constraintExpr_propagateConstants (e1, &p1, &const1);
	  
	  e2 = constraintExpr_propagateConstants (e2, &p2, &const2);
	  
	  if (p1 || p2)
	    {
	      if (!p1)
		const1 = 0;
	      
	      if (!p2)
		const2 = 0;
	      
	      if (const1 >= const2)
		if (constraintExpr_similar (e1, e2) )
		  {
		    constraintExpr_free(e1);
		    constraintExpr_free(e2);
		    return TRUE;
		  }
	    }
	  constraintExpr_free(e1);
	  constraintExpr_free(e2);
	  
	  DPRINTF(("Can't Get value"));
	  return FALSE;
	}
      
      if (constraintExpr_compare (expr2, expr1) <= 0)
	return TRUE;
      
      return FALSE;
      
    default:
      llcontbug((message("Unhandled case in switch: %q", arithType_print(ar1) ) ) );
    }
  BADEXIT;
}

static constraint constraint_searchandreplace (/*@returned@*/ constraint c, constraintExpr old, constraintExpr newExpr)
{
  llassert (constraint_isDefined(c));
  
  DPRINTF (("Starting replace lexpr [%p]: %s < %s ==> %s > in %s", c, 
	    constraintExpr_unparse (c->lexpr), 
	    constraintExpr_unparse (old), constraintExpr_unparse (newExpr),
	    constraint_unparse (c)));
  c->lexpr = constraintExpr_searchandreplace (c->lexpr, old, newExpr);
  DPRINTF (("Finished replace lexpr [%p]: %s", c, constraintExpr_unparse (c->lexpr)));
  c->expr = constraintExpr_searchandreplace (c->expr, old, newExpr);
  return c;
}

bool constraint_search (constraint c, constraintExpr old) /*@*/
{
  bool ret;
  ret = FALSE;
  
  llassert (constraint_isDefined (c));
  
  ret  = constraintExpr_search (c->lexpr, old);
  ret = ret || constraintExpr_search (c->expr, old);
  return ret;
}

/* adjust file locs and stuff */
static constraint constraint_adjust (/*@returned@*/ constraint substitute, /*@observer@*/ constraint old)
{
  fileloc loc1, loc2, loc3;

  DPRINTF ((message("Start adjust on %s and %s", constraint_unparse(substitute),
		     constraint_unparse(old))
		   ));

  llassert(constraint_isDefined(substitute));
  llassert(constraint_isDefined(old));
	   
  loc1 = constraint_getFileloc (old);
  loc2 = constraintExpr_loc (substitute->lexpr);
  loc3 = constraintExpr_loc (substitute->expr);
  
  /* special case of an equality that "contains itself" */
  if (constraintExpr_search (substitute->expr, substitute->lexpr) )
      if (fileloc_closer (loc1, loc3, loc2))
      {
	constraintExpr temp;
	DPRINTF ((message("Doing adjust on %s", constraint_unparse(substitute) )
		   ));
	temp = substitute->lexpr;
	substitute->lexpr = substitute->expr;
	substitute->expr  = temp;
	substitute = constraint_simplify(substitute);
      }

  fileloc_free (loc1);
  fileloc_free (loc2);
  fileloc_free (loc3);

  return substitute;
  
}

/* If function preforms substitutes based on inequality

   It uses the rule x >= y && b < y  ===> x >= b + 1

   Warning this is sound but throws out information
 */

constraint  inequalitySubstitute  (/*@returned@*/ constraint c, constraintList p)
{
  llassert(constraint_isDefined(c) );

  if (c->ar != GTE)
    return c;
  
  constraintList_elements (p, el)
    {
      
      llassert(constraint_isDefined(el) );
      
      if ((el->ar == LT )  )
	   {
	     constraintExpr  temp2;
	     
	     if (constraintExpr_same (el->expr, c->expr) )
	       {
		 DPRINTF((message ("inequalitySubstitute Replacing %q in %q with  %q",
				   constraintExpr_print (c->expr),
				   constraint_unparse (c),
				   constraintExpr_print (el->expr) )
			  ));
		 temp2   = constraintExpr_copy (el->lexpr);
		 constraintExpr_free(c->expr);
		 c->expr =  constraintExpr_makeIncConstraintExpr (temp2);

	       }
	     
	   }
    }
  end_constraintList_elements;

  c = constraint_simplify(c);
  return c;
}


/* drl7x 7/26/001

   THis function is like inequalitySubstitute but it adds the rule
   added the rules x >= y &&  y <= b  ===> x >= b
    x >= y &&  y < b  ===> x >= b + 1

   This is sound but sonce it throws out additional information it should only one used
   if we're oring constraints.
 */

static constraint  inequalitySubstituteStrong  (/*@returned@*/ constraint c, constraintList p)
{
  DPRINTF (( message ("inequalitySubstituteStrong examining substituting for %q", constraint_unparse(c) ) ));      

  llassert(constraint_isDefined(c) );

  if (! (constraint_isDefined(c) ) )
  {
    return c;
  }
  
  if (c->ar != GTE)
    return c;
  
  DPRINTF (( message ("inequalitySubstituteStrong examining substituting for %q with %q",
		      constraint_unparse(c), constraintList_unparse(p) ) ));      
  constraintList_elements (p, el)
    {
      
      DPRINTF (( message ("inequalitySubstituteStrong examining substituting %s on %s", constraint_unparse(el), constraint_unparse(c) ) ));      

      llassert(constraint_isDefined(el) );
      if ((el->ar == LT ) ||  (el->ar == LTE )  )
	   {
	     constraintExpr  temp2;
	     
	     if (constraintExpr_same (el->lexpr, c->expr) )
	       {
		 DPRINTF((message ("inequalitySubstitute Replacing %s in %s with  %s",
				   constraintExpr_print (c->expr),
				   constraint_unparse (c),
				   constraintExpr_print (el->expr) )
			  ));
		 temp2   = constraintExpr_copy (el->expr);
		 constraintExpr_free(c->expr);
		 if ((el->ar == LTE ) )
		   {
		     c->expr = temp2;
		   }
		 else
		   {
		     c->expr =  constraintExpr_makeIncConstraintExpr (temp2);
		   }
	       }
	     
	   }
    }
  end_constraintList_elements;

  c = constraint_simplify(c);
  return c;
}


/* This function performs substitutions based on the rule:
   for a constraint of the form expr1 >= expr2;   a < b =>
   a = b -1 for all a in expr1.  This will work in most cases.

   Like inequalitySubstitute we're throwing away some information
*/

static constraint  inequalitySubstituteUnsound  (/*@returned@*/ constraint c, constraintList p)
{
  DPRINTF (( message ("Doing inequalitySubstituteUnsound " ) ));

    llassert(constraint_isDefined(c) );
  
  if (c->ar != GTE)
    return c;
  
  constraintList_elements (p, el)
    {

      llassert(constraint_isDefined(el) );

      DPRINTF (( message ("inequalitySubstituteUnsound examining substituting %s on %s", constraint_unparse(el), constraint_unparse(c) ) ));      
       if (( el->ar == LTE) || (el->ar == LT) )
	   {
	     constraintExpr  temp2;

	     temp2   = constraintExpr_copy (el->expr);
	     
	     if (el->ar == LT)
	       temp2  =  constraintExpr_makeDecConstraintExpr (temp2);
	     
	     DPRINTF((message ("Replacing %s in %s with  %s",
			       constraintExpr_print (el->lexpr),
			       constraintExpr_print (c->lexpr),
			       constraintExpr_print (temp2) ) ));
	     
	     c->lexpr = constraintExpr_searchandreplace (c->lexpr, el->lexpr, temp2);
	     constraintExpr_free(temp2);
	   }
    }
  end_constraintList_elements;

  c = constraint_simplify(c);
  return c;
}

/*@only@*/ constraint constraint_substitute (/*@observer@*/ /*@temp@*/ constraint c, constraintList p)
{
  constraint ret = constraint_copy (c);

  constraintList_elements (p, el)
    {
      if (constraint_isDefined (el))
	{
	  if ( el->ar == EQ)
	    if (!constraint_conflict (ret, el))
	      {
		constraint temp = constraint_copy(el);
		temp = constraint_adjust(temp, ret);
		
		llassert(constraint_isDefined(temp) );
		
		
		DPRINTF (("constraint_substitute :: Substituting in %s using %s",
			  constraint_unparse (ret), constraint_unparse (temp)));
	  
		ret = constraint_searchandreplace (ret, temp->lexpr, temp->expr);
		DPRINTF (("constraint_substitute :: The new constraint is %s", constraint_unparse (ret)));;
		constraint_free(temp);
	      }
	}
    }
  end_constraintList_elements;

  ret = constraint_simplify (ret);
  DPRINTF(( message (" constraint_substitute :: The final new constraint is %s", constraint_unparse (ret) ) ));
  return ret;
}


/*@only@*/ constraintList constraintList_substituteFreeTarget (/*@only@*/ constraintList target, /*@observer@*/ constraintList subList)
{
constraintList ret;

ret = constraintList_substitute (target, subList);

constraintList_free(target);

return ret;
}

/* we try to do substitutions on each constraint in target using the constraint in sublist*/

/*@only@*/ constraintList constraintList_substitute (constraintList target,/*2observer@*/  constraintList subList)
{

  constraintList ret;

  ret = constraintList_makeNew();
  
  constraintList_elements(target, el)
  { 
    constraint temp;
    /* drl possible problem : warning make sure that a side effect is not expected */

    temp = constraint_substitute(el, subList);
    ret = constraintList_add (ret, temp);
  }
  end_constraintList_elements;

  return ret;
}

static constraint constraint_solve (/*@returned@*/ constraint c)
{

  llassert(constraint_isDefined(c) );

  DPRINTF((message ("Solving %s\n", constraint_unparse(c) ) ) );
  c->expr = constraintExpr_solveBinaryExpr (c->lexpr, c->expr);
  DPRINTF((message ("Solved and got %s\n", constraint_unparse(c) ) ) );

  return c;
}

static arithType flipAr (arithType ar)
{
  switch (ar)
    {
    case LT:
      return GT;
    case LTE:
      return GTE;
    case EQ:
      return EQ;
    case GT:
      return LT;
    case GTE:
      return LTE;
    default:
      llcontbug (message("unexpected value: case not handled"));
    }
  BADEXIT;
}

static constraint  constraint_swapLeftRight (/*@returned@*/ constraint c)
{
  constraintExpr temp;

  llassert(constraint_isDefined(c) );

  c->ar = flipAr (c->ar);
  temp = c->lexpr;
  c->lexpr = c->expr;
  c->expr = temp;
  DPRINTF(("Swaped left and right sides of constraint"));
  return c;
}



constraint constraint_simplify ( /*@returned@*/ constraint c)
{
  
  llassert(constraint_isDefined(c) );
	
  DPRINTF(( message("constraint_simplify on %q ", constraint_unparse(c) ) ));

  if (constraint_tooDeep(c))
    {
        DPRINTF(( message("constraint_simplify: constraint to complex aborting %q ", constraint_unparse(c) ) ));
      return c;

    }
  
  c->lexpr = constraintExpr_simplify (c->lexpr);
  c->expr  = constraintExpr_simplify (c->expr);

  if (constraintExpr_isBinaryExpr (c->lexpr) )
    {
      c = constraint_solve (c);
      
      c->lexpr = constraintExpr_simplify (c->lexpr);
      c->expr  = constraintExpr_simplify (c->expr);
    }
  
  if (constraintExpr_isLit(c->lexpr) && (!constraintExpr_isLit(c->expr) ) )
    {
      c = constraint_swapLeftRight(c);
      /*I don't think this will be an infinate loop*/
      c = constraint_simplify(c);
    }

  DPRINTF(( message("constraint_simplify returning  %q ", constraint_unparse(c) ) ));

  return c;
}




/* returns true  if fileloc for term1 is closer to file for term2 than term3*/

bool fileloc_closer (fileloc  loc1, fileloc  loc2, fileloc  loc3)
{

  if  (!fileloc_isDefined (loc1) )
    return FALSE;

  if  (!fileloc_isDefined (loc2) )
    return FALSE;

  if  (!fileloc_isDefined (loc3) )
    return TRUE;

  
  
  
  if (fileloc_equal (loc2, loc3) )
    return FALSE;

  if (fileloc_equal (loc1, loc2) )
    return TRUE;

    if (fileloc_equal (loc1, loc3) )
    return FALSE;

   if ( fileloc_lessthan (loc1, loc2) )
     {
       if (fileloc_lessthan (loc2, loc3) )
	 {
	   llassert (fileloc_lessthan (loc1, loc3) );
	   return TRUE;
	 }
       else
	 {
	   return FALSE;
	 }
     }

   if ( !fileloc_lessthan (loc1, loc2) )
     {
       if (!fileloc_lessthan (loc2, loc3) )
	 {
	   llassert (!fileloc_lessthan (loc1, loc3) );
	   return TRUE;
	 }
       else
	 {
	   return FALSE;
	 }
     }

   llassert(FALSE);
   return FALSE;
}


