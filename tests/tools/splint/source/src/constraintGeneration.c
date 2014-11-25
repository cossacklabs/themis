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
** constraintGeneration.c
*/

/* #define DEBUGPRINT 1 */

# include <ctype.h> /* for isdigit */
# include "splintMacros.nf"
# include "basic.h"

# include "cgrammar_tokens.h"

# include "exprChecks.h"
# include "exprNodeSList.h"

/*drl We need to access the internal representation of exprNode
  because these functions walk down the parse tree and need a richer
information than is accessible through the exprNode interface.*/
  
/*@access exprNode@*/

static /*@nullwhentrue@*/ bool exprNode_handleError (/*@temp@*/ exprNode p_e);

static void exprNode_stmt (/*@temp@*/ /*@temp@*/ exprNode p_e);
static void  exprNode_multiStatement (/*@temp@*/ exprNode p_e);

static constraintList exprNode_traverseTrueEnsuresConstraints (/*@temp@*/ exprNode p_e);
static constraintList exprNode_traverseFalseEnsuresConstraints (/*@temp@*/ exprNode p_e);

static void checkArgumentList (/*@out@*/ exprNode p_temp, exprNodeList p_arglist, fileloc p_sequencePoint) /*@modifies p_temp @*/;

static constraintList checkCall (/*@temp@*/ exprNode p_fcn, exprNodeList p_arglist);

static bool exprNode_isUnhandled (/*@temp@*/ /*@observer@*/ exprNode e)
{
  llassert(exprNode_isDefined(e));
  switch (e->kind)
    {
    case XPR_INITBLOCK:
    case XPR_EMPTY:
    case XPR_LABEL:
    case XPR_CONST:
    case XPR_VAR:
    case XPR_BODY:
    case XPR_OFFSETOF:
    case XPR_ALIGNOFT:
    case XPR_ALIGNOF:
    case XPR_VAARG:
    case XPR_ITERCALL:
    case XPR_ITER:
    case XPR_GOTO:
    case XPR_CONTINUE:
    case XPR_BREAK:
    case XPR_COMMA:
    case XPR_COND:
    case XPR_TOK:
    case XPR_FTDEFAULT:
    case XPR_DEFAULT:
    case XPR_FTCASE:
    case XPR_CASE:
    case XPR_NODE:
      DPRINTF((message ("Warning current constraint generation does not handle expression %s", exprNode_unparse(e))));
      return TRUE;
      /*@notreached@*/
      break;
    default:
      return FALSE;
      
    }
  /*not reached*/
  return FALSE;
}

/*@nullwhentrue@*/ bool exprNode_handleError (exprNode e) 
{
  if (exprNode_isError (e) || exprNode_isUnhandled (e))
    {
      return TRUE; 
    }
  
   return FALSE;
}

/* evans 2002-03-2 - parameter was dependent */
bool /*@alt void@*/ exprNode_generateConstraints (/*@temp@*/ exprNode e) 
{
  if (exprNode_isError (e))
    return FALSE;

  if (exprNode_isUnhandled (e))
    {
      DPRINTF((message("Warning ignoring %s", exprNode_unparse (e))));
      return FALSE;
    }

  DPRINTF ((message ("exprNode_generateConstraints Analyzing %s at %s", exprNode_unparse(e),
		     fileloc_unparse(exprNode_loc (e)))));
  
  if (exprNode_isMultiStatement (e))
    {
      exprNode_multiStatement(e);
    }
  else
    {
      /*        fileloc loc; */
      
      /*        loc = exprNode_getNextSequencePoint(e);  */
      /*        exprNode_exprTraverse(e, FALSE, FALSE, loc); */
      
      /*        fileloc_free(loc); */
      
      exprNode_stmt(e);
      return FALSE;
      
    }
  
  {
    constraintList c;
    
    c = constraintList_makeFixedArrayConstraints (e->uses);
    e->requiresConstraints = constraintList_reflectChangesFreePre (e->requiresConstraints, c);
    constraintList_free(c);
  }    
  
  DPRINTF ((message ("e->requiresConstraints %s", 
		     constraintList_unparseDetailed (e->requiresConstraints))));
  return FALSE;
}

static void exprNode_stmt (/*@temp@*/ exprNode e)
{
  exprNode snode;
  fileloc loc;
  
  DPRINTF (("Generating constraint for: %s", exprNode_unparse (e)));

  if (exprNode_isError(e))
    {
      return; 
    }

  /*e->requiresConstraints = constraintList_makeNew();
    e->ensuresConstraints  = constraintList_makeNew(); */
  
  /*!! s = exprNode_unparse (e); */
  
  if (e->kind == XPR_INIT)
    {
      constraintList tempList;
      DPRINTF (("Init: %s ", exprNode_unparse (e)));
      loc = exprNode_getNextSequencePoint (e); /* reduces to an expression */
      DPRINTF (("Location: %s", fileloc_unparse (loc)));
      DPRINTF (("Ensures before: %s", constraintList_unparse (e->ensuresConstraints)));
      exprNode_exprTraverse (e, FALSE, FALSE, loc);
      DPRINTF (("Ensures after: %s", constraintList_unparse (e->ensuresConstraints)));
      DPRINTF (("After traversing..."));
      fileloc_free(loc);
      
      tempList = e->requiresConstraints;
      DPRINTF (("Requires before: %s", constraintList_unparse (e->requiresConstraints)));
      e->requiresConstraints = exprNode_traverseRequiresConstraints (e);
      DPRINTF (("Requires after: %s", constraintList_unparse (e->requiresConstraints)));
      constraintList_free(tempList);

      tempList = e->ensuresConstraints;
      DPRINTF (("Ensures before: %s", constraintList_unparse (e->ensuresConstraints)));
      e->ensuresConstraints  = exprNode_traverseEnsuresConstraints(e);
      DPRINTF (("Ensures before: %s", constraintList_unparse (e->ensuresConstraints)));
      constraintList_free(tempList);
      return; 
    }

  /*drl 2/13/002 patched bug so return statement will be checked*/
  /*return is a stmt not not expression ...*/
  if (e->kind == XPR_RETURN)
    {
      constraintList tempList;
      
      loc = exprNode_getNextSequencePoint(e); /* reduces to an expression */
      
      exprNode_exprTraverse (exprData_getSingle (e->edata), FALSE, TRUE, loc);
      fileloc_free(loc);
      
      tempList = e->requiresConstraints;
      e->requiresConstraints = exprNode_traverseRequiresConstraints(e);
      constraintList_free(tempList);
    }
  
  if (e->kind != XPR_STMT)
    {
      DPRINTF (("Not Stmt"));
      DPRINTF ((message ("%s ", exprNode_unparse (e))));

      if (exprNode_isMultiStatement (e))
	{
	  exprNode_multiStatement (e); /* evans 2001-08-21: spurious return removed */
	}
      else
	{
	  loc = exprNode_getNextSequencePoint(e); /* reduces to an expression */
	  
	  exprNode_exprTraverse (e, FALSE, TRUE, loc);
	  fileloc_free(loc);
	  
	  }
	  return; 
    }
 
  DPRINTF (("Stmt"));
  DPRINTF ((message ("%s ", exprNode_unparse (e))));
     
  snode = exprData_getUopNode (e->edata);
  
  /* could be stmt involving multiple statements:
     i.e. if, while for ect.
  */
  
  if (exprNode_isMultiStatement (snode))
    {
      exprNode_multiStatement (snode);
      (void) exprNode_copyConstraints (e, snode);
      return;
    }
  
  loc = exprNode_getNextSequencePoint(e); /* reduces to an expression */
  exprNode_exprTraverse (snode, FALSE, FALSE, loc);

  fileloc_free(loc);

  constraintList_free (e->requiresConstraints);
  e->requiresConstraints = exprNode_traverseRequiresConstraints(snode);

  constraintList_free (e->ensuresConstraints);
  e->ensuresConstraints  = exprNode_traverseEnsuresConstraints(snode);
  
  DPRINTF ((message ("smtlist constraints are: pre: %s \n and \t post %s\n",
		      constraintList_unparse(e->requiresConstraints),
		      constraintList_unparse(e->ensuresConstraints))));

  return; 
}

static void exprNode_stmtList  (/*@dependent@*/ exprNode e)
{
  exprNode stmt1, stmt2;
  if (exprNode_isError (e))
    {
      return; 
    }

  /*
    Handle case of stmtList with only one statement:
    The parse tree stores this as stmt instead of stmtList
  */

  if (e->kind != XPR_STMTLIST)
    {
      exprNode_stmt(e);
      return;
    }
  llassert (e->kind == XPR_STMTLIST);
  DPRINTF(("exprNode_stmtList STMTLIST:"));
  DPRINTF ((cstring_toCharsSafe (exprNode_unparse(e))));
  stmt1 = exprData_getPairA (e->edata);
  stmt2 = exprData_getPairB (e->edata);


  DPRINTF(("exprNode_stmtlist       "));
  DPRINTF ((message("XW%s    |        %s", exprNode_unparse(stmt1), exprNode_unparse(stmt2))));
   
  exprNode_stmt (stmt1);
  DPRINTF(("\nstmt after stmtList call "));

  exprNode_stmt (stmt2);
  exprNode_mergeResolve (e, stmt1, stmt2);
  
  DPRINTF ((message ("smtlist constraints are: pre: %s \n and \t post %s\n",
		      constraintList_unparse(e->requiresConstraints),
		      constraintList_unparse(e->ensuresConstraints))));
  return;
}

