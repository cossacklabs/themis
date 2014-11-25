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
** constraintExpr.c
*/

/* #define DEBUGPRINT 1 */

# include "splintMacros.nf"
# include "basic.h"
# include "cgrammar.h"
# include "cgrammar_tokens.h"

# include "exprChecks.h"
# include "exprNodeSList.h"


static ctype constraintExpr_getOrigType (constraintExpr p_e);
static bool constraintExpr_hasTypeChange(constraintExpr p_e) /*@*/;

static /*@only@*/ constraintExpr constraintExpr_makeBinaryOpConstraintExprIntLiteral (/*@only@*/constraintExpr p_expr, int p_literal);


/*@only@*/ static constraintExpr
doSRefFixInvarConstraintTerm (/*@only@*/ constraintExpr p_e,
			      sRef p_s, ctype p_ct);

/*@only@*/ static constraintExpr 
doSRefFixConstraintParamTerm (/*@only@*/ constraintExpr p_e, /*@temp@*/ /*@observer@*/ exprNodeList p_arglist) /*@modifies p_e@*/;

static /*@only@*/ constraintExpr 
doFixResultTerm (/*@only@*/ constraintExpr p_e, /*@exposed@*/ exprNode p_fcnCall)
     /*@modifies p_e@*/;

static bool constraintExpr_canGetCType (constraintExpr p_e) /*@*/;

static ctype constraintExpr_getCType (constraintExpr p_e);

static /*@only@*/ constraintExpr constraintExpr_adjustMaxSetForCast (/*@only@*/ constraintExpr p_e,
								     ctype p_tfrom, ctype p_tto,
								     fileloc p_loc);

/*@special@*/ /*@notnull@*/ static constraintExpr constraintExpr_makeBinaryOp (void) 
     /* @allocates result->data @ @sets result->kind @ */ ;

void constraintExpr_free (/*@only@*/ constraintExpr expr)
{
  if (constraintExpr_isDefined(expr) )
    {
      switch (expr->kind)
	{
	case unaryExpr:
	  constraintExprData_freeUnaryExpr(expr->data);
	  break;
	case binaryexpr:
	  constraintExprData_freeBinaryExpr(expr->data);
	  break;
	case term:
	  constraintExprData_freeTerm(expr->data);
	  break;
	default:
	  BADEXIT;
	}

      expr->data = NULL;
      free (expr);
    }
  else
    {
      llcontbug(message("attempted to free null pointer in constraintExpr_free"));
    }
}

bool constraintExpr_isLit (constraintExpr expr)
{
  llassert (expr != NULL);
  
  if (expr->kind == term)
    {
      constraintTerm term = constraintExprData_termGetTerm (expr->data);
      if (constraintTerm_isIntLiteral (term) )
	{
	  return TRUE;
	}

    }
  return FALSE;
}

static bool isZeroBinaryOp (constraintExpr expr)
{
  constraintExpr e2;
  
  llassert (expr != NULL); /* evans 2001-07-18 */

  if (!constraintExpr_isBinaryExpr (expr) )
    {
      return FALSE;
    }

  
  e2 = constraintExprData_binaryExprGetExpr2(expr->data);

  llassert (e2 != NULL); /* evans 2001-07-18 */

  if (constraintExpr_isBinaryExpr (e2) )
    {
      constraintExpr e1;
      constraintExprBinaryOpKind  op;

      op = constraintExprData_binaryExprGetOp (e2->data);

      e1 = constraintExprData_binaryExprGetExpr1(e2->data);

	if (constraintExpr_isLit(e1) )
	  {
	    if (constraintExpr_getValue(e1) == 0 )
	      {
		return TRUE;
	      }
	  }
    }
  return FALSE;
}

/* change expr + (o - expr) to (expr -expr) */

/*@only@*/ /*@notnull@*/ static constraintExpr removeZero (/*@only@*/ /*@returned@*/ constraintExpr expr)
{
  constraintExpr expr1, expr2;
  
  constraintExpr temp;

  constraintExprBinaryOpKind  op;
  
  constraintExprBinaryOpKind  tempOp;

  llassert (expr != NULL); /* evans 2001-07-18 */

  if (!isZeroBinaryOp(expr) )
    return expr;

  
  expr1 = constraintExprData_binaryExprGetExpr1(expr->data);
  expr2 = constraintExprData_binaryExprGetExpr2(expr->data);
  op = constraintExprData_binaryExprGetOp(expr->data);

  llassert( constraintExpr_isBinaryExpr(expr2) );	    

  temp = constraintExprData_binaryExprGetExpr2 (expr2->data);
  temp = constraintExpr_copy (temp);

  tempOp = constraintExprData_binaryExprGetOp (expr2->data);

  if (op == BINARYOP_PLUS)
    op = tempOp;
  else if (op == BINARYOP_MINUS)
    {
      if (tempOp == BINARYOP_PLUS)
	op = BINARYOP_MINUS;
      else if (tempOp == BINARYOP_MINUS)
	op = BINARYOP_PLUS;
      else
	BADEXIT;
    }
  else
    BADEXIT;

  expr->data = constraintExprData_binaryExprSetExpr2(expr->data, temp);
  expr->data = constraintExprData_binaryExprSetOp(expr->data, op);

  return expr;
}


/*@only@*//*@notnull@*/  constraintExpr constraintExpr_propagateConstants (/*@only@*/ constraintExpr expr,
						/*@out@*/ bool * propagate,
						  /*@out@*/ int *literal)
{
  constraintExpr expr1;
  constraintExpr expr2;
  bool propagate1, propagate2;
  int literal1, literal2;
  constraintExprBinaryOpKind  op;
  
  propagate1 = FALSE;
  propagate2 = FALSE;
 
  literal1 = 0;
  literal2 = 0;
  
  *propagate = FALSE;
  *literal = 0;

  
  llassert (expr != NULL);
  
  /* we simplify unaryExpr elsewhere */
  if (expr->kind != binaryexpr)
    return expr;

  op = constraintExprData_binaryExprGetOp (expr->data);

  DPRINTF((message("constraintExpr_propagateConstants: binaryexpr: %s", constraintExpr_unparse(expr) ) ) );

  expr = removeZero(expr);
  
  expr1 = constraintExprData_binaryExprGetExpr1(expr->data);
  expr2 = constraintExprData_binaryExprGetExpr2(expr->data);

  expr1 = constraintExpr_copy(expr1);
  expr2 = constraintExpr_copy(expr2);

  expr1 = constraintExpr_propagateConstants (expr1, &propagate1, &literal1);
  expr2 = constraintExpr_propagateConstants (expr2, &propagate2, &literal2);

  expr1 = removeZero(expr1);
  expr2 = removeZero(expr2);

  
  *propagate = propagate1 || propagate2;

  if (op == BINARYOP_PLUS)
    *literal    = literal1 +  literal2;
  else   if (op == BINARYOP_MINUS)
    *literal    = literal1 -  literal2;
  else
    BADEXIT;
    
  if ( constraintExpr_isLit (expr1) && constraintExpr_isLit (expr2) )
    {
      long t1, t2;
      t1 = constraintExpr_getValue (expr1);
      t2 = constraintExpr_getValue (expr2);
      llassert(*propagate == FALSE);
      *propagate = FALSE;

      constraintExpr_free (expr);
      constraintExpr_free (expr1);
      constraintExpr_free (expr2);

      if (op == BINARYOP_PLUS )
	return (constraintExpr_makeIntLiteral ((t1+t2) ));
      else if (op ==  BINARYOP_MINUS)
	return (constraintExpr_makeIntLiteral ((t1-t2) ));
      else
	BADEXIT;
    }

  
  if (constraintExpr_isLit (expr1) )
    {
      *propagate = TRUE;

      *literal += constraintExpr_getValue (expr1);

      if (op == BINARYOP_PLUS)
	{
	  constraintExpr_free(expr1);
	  constraintExpr_free(expr);
	  return expr2;
	}
      else if (op == BINARYOP_MINUS)
	{
	  
	  constraintExpr temp;

	  /* this is an ugly kludge to deal with not
	     having a unary minus operation...*/

	  temp = constraintExpr_makeIntLiteral (0);
	  temp = constraintExpr_makeSubtractExpr (temp, expr2);
	  
	  constraintExpr_free(expr1);
	  constraintExpr_free(expr);

	  llassert (constraintExpr_isDefined(temp) );
	  return temp;
	}
      else
	{
	  BADBRANCH; /* evans 2001-07-18 */
	}
    }
  
  if (constraintExpr_isLit (expr2) )
    {
      *propagate = TRUE;
          
      if ( op == BINARYOP_PLUS )
	*literal += constraintExpr_getValue (expr2);
      else if (op ==  BINARYOP_MINUS)
	*literal -= constraintExpr_getValue (expr2);
      else
	BADEXIT;


      constraintExpr_free(expr2);
      constraintExpr_free(expr);
      return expr1;
    }
  
  DPRINTF((message("constraintExpr_propagateConstants returning: %s", constraintExpr_unparse(expr) ) ) );

  expr->data = constraintExprData_binaryExprSetExpr1 (expr->data, expr1);
  expr->data = constraintExprData_binaryExprSetExpr2 (expr->data, expr2);

  expr = removeZero(expr);
  return expr;
}

/*@notnull@*/ /*@only@*/ static constraintExpr constraintExpr_combineConstants (/*@only@*/ constraintExpr expr ) /*@modifies expr@*/
{
  bool propagate;
  int literal;

  DPRINTF ((message ("Before combine %s", constraintExpr_unparse(expr) ) ) );
  expr = constraintExpr_propagateConstants (expr, &propagate, &literal);
 

  if (propagate)
    {
      constraintExpr ret;

      if (literal != 0)
	{
	  ret = constraintExpr_makeBinaryOpConstraintExprIntLiteral (expr, literal);
	  expr = ret;
	}
    }
   DPRINTF ((message ("After combine %s", constraintExpr_unparse(expr) ) ) );

   llassert(constraintExpr_isDefined(expr) );
   return expr;
}

/*@special@*/
static /*@notnull@*/ constraintExpr constraintExpr_alloc (void) /*@post:isnull result->data@*/
{
  constraintExpr ret;
  ret = dmalloc (sizeof (*ret) );
  ret->kind = term;
  ret->data = NULL;
  ret->ct = FALSE;
  ret->origType = ctype_undefined; 
  return ret;
}

