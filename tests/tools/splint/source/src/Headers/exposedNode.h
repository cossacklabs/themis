/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef struct {
  ltoken tok;
  lclTypeSpecNode type;
  declaratorInvNodeList decls;
} *exposedNode;

extern /*@unused@*/ /*@only@*/ cstring exposedNode_unparse (exposedNode p_n);

