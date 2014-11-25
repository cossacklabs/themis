/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef struct {
  /*@null@*/ /*@only@*/ nameNode name;
  /*@dependent@*/ sigNode signature; /* optional */
} *lslOp;

typedef /*@only@*/ lslOp o_lslOp;

extern void lslOp_free (/*@only@*/ lslOp p_x);
extern /*@only@*/ lslOp lslOp_copy (lslOp);