/*@only@*/ static constraintExprData copyExprData (/*@observer@*/ constraintExprData data, constraintExprKind kind)
{
  constraintExprData ret;
  llassert(constraintExprData_isDefined(data));

  switch (kind)
    {
    case binaryexpr:
      ret = constraintExprData_copyBinaryExpr(data);
      break;
    case unaryExpr:
      ret = constraintExprData_copyUnaryExpr(data);
      break;
    case term:
      ret = constraintExprData_copyTerm(data);
      break;
    default:
      BADEXIT;
    }
  return ret;
}

constraintExpr constraintExpr_copy (constraintExpr expr)
{
  constraintExpr ret;
  ret = constraintExpr_alloc ();


  /*drl 03/02/2003 this shouldn't be used to copy a null
    expression but handle things cleanly if it is*/
  llassert (!constraintExpr_isUndefined(expr) );

  if (constraintExpr_isUndefined(expr) )
    {
      return constraintExpr_undefined;
    }
  
  ret->kind = expr->kind;
  
  ret->data = copyExprData (expr->data, expr->kind);
  ret->ct = expr->ct;
  ret->origType = expr->origType;
  return ret;
}


/*@only@*/ static constraintExpr oldconstraintExpr_makeTermExprNode ( /*@dependent@*/ exprNode e)
{
  constraintExpr ret;
  constraintTerm t;
  ret = constraintExpr_alloc();
  ret->kind = term;
  ret->data = dmalloc (sizeof *(ret->data) );
  t = constraintTerm_makeExprNode (e);
  ret->data = constraintExprData_termSetTerm (ret->data, t);
  ret->ct = FALSE;
  ret->origType = ctype_undefined;

  return ret;
}

/*@access exprNode@*/
constraintExpr constraintExpr_makeExprNode (exprNode e)
{
 sRef s;
 constraintExpr ret, ce1, ce2;
 exprData data;
 exprNode t, t1, t2;
 lltok tok;
 
 if (exprNode_isUndefined (e)) 
   {
     return constraintExpr_undefined;
   }
 
 data = e->edata;

 switch (e->kind)
   {
   case XPR_SIZEOF:
     t = exprData_getSingle (data);
     while (exprNode_isInParens (t) )
       {
	 t = exprData_getUopNode (t->edata);
       }
     s = exprNode_getSref (t);
     if (sRef_isFixedArray(s) )
      {
	int size;

	size = (int) sRef_getArraySize(s);
	ret = constraintExpr_makeIntLiteral (size);
      }
     else if (exprNode_isStringLiteral (t))
      {
	cstring str = multiVal_forceString (exprNode_getValue(t));
	ret = constraintExpr_makeIntLiteral (size_toLong (cstring_length (str) + 1));
      } 
     else
       {
	 DPRINTF ((message ("could not determine the size of %s", exprNode_unparse (e) ) ) );
	 ret = oldconstraintExpr_makeTermExprNode (e);
       }
     break;
     
   case XPR_OP:
      DPRINTF ((message ("Examining operation %s", exprNode_unparse (e) ) ) );
     t1 = exprData_getOpA (data);
     t2 = exprData_getOpB (data);
     tok = exprData_getOpTok (data);
     
     if (lltok_isPlus_Op (tok) || lltok_isMinus_Op (tok) )
       {
	 ce1 = constraintExpr_makeExprNode (t1);
	 ce2 = constraintExpr_makeExprNode (t2);
	 ret = constraintExpr_parseMakeBinaryOp (ce1, tok, ce2);	 
       }


        /* define this block to activate the cheesy heuristic
	for handling sizeof expressions*/
#if 0     

	
     /*
       drl 8-11-001
       
       We handle expressions containing sizeof with the rule
       (sizeof type ) * Expr = Expr

       This is the total wronge way to do this but...
       it may be better than nothing
     */

     
      
     else if (lltok_isMult(tok) )
       {
	 if  ((t1->kind == XPR_SIZEOF) || (t1->kind == XPR_SIZEOFT) )
	   {
	     ret = constraintExpr_makeExprNode(t2);
	   }
	 else if  ((t2->kind == XPR_SIZEOF) || (t2->kind == XPR_SIZEOFT) )
	   {
	     ret = constraintExpr_makeExprNode(t1);
	     } 
	 else
	   {
	   ret =  oldconstraintExpr_makeTermExprNode (e);
	   }
       }
#endif
     
     else
        ret = oldconstraintExpr_makeTermExprNode (e);
   
     break;
   case XPR_PARENS: 
     t = exprData_getUopNode (data);
     ret = constraintExpr_makeExprNode (t);
     break;
     
   case XPR_PREOP:
      t = exprData_getUopNode (data);
      tok =  exprData_getUopTok (data);
      if (lltok_isIncOp (tok))
	{
	  constraintExpr temp;
	  temp = constraintExpr_makeExprNode(t);
	  ret = constraintExpr_makeIncConstraintExpr(temp);
	}
      else if (lltok_isDecOp (tok))
	{
	  constraintExpr temp;
	  temp = constraintExpr_makeExprNode(t);
	  ret = constraintExpr_makeDecConstraintExpr(temp);
	}
      else
	ret =  oldconstraintExpr_makeTermExprNode (e);
      break;
      
   case XPR_POSTOP:
     t = exprData_getUopNode (data);
          ret = constraintExpr_makeExprNode (t);
     break;
   case XPR_CAST:
     t = exprData_getCastNode (data);
     ret = constraintExpr_makeExprNode (t);
     break;
   case XPR_COMMA:
     t = exprData_getPairA (data);
     ret = constraintExpr_makeExprNode(t);
     break;
   default:
     ret = oldconstraintExpr_makeTermExprNode (e);
     
   }
  return ret;
}

/*@noaccess exprNode@*/




/*@only@*/ constraintExpr constraintExpr_makeTermExprNode (/*@exposed@*/ exprNode e)
{
  return  oldconstraintExpr_makeTermExprNode(e);
}

static constraintExpr constraintExpr_makeTerm (/*@only@*/  constraintTerm t)
{
  constraintExpr ret;

  ret = constraintExpr_alloc();
  ret->kind = term;
  ret->data = dmalloc (sizeof *(ret->data) );
  ret->data->term = NULL;
  ret->data = constraintExprData_termSetTerm (ret->data, t);
  ret->ct = FALSE;
  ret->origType = ctype_undefined; 

  return ret;
}

constraintExpr constraintExpr_makeTermsRef (/*@temp@*/ sRef s)
{
  constraintExpr ret;
  constraintTerm t;
  ret = constraintExpr_alloc();
  ret->kind = term;
  ret->data = dmalloc (sizeof *(ret->data) );
  t = constraintTerm_makesRef (s);
  ret->data = constraintExprData_termSetTerm (ret->data, t);

  ret->ct = FALSE;
  ret->origType = ctype_undefined; 

  return ret;
}

/*@special@*/ /*@notnull@*/ static constraintExpr makeUnaryOpGeneric (void) /*@allocates result->data@*/ /*@defines result->kind@*/
{
  constraintExpr ret;
  ret = constraintExpr_alloc();
  ret->kind = unaryExpr;
  ret->data = dmalloc (sizeof *(ret->data));
  ret->data->unaryOp.expr = constraintExpr_undefined;
  return ret;
}

/*@notnull@*/ /*@only@*/ static constraintExpr constraintExpr_makeUnaryOpConstraintExpr (/*@only@*/ constraintExpr cexpr)
{
  constraintExpr ret;
  ret = makeUnaryOpGeneric();

  /*@-uniondef@*/ 
  /*@-compdef@*/
  ret->data = constraintExprData_unaryExprSetExpr (ret->data, cexpr);
  ret->data = constraintExprData_unaryExprSetOp (ret->data, UNARYOP_UNDEFINED);
  
  return ret;
  
  /*@=compdef@*/
  /*@=uniondef@*/
}


/*@only@*/ /*@notnull@*/ static constraintExpr 
constraintExpr_makeUnaryOp (/*@only@*/ constraintExpr cexpr, constraintExprUnaryOpKind Op)
{
  constraintExpr ret;
  ret = makeUnaryOpGeneric();

  ret->data = constraintExprData_unaryExprSetExpr (ret->data, cexpr);
  ret->data = constraintExprData_unaryExprSetOp (ret->data, Op);

  ret->ct = FALSE;
  ret->origType = ctype_undefined; 

  return ret;
}

/*@only@*/ /*@notnull@*/
static constraintExpr constraintExpr_makeUnaryOpExprNode (/*@exposed@*/ exprNode expr)
{
  constraintExpr ret;
  constraintExpr sub;
  sub = constraintExpr_makeExprNode (expr);
  ret = constraintExpr_makeUnaryOpConstraintExpr(sub);

  return ret;
}

/*@only@*/ /*@notnull@*/
static constraintExpr constraintExpr_makeMaxSetConstraintExpr (/*@only@*/ constraintExpr c)
{
  constraintExpr ret;
  ret = constraintExpr_makeUnaryOp (c, MAXSET);
  return ret;
}


/*@only@*/ /*@notnull@*/
static constraintExpr constraintExpr_makeSRefUnaryOp (/*@temp@*/ /*@observer@*/ sRef s,  constraintExprUnaryOpKind op)
{
  constraintExpr ret;
  constraintExpr t;

  t = constraintExpr_makeTermsRef (s);
  ret = constraintExpr_makeUnaryOpConstraintExpr (t);
  ret->data = constraintExprData_unaryExprSetOp (ret->data, op);

  return ret;
}

/*@only@*/
constraintExpr constraintExpr_makeSRefMaxRead( sRef s)
{
  return (constraintExpr_makeSRefUnaryOp (s, MAXREAD) );
}     

/*@only@*/
constraintExpr constraintExpr_makeSRefMaxset ( sRef s)
{
  return (constraintExpr_makeSRefUnaryOp (s, MAXSET) );
}

/*@only@*/
constraintExpr constraintExpr_parseMakeUnaryOp (lltok op, constraintExpr cexpr)
{
  constraintExpr ret;
  ret = constraintExpr_makeUnaryOpConstraintExpr ( cexpr);

  switch (lltok_getTok (op))
    {
    case QMAXSET:
      ret->data = constraintExprData_unaryExprSetOp (ret->data, MAXSET);
      break;
    case QMAXREAD:
      ret->data = constraintExprData_unaryExprSetOp (ret->data, MAXREAD);
      break;
    default:
      llfatalbug (message ("Unhandled operation in constraint: %s", lltok_unparse (op)));
    }
  return ret;
}

