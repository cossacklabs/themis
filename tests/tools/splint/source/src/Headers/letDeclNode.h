/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef struct {
  ltoken varid;
  /*@null@*/ lclTypeSpecNode sortspec;
  termNode term;
  sort sort;
} *letDeclNode;

extern void letDeclNode_free (/*@only@*/ /*@null@*/ letDeclNode p_x);