static exprNode doIf (/*@returned@*/  exprNode e, /*@dependent@*/ exprNode test, /*@dependent@*/ exprNode body)
{
  constraintList temp;

  DPRINTF ((message ("doIf: %s ", exprNode_unparse(e))));

  llassert (exprNode_isDefined(test));
  llassert (exprNode_isDefined (e));
  llassert (exprNode_isDefined (body));

  DPRINTF((message ("ensures constraints for %s are %s", exprNode_unparse(e), constraintList_unparseDetailed(e->ensuresConstraints))));

      DPRINTF((message ("Requires constraints for %s are %s", exprNode_unparse(e), constraintList_unparseDetailed(e->ensuresConstraints))));
      
      DPRINTF((message ("trueEnsures constraints for %s are %s", exprNode_unparse(e), constraintList_unparseDetailed(e->trueEnsuresConstraints))));

      DPRINTF((message ("falseEnsures constraints for %s are %s", exprNode_unparse(e), constraintList_unparseDetailed(e->falseEnsuresConstraints))));



      DPRINTF((message ("ensures constraints for %s are %s", exprNode_unparse(test), constraintList_unparseDetailed(test->ensuresConstraints))));

      DPRINTF((message ("Requires constraints for %s are %s", exprNode_unparse(test), constraintList_unparseDetailed(test->ensuresConstraints))));
      
      DPRINTF((message ("trueEnsures constraints for %s are %s", exprNode_unparse(test), constraintList_unparseDetailed(test->trueEnsuresConstraints))));

      DPRINTF((message ("falseEnsures constraints for %s are %s", exprNode_unparse(test), constraintList_unparseDetailed(test->falseEnsuresConstraints))));



      temp = test->trueEnsuresConstraints;
      test->trueEnsuresConstraints =  exprNode_traverseTrueEnsuresConstraints(test);
      constraintList_free(temp);

  temp = test->ensuresConstraints;
  test->ensuresConstraints = exprNode_traverseEnsuresConstraints (test);
  constraintList_free(temp);

  temp = test->requiresConstraints;
  test->requiresConstraints = exprNode_traverseRequiresConstraints (test);
  constraintList_free(temp);


  test->trueEnsuresConstraints = constraintList_substituteFreeTarget(test->trueEnsuresConstraints, test->ensuresConstraints);
  
  DPRINTF ((message ("doIf: test ensures %s ", constraintList_unparse(test->ensuresConstraints))));
    
  DPRINTF ((message ("doIf: test true ensures %s ", constraintList_unparse(test->trueEnsuresConstraints))));
  
  constraintList_free(e->requiresConstraints);

  
  e->requiresConstraints = constraintList_reflectChanges(body->requiresConstraints, test->trueEnsuresConstraints);

  e->requiresConstraints = constraintList_reflectChangesFreePre (e->requiresConstraints,
					   test->ensuresConstraints);
  temp = e->requiresConstraints;
  e->requiresConstraints = constraintList_mergeRequires (e->requiresConstraints, test->requiresConstraints);
  constraintList_free(temp);


  /* drl possible problem : warning bad */
  constraintList_free(e->ensuresConstraints);
  e->ensuresConstraints = constraintList_copy (test->ensuresConstraints);
  
  if (exprNode_mayEscape (body))
    {
      DPRINTF ((message("doIf: the if statement body %s returns or exits", exprNode_unparse(body))));
      e->ensuresConstraints = constraintList_mergeEnsuresFreeFirst (e->ensuresConstraints,
							test->falseEnsuresConstraints);
    }
  
  DPRINTF ((message ("doIf: if requiers %s ", constraintList_unparse(e->requiresConstraints))));
  
  return e;
}

/*drl added 3/4/2001
  Also used for condition i.e. ?: operation

  Precondition
  This function assumes that p, trueBranch, falseBranch have have all been traversed
  for constraints i.e. we assume that exprNode_traverseEnsuresConstraints,
  exprNode_traverseRequiresConstraints,  exprNode_traverseTrueEnsuresConstraints,
  exprNode_traverseFalseEnsuresConstraints have all been run
*/

static exprNode doIfElse (/*@returned@*/ exprNode e, /*@dependent@*/ exprNode p, /*@dependent@*/ exprNode trueBranch, /*@dependent@*/ exprNode falseBranch)
{
  constraintList c1, cons, t, t2, f, f2;
  
  llassert (exprNode_isDefined (e));
  llassert (exprNode_isDefined (p));
  llassert (exprNode_isDefined (trueBranch));
  llassert (exprNode_isDefined (falseBranch));
  DPRINTF ((message ("doIfElse: %s ", exprNode_unparse(e))));
  
  /* do requires clauses */
  c1 = constraintList_copy (p->ensuresConstraints);
  
  t = constraintList_reflectChanges(trueBranch->requiresConstraints, p->trueEnsuresConstraints);
  t = constraintList_reflectChangesFreePre (t, p->ensuresConstraints);
  
  cons = constraintList_reflectChanges(falseBranch->requiresConstraints, p->falseEnsuresConstraints);
  cons  = constraintList_reflectChangesFreePre (cons, c1);
  
  constraintList_free (e->requiresConstraints);
  e->requiresConstraints = constraintList_mergeRequiresFreeFirst (t, cons);
  e->requiresConstraints = constraintList_mergeRequiresFreeFirst (e->requiresConstraints, p->requiresConstraints);
  
  /* do ensures clauses
     find the  the ensures lists for each subbranch
  */

  t = constraintList_mergeEnsures (p->trueEnsuresConstraints, trueBranch->ensuresConstraints);
  t2 = t;
  t = constraintList_mergeEnsures (p->ensuresConstraints, t);
  constraintList_free(t2);
  
  f = constraintList_mergeEnsures (p->falseEnsuresConstraints, falseBranch->ensuresConstraints);
  f2 = f;
  f = constraintList_mergeEnsures (p->ensuresConstraints, f);
  constraintList_free(f2);
  
  /* find ensures for whole if/else statement */
  
  constraintList_free(e->ensuresConstraints);
  
  e->ensuresConstraints = constraintList_logicalOr (t, f);
  
  constraintList_free(t);
  constraintList_free(f);
  constraintList_free(cons);
  constraintList_free(c1);
  
  DPRINTF ((message ("doIfElse: if requires %q ", constraintList_unparse(e->requiresConstraints))));
  DPRINTF ((message ("doIfElse: if ensures %q ", constraintList_unparse(e->ensuresConstraints))));
  
  return e;
}

static exprNode doWhile (/*@returned@*/ exprNode e, /*@dependent@*/ exprNode test, /*@dependent@*/ exprNode body)
{
  DPRINTF ((message ("doWhile: %s ", exprNode_unparse(e))));
  return doIf (e, test, body);
}

/*@only@*/ constraintList constraintList_makeFixedArrayConstraints (/*@observer@*/ sRefSet s)
{
  constraintList ret;
  constraint con;
  ret = constraintList_makeNew();
 
  sRefSet_elements (s, el)
    {
      if (sRef_isFixedArray(el))
	{
	  size_t size;
	  DPRINTF((message("%s is a fixed array",
			    sRef_unparse(el))));
	  size = sRef_getArraySize(el);
	  DPRINTF((message("%s is a fixed array with size %d",
			    sRef_unparse(el), (int)size)));
	  con = constraint_makeSRefSetBufferSize (el, size_toLong (size - 1));
	  ret = constraintList_add(ret, con);
	}
      else
	{
	  DPRINTF((message("%s is not a fixed array",
			    sRef_unparse(el))));
	  
	  
	  if (sRef_isExternallyVisible (el))
	    {
	      /*
		DPRINTF((message("%s is externally visible",
		sRef_unparse(el))));
		con = constraint_makeSRefWriteSafeInt(el, 0);
		ret = constraintList_add(ret, con);
		
		con = constraint_makeSRefReadSafeInt(el, 0);
		
		ret = constraintList_add(ret, con);
	      */
	    }
	}
    }
  end_sRefSet_elements ;
  
  DPRINTF((message("constraintList_makeFixedArrayConstraints returning %s",
		    constraintList_unparse(ret))));
  return ret;
}

# if 0
exprNode makeDataTypeConstraints (/*@returned@*/ exprNode e)
{
  constraintList c;
  DPRINTF(("makeDataTypeConstraints"));

  c = constraintList_makeFixedArrayConstraints (e->uses);
  
  e->ensuresConstraints = constraintList_addListFree (e->ensuresConstraints, c);
 
 return e;
}
# endif

static void doFor (/*@dependent@*/ exprNode e, /*@dependent@*/ exprNode forPred, /*@dependent@*/ exprNode forBody)
{
  exprNode init, test, inc;
  /* merge the constraints: modle as if statement */

  /* init
     if (test)
     for body
     inc        */
  
  llassert (exprNode_isDefined (e));
  llassert (exprNode_isDefined (forPred));
  llassert (exprNode_isDefined (forBody));

  init = exprData_getTripleInit (forPred->edata);
  test = exprData_getTripleTest (forPred->edata);
  inc = exprData_getTripleInc (forPred->edata);
  
  if (((exprNode_isError (test) /*|| (exprNode_isError(init))*/) || (exprNode_isError (inc))))
    {
      DPRINTF ((message ("strange for statement:%s, ignoring it", exprNode_unparse(e))));
      return;
    }
  
  exprNode_forLoopHeuristics(e, forPred, forBody);
  
  constraintList_free(e->requiresConstraints);
  e->requiresConstraints = constraintList_reflectChanges(forBody->requiresConstraints, test->ensuresConstraints);
  e->requiresConstraints = constraintList_reflectChangesFreePre (e->requiresConstraints, test->trueEnsuresConstraints);
  e->requiresConstraints = constraintList_reflectChangesFreePre (e->requiresConstraints, forPred->ensuresConstraints);
  
  if (!forBody->canBreak)
    {
      e->ensuresConstraints = constraintList_addListFree(e->ensuresConstraints, constraintList_copy(forPred->ensuresConstraints));
      e->ensuresConstraints = constraintList_addListFree(e->ensuresConstraints,constraintList_copy(test->falseEnsuresConstraints));
    }
  else
    {
      DPRINTF(("Can break"));
    }
}

static /*@dependent@*/ exprNode exprNode_makeDependent(/*@returned@*/  exprNode e)
{
  /* !!! DRL - this is ridiculous!  Read the manual on memory annotations please! */
  return e;
}