/*@only@*/
constraintExpr constraintExpr_makeMaxSetExpr (/*@exposed@*/ exprNode expr)
{
  constraintExpr ret;
  ret = constraintExpr_makeExprNode (expr);

  ret = constraintExpr_makeMaxSetConstraintExpr (ret);

  llassert (ret != NULL);
  return ret;
}

/*@only@*/
constraintExpr  constraintExpr_makeMaxReadExpr (exprNode expr)
{
  constraintExpr ret;
  ret = constraintExpr_makeUnaryOpExprNode(expr);
  ret->data      = constraintExprData_unaryExprSetOp (ret->data, MAXREAD);
  return ret; 
}

# if 0
/*@only@*/
/*@unused@*/ static constraintExpr  constraintExpr_makeMinSetExpr (/*@exposed@*/ exprNode expr)
{
  constraintExpr ret;
  ret = constraintExpr_makeUnaryOpExprNode(expr);
  ret->data      = constraintExprData_unaryExprSetOp (ret->data, MINSET);
  return ret;
}

/*@only@*/
/*@unused@*/ static constraintExpr constraintExpr_makeMinReadExpr (/*@exposed@*/ exprNode expr)
{
  constraintExpr ret;
  ret = constraintExpr_makeUnaryOpExprNode(expr);
  ret->data      = constraintExprData_unaryExprSetOp (ret->data, MINREAD);
  return ret;
}
# endif

/*@only@*/
constraintExpr constraintExpr_makeValueExpr (/*@exposed@*/ exprNode expr)
{
  constraintExpr ret;
  ret = constraintExpr_makeExprNode (expr);
  return ret;
}

/*@only@*/ /*@notnull@*/
constraintExpr constraintExpr_makeIntLiteral (long i)
{
  constraintExpr ret;
  constraintTerm t;
  ret = constraintExpr_alloc();
  ret->kind = term;
  ret->data = dmalloc (sizeof *(ret->data) );
  t = constraintTerm_makeIntLiteral (i);
  ret->data = constraintExprData_termSetTerm (ret->data, t);

  ret->ct = FALSE;
  ret->origType = ctype_undefined; 

  return ret;
}

/*
constraintExpr constraintExpr_makeValueInt (int i)
{
  return constraintExpr_makeIntLiteral (i);
}
*/

/*@only@*/ /*@notnull@*/
 /*@special@*/ static constraintExpr constraintExpr_makeBinaryOp (void)
      /*@allocates result->data @*/ /*@sets result->kind @*/
{
  constraintExpr ret;
  ret = constraintExpr_alloc();
  ret->kind = binaryexpr;
  ret->data = dmalloc ( sizeof *(ret->data) );

  ret->data->binaryOp.expr1 = constraintExpr_undefined;
  ret->data->binaryOp.expr2 = constraintExpr_undefined;
  
  return ret;
}


static /*@notnull@*/ /*@only@*/ constraintExpr constraintExpr_makeBinaryOpConstraintExpr (/*@only@*/constraintExpr expr1, /*@only@*/ constraintExpr expr2)
     
{
  constraintExpr ret;

  ret = constraintExpr_makeBinaryOp();
  ret->data = constraintExprData_binaryExprSetExpr1 (ret->data, expr1);
  ret->data = constraintExprData_binaryExprSetExpr2 (ret->data, expr2);
  ret->data = constraintExprData_binaryExprSetOp (ret->data, BINARYOP_UNDEFINED);

  ret->ct = FALSE;
  ret->origType = ctype_undefined; 

  return ret;
}

/*@only@*/
constraintExpr constraintExpr_parseMakeBinaryOp (/*@only@*/ constraintExpr expr1, lltok op,/*@only@*/ constraintExpr expr2)
{
  constraintExpr ret;
  ret = constraintExpr_makeBinaryOpConstraintExpr (expr1, expr2);

  if (lltok_getTok (op) == TPLUS)
    {
      ret->data = constraintExprData_binaryExprSetOp(ret->data, BINARYOP_PLUS);
    }
  else if (lltok_getTok (op) == TMINUS)
    {
      ret->data = constraintExprData_binaryExprSetOp(ret->data, BINARYOP_MINUS);
    }
  else
    {
      llassert (FALSE);
    }

  return ret;
}

# if 0
/*@only@*/
/*@unused@*/ static constraintExpr constraintExpr_makeBinaryOpExprNode (/*@exposed@*/ exprNode expr1, /*@exposed@*/ exprNode expr2)
{
  constraintExpr ret;
  constraintExpr sub1, sub2;
  sub1 = constraintExpr_makeTermExprNode (expr1);
  sub2 = constraintExpr_makeTermExprNode (expr2);
  ret = constraintExpr_makeBinaryOpConstraintExpr(sub1, sub2);
  return ret;
}
# endif

static /*@notnull@*/ /*@only@*/
constraintExpr constraintExpr_makeBinaryOpConstraintExprIntLiteral (/*@only@*/ constraintExpr expr, int literal)
{
  constraintExpr ret;
  constraintExpr constExpr;

  constExpr = constraintExpr_makeIntLiteral (literal);
  ret = constraintExpr_makeBinaryOpConstraintExpr (expr, constExpr);
  ret->data = constraintExprData_binaryExprSetOp(ret->data, BINARYOP_PLUS);
  return ret;
}

/*@only@*/
constraintExpr constraintExpr_makeDecConstraintExpr (/*@only@*/constraintExpr expr)
{
  constraintExpr ret;
  constraintExpr inc;

  inc = constraintExpr_makeIntLiteral (1);
  ret = constraintExpr_makeBinaryOpConstraintExpr (expr, inc);
  ret->data = constraintExprData_binaryExprSetOp(ret->data, BINARYOP_MINUS);
  return ret;
}


/*@only@*/  constraintExpr constraintExpr_makeSubtractExpr (/*@only@*/ constraintExpr expr, /*@only@*/ constraintExpr addent)
{
  constraintExpr  ret;
  
  DPRINTF ((message ("Making  subtract expression") ) );

  ret = constraintExpr_makeBinaryOpConstraintExpr (expr, addent);
  ret->data = constraintExprData_binaryExprSetOp (ret->data, BINARYOP_MINUS);
  return ret;
}

/*@only@*/
constraintExpr constraintExpr_makeAddExpr (/*@only@*/
constraintExpr expr, /*@only@*/
constraintExpr addent)
{
  constraintExpr  ret;
  
  DPRINTF ((message ("Doing addTerm simplification") ) );

  ret = constraintExpr_makeBinaryOpConstraintExpr (expr, addent);
  ret->data = constraintExprData_binaryExprSetOp (ret->data, BINARYOP_PLUS);
  return ret;
}


/*@only@*/
constraintExpr constraintExpr_makeIncConstraintExpr (/*@only@*/ constraintExpr expr)
{
  constraintExpr ret;
  constraintExpr inc;

  inc = constraintExpr_makeIntLiteral (1);
  ret = constraintExpr_makeBinaryOpConstraintExpr (expr, inc);
  ret->data = constraintExprData_binaryExprSetOp(ret->data, BINARYOP_PLUS);
  return ret;
}

/*@only@*/
static cstring constraintExprUnaryOpKind_print (constraintExprUnaryOpKind op)
{
  switch (op)
    {
    case MAXSET:
      return message("maxSet");
    case MINSET:
      return message("minSet");
    case MAXREAD:
      return message("maxRead");
    case MINREAD:
      return message("minRead");
    default:
      llassert(FALSE);
      return message ("<(Unary OP OTHER>");
    }
}


/*@only@*/
static cstring constraintExprBinaryOpKind_print (constraintExprBinaryOpKind op)
{
  
  switch (op)
    {
    case BINARYOP_PLUS:
      return message("+");
    case BINARYOP_MINUS:
      return message("-");

    default:
      llassert(FALSE);
      return message ("<binary OP Unknown>");
    }
}

bool constraintExpr_similar (constraintExpr expr1, constraintExpr expr2)
{
  constraintExprKind kind;
  
  llassert (expr1 != NULL);
  llassert (expr2 != NULL);
  if (expr1->kind != expr2->kind)
    return FALSE;
  
  kind = expr1->kind;
  
  switch (kind)
    {
    case term:
      return constraintTerm_similar (constraintExprData_termGetTerm(expr1->data),
				  constraintExprData_termGetTerm(expr2->data) );
      /*@notreached@*/ break;
      
    case unaryExpr:
      if (constraintExprData_unaryExprGetOp (expr1->data) != constraintExprData_unaryExprGetOp (expr2->data) )
	return FALSE;
      
      return (constraintExpr_similar (
	      constraintExprData_unaryExprGetExpr (expr1->data),
	      constraintExprData_unaryExprGetExpr (expr2->data)
	      ));
      
    case binaryexpr:
      if (constraintExprData_binaryExprGetOp (expr1->data) != constraintExprData_binaryExprGetOp (expr2->data) )
	return FALSE;
      
      if (! constraintExpr_similar (constraintExprData_binaryExprGetExpr1 (expr1->data),
				 constraintExprData_binaryExprGetExpr1 (expr2->data)) )
	return FALSE;
      
      if (! constraintExpr_similar (constraintExprData_binaryExprGetExpr2 (expr1->data),
				 constraintExprData_binaryExprGetExpr2 (expr2->data)) )
	return FALSE;
      else
	return TRUE;
      /*@notreached@*/
      break;
      
    default:
      llassert(FALSE);
      return FALSE;
    }
  /*@notreached@*/
  return FALSE;
}

