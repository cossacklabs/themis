/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/       

# ifndef VARNODE_H
# define VARNODE_H

typedef struct { /* with sort, useful in quantified */
  ltoken varid;
  bool isObj; 
  lclTypeSpecNode type;
  sort sort;
} *varNode; 

extern varNode varNode_copy (varNode p_x);
extern void varNode_free (/*@only@*/ /*@null@*/ varNode p_x);

# else
# error "Multiple include"
# endif