static void 
exprNode_doGenerateConstraintSwitch 
  (/*@dependent@*/ exprNode switchExpr,
   /*@dependent@*/ exprNode body,
   /*@special@*/ constraintList *currentRequires,
   /*@special@*/ constraintList *currentEnsures,
   /*@special@*/ constraintList *savedRequires,
   /*@special@*/ constraintList *savedEnsures)
  /*@post:only *currentRequires, *currentEnsures, *savedRequires, *savedEnsures @*/ 
  /*@sets *currentRequires,  *currentEnsures,  *savedRequires, *savedEnsures @*/
{
  exprNode stmt, stmtList;

  DPRINTF((message("exprNode_doGenerateConstraintSwitch: (switch %s) %s",
		    exprNode_unparse(switchExpr), exprNode_unparse(body)
		   )));

  if (exprNode_isError(body))
    {
      *currentRequires = constraintList_makeNew ();
      *currentEnsures = constraintList_makeNew ();

      *savedRequires = constraintList_makeNew ();
      *savedEnsures = constraintList_makeNew ();
      /*@-onlytrans@*/
      return;
      /*@=onlytrans@*/      
    }

  if (body->kind != XPR_STMTLIST)
    {
      DPRINTF((message("exprNode_doGenerateConstraintSwitch: non stmtlist: %s",
		       exprNode_unparse(body))));
      stmt = body;
      stmtList = exprNode_undefined;
      stmt = exprNode_makeDependent(stmt);
      stmtList = exprNode_makeDependent(stmtList);
    }
  else
    {
      stmt     = exprData_getPairB(body->edata);
      stmtList = exprData_getPairA(body->edata);
      stmt = exprNode_makeDependent(stmt);
      stmtList = exprNode_makeDependent(stmtList);
    }

  DPRINTF((message("exprNode_doGenerateConstraintSwitch: stmtlist: %s stmt: %s",
		   exprNode_unparse(stmtList), exprNode_unparse(stmt))
	  ));


  exprNode_doGenerateConstraintSwitch (switchExpr, stmtList, currentRequires, currentEnsures,
				       savedRequires, savedEnsures);

  if (exprNode_isError(stmt))
    /*@-onlytrans@*/
    return;
    /*@=onlytrans@*/

  exprNode_stmt(stmt);

  switchExpr = exprNode_makeDependent (switchExpr);
    
  if (! exprNode_isCaseMarker(stmt))
    {

      constraintList temp;

      DPRINTF ((message("Got normal statement %s (requires %s ensures %s)", exprNode_unparse(stmt),
			 constraintList_unparse(stmt->requiresConstraints), constraintList_unparse(stmt->ensuresConstraints))));

      temp = constraintList_reflectChanges (stmt->requiresConstraints,
					    *currentEnsures);

            *currentRequires = constraintList_mergeRequiresFreeFirst(
								     *currentRequires,
								     temp);

	    constraintList_free(temp);

	          *currentEnsures = constraintList_mergeEnsuresFreeFirst
		    (*currentEnsures,
		     stmt->ensuresConstraints);
		  DPRINTF((message("returning from exprNode_doGenerateConstraintSwitch: (switch %s) %s currentRequires:"
				    "%s currentEnsures:%s",
				    exprNode_unparse(switchExpr), exprNode_unparse(body),
				    constraintList_unparse(*currentRequires), constraintList_unparse(*currentEnsures)
				   )));
		  /*@-onlytrans@*/
		  return;
		  /*@=onlytrans@*/

    }

  if (exprNode_isCaseMarker(stmt) && exprNode_mustEscape(stmtList))
    {
      /*
      ** merge current and saved constraint with Logical Or...
      ** make a constraint for ensures
      */

      constraintList temp;
      constraint con;

      DPRINTF ((message("Got case marker")));

      if (constraintList_isUndefined(*savedEnsures) &&
	  constraintList_isUndefined(*savedRequires))
	{
	  llassert(constraintList_isUndefined(*savedEnsures));
	  llassert(constraintList_isUndefined(*savedRequires));
	  *savedEnsures  = constraintList_copy(*currentEnsures);
	  *savedRequires = constraintList_copy(*currentRequires);
	}
      else
	{
	  DPRINTF ((message("Doing logical or")));
	  temp = constraintList_logicalOr (*savedEnsures, *currentEnsures);
	  constraintList_free (*savedEnsures);
	  *savedEnsures = temp;
	  
	  *savedRequires = constraintList_mergeRequiresFreeFirst (*savedRequires, *currentRequires);
	}
      
      con = constraint_makeEnsureEqual (switchExpr, exprData_getSingle (stmt->edata), exprNode_loc (stmt));

      constraintList_free (*currentEnsures);
      *currentEnsures = constraintList_makeNew();
      *currentEnsures = constraintList_add(*currentEnsures, con);

      constraintList_free(*currentRequires);
      *currentRequires = constraintList_makeNew();
      DPRINTF (("exprNode_doGenerateConstraintSwitch: (switch %s) %s savedRequires:"
		"%s savedEnsures:%s",
		exprNode_unparse(switchExpr), exprNode_unparse(body),
		constraintList_unparse(*savedRequires), constraintList_unparse(*savedEnsures)
		));
    }
  else if (exprNode_isCaseMarker(stmt)) /* prior case has no break. */
    {
      /* 
	 We don't do anything to the sved constraints because the case hasn't ended
	 The new ensures constraints for the case will be:
	 the constraint for the case statement (CASE_LABEL == SWITCH_EXPR) logicalOr currentEnsures
      */
      
      constraintList temp;
      constraint con;
      constraintList ensuresTemp;

      con = constraint_makeEnsureEqual (switchExpr, exprData_getSingle (stmt->edata), exprNode_loc (stmt));
      
      ensuresTemp = constraintList_makeNew ();
      ensuresTemp = constraintList_add (ensuresTemp, con);

      if (exprNode_isError (stmtList))
	{
	  constraintList_free (*currentEnsures);
	  *currentEnsures = constraintList_copy (ensuresTemp);
	  constraintList_free (ensuresTemp);
	}
      else
	{
	  temp = constraintList_logicalOr (*currentEnsures, ensuresTemp);
	  constraintList_free (*currentEnsures);
	  constraintList_free (ensuresTemp);
	  *currentEnsures = temp;
	}

      constraintList_free (*currentRequires);
      *currentRequires = constraintList_makeNew();
    }
  else
    {
      /*
	we handle the case of ! exprNode_isCaseMarker above
	the else if clause should always be true.
      */
      BADEXIT;
    }

  DPRINTF (("returning from exprNode_doGenerateConstraintSwitch: (switch %s) %s currentRequires:"
	    "%s currentEnsures:%s",
	    exprNode_unparse(switchExpr), exprNode_unparse(body),
	    constraintList_unparse(*currentRequires), constraintList_unparse(*currentEnsures)
	    ));

  /*@-onlytrans@*/ 
  return;
  /*@=onlytrans@*/ 
}


static void exprNode_generateConstraintSwitch (/*@notnull@*/ exprNode switchStmt)
{
  constraintList constraintsRequires;
  constraintList constraintsEnsures;
  constraintList lastRequires;
  constraintList lastEnsures;

  exprNode body;
  exprNode switchExpr;

  switchExpr = exprData_getPairA (switchStmt->edata);
  body = exprData_getPairB (switchStmt->edata);
  
  if (!exprNode_isDefined (body))
    {
      return;
    }

  DPRINTF((message("")));
  
  if (body->kind == XPR_BLOCK)
    body = exprData_getSingle(body->edata);

  
  constraintsRequires = constraintList_undefined;
  constraintsEnsures = constraintList_undefined;

  lastRequires = constraintList_makeNew();
  lastEnsures = constraintList_makeNew();
  

  /*@-mustfree@*/ 
  /* evans 2002-01-01: spurious warnings for these becuase of stack allocated storage */
  exprNode_doGenerateConstraintSwitch (switchExpr, body, &lastRequires, 
				       &lastEnsures, &constraintsRequires, &constraintsEnsures);
  /*@=mustfree@*/

  /*
    merge current and saved constraint with Logical Or...
    make a constraint for ensures
  */

  constraintList_free(switchStmt->requiresConstraints);
  constraintList_free(switchStmt->ensuresConstraints);

  if (constraintList_isDefined(constraintsEnsures) && constraintList_isDefined(constraintsRequires))
    {
      switchStmt->ensuresConstraints = constraintList_logicalOr(constraintsEnsures, lastEnsures);
      switchStmt->requiresConstraints =   constraintList_mergeRequires(constraintsRequires, lastRequires);
      constraintList_free (constraintsRequires);
      constraintList_free (constraintsEnsures);
    }
  else
    {
      switchStmt->ensuresConstraints =    constraintList_copy(lastEnsures);
      switchStmt->requiresConstraints =   constraintList_copy(lastRequires);
    }

  constraintList_free (lastRequires);
  constraintList_free (lastEnsures);

  DPRINTF(((message(" exprNode_generateConstraintSwitch returning requires: %s and ensures %s",
		     constraintList_unparse(switchStmt->requiresConstraints),
		     constraintList_unparse(switchStmt->ensuresConstraints)
		    )
	    )));
}

static exprNode doSwitch (/*@returned@*/ /*@notnull@*/ exprNode e)
{
  exprNode body;
  exprData data;

  data = e->edata;
  DPRINTF ((message ("doSwitch for: switch (%s) %s",
		      exprNode_unparse (exprData_getPairA (data)),
		      exprNode_unparse (exprData_getPairB (data)))));

  body = exprData_getPairB (data);
  exprNode_generateConstraintSwitch (e);
  return e;
}

