/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/

typedef struct {
  ltokenList traitid; /* a list of traitId ltoken's */
  /*@null@*/ renamingNode rename; /* a list of renamings */
} *traitRefNode;

extern void traitRefNode_free (/*@only@*/ /*@null@*/ traitRefNode p_x);
