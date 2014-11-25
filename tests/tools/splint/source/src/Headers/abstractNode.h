/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef struct {
  ltoken tok;
  bool isMutable; /* TRUE means it is mutable */
  bool isRefCounted;
  ltoken name;
  sort sort;
  abstBodyNode body;
} *abstractNode;

extern /*@unused@*/ /*@only@*/ cstring abstractNode_unparse(abstractNode p_n);