void exprNode_multiStatement (/*@dependent@*/ exprNode e)
{
  
  bool ret;
  exprData data;
  exprNode e1, e2;
  exprNode p, trueBranch, falseBranch;
  exprNode forPred, forBody;
  exprNode test;

  constraintList temp;

  DPRINTF((message ("exprNode_multistatement Analysising %s %s at", exprNode_unparse(e),
		    fileloc_unparse(exprNode_getfileloc(e)))));
  
  if (exprNode_handleError (e))
    {
      return; 
    }

  data = e->edata;

  ret = TRUE;

  switch (e->kind)
    {
      
    case XPR_FOR:
      forPred = exprData_getPairA (data);
      forBody = exprData_getPairB (data);
      
      /* First generate the constraints */
      exprNode_generateConstraints (forPred);
      exprNode_generateConstraints (forBody);


      doFor (e, forPred, forBody);
     
      break;

    case XPR_FORPRED:
      exprNode_generateConstraints (exprData_getTripleInit (data));
      test = exprData_getTripleTest (data);
      exprNode_exprTraverse (test,FALSE, FALSE, exprNode_loc(e));
      exprNode_generateConstraints (exprData_getTripleInc (data));
    
      if (!exprNode_isError(test))
	{
	  constraintList temp2;
	  temp2 = test->trueEnsuresConstraints;
	  test->trueEnsuresConstraints =  exprNode_traverseTrueEnsuresConstraints(test);
	  constraintList_free(temp2);
	}
      
      exprNode_generateConstraints (exprData_getTripleInc (data));
      break;

    case XPR_WHILE:
      e1 = exprData_getPairA (data);
      e2 = exprData_getPairB (data);
      
       exprNode_exprTraverse (e1,
			      FALSE, FALSE, exprNode_loc(e1));
       
       exprNode_generateConstraints (e2);

       e = doWhile (e, e1, e2);
      
      break; 

    case XPR_IF:
      DPRINTF(("IF:"));
      DPRINTF ((exprNode_unparse(e)));
      e1 = exprData_getPairA (data);
      e2 = exprData_getPairB (data);

      exprNode_exprTraverse (e1, FALSE, FALSE, exprNode_loc(e1));

      exprNode_generateConstraints (e2);
      e = doIf (e, e1, e2);
      break;
     
    case XPR_IFELSE:
      DPRINTF(("Starting IFELSE"));
      p = exprData_getTriplePred (data);

      trueBranch = exprData_getTripleTrue (data);
      falseBranch = exprData_getTripleFalse (data);
      
      exprNode_exprTraverse (p,
			     FALSE, FALSE, exprNode_loc(p));
      exprNode_generateConstraints (trueBranch);
      exprNode_generateConstraints (falseBranch);

      llassert (exprNode_isDefined (p));
      temp = p->ensuresConstraints;
      p->ensuresConstraints = exprNode_traverseEnsuresConstraints (p);
      constraintList_free(temp);

      temp = p->requiresConstraints;
      p->requiresConstraints = exprNode_traverseRequiresConstraints (p);
      constraintList_free(temp);

      temp = p->trueEnsuresConstraints;
      p->trueEnsuresConstraints =  exprNode_traverseTrueEnsuresConstraints(p);
      constraintList_free(temp);



      DPRINTF((message("p->trueEnsuresConstraints before substitue %s", constraintList_unparse(p->trueEnsuresConstraints)  )
	       ));

            /*drl 10/10/2002 this is a bit of a hack but the reason why we do this is so that any function post conditions or similar things get applied correctly to each branch.  e.g. in strlen(s) < 5 we want the trueEnsures to be maxRead(s) < 5*/

      p->trueEnsuresConstraints = constraintList_substituteFreeTarget (p->trueEnsuresConstraints,
								       p->ensuresConstraints);
      
      DPRINTF(( message ("p->trueEnsuresConstraints after substitue %s", constraintList_unparse(p->trueEnsuresConstraints) )
		));
      
      temp = p->falseEnsuresConstraints;
      p->falseEnsuresConstraints =  exprNode_traverseFalseEnsuresConstraints(p);
      constraintList_free(temp);

      /*See comment on trueEnsures*/
      p->falseEnsuresConstraints = constraintList_substituteFreeTarget (p->falseEnsuresConstraints,
								       p->ensuresConstraints);
      
      e = doIfElse (e, p, trueBranch, falseBranch);
      DPRINTF(("Done IFELSE"));
      break;
      
    case XPR_DOWHILE:

      e2 = (exprData_getPairB (data));
      e1 = (exprData_getPairA (data));

      DPRINTF((message ("do { %s } while (%s)", exprNode_unparse(e2), exprNode_unparse(e1))));
      exprNode_generateConstraints (e2);
      exprNode_generateConstraints (e1);
      e = exprNode_copyConstraints (e, e2);
      DPRINTF ((message ("e = %s  ", constraintList_unparse(e->requiresConstraints))));
      
      break;
      
    case XPR_BLOCK:
      {
	exprNode tempExpr;

	tempExpr = exprData_getSingle (data);

	exprNode_generateConstraints (tempExpr);

	if (exprNode_isDefined(tempExpr) )
	  {
	    constraintList_free(e->requiresConstraints);
	    e->requiresConstraints = constraintList_copy (tempExpr->requiresConstraints);
	    constraintList_free(e->ensuresConstraints);
	    e->ensuresConstraints = constraintList_copy (tempExpr->ensuresConstraints);
	  }
	else
	  {
	    llassert(FALSE);
	  }
      }
      break;

    case XPR_SWITCH:
      e = doSwitch (e);
      break;
    case XPR_STMT:
    case XPR_STMTLIST:
      exprNode_stmtList (e);
      return ;
      /*@notreached@*/
      break;
    default:
      ret=FALSE;
    }
  return; 
}

static bool lltok_isBoolean_Op (lltok tok)
{
  /*this should really be a switch statement but
    I don't want to violate the abstraction
    maybe this should go in lltok.c */
  
  if (lltok_isEqOp (tok))
	{
	  return TRUE;
	}
      if (lltok_isAndOp (tok))

	{

	  return TRUE;	  	  
	}
   if (lltok_isOrOp (tok))
	{
	  return TRUE;	  	
	}

   if (lltok_isGt_Op (tok))
     {
       return TRUE;
     }
   if (lltok_isLt_Op (tok))
     {
       return TRUE;
     }

   if (lltok_isLe_Op (tok))
     {
       return TRUE;
     }
   
   if (lltok_isGe_Op (tok))
     {
       return TRUE;
     }
   
   return FALSE;

}


static void exprNode_booleanTraverse (/*@dependent@*/ exprNode e, /*@unused@*/ bool definatelv, /*@unused@*/ bool definaterv,  fileloc sequencePoint)
{
  constraint cons;
  exprNode t1, t2;
  exprData data;
  lltok tok;
  constraintList tempList, temp;

  if (exprNode_isUndefined(e) )
    {
      llassert (exprNode_isDefined(e) );
      return;
    }
  
  data = e->edata;
  
  tok = exprData_getOpTok (data);
  t1 = exprData_getOpA (data);
  t2 = exprData_getOpB (data);

  /* drl 3/2/2003 we know this because of the type of expression*/
  llassert( exprNode_isDefined(t1) &&   exprNode_isDefined(t2) );
  
  
  tempList = constraintList_undefined;
  
  /* arithmetic tests */
  
  if (lltok_isEqOp (tok))
    {
      cons =  constraint_makeEnsureEqual (t1, t2, sequencePoint);
      e->trueEnsuresConstraints = constraintList_add(e->trueEnsuresConstraints, cons);
    }
  
  
  if (lltok_isLt_Op (tok))
    {
      cons =  constraint_makeEnsureLessThan (t1, t2, sequencePoint);
      e->trueEnsuresConstraints = constraintList_add(e->trueEnsuresConstraints, cons);
      cons = constraint_makeEnsureGreaterThanEqual (t1, t2, sequencePoint);
      e->falseEnsuresConstraints = constraintList_add(e->falseEnsuresConstraints, cons);
    }
   
  if (lltok_isGe_Op (tok))
    {
      cons = constraint_makeEnsureGreaterThanEqual (t1, t2, sequencePoint);
      e->trueEnsuresConstraints = constraintList_add(e->trueEnsuresConstraints, cons);
      
      cons =  constraint_makeEnsureLessThan (t1, t2, sequencePoint);
      e->falseEnsuresConstraints = constraintList_add(e->falseEnsuresConstraints, cons);
    }
  
  if (lltok_isGt_Op (tok))
    {
      cons =  constraint_makeEnsureGreaterThan (t1, t2, sequencePoint);
      e->trueEnsuresConstraints = constraintList_add(e->trueEnsuresConstraints, cons);
      cons = constraint_makeEnsureLessThanEqual (t1, t2, sequencePoint);
      e->falseEnsuresConstraints = constraintList_add(e->falseEnsuresConstraints, cons);
    }
  
  if (lltok_isLe_Op (tok))
    {
      cons = constraint_makeEnsureLessThanEqual (t1, t2, sequencePoint);
      e->trueEnsuresConstraints = constraintList_add(e->trueEnsuresConstraints, cons);
      
      cons =  constraint_makeEnsureGreaterThan (t1, t2, sequencePoint);
      e->falseEnsuresConstraints = constraintList_add(e->falseEnsuresConstraints, cons);
    }
  
  /* Logical operations */
  
  if (lltok_isAndOp (tok))
    {
      /* true ensures  */
      tempList = constraintList_copy (t1->trueEnsuresConstraints);
      tempList = constraintList_addList (tempList, t2->trueEnsuresConstraints);
      e->trueEnsuresConstraints = constraintList_addListFree(e->trueEnsuresConstraints, tempList);
      
      /* false ensures: fens t1 or tens t1 and fens t2 */
      tempList = constraintList_copy (t1->trueEnsuresConstraints);
      tempList = constraintList_addList (tempList, t2->falseEnsuresConstraints);
      temp = tempList;
      tempList = constraintList_logicalOr (tempList, t1->falseEnsuresConstraints);
      constraintList_free (temp);
      
      /* evans - was constraintList_addList - memory leak detected by splint */
      e->falseEnsuresConstraints = constraintList_addListFree (e->falseEnsuresConstraints, tempList);
    }
  else if (lltok_isOrOp (tok))
    {
      /* false ensures */
      tempList = constraintList_copy (t1->falseEnsuresConstraints);
      tempList = constraintList_addList (tempList, t2->falseEnsuresConstraints);
      e->falseEnsuresConstraints = constraintList_addListFree(e->falseEnsuresConstraints, tempList);
      
      /* true ensures: tens t1 or fens t1 and tens t2 */
      tempList = constraintList_copy (t1->falseEnsuresConstraints);
      tempList = constraintList_addList (tempList, t2->trueEnsuresConstraints);
      
      temp = tempList;
      tempList = constraintList_logicalOr (tempList, t1->trueEnsuresConstraints);
      constraintList_free(temp);

      e->trueEnsuresConstraints = constraintList_addListFree(e->trueEnsuresConstraints, tempList);
      tempList = constraintList_undefined;
    }
  else
    {
      DPRINTF((message("%s is not a boolean operation", lltok_unparse(tok))));
    } 
}

