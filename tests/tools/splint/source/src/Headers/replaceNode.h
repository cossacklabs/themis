/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef struct {
  ltoken tok;
  typeNameNode typename; 
  bool isCType; /* TRUE means it is a CType */
  union {
    struct { 
      /*@only@*/ /*@null@*/ nameNode name; 
      /*@only@*/ /*@null@*/ sigNode signature;
    } renamesortname;
    ltoken ctype; 
  } content;
} *replaceNode; 

extern void replaceNode_free (/*@only@*/ /*@null@*/ replaceNode p_x);
extern /*@only@*/ cstring replaceNode_unparse (/*@null@*/ replaceNode p_x);