bool constraintExpr_same (constraintExpr expr1, constraintExpr expr2)
{
  constraintExprKind kind;
  
  llassert (expr1 != NULL);
  llassert (expr2 != NULL);
  if (expr1->kind != expr2->kind)
    return FALSE;
  
  kind = expr1->kind;
  
  switch (kind)
    {
    case term:
      return constraintTerm_similar (constraintExprData_termGetTerm(expr1->data),
				  constraintExprData_termGetTerm(expr2->data) );
      /*@notreached@*/ break;
      
    case unaryExpr:
      if (constraintExprData_unaryExprGetOp (expr1->data) != constraintExprData_unaryExprGetOp (expr2->data) )
	return FALSE;

      return (constraintExpr_same (
	      constraintExprData_unaryExprGetExpr (expr1->data),
	      constraintExprData_unaryExprGetExpr (expr2->data)
	      ));
      
            
    case binaryexpr:
      if (constraintExprData_binaryExprGetOp (expr1->data) != constraintExprData_binaryExprGetOp (expr2->data) )
	return FALSE;
      
      if (! constraintExpr_same (constraintExprData_binaryExprGetExpr1 (expr1->data),
				 constraintExprData_binaryExprGetExpr1 (expr2->data)) )
	return FALSE;
      
      if (! constraintExpr_same (constraintExprData_binaryExprGetExpr2 (expr1->data),
				 constraintExprData_binaryExprGetExpr2 (expr2->data)) )
	return FALSE;
      else
	return TRUE;
      /*@notreached@*/ break;
      
    default:
      llassert(FALSE);
      return FALSE;
    }

  /*@notreached@*/
  BADEXIT;
}

bool 
constraintExpr_search (/*@observer@*/ constraintExpr c, 
		       /*@observer@*/ constraintExpr old)
{
  bool ret = FALSE;
  constraintExprKind kind;
  constraintExpr temp;
  
  if (constraintExpr_similar (c, old))
    {
      DPRINTF (("Found  %q", constraintExpr_unparse (old)));
      return TRUE;
    }

  llassert (constraintExpr_isDefined (c) && constraintExpr_isDefined(old) );

  if ( !(constraintExpr_isDefined (c) && constraintExpr_isDefined(old) ) )
    return FALSE;

  kind = c->kind;
  
  switch (kind)
    {
    case term:
      break;      
    case unaryExpr:
      temp = constraintExprData_unaryExprGetExpr (c->data);
      ret = ret || constraintExpr_search (temp, old);
      break;           
    case binaryexpr:
      
      temp = constraintExprData_binaryExprGetExpr1 (c->data);
      ret = ret || constraintExpr_search(temp, old);
           
      temp = constraintExprData_binaryExprGetExpr2 (c->data);
      ret = ret || constraintExpr_search(temp, old);
      break;
    default:
      llassert(FALSE);
    }
  return ret;
  
}


/*@only@*/ constraintExpr 
constraintExpr_searchandreplace (/*@only@*/ /*@unique@*/ constraintExpr c, /*@temp@*/ constraintExpr old, 
				 /*@temp@*/ constraintExpr newExpr )
{
  constraintExprKind kind;
  constraintExpr temp;
  constraintExpr ret;

  llassert (constraintExpr_isDefined (newExpr) && (constraintExpr_isDefined (old) && constraintExpr_isDefined(c) ) );
  
  if (constraintExpr_similar (c, old))
    {
      ctype newType = ctype_unknown;
      ctype cType = ctype_unknown;
      
      ret = constraintExpr_copy (newExpr);
      llassert(constraintExpr_isDefined(ret) );
      /*drl if newExpr != NULL then ret will != NULL*/
      
      DPRINTF (("Replacing %s with %s in %s", 
		constraintExpr_unparse (old), constraintExpr_unparse (newExpr),
		constraintExpr_unparse (c)));

      if (constraintExpr_canGetCType (c) && constraintExpr_canGetCType (newExpr))
	{
	  cType = constraintExpr_getCType(c);
	  newType =  constraintExpr_getCType (newExpr);
	  
	  if (ctype_match (cType,newType))
	    {
	      DPRINTF (("constraintExpr_searchandreplace: replacing "
			" %s with type %s with %s with type %s",
			constraintExpr_unparse (c), ctype_unparse(cType),
			constraintExpr_unparse (newExpr), ctype_unparse(newType)));
	      
	      ret->ct = TRUE;
	      ret->origType = cType;
	      DPRINTF (("Type: %s", ctype_unparse (constraintExpr_getCType (ret))));
	    }
	}
      
      if (constraintExpr_hasMaxSet (c))
	{
	  if (constraintExpr_hasTypeChange (c))
	    {
	      fileloc loc = constraintExpr_loc (c);
	      DPRINTF (("constraintExpr_searchandreplace: encountered "
			"MaxSet with changed type %s ",
			constraintExpr_unparse (c)));
	      
	      if (c->kind == unaryExpr) 
		{
		  constraintExpr ce = constraintExprData_unaryExprGetExpr (c->data);
		  DPRINTF (("Its a unary! %s / %s",
			    ctype_unparse (constraintExpr_getCType (ce)),
			    ctype_unparse (constraintExpr_getOrigType (ce))));
		  ret = constraintExpr_adjustMaxSetForCast (ret, constraintExpr_getCType (ce),
							    constraintExpr_getOrigType (ce),
							    loc);
		}
	      else
		{
		  /* fix this with a conversation */
		  DPRINTF (("Types: %s / %s", ctype_unparse (newType), ctype_unparse (cType)));
		  ret = constraintExpr_adjustMaxSetForCast (ret, constraintExpr_getCType (c), 
							    constraintExpr_getOrigType(c),
							    loc);
		}
	    }
	}

      constraintExpr_free (c);
      DPRINTF (("ret: %s", constraintExpr_unparse (ret)));
      return ret;
    }

  kind = c->kind;
  
  switch (kind)
    {
    case term:
      break;      
    case unaryExpr:
      DPRINTF (("Making unary expression!"));
      temp = constraintExprData_unaryExprGetExpr (c->data);
      temp = constraintExpr_copy (temp);
      temp = constraintExpr_searchandreplace (temp, old, newExpr);
      c->data = constraintExprData_unaryExprSetExpr (c->data, temp);
      break;           
    case binaryexpr:
      DPRINTF (("Making binary expression!"));
      temp = constraintExprData_binaryExprGetExpr1 (c->data);
      temp = constraintExpr_copy (temp);
      temp = constraintExpr_searchandreplace (temp, old, newExpr);
      c->data = constraintExprData_binaryExprSetExpr1 (c->data, temp);
       
      temp = constraintExprData_binaryExprGetExpr2 (c->data);
      temp = constraintExpr_copy (temp);
      temp = constraintExpr_searchandreplace (temp, old, newExpr);
      c->data = constraintExprData_binaryExprSetExpr2 (c->data, temp);
      break;
    default:
      llassert (FALSE);
    }
  
  DPRINTF (("ret: %s", constraintExpr_unparse (c)));
  return c;
}

/*@notnull@*/ static constraintExpr constraintExpr_simplifyChildren (/*@returned@*/ /*@notnull@*/ constraintExpr c)
{
  constraintExprKind kind;
  constraintExpr temp;

  kind = c->kind;
  
  switch (kind)
    {
    case term:
      break;      
    case unaryExpr:
      temp = constraintExprData_unaryExprGetExpr (c->data);
      temp = constraintExpr_copy(temp);
      temp = constraintExpr_simplify (temp);
      c->data = constraintExprData_unaryExprSetExpr (c->data, temp);
      break;           
    case binaryexpr:
      DPRINTF((message("constraintExpr_simplfiyChildren: simplify binary expression: %s",constraintExpr_unparse(c) ) ) );
      temp = constraintExprData_binaryExprGetExpr1 (c->data);
      temp = constraintExpr_copy(temp);
      temp = constraintExpr_simplify (temp);

      c->data = constraintExprData_binaryExprSetExpr1 (c->data, temp);
       
      temp = constraintExprData_binaryExprGetExpr2 (c->data);
      temp = constraintExpr_copy(temp);
      temp = constraintExpr_simplify (temp);

      c->data = constraintExprData_binaryExprSetExpr2 (c->data, temp);
      break;
    default:
      llassert(FALSE);
    }
  return c;
  
}


constraintExpr constraintExpr_setFileloc (/*@returned@*/ constraintExpr c, fileloc loc) /*@modifies c @*/
{
  constraintTerm t;
  constraintExpr temp;

  llassert(c != NULL);
  
  switch (c->kind)
    {
    case term:
      t = constraintExprData_termGetTerm (c->data);
      t = constraintTerm_copy(t);
      t = constraintTerm_setFileloc (t, loc);
      c->data = constraintExprData_termSetTerm (c->data, t);
      break;
    case binaryexpr:
      
      temp = constraintExprData_binaryExprGetExpr1 (c->data);
      temp = constraintExpr_copy(temp);
      temp = constraintExpr_setFileloc (temp, loc);
      c->data = constraintExprData_binaryExprSetExpr1 (c->data, temp);
      
      temp = constraintExprData_binaryExprGetExpr2 (c->data);
      temp = constraintExpr_copy(temp);
      temp = constraintExpr_setFileloc (temp, loc);
      c->data = constraintExprData_binaryExprSetExpr2 (c->data, temp);
      break;
    case unaryExpr:
      temp = constraintExprData_unaryExprGetExpr (c->data);
      temp = constraintExpr_copy(temp);
      temp = constraintExpr_setFileloc (temp, loc);
      c->data = constraintExprData_unaryExprSetExpr (c->data, temp);
      break;
    }
  return c;
}

static /*@only@*/ constraintExpr constraintExpr_simplifybinaryExpr (/*@only@*/ /*@notnull@*/ constraintExpr c)
{
  constraintExpr e1, e2;
  constraintExprBinaryOpKind  op;
  
  e1 = constraintExprData_binaryExprGetExpr1 (c->data);
  e2 = constraintExprData_binaryExprGetExpr2 (c->data);

  if (constraintExpr_canGetValue (e1) && constraintExpr_canGetValue(e2) )
    {
      long i;

      i = constraintExpr_getValue(e1) + constraintExpr_getValue (e2);
      constraintExpr_free(c);
      c = constraintExpr_makeIntLiteral (i);
    }
  else
    {
      op = constraintExprData_binaryExprGetOp (c->data);      
      if (op == BINARYOP_MINUS)
	if (constraintExpr_similar(e1, e2) )
	  {
	    constraintExpr_free(c);
	    c =  constraintExpr_makeIntLiteral (0);
	  }
    }
  
  return c;
}

/*
  this thing takes the lexpr and expr of a constraint and modifies lexpr
  and returns a (possiblly new) value for expr
*/
/* if lexpr is a binary express say x + y, we set lexpr to x and return a value for expr such as expr_old - y */

/* the approach is a little Kludgy but seems to work.  I should probably use something cleaner at some point ... */


