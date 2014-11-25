/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef enum {
  PRIV_CONST, PRIV_VAR, 
  PRIV_TYPE, PRIV_FUNCTION 
  } privateKind;

typedef struct {
  privateKind kind;
  union {
    constDeclarationNode constdeclaration;
    varDeclarationNode vardeclaration;	
    typeNode type;
    fcnNode fcn;
  } content;
} *privateNode;

extern /*@unused@*/ /*@only@*/ cstring privateNode_unparse(privateNode p_n);
