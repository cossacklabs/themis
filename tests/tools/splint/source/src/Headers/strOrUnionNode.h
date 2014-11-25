/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/

typedef enum {SU_STRUCT, SU_UNION} suKind;

typedef struct {
  suKind kind;
  ltoken tok; /* also encodes if it's struct or union */
  ltoken opttagid;
  sort sort;
  /*@owned@*/ stDeclNodeList structdecls;
} *strOrUnionNode;

extern /*@unused@*/ cstring 
  strOrUnionNode_unparse (/*@null@*/ strOrUnionNode p_n);