void exprNode_exprTraverse (/*@dependent@*/ exprNode e, 
			    bool definatelv, bool definaterv,  
			    /*@observer@*/ /*@temp@*/ fileloc sequencePoint)
{
  exprNode t1, t2, fcn;
  lltok tok;
  exprData data;
  constraint cons;
  constraintList temp;

  if (exprNode_isError(e))
    {
      return; 
    }
  
  DPRINTF (("exprNode_exprTraverse analyzing %s at %s", 
	    exprNode_unparse (e),
	    fileloc_unparse (exprNode_loc (e))));
  
  if (exprNode_isUnhandled (e))
    {
      return;
    }
  
  data = e->edata;
  
  switch (e->kind)
    {
    case XPR_WHILEPRED:
      t1 = exprData_getSingle (data);
      exprNode_exprTraverse (t1,  definatelv, definaterv, sequencePoint);
      e = exprNode_copyConstraints (e, t1);
      break;

    case XPR_FETCH:

      if (definatelv)
	{
	  t1 =  (exprData_getPairA (data));
	  t2 =  (exprData_getPairB (data));
	  cons =  constraint_makeWriteSafeExprNode (t1, t2);
	}
      else 
	{
	  t1 =  (exprData_getPairA (data));
	  t2 =  (exprData_getPairB (data));
	  cons = constraint_makeReadSafeExprNode (t1, t2);
	}
      
      e->requiresConstraints = constraintList_add(e->requiresConstraints, cons);
      cons = constraint_makeEnsureMaxReadAtLeast (t1, t2, sequencePoint);
      e->ensuresConstraints = constraintList_add(e->ensuresConstraints, cons);

      cons = constraint_makeEnsureLteMaxRead (t2, t1);
      e->trueEnsuresConstraints = constraintList_add(e->trueEnsuresConstraints, cons);
	
      exprNode_exprTraverse (exprData_getPairA (data), FALSE, TRUE, sequencePoint);
      exprNode_exprTraverse (exprData_getPairB (data), FALSE, TRUE, sequencePoint);
      
      break;
      
    case XPR_PARENS: 
      exprNode_exprTraverse (exprData_getUopNode (e->edata), definatelv, definaterv, sequencePoint);
      break;
    case XPR_INIT:
      {
	t2 = exprData_getInitNode (data);
	
	DPRINTF (("initialization ==> %s",exprNode_unparse (t2)));
	
	exprNode_exprTraverse (t2, definatelv, TRUE, sequencePoint);
	
	/* This test is nessecary because some expressions generate a null expression node. 
	   function pointer do that -- drl */

        if (!exprNode_isError (e) && !exprNode_isError (t2))
	  {
	    cons =  constraint_makeEnsureEqual (e, t2, sequencePoint);
	    e->ensuresConstraints = constraintList_add(e->ensuresConstraints, cons);
	  }
      }
      
      break;
    case XPR_ASSIGN:
      t1 = exprData_getOpA (data);
      t2 = exprData_getOpB (data);
      DPRINTF (("Assignment constraints t1: %s", constraintList_unparse (t1->ensuresConstraints)));
      DPRINTF (("Assignment constraints t2: %s", constraintList_unparse (t2->ensuresConstraints)));
      exprNode_exprTraverse (t1, TRUE, definaterv, sequencePoint); 
      DPRINTF (("Assignment constraints t1: %s", constraintList_unparse (t1->ensuresConstraints)));
      DPRINTF (("Assignment constraints t2: %s", constraintList_unparse (t2->ensuresConstraints)));
      exprNode_exprTraverse (t2, definatelv, TRUE, sequencePoint);
      DPRINTF (("Assignment constraints t1: %s", constraintList_unparse (t1->ensuresConstraints)));
      DPRINTF (("Assignment constraints t2: %s", constraintList_unparse (t2->ensuresConstraints)));

      /* this test is nessecary because some expressions generate a null expression node. 
	 function pointer do that -- drl */
      
      if ((!exprNode_isError (t1)) && (!exprNode_isError(t2)))
	{
	  cons = constraint_makeEnsureEqual (t1, t2, sequencePoint);
	  DPRINTF (("Ensure equal constraint: %s", constraint_unparse (cons)));
	  e->ensuresConstraints = constraintList_add (e->ensuresConstraints, cons);
	  DPRINTF (("Assignment constraints: %s", constraintList_unparse (e->ensuresConstraints)));
	}
      break;
    case XPR_OP:
      t1 = exprData_getOpA (data);
      t2 = exprData_getOpB (data);
      tok = exprData_getOpTok (data);      

      if (lltok_getTok (tok) == ADD_ASSIGN)
	{
	  exprNode_exprTraverse (t1, TRUE, definaterv, sequencePoint);
	  exprNode_exprTraverse (t2, definatelv, TRUE, sequencePoint);

	  cons = constraint_makeAddAssign (t1, t2,  sequencePoint);
	  e->ensuresConstraints = constraintList_add(e->ensuresConstraints, cons);
	}
      else if (lltok_getTok (tok) == SUB_ASSIGN)
	{
	  exprNode_exprTraverse (t1, TRUE, definaterv, sequencePoint);
	  exprNode_exprTraverse (t2, definatelv, TRUE, sequencePoint);

	  cons = constraint_makeSubtractAssign (t1, t2,  sequencePoint);
	  e->ensuresConstraints = constraintList_add(e->ensuresConstraints, cons);
	}
      else
	{
	  exprNode_exprTraverse (t1, definatelv, definaterv, sequencePoint);
	  exprNode_exprTraverse (t2, definatelv, definaterv, sequencePoint);
	}
      
      if (lltok_isBoolean_Op (tok))
	exprNode_booleanTraverse (e, definatelv, definaterv, sequencePoint);

      break;
    case XPR_SIZEOFT:
      /*drl 4-11-03 I think this is the same as the next case...*/
      
      break;
      
    case XPR_SIZEOF:
      /* drl  7-16-01
	 C standard says operand to sizeof isn't evaluated unless
	 its a variable length array.  So we don't generate constraints.
      */
	 
      break;
      
    case XPR_CALL:
      fcn = exprData_getFcn(data);
      
      exprNode_exprTraverse (fcn, definatelv, definaterv, sequencePoint);
      DPRINTF (("Got call that %s (%s)", 
		exprNode_unparse(fcn), exprNodeList_unparse (exprData_getArgs (data))));

      llassert( exprNode_isDefined(fcn) );
      
      fcn->requiresConstraints = 
	constraintList_addListFree (fcn->requiresConstraints,
				    checkCall (fcn, exprData_getArgs (data)));      
      
      fcn->ensuresConstraints = 
	constraintList_addListFree (fcn->ensuresConstraints,
				    exprNode_getPostConditions(fcn, exprData_getArgs (data),e ));
      
      t1 = exprNode_createNew (exprNode_getType (e));
      checkArgumentList (t1, exprData_getArgs(data), sequencePoint);
      exprNode_mergeResolve (e, t1, fcn);
      exprNode_free(t1);
      break;
      
    case XPR_RETURN:
      exprNode_exprTraverse (exprData_getSingle (data), definatelv, definaterv, sequencePoint);
      break;
  
    case XPR_NULLRETURN:
      
      break;
      
      
    case XPR_FACCESS:
      exprNode_exprTraverse (exprData_getFieldNode (data), definatelv, definaterv, sequencePoint);
      break;
   
    case XPR_ARROW:
      exprNode_exprTraverse (exprData_getFieldNode (data), definatelv, definaterv, sequencePoint);
      break;
   
    case XPR_STRINGLITERAL:

      break;
      
    case XPR_NUMLIT:

      break;
      
    case XPR_PREOP: 
      t1 = exprData_getUopNode(data);

      
      /* drl 3/2/2003 we know this because of the type of expression*/
      llassert( exprNode_isDefined(t1) );
  
      
      tok = (exprData_getUopTok (data));
      exprNode_exprTraverse (t1, definatelv, definaterv, sequencePoint);
      /*handle * pointer access */
      if (lltok_isIncOp (tok))
	{
	  DPRINTF(("doing ++(var)"));
	  t1 = exprData_getUopNode (data);
	  cons = constraint_makeMaxSetSideEffectPostIncrement (t1, sequencePoint);
	  e->ensuresConstraints = constraintList_add (e->ensuresConstraints, cons);
	}
      else if (lltok_isDecOp (tok))
	{
	  DPRINTF(("doing --(var)"));
	  t1 = exprData_getUopNode (data);
	  cons = constraint_makeMaxSetSideEffectPostDecrement (t1, sequencePoint);
	  e->ensuresConstraints = constraintList_add (e->ensuresConstraints, cons);
	}
      else if (lltok_isMult(tok ))
	{
	  if (definatelv)
	    {
	      cons = constraint_makeWriteSafeInt (t1, 0);
	    }
	  else
	    {
	      cons = constraint_makeReadSafeInt (t1, 0);
	    }
  	      e->requiresConstraints = constraintList_add(e->requiresConstraints, cons);
	}
      else if (lltok_isNotOp (tok))
	/* ! expr */
	{
	  constraintList_free(e->trueEnsuresConstraints);

	  e->trueEnsuresConstraints  = constraintList_copy (t1->falseEnsuresConstraints);
	  constraintList_free(e->falseEnsuresConstraints);
	  e->falseEnsuresConstraints = constraintList_copy (t1->trueEnsuresConstraints);
	}
      
      else if (lltok_isAmpersand_Op (tok))
	{
	  break;
	}
      else if (lltok_isMinus_Op (tok))
	{
	  break;
	}
      else if (lltok_isExcl_Op (tok))
	{
	  break;
	}
      else if (lltok_isTilde_Op (tok))
	{
	  break;
	}
      else
	{
	  llcontbug (message("Unsupported preop in %s", exprNode_unparse(e)));
	  BADEXIT;
	}
      break;
      
    case XPR_POSTOP:
      exprNode_exprTraverse (exprData_getUopNode (data), TRUE, 
			     definaterv, sequencePoint);
      
      if (lltok_isIncOp (exprData_getUopTok (data)))
	{
	  DPRINTF(("doing ++"));
	  t1 = exprData_getUopNode (data);
	  cons = constraint_makeMaxSetSideEffectPostIncrement (t1, sequencePoint);
	  e->ensuresConstraints = constraintList_add (e->ensuresConstraints, cons);
	}
       if (lltok_isDecOp (exprData_getUopTok (data)))
	{
	  DPRINTF(("doing --"));
	  t1 = exprData_getUopNode (data);
	  cons = constraint_makeMaxSetSideEffectPostDecrement (t1, sequencePoint);
	  e->ensuresConstraints = constraintList_add (e->ensuresConstraints, cons);
	}
      break;
    case XPR_CAST:
      {
	t2 =  exprData_getCastNode (data);
	DPRINTF ((message ("Examining cast (%q)%s", 
			    qtype_unparse (exprData_getCastType (data)),
			    exprNode_unparse (t2))
		  ));
	exprNode_exprTraverse (t2, definatelv, definaterv, sequencePoint);
      }
      break;
      
    case XPR_COND:
      {
	exprNode pred, trueBranch, falseBranch;
	llassert(FALSE);
	pred = exprData_getTriplePred (data);
	trueBranch = exprData_getTripleTrue (data);
	falseBranch = exprData_getTripleFalse (data);
	
	llassert (exprNode_isDefined (pred));
	llassert (exprNode_isDefined (trueBranch));
	llassert (exprNode_isDefined (falseBranch));

	exprNode_exprTraverse (pred, FALSE, TRUE, sequencePoint);
	
	temp = pred->ensuresConstraints;
	pred->ensuresConstraints = exprNode_traverseEnsuresConstraints(pred);
	constraintList_free(temp);
	
	temp = pred->requiresConstraints;
	pred->requiresConstraints = exprNode_traverseRequiresConstraints(pred);
	constraintList_free(temp);
	
	temp = pred->trueEnsuresConstraints;
	pred->trueEnsuresConstraints = exprNode_traverseTrueEnsuresConstraints(pred);
	constraintList_free(temp);
	
	temp = pred->falseEnsuresConstraints;
	pred->falseEnsuresConstraints = exprNode_traverseFalseEnsuresConstraints(pred);
	constraintList_free(temp);
	
	exprNode_exprTraverse (trueBranch, FALSE, TRUE, sequencePoint);
	
	temp = trueBranch->ensuresConstraints;
	trueBranch->ensuresConstraints = exprNode_traverseEnsuresConstraints(trueBranch);
	constraintList_free(temp);
	
	temp = trueBranch->requiresConstraints;
	trueBranch->requiresConstraints = exprNode_traverseRequiresConstraints(trueBranch);
	constraintList_free(temp);
	
	temp = trueBranch->trueEnsuresConstraints;
	trueBranch->trueEnsuresConstraints =  exprNode_traverseTrueEnsuresConstraints(trueBranch);
	constraintList_free(temp);
	
	temp = trueBranch->falseEnsuresConstraints;
	trueBranch->falseEnsuresConstraints = exprNode_traverseFalseEnsuresConstraints(trueBranch);
	constraintList_free(temp);
	
	exprNode_exprTraverse (falseBranch, FALSE, TRUE, sequencePoint);
	
	temp = falseBranch->ensuresConstraints;
	falseBranch->ensuresConstraints = exprNode_traverseEnsuresConstraints(falseBranch);
	constraintList_free(temp);
	
	
	temp = falseBranch->requiresConstraints;
	falseBranch->requiresConstraints = exprNode_traverseRequiresConstraints(falseBranch);
	constraintList_free(temp);
	
	temp = falseBranch->trueEnsuresConstraints;
	falseBranch->trueEnsuresConstraints =  exprNode_traverseTrueEnsuresConstraints(falseBranch);
	constraintList_free(temp);
	
	temp = falseBranch->falseEnsuresConstraints;
	falseBranch->falseEnsuresConstraints = exprNode_traverseFalseEnsuresConstraints(falseBranch);
	constraintList_free(temp);
	
	/* if pred is true e equals true otherwise pred equals false */
	
	cons =  constraint_makeEnsureEqual (e, trueBranch, sequencePoint);
	trueBranch->ensuresConstraints = constraintList_add(trueBranch->ensuresConstraints, cons);
	
	cons =  constraint_makeEnsureEqual (e, trueBranch, sequencePoint);
	falseBranch->ensuresConstraints = constraintList_add(falseBranch->ensuresConstraints, cons);
	
	e = doIfElse (e, pred, trueBranch, falseBranch);
      }
      break;
    case XPR_COMMA:
      llassert(FALSE);
      t1 = exprData_getPairA (data);
      t2 = exprData_getPairB (data);
      /* we essiantially treat this like expr1; expr2
	 of course sequencePoint isn't adjusted so this isn't completely accurate
	 problems...
      */
      exprNode_exprTraverse (t1, FALSE, FALSE, sequencePoint);
      exprNode_exprTraverse (t2, definatelv, definaterv, sequencePoint);
      exprNode_mergeResolve (e, t1, t2);
      break;

    default:
      break;
    }

  e->requiresConstraints = constraintList_preserveOrig (e->requiresConstraints);
  e->ensuresConstraints = constraintList_preserveOrig (e->ensuresConstraints);
  e->requiresConstraints = constraintList_addGeneratingExpr (e->requiresConstraints, e);
  e->ensuresConstraints = constraintList_addGeneratingExpr (e->ensuresConstraints, e);
  e->requiresConstraints = constraintList_removeSurpressed (e->requiresConstraints);
  
  DPRINTF (("ensures constraints for %s are %s", 
	    exprNode_unparse(e), constraintList_unparseDetailed (e->ensuresConstraints)));
  
  DPRINTF (("Requires constraints for %s are %s", exprNode_unparse(e), 
	    constraintList_unparseDetailed(e->ensuresConstraints)));

  DPRINTF (("trueEnsures constraints for %s are %s", exprNode_unparse(e), 
	    constraintList_unparseDetailed(e->trueEnsuresConstraints)));
  
  DPRINTF (("falseEnsures constraints for %s are %s", exprNode_unparse(e), 
	    constraintList_unparseDetailed(e->falseEnsuresConstraints)));
  return;
}


