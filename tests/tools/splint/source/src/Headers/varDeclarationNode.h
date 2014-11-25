/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/

# ifndef VARDECLNODE_H
# define VARDECLNODE_H

typedef struct {
  bool isSpecial;
  /*@dependent@*/ /*@reldef@*/ sRef sref;
  bool isGlobal; /* global or varDeclaration */
  bool isPrivate; /* static variable, within a function defn */
  qualifierKind qualifier; /* QLF_NONE, QLF_CONST, or QLF_VOLATILE */
  lclTypeSpecNode type;
  initDeclNodeList decls;
} *varDeclarationNode;

extern void varDeclarationNode_free (/*@only@*/ /*@null@*/ varDeclarationNode p_x);
extern /*@unused@*/ /*@only@*/ cstring 
  varDeclarationNode_unparse (/*@null@*/ varDeclarationNode p_x) /*@*/;

# else
# error "Multiple include"
# endif