/*@only@*/ constraintExpr constraintExpr_solveBinaryExpr (constraintExpr lexpr, /*@only@*/ constraintExpr expr)
{
  constraintExpr expr1, expr2;
  constraintExprBinaryOpKind op;

  llassert(constraintExpr_isDefined (lexpr)  && constraintExpr_isDefined (expr)  );
  
  if (lexpr->kind != binaryexpr)
    return expr;

  expr2 = constraintExprData_binaryExprGetExpr2 (lexpr->data);
  expr1 = constraintExprData_binaryExprGetExpr1 (lexpr->data);

  op    = constraintExprData_binaryExprGetOp (lexpr->data);

  expr1 = constraintExpr_copy(expr1);
  expr2 = constraintExpr_copy(expr2);
  
  llassert(constraintExpr_isDefined (expr1)  && constraintExpr_isDefined (expr2)  );
  
  /* drl possible problem : warning make sure this works */
  
  lexpr->kind = expr1->kind;
  sfree (lexpr->data);
  
  lexpr->data = copyExprData (expr1->data, expr1->kind);
  constraintExpr_free(expr1);
  
  if (op == BINARYOP_PLUS)
    expr = constraintExpr_makeSubtractExpr (expr, expr2);
  else if (op == BINARYOP_MINUS)
    expr = constraintExpr_makeAddExpr (expr, expr2);
  else
    BADEXIT;
  
  
  return expr;

  /*
    #warning this needs to be checked
    expr = constraintExpr_solveBinaryExpr (expr1, expr);
    
    expr = constraintExpr_solveBinaryExpr (expr2, expr);
    return expr;
  */
}

static /*@only@*/ constraintExpr constraintExpr_simplifyunaryExpr (/*@only@*/ constraintExpr c)
{
  constraintExpr exp;

  llassert(constraintExpr_isDefined (c) );
  llassert (c->kind == unaryExpr);

  DPRINTF ((message ("Doing constraintExpr_simplifyunaryExpr:%s", constraintExpr_unparse (c) ) ) );
  
  if ((constraintExprData_unaryExprGetOp (c->data) != MAXSET) &&
       (constraintExprData_unaryExprGetOp (c->data) != MAXREAD) )
    {
      return c;
    }
  
  exp = constraintExprData_unaryExprGetExpr (c->data);
  exp = constraintExpr_copy(exp);
  
  llassert(constraintExpr_isDefined (exp)  );
  
  if (exp->kind == term)
    {
      constraintTerm cterm;

      cterm = constraintExprData_termGetTerm (exp->data);
      
      if (constraintTerm_isStringLiteral(cterm) )
	{
	  cstring val;
	  val = constraintTerm_getStringLiteral (cterm);
	  if (constraintExprData_unaryExprGetOp (c->data) == MAXSET)
	    {
	      constraintExpr temp;

	      temp = constraintExpr_makeIntLiteral ((int)strlen (cstring_toCharsSafe(val) ) );
	      cstring_free(val);	      
	      constraintExpr_free(c);
	      constraintExpr_free(exp);

	      return temp;
	      
	    }
	  if (constraintExprData_unaryExprGetOp (c->data) == MAXREAD)
	    {
	      constraintExpr temp;

	      temp = constraintExpr_makeIntLiteral ((int)strlen (cstring_toCharsSafe(val) ) );
	      cstring_free(val);	      
	      constraintExpr_free(c);
	      constraintExpr_free(exp);

	      return temp;
	    }
	  BADEXIT;
	}

      /* slight Kludge to handle var [] = { , , };
      ** type syntax  I don't think this is sound but it should be good
      ** enough.  The C standard is very confusing about initialization
      ** -- DRL 7/25/01
      */
      
      if (constraintTerm_isInitBlock(cterm) )
	{
	  constraintExpr temp;
	  int len;

	  len = constraintTerm_getInitBlockLength(cterm);

	  /* -- drl 12/08/2003 : decrementing to fix off by one error */

	  len--;

	  temp = constraintExpr_makeIntLiteral (len );
	  
	  constraintExpr_free(c);
	  DPRINTF(( message("Changed to %q", constraintExpr_print(temp)
			    ) ));
	  constraintExpr_free(exp);
	  return temp;
	}
      
      constraintExpr_free(exp);
      return c;
    }
  
  if (exp->kind != binaryexpr)
    {
      constraintExpr_free(exp);
      return c;
    }
  
  if (constraintExprData_binaryExprGetOp (exp->data) == BINARYOP_PLUS  )
    {
 
      /* if (constraintExpr_canGetValue (constraintExprData_binaryExprGetExpr2 (exp->data) ) ) */
	{
	
	  constraintExpr  temp, temp2;

	  DPRINTF ((message ("Doing fancy simplification") ) );

	  temp = constraintExprData_binaryExprGetExpr2 (exp->data);

	  temp2 = constraintExprData_binaryExprGetExpr1 (exp->data);

	  temp2 = constraintExpr_copy(temp2);
	  c->data = constraintExprData_unaryExprSetExpr (c->data, temp2);
	  
	  
	  temp = constraintExpr_copy (temp);

	  c = constraintExpr_makeSubtractExpr (c, temp);

	  DPRINTF ((message ("Done fancy simplification:%s", constraintExpr_unparse (c) ) ) );
	}
    }
  
  DPRINTF ((message ("constraintExpr_simplifyUnaryExpr: Done simplification:%s", constraintExpr_unparse (c) ) ) );

  constraintExpr_free(exp);
  return c;
}


/*@only@*/ constraintExpr constraintExpr_simplify (/*@only@*/ constraintExpr c)
{
  constraintExprKind kind;
  constraintExpr ret;
  constraintTerm t;
  
  DPRINTF ((message ("Doing constraintExpr_simplify:%s", constraintExpr_unparse (c) ) ) );  
  

  
  llassert ( constraintExpr_isDefined (c) );
  if (constraintExpr_isUndefined (c) )
    {
      return constraintExpr_undefined;
    }
  
  ret =  constraintExpr_copy(c);
  llassert(constraintExpr_isDefined (ret) );
	   
  constraintExpr_free(c);

  ret = constraintExpr_simplifyChildren (ret);

  ret = constraintExpr_combineConstants (ret);
  
  ret = constraintExpr_simplifyChildren (ret);
  

  kind = ret->kind;
  
  switch (kind)
    {
    case term:
      t = constraintExprData_termGetTerm (ret->data);
      t = constraintTerm_copy(t);
      t = constraintTerm_simplify (t);
      ret->data = constraintExprData_termSetTerm (ret->data, t);
      break;      
    case unaryExpr:
      ret = constraintExpr_simplifyunaryExpr (ret);
      break;           
    case binaryexpr:
      ret = constraintExpr_simplifybinaryExpr (ret);      
      break;
    default:
      llassert(FALSE);
    }    
  
  DPRINTF ((message ("constraintExpr_simplify returning :%s", constraintExpr_unparse (ret) ) ) );  
  return ret;
  
}

/*@only@*/
cstring constraintExpr_unparse (/*@temp@*/ constraintExpr ex) /*@*/
{
  cstring st;
  constraintExprKind kind;

  llassert (ex != NULL);

  kind = ex->kind;
  
  switch (kind)
    {
    case term:
      if (context_getFlag (FLG_PARENCONSTRAINT) )
	{
	  st = message ("(%q) ", constraintTerm_unparse (constraintExprData_termGetTerm (ex->data)));
	}
      else
	{
	  st = message ("%q", constraintTerm_unparse (constraintExprData_termGetTerm (ex->data)));
	}
      break;
    case unaryExpr:
      st = message ("%q(%q)",
		    constraintExprUnaryOpKind_print (constraintExprData_unaryExprGetOp (ex->data) ),
		    constraintExpr_unparse (constraintExprData_unaryExprGetExpr (ex->data) )
   		    );
      break;
    case binaryexpr:
      if (context_getFlag (FLG_PARENCONSTRAINT) )
	{
	  st = message ("(%q) %q (%q)",
			constraintExpr_unparse (constraintExprData_binaryExprGetExpr1 (ex->data) ),
			constraintExprBinaryOpKind_print (constraintExprData_binaryExprGetOp (ex->data)),
			constraintExpr_unparse (constraintExprData_binaryExprGetExpr2 (ex->data) )
			);
	}
      else
	{
	  st = message ("%q %q %q",
			constraintExpr_unparse (constraintExprData_binaryExprGetExpr1 (ex->data)),
			constraintExprBinaryOpKind_print (constraintExprData_binaryExprGetOp (ex->data)),
			constraintExpr_unparse (constraintExprData_binaryExprGetExpr2 (ex->data))
			);
	}
      
      break;
    default:
      llassert(FALSE);
      st = message ("error");
      
    }

  DPRINTF((message ("constraintExpr_unparse: '%s'",st) ) );
  return st;
}

constraintExpr constraintExpr_doSRefFixBaseParam (/*@returned@*/  constraintExpr expr, exprNodeList arglist)
{
  constraintTerm Term;
  constraintExprKind kind;
  constraintExpr expr1, expr2;
  constraintExprData data;
  llassert (expr != NULL);

  data = expr->data;
  
  kind = expr->kind;
  
  switch (kind)
    {
    case term:
      Term = constraintExprData_termGetTerm(data);
      Term = constraintTerm_copy(Term);

      Term = constraintTerm_doSRefFixBaseParam (Term, arglist);
      data = constraintExprData_termSetTerm(data, Term);
      break;
    case unaryExpr:
      expr1 = constraintExprData_unaryExprGetExpr (data);
      expr1 = constraintExpr_copy(expr1);

      expr1 = constraintExpr_doSRefFixBaseParam (expr1, arglist);
      data = constraintExprData_unaryExprSetExpr (data, expr1);
      break;
    case binaryexpr:
      expr1 = constraintExprData_binaryExprGetExpr1 (data);
      expr2 = constraintExprData_binaryExprGetExpr2 (data);
      
      expr1 = constraintExpr_copy(expr1);
      expr2 = constraintExpr_copy(expr2);

      expr1 = constraintExpr_doSRefFixBaseParam (expr1, arglist);
      data = constraintExprData_binaryExprSetExpr1 (data, expr1);
      expr2 = constraintExpr_doSRefFixBaseParam (expr2, arglist);
      data = constraintExprData_binaryExprSetExpr2 (data, expr2);
      
      break;
    default:
      llassert(FALSE);
      data = NULL;
    }
  return expr;
}