constraintList exprNode_traverseTrueEnsuresConstraints (exprNode e)
{
  exprNode t1;

  bool handledExprNode;
  exprData data;
  constraintList ret;

  if (exprNode_handleError (e))
    {
      ret = constraintList_makeNew();
      return ret;
    }

  ret = constraintList_copy (e->trueEnsuresConstraints);
   
  handledExprNode = TRUE;
   
  data = e->edata;
  
  switch (e->kind)
    {
    case XPR_WHILEPRED:
      t1 = exprData_getSingle (data);
      ret = constraintList_addListFree (ret, exprNode_traverseTrueEnsuresConstraints (t1));
      break;
      
    case XPR_FETCH:
      
      ret = constraintList_addListFree (ret,
				    exprNode_traverseTrueEnsuresConstraints
				    (exprData_getPairA (data)));
        
      ret = constraintList_addListFree (ret,
				    exprNode_traverseTrueEnsuresConstraints
				    (exprData_getPairB (data)));
      break;
    case XPR_PREOP:
          
      ret = constraintList_addListFree (ret,
				    exprNode_traverseTrueEnsuresConstraints
				    (exprData_getUopNode (data)));
      break;
      
    case XPR_PARENS: 
      ret = constraintList_addListFree (ret, exprNode_traverseTrueEnsuresConstraints
				    (exprData_getUopNode (data)));
      break;

    case XPR_INIT:
      ret = constraintList_addListFree (ret,
					exprNode_traverseTrueEnsuresConstraints
					(exprData_getInitNode (data)));
	break;


    case XPR_ASSIGN:
        ret = constraintList_addListFree (ret,
				    exprNode_traverseTrueEnsuresConstraints
				    (exprData_getOpA (data)));
        
       ret = constraintList_addListFree (ret,
				    exprNode_traverseTrueEnsuresConstraints
				    (exprData_getOpB (data)));
       break;
    case XPR_OP:
       ret = constraintList_addListFree (ret,
				    exprNode_traverseTrueEnsuresConstraints
				    (exprData_getOpA (data)));
        
       ret = constraintList_addListFree (ret,
				    exprNode_traverseTrueEnsuresConstraints
				    (exprData_getOpB (data)));
       break;
    case XPR_SIZEOFT:
      break;
      
    case XPR_SIZEOF:
          
       ret = constraintList_addListFree (ret,
					 exprNode_traverseTrueEnsuresConstraints
					 (exprData_getSingle (data)));
       break;
      
    case XPR_CALL:
      ret = constraintList_addListFree (ret,
				     exprNode_traverseTrueEnsuresConstraints
				    (exprData_getFcn (data)));
      break;
      
    case XPR_RETURN:
      ret = constraintList_addListFree (ret,
				    exprNode_traverseTrueEnsuresConstraints
				    (exprData_getSingle (data)));
      break;
  
    case XPR_NULLRETURN:
      break;
            
    case XPR_FACCESS:
      ret = constraintList_addListFree (ret,
					exprNode_traverseTrueEnsuresConstraints
					(exprData_getFieldNode (data)));
      break;
   
    case XPR_ARROW:
      ret = constraintList_addListFree (ret,
					exprNode_traverseTrueEnsuresConstraints
					(exprData_getFieldNode (data)));
      break;
   
    case XPR_STRINGLITERAL:
      break;
      
    case XPR_NUMLIT:
      break;
    case XPR_POSTOP:

           ret = constraintList_addListFree (ret,
				    exprNode_traverseTrueEnsuresConstraints
				    (exprData_getUopNode (data)));
	   break;

    case XPR_CAST:

      ret = constraintList_addListFree (ret,
				    exprNode_traverseTrueEnsuresConstraints
				    (exprData_getCastNode (data)));
      break;

    default:
      break;
    }

  return ret;
}

