#ifndef __constraintExpr_h__

#define __constraintExpr_h__

typedef enum
{
  binaryexpr,
  unaryExpr,
  term
} constraintExprKind;

struct s_constraintExpr {
  constraintExprKind kind;
  bool ct; /*changed type */
  ctype origType;
  constraintExprData data;
};



/*@constant null constraintExpr constraintExpr_undefined; @*/
# define constraintExpr_undefined ((constraintExpr)NULL)

extern /*@falsewhennull@*/ bool constraintExpr_isDefined (constraintExpr p_e) /*@*/ ;
extern /*@unused@*/ /*@nullwhentrue@*/ bool constraintExpr_isUndefined (constraintExpr p_e) /*@*/ ;
extern /*@unused@*/ /*@nullwhentrue@*/ bool constraintExpr_isError (constraintExpr p_e) /*@*/ ;

# define constraintExpr_isDefined(e)        ((e) != constraintExpr_undefined)
# define constraintExpr_isUndefined(e)      ((e) == constraintExpr_undefined)
# define constraintExpr_isError(e)          ((e) == constraintExpr_undefined)


void constraintExpr_free ( /*@only@*/ constraintExpr p_expr);

constraintExpr constraintExpr_setFileloc (/*@returned@*/ constraintExpr p_c, fileloc p_loc) /*@modifies p_c@*/;

constraintExpr constraintExpr_copy (constraintExpr p_expr) /*@*/;

/*@only@*/ cstring constraintExpr_unparse (/*@temp@*/ /*@observer@*/ constraintExpr p_ex) /*@*/;
extern cstring constraintExpr_print (constraintExpr p_expr) /*@*/;


bool constraintExpr_similar (constraintExpr p_expr1, constraintExpr p_expr2) /*@*/;
bool constraintExpr_same (constraintExpr p_expr1, constraintExpr p_expr2) /*@*/;
/*@only@*/ constraintExpr constraintExpr_searchandreplace (/*@only@*/ /*@unique@*/ constraintExpr p_c, /*@temp@*/ /*@observer@*/ constraintExpr p_old, /*@temp@*/ /*@observer@*/ constraintExpr p_newExpr ) /*@modifies p_c@*/;

bool constraintExpr_canGetValue (constraintExpr p_expr) /*@*/;
long constraintExpr_getValue (constraintExpr p_expr) /*@*/;

int constraintExpr_compare (constraintExpr p_expr1, constraintExpr p_expr2) /*@*/;

/* constraintExpr constraintExpr_makeValueInt (int i); */

/*@only@*/ /*@notnull@*/constraintExpr constraintExpr_makeIntLiteral (long p_i);

/*@only@*/ constraintExpr constraintExpr_makeValueExpr (/*@exposed@*/ exprNode p_expr);

/*@only@*/ constraintExpr constraintExpr_makeMaxSetExpr (/*@exposed@*/ exprNode p_expr);

/*@only@*/ constraintExpr  constraintExpr_makeMaxReadExpr (/*@exposed@*/ exprNode p_expr);

/*@only@*/ constraintExpr constraintExpr_makeIncConstraintExpr (/*@only@*/ constraintExpr p_expr);

/*@only@*/ constraintExpr constraintExpr_makeDecConstraintExpr (/*@only@*/ constraintExpr p_expr);

/*@only@*/ constraintExpr constraintExpr_simplify (/*@only@*/ constraintExpr p_c);

/*@only@*/ constraintExpr constraintExpr_solveBinaryExpr (constraintExpr p_lexpr, /*@only@*/ constraintExpr p_expr) /*@modifies p_lexpr, p_expr @*/;

bool constraintExpr_search (/*@observer@*/ /*@temp@*/ constraintExpr p_c, /*@observer@*/ /*@temp@*/ constraintExpr p_old);

/*@only@*/ fileloc constraintExpr_loc (constraintExpr p_expr);


/*@only@*/ constraintExpr constraintExpr_makeSRefMaxset ( /*@temp@*/ /*@observer@*/ sRef p_s);

/*@only@*/ constraintExpr constraintExpr_makeSRefMaxRead(/*@temp@*/ /*@observer@*/ sRef p_s);

/*@only@*/ constraintExpr constraintExpr_makeTermsRef (/*@temp@*/ sRef  p_s);

constraintExpr constraintExpr_doSRefFixBaseParam ( /*@returned@*/ constraintExpr p_expr, exprNodeList p_arglist);

/*@only@*/ constraintExpr constraintExpr_makeExprNode (/*@dependent@*/  exprNode p_e);

/*@only@*/ constraintExpr constraintExpr_doFixResult (/*@only@*/  constraintExpr p_expr, /*@observer@*/ exprNode p_fcnCall);

/*@falsewhennull@*/ bool constraintExpr_isLit (constraintExpr p_expr) /*@*/ ;

/*@only@*/ constraintExpr constraintExpr_makeAddExpr (/*@only@*/ constraintExpr p_expr, /*@only@*/ constraintExpr p_addent);

/*@only@*/  constraintExpr constraintExpr_makeSubtractExpr (/*@only@*/ constraintExpr p_expr, /*@only@*/ constraintExpr p_addent);

/*@only@*/ constraintExpr constraintExpr_parseMakeUnaryOp (lltok p_op,/*@only@*/ constraintExpr p_cexpr);

constraintExpr constraintExpr_parseMakeBinaryOp (/*@only@*/ constraintExpr p_expr1, lltok p_op, /*@only@*/ constraintExpr p_expr2);

bool constraintExpr_hasMaxSet (/*@observer@*/ /*@temp@*/ constraintExpr p_expr);



/*@only@*/ constraintExpr constraintExpr_doSRefFixConstraintParam (/*@returned@*/ /*@only@*/ constraintExpr p_expr, exprNodeList p_arglist) /*@modifies p_expr@*/;

/*@only@*/
constraintExpr constraintExpr_propagateConstants (/*@only@*/ constraintExpr p_expr,
						/*@out@*/ bool * p_propagate,
						  /*@out@*/ int *p_literal);

/*@falsewhennull@*/ bool constraintExpr_isBinaryExpr (/*@observer@*/ /*@temp@*/ constraintExpr p_c) /*@*/ ;

extern void  constraintExpr_dump (/*@observer@*/ /*@temp@*/ constraintExpr p_expr,  FILE *p_f);

extern /*@only@*/ constraintExpr  constraintExpr_undump (FILE *p_f);

extern /*@only@*/ constraintExpr constraintExpr_makeTermExprNode (/*@exposed@*/ exprNode p_e) ;

/* drl added 8/8/001*/
bool constraintExpr_isTerm (/*@observer@*/ /*@temp@*/ constraintExpr p_c);

/* drl added 8/8/001*/
/*@observer@*/ /*@temp@*/ constraintTerm constraintExpr_getTerm ( /*@temp@*/ /*@observer@*/ constraintExpr p_c);

/* drl added 8/8/001*/
int constraintExpr_getDepth (/*@observer@*/ /*@temp@*/ constraintExpr p_ex);


/*drl 1/6/2001: I didn't think these functions were solid enough to include in the   stable  release of splint.*/
/* drl added 12/30/001*/
/* extern / *@only@* / constraintExpr constraintExpr_doSRefFixInvarConstraint (/ *@only@* / constraintExpr p_expr, sRef p_s, ctype p_ct); */



/*drl added 12/19 */
bool  constraintExpr_isConstantOnly ( constraintExpr p_e );

#else
# error "Multiple include"
#endif
