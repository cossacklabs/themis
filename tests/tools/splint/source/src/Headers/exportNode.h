/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef enum 
{ 
  XPK_CONST, XPK_VAR, XPK_TYPE, 
  XPK_FCN, XPK_CLAIM, XPK_ITER 
} exportKind;

typedef struct {
  exportKind kind;
  union {
    constDeclarationNode constdeclaration;
    varDeclarationNode vardeclaration;
    typeNode type;
    fcnNode fcn;
    claimNode claim;
    iterNode iter;
  } content;
} *exportNode;

extern /*@unused@*/ /*@only@*/ cstring exportNode_unparse (exportNode p_n);