constraintList exprNode_traverseFalseEnsuresConstraints (exprNode e)
{
  exprNode t1;
  bool handledExprNode;
  exprData data;
  constraintList ret;
  
  if (exprNode_handleError (e))
    {
      ret = constraintList_makeNew();
      return ret;
    }
  
  ret = constraintList_copy (e->falseEnsuresConstraints);
  handledExprNode = TRUE;
  data = e->edata;
  
  switch (e->kind)
    {
   case XPR_WHILEPRED:
      t1 = exprData_getSingle (data);
      ret = constraintList_addListFree (ret,exprNode_traverseFalseEnsuresConstraints (t1));
      break;
      
    case XPR_FETCH:
      
      ret = constraintList_addListFree (ret,
				    exprNode_traverseFalseEnsuresConstraints
				    (exprData_getPairA (data)));
        
      ret = constraintList_addListFree (ret,
				    exprNode_traverseFalseEnsuresConstraints
				    (exprData_getPairB (data)));
      break;
    case XPR_PREOP:
          
      ret = constraintList_addListFree (ret,
				    exprNode_traverseFalseEnsuresConstraints
				    (exprData_getUopNode (data)));
      break;
      
    case XPR_PARENS: 
      ret = constraintList_addListFree (ret, exprNode_traverseFalseEnsuresConstraints
				    (exprData_getUopNode (data)));
      break;
    case XPR_INIT:
        ret = constraintList_addListFree (ret,
				    exprNode_traverseFalseEnsuresConstraints
				    (	exprData_getInitNode (data)));
	break;

    case XPR_ASSIGN:
        ret = constraintList_addListFree (ret,
				    exprNode_traverseFalseEnsuresConstraints
				    (exprData_getOpA (data)));
        
       ret = constraintList_addListFree (ret,
				    exprNode_traverseFalseEnsuresConstraints
				    (exprData_getOpB (data)));
       break;
    case XPR_OP:
       ret = constraintList_addListFree (ret,
				    exprNode_traverseFalseEnsuresConstraints
				    (exprData_getOpA (data)));
        
       ret = constraintList_addListFree (ret,
				    exprNode_traverseFalseEnsuresConstraints
				    (exprData_getOpB (data)));
       break;
    case XPR_SIZEOFT:
      break;
      
    case XPR_SIZEOF:
          
       ret = constraintList_addListFree (ret,
				    exprNode_traverseFalseEnsuresConstraints
				     (exprData_getSingle (data)));
       break;
      
    case XPR_CALL:
      ret = constraintList_addListFree (ret,
				     exprNode_traverseFalseEnsuresConstraints
				    (exprData_getFcn (data)));
      break;
      
    case XPR_RETURN:
      ret = constraintList_addListFree (ret,
				    exprNode_traverseFalseEnsuresConstraints
				    (exprData_getSingle (data)));
      break;
  
    case XPR_NULLRETURN:
      break;
            
    case XPR_FACCESS:
      ret = constraintList_addListFree (ret,
					exprNode_traverseFalseEnsuresConstraints
					(exprData_getFieldNode (data)));
      break;
      
    case XPR_ARROW:
      ret = constraintList_addListFree (ret,
					exprNode_traverseFalseEnsuresConstraints
					(exprData_getFieldNode (data)));
      break;
   
    case XPR_STRINGLITERAL:
      break;
      
    case XPR_NUMLIT:
      break;
    case XPR_POSTOP:

           ret = constraintList_addListFree (ret,
				    exprNode_traverseFalseEnsuresConstraints
				    (exprData_getUopNode (data)));
	   break;
	   
    case XPR_CAST:

      ret = constraintList_addListFree (ret,
				    exprNode_traverseFalseEnsuresConstraints
				    (exprData_getCastNode (data)));
      break;

    default:
      break;
    }

  return ret;
}


/* walk down the tree and get all requires Constraints in each subexpression*/
/*@only@*/ constraintList exprNode_traverseRequiresConstraints (exprNode e)
{
  exprNode t1;

  bool handledExprNode;
  exprData data;
  constraintList ret;

   if (exprNode_handleError (e))
     {
       ret = constraintList_makeNew();
       return ret;
     }

  ret = constraintList_copy (e->requiresConstraints);  
  handledExprNode = TRUE;
  data = e->edata;
  
  switch (e->kind)
    {
   case XPR_WHILEPRED:
      t1 = exprData_getSingle (data);
      ret = constraintList_addListFree (ret, exprNode_traverseRequiresConstraints (t1));
      break;
      
    case XPR_FETCH:
      
      ret = constraintList_addListFree (ret,
				    exprNode_traverseRequiresConstraints
				    (exprData_getPairA (data)));
        
      ret = constraintList_addListFree (ret,
				    exprNode_traverseRequiresConstraints
				    (exprData_getPairB (data)));
      break;
    case XPR_PREOP:
          
      ret = constraintList_addListFree (ret,
				    exprNode_traverseRequiresConstraints
				    (exprData_getUopNode (data)));
      break;
      
    case XPR_PARENS: 
      ret = constraintList_addListFree (ret, exprNode_traverseRequiresConstraints
				    (exprData_getUopNode (data)));
      break;
    case XPR_INIT:
      ret = constraintList_addListFree (ret,
					exprNode_traverseRequiresConstraints
					(exprData_getInitNode (data)));
	break;

    case XPR_ASSIGN:
        ret = constraintList_addListFree (ret,
				    exprNode_traverseRequiresConstraints
				    (exprData_getOpA (data)));
        
       ret = constraintList_addListFree (ret,
				    exprNode_traverseRequiresConstraints
				    (exprData_getOpB (data)));
       break;
    case XPR_OP:
       ret = constraintList_addListFree (ret,
				    exprNode_traverseRequiresConstraints
				    (exprData_getOpA (data)));
        
       ret = constraintList_addListFree (ret,
				    exprNode_traverseRequiresConstraints
				    (exprData_getOpB (data)));
       break;
    case XPR_SIZEOFT:
      break;
      
    case XPR_SIZEOF:
          
       ret = constraintList_addListFree (ret,
				    exprNode_traverseRequiresConstraints
				     (exprData_getSingle (data)));
       break;
      
    case XPR_CALL:
      ret = constraintList_addListFree (ret,
				     exprNode_traverseRequiresConstraints
				    (exprData_getFcn (data)));
      break;
      
    case XPR_RETURN:
      ret = constraintList_addListFree (ret,
				    exprNode_traverseRequiresConstraints
				    (exprData_getSingle (data)));
      break;
  
    case XPR_NULLRETURN:
      break;
            
    case XPR_FACCESS:
      ret = constraintList_addListFree (ret,
					exprNode_traverseRequiresConstraints
					(exprData_getFieldNode (data)));
      break;
      
    case XPR_ARROW:
      ret = constraintList_addListFree (ret,
					exprNode_traverseRequiresConstraints
					(exprData_getFieldNode (data)));
      break;
   
    case XPR_STRINGLITERAL:
      break;
      
    case XPR_NUMLIT:
      break;
    case XPR_POSTOP:

           ret = constraintList_addListFree (ret,
				    exprNode_traverseRequiresConstraints
				    (exprData_getUopNode (data)));
	   break;
	   
    case XPR_CAST:

      ret = constraintList_addListFree (ret,
				    exprNode_traverseRequiresConstraints
				    (exprData_getCastNode (data)));
      break;

    default:
      break;
    }

  return ret;
}