/*
/ *@only@* / constraintExpr constraintExpr_doSRefFixInvarConstraint (/ *@only@* / constraintExpr expr, sRef s, ctype ct)
{
  constraintExprKind kind;
  constraintExpr expr1, expr2;
  constraintExprData data;
  llassert (expr != NULL);

  data = expr->data;
  
  kind = expr->kind;
  
  switch (kind)
    {
    case term:
      expr = doSRefFixInvarConstraintTerm (expr, s, ct);
      break;
    case unaryExpr:
      expr1 = constraintExprData_unaryExprGetExpr (data);
      expr1 = constraintExpr_copy(expr1);
      expr1 = constraintExpr_doSRefFixInvarConstraint (expr1, s, ct);
      data = constraintExprData_unaryExprSetExpr (data, expr1);
      break;
    case binaryexpr:
      expr1 = constraintExprData_binaryExprGetExpr1 (data);
      expr2 = constraintExprData_binaryExprGetExpr2 (data);
      
      expr1 = constraintExpr_copy(expr1);
      expr2 = constraintExpr_copy(expr2);

      expr1 = constraintExpr_doSRefFixInvarConstraint (expr1, s, ct);
      data = constraintExprData_binaryExprSetExpr1 (data, expr1);
      expr2 = constraintExpr_doSRefFixInvarConstraint (expr2, s, ct);
      data = constraintExprData_binaryExprSetExpr2 (data, expr2);
      
      break;
    default:
      llassert(FALSE);
      data = NULL;
    }
  return expr;
}
*/

/*@only@*/ constraintExpr constraintExpr_doSRefFixConstraintParam (/*@only@*/ constraintExpr expr, exprNodeList arglist) /*@modifies expr@*/
{
  constraintExprKind kind;
  constraintExpr expr1, expr2;
  constraintExprData data;
  llassert (expr != NULL);

  data = expr->data;
  
  kind = expr->kind;
  
  switch (kind)
    {
    case term:
      expr = doSRefFixConstraintParamTerm (expr, arglist);
      break;
    case unaryExpr:
      expr1 = constraintExprData_unaryExprGetExpr (data);
      expr1 = constraintExpr_copy(expr1);
      expr1 = constraintExpr_doSRefFixConstraintParam (expr1, arglist);
      data = constraintExprData_unaryExprSetExpr (data, expr1);
      break;
    case binaryexpr:
      expr1 = constraintExprData_binaryExprGetExpr1 (data);
      expr2 = constraintExprData_binaryExprGetExpr2 (data);
      
      expr1 = constraintExpr_copy(expr1);
      expr2 = constraintExpr_copy(expr2);

      expr1 = constraintExpr_doSRefFixConstraintParam (expr1, arglist);
      data = constraintExprData_binaryExprSetExpr1 (data, expr1);
      expr2 = constraintExpr_doSRefFixConstraintParam (expr2, arglist);
      data = constraintExprData_binaryExprSetExpr2 (data, expr2);
      
      break;
    default:
      llassert(FALSE);
      data = NULL;
    }
  return expr;
}

/*@only@*/ constraintExpr constraintExpr_doFixResult (/*@only@*/  constraintExpr expr, /*@observer@*/ exprNode fcnCall)
{
  constraintExprKind kind;
  constraintExpr expr1, expr2;
  constraintExprData data;
  llassert (expr != NULL);

  data = expr->data;
  
  kind = expr->kind;
  
  switch (kind)
    {
    case term:
      expr = doFixResultTerm (expr, fcnCall);
      break;
    case unaryExpr:
      expr1 = constraintExprData_unaryExprGetExpr (data);
      expr1 = constraintExpr_copy(expr1);

      expr1 = constraintExpr_doFixResult (expr1, fcnCall);
      data = constraintExprData_unaryExprSetExpr (data, expr1);
      break;
    case binaryexpr:
      expr1 = constraintExprData_binaryExprGetExpr1 (data);
      expr2 = constraintExprData_binaryExprGetExpr2 (data);
      
      expr1 = constraintExpr_copy(expr1);
      expr2 = constraintExpr_copy(expr2);

      expr1 = constraintExpr_doFixResult (expr1, fcnCall);
      data = constraintExprData_binaryExprSetExpr1 (data, expr1);
      expr2 = constraintExpr_doFixResult (expr2, fcnCall);
      data = constraintExprData_binaryExprSetExpr2 (data, expr2);
      
      break;
    default:
      llassert(FALSE);
      data = NULL;
    }
  return expr;
}

cstring constraintExpr_print (constraintExpr expr) /*@*/
{
  return constraintExpr_unparse (expr);
}

bool constraintExpr_hasMaxSet (constraintExpr expr) /*@*/
{
  cstring t;

  t = constraintExpr_unparse(expr);

  if (cstring_containsLit(t, "maxSet") != NULL )
    {
      cstring_free(t);
      return (TRUE);
    }
  else
    {
      cstring_free(t);
      return FALSE;
    }
}



      /*returns 1 0 -1 like strcmp
	1 => expr1 > expr2
	0 => expr1 == expr2
	-1 => expr1 < expr2
       */

int constraintExpr_compare (constraintExpr expr1, constraintExpr expr2)
{
  long value1, value2;

  if (constraintExpr_similar (expr1, expr2) )
    {
      return 0;
    }
  
  value1 = constraintExpr_getValue(expr1);
  value2 = constraintExpr_getValue(expr2);

  if (value1 > value2)
    return 1;

  if (value1 == value2)
    return 0;

  else
    return -1;
}

long constraintExpr_getValue (constraintExpr expr)
{
  llassert (constraintExpr_isDefined(expr) );
  llassert (expr->kind == term);
  
  return (constraintTerm_getValue (constraintExprData_termGetTerm (expr->data)));
}

bool constraintExpr_canGetValue (constraintExpr expr)
{
  llassert ( constraintExpr_isDefined (expr) );
  if (constraintExpr_isUndefined (expr) )
    {
      return FALSE;
    }

  switch (expr->kind)
    {
    case term:
      return constraintTerm_canGetValue (constraintExprData_termGetTerm (expr->data) );
    default:
      return FALSE;
      
    }

  BADEXIT;
}

fileloc constraintExpr_loc (constraintExpr expr)
{
  constraintExpr e;
constraintTerm t;
  constraintExprKind kind;

  llassert ( constraintExpr_isDefined (expr) );
  if (constraintExpr_isUndefined (expr) )
    {
      return fileloc_undefined;
    }

  
 kind = expr->kind;
  
  switch (kind)
    {
    case term:
      t = constraintExprData_termGetTerm (expr->data);
      return (constraintTerm_getFileloc (t) );
      /*@notreached@*/
      break;      
    case unaryExpr:
      e = constraintExprData_unaryExprGetExpr (expr->data);
      return (constraintExpr_loc (e) );
      /*@notreached@*/
      break;           
    case binaryexpr:
      e = constraintExprData_binaryExprGetExpr1 (expr->data);
      return (constraintExpr_loc (e) );
      /*@notreached@*/
      break;
    }
  llassert (FALSE);
  return (fileloc_undefined);
}

/*drl moved from constriantTerm.c 5/20/001*/
static /*@only@*/ constraintExpr 
doFixResultTerm (/*@only@*/ constraintExpr e, /*@exposed@*/ exprNode fcnCall)
{
  constraintTerm t;
  sRef s;
  constraintExprData data;
  constraintExprKind kind;
  constraintExpr ret;
  
  llassert (constraintExpr_isDefined (e) );
  
  data = e->data;
  kind = e->kind;
  
  llassert (kind == term);
  
  t = constraintExprData_termGetTerm (data);
  llassert (constraintTerm_isDefined (t));
  
  ret = e;

  switch (constraintTerm_getKind (t))
    {
    case CTT_EXPR:
    case CTT_INTLITERAL:
      break;
      
    case CTT_SREF:
      s = constraintTerm_getSRef(t);
      if (sRef_isResult (s))
	{
	  ret = constraintExpr_makeExprNode(fcnCall);
	  constraintExpr_free(e);
	  e = NULL;
	}
      else
	{
	  e = NULL;
	}
      break;
    default:
      BADEXIT;
    }
  
  return ret;
  
}

#if 0

/*to be used for structure checking */

/ *@only@* / static constraintExpr
doSRefFixInvarConstraintTerm (/ *@only@* / constraintExpr e, sRef s, ctype ct)
{
  constraintTerm t;

  constraintExprData data = e->data;
  
  constraintExprKind kind = e->kind;
  
  constraintExpr ret;

  llassert(kind == term);

  t = constraintExprData_termGetTerm (data);
  llassert (constraintTerm_isDefined(t) );

  ret = e;

  DPRINTF (("Fixing: %s", constraintExpr_print (e)));

  switch (constraintTerm_getKind(t))
    {
    case CTT_EXPR:
      DPRINTF((message ("%q @ %q ", constraintTerm_unparse(t),
			fileloc_unparse (constraintTerm_getFileloc(t) ) ) ));
      break;
    case CTT_INTLITERAL:
      DPRINTF((message (" %q ", constraintTerm_unparse (t)) ));
      break;
      
    case CTT_SREF:
      / * evans 2001-07-24: constants should use the original term * /
      if (!constraintTerm_canGetValue (t))
	{
	  sRef snew;
	  DPRINTF ((message("Doing sRef_fixInvarConstraint for %q ", 
			     constraintTerm_unparse (t) ) ));

	  snew = fixSref (ct, s, constraintTerm_getSRef(t));

	  ret = constraintExpr_makeTermsRef(snew);
	  
	  constraintExpr_free (e);
	  
	  DPRINTF (( message("After Doing sRef_fixConstraintParam constraintExpr is %q ", 
			     constraintExpr_print (ret) ) ));
	  / *@-branchstate@* /
	} / *@=branchstate@* /

      break;
    default:
      BADEXIT;
    }

  return ret;
  
}
#endif
 
