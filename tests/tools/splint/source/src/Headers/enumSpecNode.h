/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef struct {
  ltoken tok;
  ltoken opttagid;
  /*@owned@*/ ltokenList enums;
  sort sort;
} *enumSpecNode;

extern /*@unused@*/ /*@only@*/ 
  cstring enumSpecNode_unparse(/*@null@*/ enumSpecNode p_n) /*@*/ ;

