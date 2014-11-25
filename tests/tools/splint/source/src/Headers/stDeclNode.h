/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/

typedef struct {
  lclTypeSpecNode    lcltypespec;
  declaratorNodeList declarators;
} *stDeclNode;

extern void stDeclNode_free (/*@only@*/ /*@null@*/ stDeclNode p_x);
extern stDeclNode stDeclNode_copy (stDeclNode p_x);