/*drl moved from constriantTerm.c 5/20/001*/
/*@only@*/ static constraintExpr 
doSRefFixConstraintParamTerm (/*@only@*/ constraintExpr e, /*@observer@*/ /*@temp@*/ exprNodeList arglist)
{
  constraintTerm t;

  constraintExprData data;
  
  constraintExprKind kind;
  
  constraintExpr ret;


  llassert(constraintExpr_isDefined (e) );

  data = e->data;
  
  kind = e->kind;

  

  llassert(kind == term);

  t = constraintExprData_termGetTerm (data);
  llassert (constraintTerm_isDefined(t) );

  ret = e;

  DPRINTF (("Fixing: %s", constraintExpr_print (e)));

  switch (constraintTerm_getKind(t))
    {
    case CTT_EXPR:
      DPRINTF((message ("%q @ %q ", constraintTerm_unparse(t),
			fileloc_unparse (constraintTerm_getFileloc(t) ) ) ));
      break;
    case CTT_INTLITERAL:
      DPRINTF((message (" %q ", constraintTerm_unparse (t)) ));
      break;
    case CTT_SREF:
      /* evans 2001-07-24: constants should use the original term */
      if (!constraintTerm_canGetValue (t))
	{
	  DPRINTF ((message("Doing sRef_fixConstraintParam for %q ", 
			     constraintTerm_unparse (t) ) ));
	  ret = sRef_fixConstraintParam (constraintTerm_getSRef(t), arglist);
	  
	  constraintExpr_free (e);
	  
	  DPRINTF (( message("After Doing sRef_fixConstraintParam constraintExpr is %q ", 
			     constraintExpr_print (ret) ) ));
	  /*@-branchstate@*/
	} /*@=branchstate@*/

      break;
    default:
      BADEXIT;
    }

  return ret;
  
}


#if 0
bool constraintExpr_includesTerm (constraintExpr expr, constraintTerm term)
{
  if (constraintTerm_hasTerm (expr->term, term) )
    return TRUE;

  if ((expr->expr) != NULL)
    {
      return ( constraintExpr_includesTerm (expr->expr, term) );
    }
  return FALSE;

}
#endif

/*drl added 6/11/01 */
bool constraintExpr_isBinaryExpr (/*@observer@*/ constraintExpr c)
{

  llassert(constraintExpr_isDefined (c) );

  if ( ! (constraintExpr_isDefined (c) ) )
    return FALSE;
  
  if (c->kind == binaryexpr)
    return TRUE;

  else
    return FALSE;
}

/*drl added 8/08/001 */
bool constraintExpr_isTerm (/*@observer@*/ constraintExpr c) /*@*/
{
  llassert(constraintExpr_isDefined (c) );
  
  if (c->kind == term)
    return TRUE;

  else
    return FALSE;
}

/*@observer@*/ /*@temp@*/ constraintTerm constraintExpr_getTerm ( /*@temp@*/ /*@observer@*/ constraintExpr c) /*@*/
{
  constraintTerm term;
  
  llassert(constraintExpr_isDefined (c) );
  
  llassert(constraintExpr_isTerm(c) );

  term = constraintExprData_termGetTerm(c->data);

  return term;
}

static void  binaryExpr_dump (/*@observer@*/ constraintExprData data,  FILE *f)
{
  constraintExpr expr1;
  constraintExprBinaryOpKind binaryOp;
  constraintExpr expr2;


  binaryOp = constraintExprData_binaryExprGetOp (data);

  fprintf(f, "%d\n", (int) binaryOp);
  
  expr1 = constraintExprData_binaryExprGetExpr1 (data);
  expr2 = constraintExprData_binaryExprGetExpr2 (data);

  fprintf(f, "e1\n");

  constraintExpr_dump(expr1, f);

  fprintf(f, "e2\n");
  constraintExpr_dump(expr2, f);
}


static constraintExpr  binaryExpr_undump (FILE *f)
{
  constraintExpr expr1;
  constraintExprBinaryOpKind binaryOp;
  constraintExpr expr2;

  constraintExpr ret;

  

  char * str;
  char * os;

  os = mstring_create (MAX_DUMP_LINE_LENGTH);

  str = fgets(os, MAX_DUMP_LINE_LENGTH, f);

  if (! mstring_isDefined(str) )
    {
      llfatalbug(message("Library file is corrupted") );
    }
  
  binaryOp = (constraintExprBinaryOpKind) reader_getInt(&str);
  
  str = fgets(os, MAX_DUMP_LINE_LENGTH, f);

  if (! mstring_isDefined(str) )
    {
      llfatalbug(message("Library file is corrupted") );
    }

  reader_checkChar (&str, 'e');
  reader_checkChar (&str, '1');
  
  expr1 = constraintExpr_undump (f);

  str = fgets(os, MAX_DUMP_LINE_LENGTH, f);

  reader_checkChar (&str, 'e');
  reader_checkChar (&str, '2');  

  expr2 = constraintExpr_undump (f);

  ret = constraintExpr_makeBinaryOpConstraintExpr (expr1, expr2);
  ret->data = constraintExprData_binaryExprSetOp(ret->data, binaryOp);

  free(os);
  return ret;
}



static void  unaryExpr_dump (/*@observer@*/ constraintExprData data,  FILE *f)
{

  constraintExpr expr;
  constraintExprUnaryOpKind unaryOp;

  unaryOp = constraintExprData_unaryExprGetOp (data);

  fprintf(f, "%d\n", (int) unaryOp);
  
  expr = constraintExprData_unaryExprGetExpr (data);

  constraintExpr_dump(expr, f);  
}

static  constraintExpr  unaryExpr_undump ( FILE *f)
{

  constraintExpr expr;
  constraintExprUnaryOpKind unaryOp;
  constraintExpr ret;
  
  char * str;
  char * os;

  str = mstring_create (MAX_DUMP_LINE_LENGTH);
  os = str;
  str = fgets(os, MAX_DUMP_LINE_LENGTH, f);

  if (! mstring_isDefined(str) )
    {
      llfatalbug(message("Library file is corrupted") );
    }

  unaryOp = (constraintExprUnaryOpKind) reader_getInt(&str);
  
  expr = constraintExpr_undump (f);

  ret = constraintExpr_makeUnaryOp (expr, unaryOp);

  free(os);
  
  return ret;
}

void  constraintExpr_dump (/*@observer@*/ constraintExpr expr,  FILE *f)
{
  constraintExprKind kind;
  constraintTerm t;
  

  llassert(constraintExpr_isDefined(expr) );
  
  DPRINTF((message("constraintExpr_dump:: dumping constraintExpr %s",
		   constraintExpr_unparse(expr)
		   ) ));
  
  kind = expr->kind;
  
  fprintf(f,"%d\n", (int) kind);
  
  switch (kind)
    {
    case term:
      t = constraintExprData_termGetTerm (expr->data);
      constraintTerm_dump (t, f);
      break;      
    case unaryExpr:
      unaryExpr_dump (expr->data, f);
      break;           
    case binaryexpr:
      binaryExpr_dump  (expr->data, f);
      break;
    }  
}

/*@only@*/ constraintExpr  constraintExpr_undump (FILE *f)
{
  constraintExprKind kind;
  constraintTerm t;
  constraintExpr ret;
  
  char * s;
  char * os;
  
  s = mstring_create (MAX_DUMP_LINE_LENGTH);

  os = s;
  
  s = fgets(os, MAX_DUMP_LINE_LENGTH, f);

  if (! mstring_isDefined(s) )
    {
      llfatalbug(message("Library file is corrupted") );
    }

  kind = (constraintExprKind) reader_getInt(&s);

  free (os);
  
  switch (kind)
    {
    case term:
      t = constraintTerm_undump (f);
      ret = constraintExpr_makeTerm(t);
      break;      
    case unaryExpr:
      ret = unaryExpr_undump (f);
      break;           
    case binaryexpr:
      ret = binaryExpr_undump  (f);
      break;
    }

  return ret;

}

int constraintExpr_getDepth (constraintExpr ex)
{
  int ret;
  
  constraintExprKind kind;

  llassert (ex != NULL);

  kind = ex->kind;
  
  switch (kind)
    {
    case term:
      ret = 1;
      break;
    case unaryExpr:
      ret =  constraintExpr_getDepth (constraintExprData_unaryExprGetExpr (ex->data) );
      ret++;
      
      break;
    case binaryexpr:
      ret = 0;
      ret = constraintExpr_getDepth (constraintExprData_binaryExprGetExpr1 (ex->data) );

      ret++;

      ret += constraintExpr_getDepth (constraintExprData_binaryExprGetExpr2 (ex->data) );

      break;
    default:
      BADEXIT;
    }

  return ret;
}


bool constraintExpr_canGetCType (constraintExpr e) /*@*/
{
  if (constraintExpr_isUndefined(e))
    return FALSE;
  
  if (e->kind == term)
    {
      return TRUE;
    }
  else
    {
      DPRINTF (("constraintExpr_canGetCType: can't get type for %s", constraintExpr_unparse (e)));
      return FALSE;
    }
}

ctype constraintExpr_getCType (constraintExpr e) /*@*/
{
  constraintTerm t;

  llassert (constraintExpr_isDefined (e));
  llassert (constraintExpr_canGetCType (e));

  switch (e->kind)
    {
    case term:
      t = constraintExprData_termGetTerm (e->data);
      return (constraintTerm_getCType(t) );
      /* assume that a unary expression will be an int ... */
    case unaryExpr:
      return ctype_unknown; /* was ctype_signedintegral; */
      /* drl for just return type of first operand */
    case binaryexpr:
      return (constraintExpr_getCType (constraintExprData_binaryExprGetExpr1 (e->data)));
    default:
      BADEXIT;
    }
  BADEXIT;
}

/* drl add 10-5-001 */

static bool constraintExpr_hasTypeChange (constraintExpr e)
{
  llassert(constraintExpr_isDefined(e));

  if (constraintExpr_isDefined((e)) && (e->ct == TRUE))
    {
      return TRUE;
    }

  if (e->kind == unaryExpr)
    {
      if (constraintExprData_unaryExprGetOp (e->data) == MAXSET)
	{
	  constraintExpr ce = constraintExprData_unaryExprGetExpr(e->data);
	  DPRINTF (("Unary type change: [%x] %s", ce, constraintExpr_unparse (ce)));
	  DPRINTF (("Types: %s / %s", ctype_unparse (constraintExpr_getCType (ce)),
		    ctype_unparse (constraintExpr_getOrigType (ce))));
	  return (constraintExpr_hasTypeChange(ce));
	}
    }
  
  return FALSE;
}

/* drl add 10-5-001 */

