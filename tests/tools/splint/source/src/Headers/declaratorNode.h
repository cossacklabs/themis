/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** declaratorNode.h
*/

typedef struct {
  ltoken id;
  /*@null@*/ typeExpr type;
  bool isRedecl;
} *declaratorNode;

extern /*@only@*/ cstring declaratorNode_unparse(declaratorNode p_x);
extern void declaratorNode_free (/*@null@*/ /*@only@*/ declaratorNode p_x);
