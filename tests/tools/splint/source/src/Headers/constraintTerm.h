#ifndef __constraintTerm_h__

#define __constraintTerm_h__

typedef union
{
  /*@exposed@*/ /*@dependent@*/ exprNode expr;
  /*@only@*/  sRef     sref;
  long intlit;
} constraintTermValue;

typedef enum
{
  CTT_ERRORBADCONSTRAINTTERMTYPE,
  CTT_EXPR, 
  CTT_SREF,
  CTT_INTLITERAL
} constraintTermType;

struct s_constraintTerm {
  /*@only@*/ fileloc loc;
  constraintTermValue value;
  constraintTermType kind;
} ;

abst_typedef struct s_constraintTerm *constraintTerm;


extern bool constraintTerm_isDefined (constraintTerm p_t) /*@*/ ;

extern constraintTermType constraintTerm_getKind (constraintTerm) ;
extern /*@exposed@*/ sRef constraintTerm_getSRef (constraintTerm) ;

void constraintTerm_free (/*@only@*/ constraintTerm p_term);

constraintTerm constraintTerm_simplify (/*@returned@*/ constraintTerm p_term) /*@modifies p_term@*/ ;

/*@only@*/ constraintTerm constraintTerm_makeExprNode (/*@dependent@*/ exprNode p_e) /*@*/;

constraintTerm constraintTerm_copy (constraintTerm p_term) /*@*/;

bool constraintTerm_similar (constraintTerm p_term1, constraintTerm p_term2) /*@*/;

extern bool constraintTerm_canGetValue (constraintTerm p_term)/*@*/;
extern long constraintTerm_getValue (constraintTerm p_term) /*@*/;
extern void constraintTerm_setValue (constraintTerm p_term, long p_value) /*@modifies p_term@*/;

extern fileloc constraintTerm_getFileloc (constraintTerm p_t) /*@*/;

extern bool constraintTerm_isIntLiteral (constraintTerm p_term) /*@*/;

extern constraintTerm constraintTerm_makesRef  (/*@temp@*/ /*@observer@*/ sRef p_s) /*@*/;

/*@unused@*/ bool constraintTerm_probSame (constraintTerm p_term1, constraintTerm p_term2) /*@*/;

constraintTerm constraintTerm_setFileloc (/*@returned@*/ constraintTerm p_term, fileloc p_loc) /*@modifies p_term@*/;

constraintTerm constraintTerm_makeIntLiteral (long p_i) /*@*/;

bool constraintTerm_isStringLiteral (constraintTerm p_c) /*@*/;
cstring constraintTerm_getStringLiteral (constraintTerm p_c) /*@*/;


constraintTerm constraintTerm_doSRefFixBaseParam (/*@returned@*/ constraintTerm p_term, exprNodeList p_arglist) /*@modifies p_term@*/;

extern cstring constraintTerm_unparse (constraintTerm)  /*@*/ ;

extern void constraintTerm_dump ( /*@observer@*/ constraintTerm p_t,  FILE * p_f);
extern /*@only@*/ constraintTerm constraintTerm_undump ( FILE * p_f);
extern bool constraintTerm_isInitBlock (/*@observer@*/ /*@temp@*/ constraintTerm p_c) /*@*/;
extern int constraintTerm_getInitBlockLength (/*@observer@*/ /*@temp@*/ constraintTerm p_c) /*@*/;
extern bool constraintTerm_isExprNode (/*@observer@*/ /*@temp@*/ constraintTerm p_c) /*@*/;
extern ctype constraintTerm_getCType (constraintTerm p_term);

extern /*@exposed@*/ exprNode constraintTerm_getExprNode (constraintTerm p_t);
extern /*@exposed@*/ sRef constraintTerm_getsRef (constraintTerm p_t);

/*drl added 12/19/2002*/
extern bool constraintTerm_isConstantOnly (constraintTerm p_term);
     
#else
#error "Multiple Include!"
#endif
