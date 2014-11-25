/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mtDeclarationNode.h
*/

# ifndef MTDECLARATIONNODE_H
# define MTDECLARATIONNODE_H

struct s_mtDeclarationNode {
  fileloc loc;
  cstring name;
  mtDeclarationPieces pieces;
} ;

/* mtDeclarationNode defined in forwardTypes.h */

extern mtDeclarationNode mtDeclarationNode_create (/*@only@*/ mttok,
						   /*@only@*/ mtDeclarationPieces) /*@*/ ;

extern /*@observer@*/ cstring mtDeclarationNode_getName (mtDeclarationNode) /*@*/ ;

extern /*@observer@*/ fileloc mtDeclarationNode_getLoc (mtDeclarationNode) /*@*/ ;

extern cstring mtDeclarationNode_unparse (mtDeclarationNode p_node) /*@*/ ;
extern void mtDeclarationNode_process (mtDeclarationNode p_node, bool p_isglobal) /*@modifies internalState@*/ ;
extern void mtDeclarationNode_free (/*@only@*/ mtDeclarationNode p_node) ;

# else
# error "Multiple include"
# endif