static ctype constraintExpr_getOrigType (constraintExpr e)
{
  llassert (constraintExpr_isDefined (e));
  llassert (constraintExpr_hasTypeChange (e));
  
  if (e->ct == TRUE) 
    {
      return e->origType;
    }

  if (e->kind == unaryExpr)
    {
      if (constraintExprData_unaryExprGetOp (e->data) == MAXSET)
	{
	  constraintExpr ce = constraintExprData_unaryExprGetExpr (e->data);
	  return (constraintExpr_getOrigType(ce));
	}
	
    }

  BADEXIT;
} 

/*drl added these around 10/18/001*/

static /*@only@*/ constraintExpr 
constraintExpr_div (/*@only@*/ constraintExpr e, ctype tfrom, ctype tto, fileloc loc)
{
  int sizefrom = ctype_getSize (tfrom);
  int sizeto = ctype_getSize (tto);

  DPRINTF (("constraintExpr_div: %s", constraintExpr_unparse (e)));
  DPRINTF (("Types: %s / %s",
	    ctype_unparse (tfrom),
	    ctype_unparse (tto)));
  
  if (sizefrom == -1) {
    llbug (message ("constraintExpr_div: type size unknown: %s", ctype_unparse (tfrom)));
  }

  if (sizeto == -1) {
    llbug (message ("constraintExpr_div: type size unknown: %s", ctype_unparse (tto)));
  }

  if (sizeto == sizefrom) 
    {
      DPRINTF (("Sizes match: %d / %d", sizeto, sizefrom));
      ; /* Sizes match, a-ok */
    }
  else
    {
      float scale = (float) sizefrom / (float) sizeto;
      constraintTerm ct;
      long val;
      float fnewval;
      long newval;

      llassert (e != NULL);
      llassert (e->kind == term);
      ct = constraintExprData_termGetTerm (e->data);
      DPRINTF (("constraint: %s / %s", constraintExpr_unparse (e), constraintTerm_unparse (ct)));
      llassert (constraintTerm_canGetValue (ct));
      val = constraintTerm_getValue (ct);

      DPRINTF (("Scaling constraints by: %ld * %f", val, scale));

      fnewval = ((float) val) * scale;
      newval = (long) fnewval;

      DPRINTF (("Values: %f / %ld", fnewval, newval));

      if ((fnewval - (float) newval) > FLT_EPSILON) 
	{
	  voptgenerror (FLG_ALLOCMISMATCH,
			message ("Allocated memory is converted to type %s of (size %d), "
				 "which is not divisible into original allocation of space "
				 "for %d elements of type %s (size %d)",
				 ctype_unparse (tto), sizeto,
				 long_toInt (val), ctype_unparse (tfrom), sizefrom),
			loc);
	}  

      constraintTerm_setValue (ct, newval);
    }

  DPRINTF (("After div: %s", constraintExpr_unparse (e)));
  return e;
}


/*@access exprNode@*/ 
static /*@only@*/ constraintExpr 
constraintTerm_simpleDivTypeExprNode (/*@only@*/ constraintExpr e, ctype tfrom, ctype tto, fileloc loc)
{
  exprData data;
  exprNode t1, t2, expr;
  lltok tok;
  constraintTerm t;

  llassert (constraintExpr_isDefined(e) );
  
  DPRINTF (("constraintTerm_simpleDivTypeExprNode e=%s [%s => %s]", constraintExpr_print(e), 
	    ctype_unparse(tfrom), ctype_unparse (tto)));
  
  t = constraintExprData_termGetTerm (e->data);
  
  expr = constraintTerm_getExprNode (t);

  llassert (constraintExpr_isDefined(e));
  llassert (exprNode_isDefined(expr));
  
  if (expr->kind == XPR_OP)
    {
      data = expr->edata;
      
      t1 = exprData_getOpA (data);
      t2 = exprData_getOpB (data);
      tok = exprData_getOpTok (data);

      if (lltok_isMult (tok))
	{
	  /*
	  ** If the sizeof is first, flip them.
	  */

	  llassert (exprNode_isDefined(t1) && exprNode_isDefined(t2));

	  if (t2->kind == XPR_SIZEOF || t2->kind == XPR_SIZEOFT) 
	    {
	      exprNode tmp = t1;
	      t1 = t2;
	      t2 = tmp;
	    }
	  
	  /*drl 3/2/2003 we know this from the fact that it's a
	    multiplication operation...*/
	  
	  if (t1->kind == XPR_SIZEOF || t1->kind == XPR_SIZEOFT)
	    {
	      ctype multype;
	      
	      if (t1->kind == XPR_SIZEOFT)
		{
		  multype = qtype_getType (exprData_getType (t1->edata));
		}
	      else
		{
		  exprNode tempE = exprData_getSingle (t1->edata);
		  multype = exprNode_getType (tempE); 
		}

	      DPRINTF (("Here we go sizeof: %s / %s / %s",
			ctype_unparse (multype), ctype_unparse (tfrom), ctype_unparse (tto)));
	      llassert (ctype_isPointer (tfrom));

	      if (ctype_almostEqual (ctype_makePointer (multype), tto))
		{
		  /* this is a bit sloopy but ... */
		  constraintExpr_free (e);
		  DPRINTF (("Sizeof types match okay!"));
		  return constraintExpr_makeExprNode (t2);
		}
	      else
		{
		  int sizemul = ctype_getSize (multype);
		  ctype tobase = ctype_baseArrayPtr (tto);
		  int sizeto = ctype_getSize (tobase);

		  DPRINTF (("Types: %s / %s / %s",
			    ctype_unparse (tfrom), ctype_unparse (tto), ctype_unparse (multype)));

		  voptgenerror (FLG_ALLOCMISMATCH,
				message ("Allocated memory is used as a different type (%s) from the sizeof type (%s)",
					 ctype_unparse (tobase), ctype_unparse (multype)),
				loc);
		  
		  if (sizemul == sizeto) 
		    {
		      constraintExpr_free (e);
		      DPRINTF (("Sizeof types match okay!"));
		      return constraintExpr_makeExprNode (t2);
		    }
		  else
		    {
		      /* nothing was here */
		      DPRINTF (("MISMATCHING TYPES!"));
		      return (constraintExpr_div (constraintExpr_makeExprNode (t2), multype, tto, loc));
		    }
		}
	    }
	  else
	    {
	      DPRINTF (("NOT A SIZEOF!"));
	      /* empty */
	    }
	}
      else 
	{
	  DPRINTF (("Not a mult: %s", constraintExpr_unparse (e)));
	}
    }

  return (constraintExpr_div (e, tfrom, tto, loc));
}
/*@noaccess exprNode@*/ 

static /*@only@*/ constraintExpr simpleDivType (/*@only@*/ constraintExpr e, ctype tfrom, ctype tto, fileloc loc)
{
  DPRINTF (("simpleDiv got %s", constraintExpr_unparse(e)));
  DPRINTF (("Types: %s / %s",
	    ctype_unparse (tfrom),
	    ctype_unparse (tto)));

  llassert (constraintExpr_isDefined(e));
  
  switch (e->kind)
    {
    case term:
      {
	constraintTerm t = constraintExprData_termGetTerm (e->data);

	DPRINTF (("Term: %s", constraintTerm_unparse (t)));

	if (constraintTerm_isExprNode (t))
	  {
	    return constraintTerm_simpleDivTypeExprNode (e, tfrom, tto, loc);
	    
	    /* search for * size of ct and remove */
	  }
	DPRINTF (("Here: %s / %s -> %s", constraintExpr_unparse (e), ctype_unparse (tfrom), ctype_unparse (tto)));
	return constraintExpr_div (e, tfrom, tto, loc);
      }
      
    case binaryexpr:
      {
	constraintExpr temp;
	
	temp = constraintExprData_binaryExprGetExpr1 (e->data);
	temp = constraintExpr_copy(temp);
	temp = simpleDivType (temp, tfrom, tto, loc);
	
	e->data = constraintExprData_binaryExprSetExpr1 (e->data, temp);
	
	temp = constraintExprData_binaryExprGetExpr2 (e->data);
	temp = constraintExpr_copy(temp);
	temp = simpleDivType (temp, tfrom, tto, loc);
	e->data = constraintExprData_binaryExprSetExpr2 (e->data, temp);

	DPRINTF (("simpleDiv binaryexpr returning %s ", constraintExpr_unparse(e)));
	return e;
      }
    case unaryExpr:
      {
	return constraintExpr_div (e, tfrom, tto, loc);
      }

    default:
      BADEXIT;
    }
}

static /*@only@*/ constraintExpr constraintExpr_adjustMaxSetForCast (/*@only@*/ constraintExpr e, ctype tfrom, 
								     ctype tto, fileloc loc)
{
  DPRINTF (("constraintExpr_adjustMaxSetForCast got %s [%s => %s]", constraintExpr_unparse(e), 
	    ctype_unparse (tfrom), ctype_unparse (tto)));
  
  e = constraintExpr_makeIncConstraintExpr (e);
  e = constraintExpr_simplify (e);
  e = simpleDivType (e, tfrom, tto, loc);
  e = constraintExpr_makeDecConstraintExpr (e);
  e = constraintExpr_simplify (e);
  
  DPRINTF (("constraintExpr_adjustMaxSetForCast returning %s ", constraintExpr_unparse(e)));
  return e;
}


bool constraintExpr_isConstantOnly (constraintExpr e)
{
  DPRINTF (("constraintExpr_isConstantOnly %s ", constraintExpr_unparse(e)));
  llassert (constraintExpr_isDefined(e));

  switch (e->kind)
    {
    case term:
      {
	constraintTerm t = constraintExprData_termGetTerm(e->data);
	
	if (constraintTerm_isConstantOnly (t))
	  {
	    return TRUE;
	  }
	else
	  {
	    return FALSE;
	  }
      }
      
    case binaryexpr:
      {
	constraintExpr temp1 = constraintExprData_binaryExprGetExpr1 (e->data);
	constraintExpr temp2 = constraintExprData_binaryExprGetExpr2 (e->data);
	
	if (constraintExpr_isConstantOnly(temp1) &&
	    constraintExpr_isConstantOnly(temp2) )
	  {
	    return TRUE;
	  }
	else
	  {
	    return FALSE;
	  }
      }
      
    case unaryExpr:
      {
	constraintExpr temp;
	
	temp = constraintExprData_unaryExprGetExpr (e->data );

	if (constraintExpr_isConstantOnly(temp) )
	  {
	    return TRUE;
	  }
	else
	  {
	    return FALSE;
	  }
      }
    default:
      BADEXIT;
    }
}

