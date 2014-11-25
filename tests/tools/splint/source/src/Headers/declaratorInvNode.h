/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** declaratorInvNode.h
*/

typedef struct {
  declaratorNode declarator;
  abstBodyNode body;
} *declaratorInvNode;

extern void declaratorInvNode_free (/*@only@*/ /*@null@*/ declaratorInvNode p_x);
extern /*@only@*/ cstring declaratorInvNode_unparse (declaratorInvNode p_d);

