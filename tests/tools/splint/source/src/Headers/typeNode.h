/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/

# ifndef TYPENODE_H
# define TYPENODE_H

typedef struct {
  typeKind kind;
  union {
    abstractNode abstract;
    exposedNode exposed;
    taggedUnionNode taggedunion;  
  } content;
} *typeNode;

extern /*@unused@*/ /*@only@*/ cstring
  typeNode_unparse (/*@null@*/ typeNode p_t) /*@*/ ;

# else
# error "Multiple include"
# endif