/* walk down the tree and get all Ensures Constraints in each subexpression*/
/*@only@*/ constraintList exprNode_traverseEnsuresConstraints (exprNode e)
{
  exprNode t1;

  bool handledExprNode;
  exprData data;
  constraintList ret;

  if (exprNode_handleError (e))
    {
      ret = constraintList_makeNew();
      return ret;
    }
  
  ret = constraintList_copy (e->ensuresConstraints);   
  handledExprNode = TRUE;
  
  data = e->edata;
  
  DPRINTF ((message ("exprnode_traversEnsuresConstraints call for %s with "
		     "constraintList of %s",
		     exprNode_unparse (e),
		     constraintList_unparse(e->ensuresConstraints)
		     )
	    ));
  
  
  switch (e->kind)
    {
    case XPR_WHILEPRED:
      t1 = exprData_getSingle (data);
      ret = constraintList_addListFree (ret,exprNode_traverseEnsuresConstraints (t1));
      break;
      
    case XPR_FETCH:
      ret = constraintList_addListFree (ret,
					exprNode_traverseEnsuresConstraints
					(exprData_getPairA (data)));
      
      ret = constraintList_addListFree (ret,
					exprNode_traverseEnsuresConstraints
					(exprData_getPairB (data)));
      break;
    case XPR_PREOP:
      ret = constraintList_addListFree (ret,
					exprNode_traverseEnsuresConstraints
					(exprData_getUopNode (data)));
      break;
      
    case XPR_PARENS: 
      ret = constraintList_addListFree (ret, exprNode_traverseEnsuresConstraints
					(exprData_getUopNode (data)));
      break;
      
    case XPR_INIT:
      ret = constraintList_addListFree (ret,
					exprNode_traverseEnsuresConstraints
					(exprData_getInitNode (data)));
      break;
      
      
    case XPR_ASSIGN:
      ret = constraintList_addListFree (ret,
					exprNode_traverseEnsuresConstraints
					(exprData_getOpA (data)));
      
      ret = constraintList_addListFree (ret,
					exprNode_traverseEnsuresConstraints
					(exprData_getOpB (data)));
      break;
    case XPR_OP:
      ret = constraintList_addListFree (ret,
					exprNode_traverseEnsuresConstraints
					(exprData_getOpA (data)));
      
      ret = constraintList_addListFree (ret,
					exprNode_traverseEnsuresConstraints
					(exprData_getOpB (data)));
      break;
    case XPR_SIZEOFT:
      break;
      
    case XPR_SIZEOF:
      ret = constraintList_addListFree (ret,
					exprNode_traverseEnsuresConstraints
					(exprData_getSingle (data)));
      break;
    case XPR_CALL:
      ret = constraintList_addListFree (ret,
					exprNode_traverseEnsuresConstraints
					(exprData_getFcn (data)));
      break;
    case XPR_RETURN:
      ret = constraintList_addListFree (ret,
					exprNode_traverseEnsuresConstraints
					(exprData_getSingle (data)));
      break;
    case XPR_NULLRETURN:
      break;
    case XPR_FACCESS:
      ret = constraintList_addListFree (ret,
					exprNode_traverseEnsuresConstraints
					(exprData_getFieldNode (data)));
      break;
    case XPR_ARROW:
      ret = constraintList_addListFree (ret,
					exprNode_traverseEnsuresConstraints
					(exprData_getFieldNode (data)));
      break;
    case XPR_STRINGLITERAL:
      break;
    case XPR_NUMLIT:
      break;
    case XPR_POSTOP:
      ret = constraintList_addListFree (ret,
					exprNode_traverseEnsuresConstraints
					(exprData_getUopNode (data)));
      break;
    case XPR_CAST:
      ret = constraintList_addListFree (ret,
					exprNode_traverseEnsuresConstraints
					(exprData_getCastNode (data)));
      break;
    default:
      break;
    }
  
  DPRINTF((message ("exprnode_traversEnsuresConstraints call for %s with "
		    "constraintList of  is returning %s",
		    exprNode_unparse (e),
		    constraintList_unparse(ret))));
  
  return ret;
}

/*drl moved out of constraintResolve.c 07-02-001 */
void checkArgumentList (/*@out@*/ exprNode temp, exprNodeList arglist,
			fileloc sequencePoint)
{
  
  llassert(temp != NULL );
  
  temp->requiresConstraints = constraintList_makeNew();
  temp->ensuresConstraints = constraintList_makeNew();
  temp->trueEnsuresConstraints = constraintList_makeNew();
  temp->falseEnsuresConstraints = constraintList_makeNew();
  
  exprNodeList_elements (arglist, el)
    {
      constraintList temp2;

      llassert(exprNode_isDefined(el) );

      exprNode_exprTraverse (el, FALSE, FALSE, sequencePoint);
      temp2 = el->requiresConstraints;
      el->requiresConstraints = exprNode_traverseRequiresConstraints(el);
      constraintList_free(temp2);

      temp2 = el->ensuresConstraints;
      el->ensuresConstraints  = exprNode_traverseEnsuresConstraints(el);
      constraintList_free(temp2);

      temp->requiresConstraints = constraintList_addList(temp->requiresConstraints,
							    el->requiresConstraints);
      
      temp->ensuresConstraints = constraintList_addList(temp->ensuresConstraints,
							   el->ensuresConstraints);
    }
  end_exprNodeList_elements;
  
}

/*drl moved out of constraintResolve.c 07-03-001 */
constraintList exprNode_getPostConditions (exprNode fcn, exprNodeList arglist, exprNode fcnCall)
{
  constraintList postconditions;
  uentry temp;
  DPRINTF((message ("Got call that %s (%s) ",  exprNode_unparse(fcn),   exprNodeList_unparse (arglist))));

  temp = exprNode_getUentry (fcn);

  postconditions = uentry_getFcnPostconditions (temp);

  if (constraintList_isDefined (postconditions))
    {
      postconditions = constraintList_doSRefFixConstraintParam (postconditions, arglist);
      postconditions = constraintList_doFixResult (postconditions, fcnCall);
    }
  else
    {
      postconditions = constraintList_makeNew();
    }
  
  return postconditions;
}

/*
comment this out for now
we'll include it in a production release when its stable...

  void findStructs (exprNodeList arglist)
{

  ctype ct, rt;
  
  DPRINTF((
	   message("doing findStructs: %s", exprNodeList_unparse(arglist))
	  ));


  exprNodeList_elements(arglist, expr)
    {
      ct = exprNode_getType(expr);

      rt =  ctype_realType (ct);
      
      if (ctype_isStruct (rt))
	DPRINTF((message("Found structure %s", exprNode_unparse(expr))
		 ));
      if (hasInvariants(ct))
	{
	  constraintList invars;

	  invars = getInvariants(ct);


	  DPRINTF((message ("findStructs has invariants %s ", constraintList_unparse (invars))
		   ));
	  
	  invars = constraintList_doSRefFixStructConstraint(invars, exprNode_getSref(expr), ct);

	  
	  DPRINTF((message ("findStructs finded invariants to be %s ", constraintList_unparse (invars))
		   ));
	}
    }
  end_exprNodeList_elements;
}

*/

/*drl moved out of constraintResolve.c 07-02-001 */
constraintList checkCall (/*@dependent@*/ exprNode fcn, exprNodeList arglist)
{
  constraintList preconditions;
  uentry temp;
  DPRINTF((message ("Got call that %s (%s) ",  exprNode_unparse(fcn),   exprNodeList_unparse (arglist))));

  temp = exprNode_getUentry (fcn);

  preconditions = uentry_getFcnPreconditions (temp);

  if (constraintList_isDefined(preconditions))
    {
      preconditions = constraintList_togglePost (preconditions);
      preconditions = constraintList_preserveCallInfo(preconditions, fcn);
      preconditions = constraintList_doSRefFixConstraintParam (preconditions, arglist);
    }
  else
    {
      if (constraintList_isUndefined(preconditions))
	preconditions = constraintList_makeNew();
    }
  
  if (context_getFlag (FLG_IMPBOUNDSCONSTRAINTS))
    {

      /*
      uentryList_elements (arglist, el)
	{
	  sRef s;
	  DPRINTF((message("setImplicitfcnConstraints doing: %s", uentry_unparse(el) ) ));
	  
	  s = uentry_getSref(el);
	  if (sRef_isReference (s) )
	    {
	      DPRINTF((message ("%s is a pointer", sRef_unparse(s) ) ));
	    }
	  else
	    {
	      DPRINTF((message ("%s is NOT a pointer", sRef_unparse(s) ) ));
	    }
	  //drl 4/26/01
	  //chagned this from MaxSet(s) == 0 to MaxSet(s) >= 0 
	  c = constraint_makeSRefWriteSafeInt (s, 0);
	  
	  implicitFcnConstraints = constraintList_add(implicitFcnConstraints , c);
	  
	  //drl 10/23/2002 added support for out
	  if (!uentry_isOut(el) )
	    {
	      c = constraint_makeSRefReadSafeInt (s, 0);
	      implicitFcnConstraints = constraintList_add(implicitFcnConstraints , c);
	    }
        
	  
	}
      */
    }
  
  DPRINTF ((message("Done checkCall\n")));
  DPRINTF ((message("Returning list %q ", constraintList_unparseDetailed(preconditions))));

  /*
    drl we're going to comment this out for now
    we'll include it if we're sure it's working
    
    findStructs(arglist);
  */
  
  return preconditions;
}

/*drl added this function 10.29.001
  takes an exprNode of the form const + const
  and sets the value
*/
/*drl
  I'm a bit nervous about modifying the exprNode
  but this is the easy way to do this
  If I have time I'd like to cause the exprNode to get created correctly in the first place */
void exprNode_findValue(exprNode e)
{
  exprData data;

  exprNode t1, t2;
  lltok tok;

  llassert(exprNode_isDefined(e) );
 
  data = e->edata;
  
  if (exprNode_hasValue(e))
    return;

  if (e->kind == XPR_OP)
    {
      t1 = exprData_getOpA (data);
     t2 = exprData_getOpB (data);
     tok = exprData_getOpTok (data);

     exprNode_findValue(t1);
     exprNode_findValue(t2);

     if (!(exprNode_knownIntValue(t1) && (exprNode_knownIntValue(t2))))
       return;
     
     if (lltok_isPlus_Op (tok))
       {
	 long v1, v2;

	 v1 = exprNode_getLongValue(t1);
	 v2 = exprNode_getLongValue(t2);

	 if (multiVal_isDefined(e->val))
	   multiVal_free (e->val);
	 
	 e->val = multiVal_makeInt (v1 + v2);
       }

     if (lltok_isMinus_Op (tok)) 
       {
	 long v1, v2;

	 v1 = exprNode_getLongValue(t1);
	 v2 = exprNode_getLongValue(t2);

	 if (multiVal_isDefined(e->val))                    
	   {
	     multiVal_free (e->val);
	   }
	 
	 e->val = multiVal_makeInt (v1 - v2);
       }

     /*drl I should really do * and / at some point */
     
    }

}

