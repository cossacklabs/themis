/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/

typedef struct {
  stDeclNodeList structdecls;
  declaratorNode declarator; 
} *taggedUnionNode;

extern /*@unused@*/ /*@only@*/ cstring 
  taggedUnionNode_unparse (taggedUnionNode p_n) /*@*/ ;
