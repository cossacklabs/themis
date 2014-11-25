/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef struct {
  ltoken name;
  /*@null@*/ lclTypeSpecNode typespec;
  declaratorNode declarator;
  globalList globals;
  varDeclarationNodeList inits;
  letDeclNodeList lets;
  /*@null@*/ lclPredicateNode checks;
  /*@null@*/ lclPredicateNode require;
  /*@null@*/ modifyNode modify;
  /*@null@*/ lclPredicateNode ensures;
  /*@null@*/ lclPredicateNode claim;
  qual special;
} *fcnNode;

extern void fcnNode_free (/*@null@*/ /*@only@*/ fcnNode p_f);
extern /*@only@*/ cstring fcnNode_unparse (/*@null@*/ fcnNode p_f);
