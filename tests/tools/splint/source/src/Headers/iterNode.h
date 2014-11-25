/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef struct {
  ltoken name;
  paramNodeList params;
} *iterNode;

extern /*@unused@*/ /*@only@*/ cstring 
  iterNode_unparse (/*@null@*/ iterNode p_i) /*@*/ ;
