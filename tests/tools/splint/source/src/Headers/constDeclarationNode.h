/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef struct {
  lclTypeSpecNode type;
  initDeclNodeList decls;
} *constDeclarationNode;

extern /*@unused@*/ /*@only@*/ cstring 
  constDeclarationNode_unparse (/*@null@*/ constDeclarationNode p_x);
