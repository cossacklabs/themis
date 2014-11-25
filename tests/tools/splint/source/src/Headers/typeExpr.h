/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** typeExpr.h
*/

typedef enum 
{
  TEXPR_BASE, TEXPR_PTR, TEXPR_ARRAY, TEXPR_FCN
  } typeExprKind;

struct s_typeExpr 
{
  int wrapped; /* number of balanced parentheses around this program node */
  typeExprKind kind; /* different kinds of type constructors */
  union {
    ltoken base;
    /*@null@*/ typeExpr pointer;
    struct {
      /*@null@*/ typeExpr elementtype;
      /*@null@*/ termNode size;
      } array;
    struct {
      /*@null@*/ typeExpr returntype; /* not really ... return-part maybe */
      paramNodeList args;
    } function;
  } content;
  sort sort;
} ; 

extern void typeExpr_free (/*@only@*/ /*@null@*/ typeExpr p_x);
extern /*@only@*/ cstring typeExpr_unparse (/*@null@*/ typeExpr p_x);
extern /*@only@*/ cstring typeExpr_unparseNoBase (/*@null@*/ typeExpr p_x);

/* like a declaratorNode but without varId */
typedef typeExpr abstDeclaratorNode;

extern void abstDeclaratorNode_free (/*@only@*/ /*@null@*/ abstDeclaratorNode p_x);
# define abstDeclaratorNode_free(x) typeExpr_free(x);


