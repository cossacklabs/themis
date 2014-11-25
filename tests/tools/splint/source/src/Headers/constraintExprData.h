#ifndef __constraintExprData_h__
#define __constraintExprData_h__

typedef enum
{
  BINARYOP_UNDEFINED,
  BINARYOP_PLUS,
  BINARYOP_MINUS
}
constraintExprBinaryOpKind;

typedef enum
{
  UNARYOP_UNDEFINED,
  MAXSET, MINSET, MAXREAD, MINREAD 
 }
constraintExprUnaryOpKind;

typedef struct constraintExprBinaryOp_
{
  constraintExpr expr1;
  constraintExprBinaryOpKind binaryOp;
  constraintExpr expr2;
} constraintExprBinaryOp;

typedef struct constraintExprUnaryOp_
{
  constraintExpr expr;
  constraintExprUnaryOpKind unaryOp;
} constraintExprUnaryOp;

typedef union constraintExprData
{
  constraintExprBinaryOp binaryOp;
  constraintExprUnaryOp unaryOp;
  constraintTerm term;
} *constraintExprData;

extern /*@falsewhennull@*/ bool constraintExprData_isDefined (/*@temp@*/ /*@observer@*/ /*@reldef@*/ constraintExprData p_e) /*@*/ ;
# define constraintExprData_isDefined(e)      ((e) != NULL)

extern void constraintExprData_freeBinaryExpr (/*@only@*/ constraintExprData) ;
extern void constraintExprData_freeUnaryExpr (/*@only@*/ constraintExprData) ;
extern void constraintExprData_freeTerm (/*@only@*/ constraintExprData) ;

extern constraintExprData constraintExprData_termSetTerm ( /*@returned@*/ /*@partial@*/ constraintExprData p_data, /*@only@*/ constraintTerm p_term);

extern /*@exposed@*/ constraintTerm 
constraintExprData_termGetTerm (/*@observer@*/ constraintExprData p_data) /*@*/;

extern constraintExprUnaryOpKind 
constraintExprData_unaryExprGetOp (/*@observer@*/ /*@reldef@*/ constraintExprData p_data) /*@*/;

extern /*@observer@*/ constraintExpr 
constraintExprData_unaryExprGetExpr (/*@observer@*/ /*@reldef@*/constraintExprData p_data) /*@*/;

extern constraintExprData  
constraintExprData_unaryExprSetOp (/*@partial@*/ /*@returned@*/ constraintExprData p_data, constraintExprUnaryOpKind p_op);

extern constraintExprData  constraintExprData_unaryExprSetExpr (/*@partial@*/ /*@returned@*/ constraintExprData p_data,  /*@only@*/ constraintExpr p_expr);

extern constraintExprBinaryOpKind  constraintExprData_binaryExprGetOp (/*@partial@*/constraintExprData p_data) /*@*/;

extern /*@observer@*/ constraintExpr  constraintExprData_binaryExprGetExpr1 (/*@observer@*/ /*@reldef@*/constraintExprData p_data)/*@*/;

extern /*@observer@*/ constraintExpr  constraintExprData_binaryExprGetExpr2 (/*@observer@*/ /*@reldef@*/constraintExprData p_data)/*@*/;

extern constraintExprData  constraintExprData_binaryExprSetExpr1 (/*@partial@*/ /*@returned@*/ constraintExprData p_data, /*@only@*/ constraintExpr p_expr) ;

extern constraintExprData  constraintExprData_binaryExprSetExpr2  (/*@partial@*/ /*@returned@*/  constraintExprData p_data, /*@only@*/ constraintExpr p_expr);

extern constraintExprData  constraintExprData_binaryExprSetOp (/*@partial@*/ /*@returned@*/ /*@out@*/constraintExprData p_data, constraintExprBinaryOpKind p_op);

extern /*@only@*/ constraintExprData constraintExprData_copyBinaryExpr(/*@observer@*/ constraintExprData p_data);
extern /*@only@*/ constraintExprData constraintExprData_copyUnaryExpr(/*@observer@*/ constraintExprData p_data);
extern /*@only@*/ constraintExprData constraintExprData_copyTerm (/*@observer@*/ constraintExprData p_data);

#else
#error "Multiple Include"
#endif
