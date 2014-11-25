/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef struct {
  ltoken tok;
  ltokenList domain; 
  ltoken range;
  unsigned long int key;
} *sigNode;

extern /*@only@*/ cstring sigNode_unparse (/*@null@*/ sigNode p_n) /*@*/ ;
extern void sigNode_free (/*@only@*/ /*@null@*/ sigNode p_x);
extern /*@only@*/ sigNode sigNode_copy (sigNode p_s) /*@*/ ;
extern void sigNode_markOwned (/*@owned@*/ sigNode p_n);

