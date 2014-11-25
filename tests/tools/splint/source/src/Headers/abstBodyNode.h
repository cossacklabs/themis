/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

struct s_abstBodyNode {
  ltoken tok; /* check for NULLTOKEN before using it */
  /*@null@*/ lclPredicateNode typeinv;
  fcnNodeList fcns; /* only for abstBody, not for optExposedBody */
} ;

extern /*@unused@*/ /*@only@*/ cstring abstBodyNode_unparse (abstBodyNode p_n);

