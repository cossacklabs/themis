/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef struct {
  bool isOpId; /* TRUE means is opId */
  union {
    ltoken opid;
    /*@null@*/ opFormNode opform;
  } content;
} *nameNode;

extern void nameNode_free (/*@null@*/ /*@only@*/ nameNode p_n);
extern /*@only@*/ /*@null@*/ nameNode nameNode_copy (/*@null@*/ nameNode p_n) /*@*/ ;
extern /*@only@*/ cstring nameNode_unparse (/*@null@*/ nameNode p_n) /*@*/ ;
extern /*@only@*/ nameNode nameNode_copySafe (nameNode p_n) /*@*/ ;
