/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mtDefaultsNode.h
*/

# ifndef MTDEFAULTSNODE_H
# define MTDEFAULTSNODE_H

struct s_mtDefaultsNode {
  mtDefaultsDeclList decls;
  fileloc loc;
} ;

extern mtDefaultsNode mtDefaultsNode_create (/*@only@*/ mttok,
					     /*@only@*/ mtDefaultsDeclList) /*@*/ ;

extern void mtDefaultsNode_free (/*@only@*/ mtDefaultsNode) ;

extern /*@observer@*/ mtDefaultsDeclList mtDefaultsNode_getDecls (mtDefaultsNode) /*@*/ ;
# define mtDefaultsNode_getDecls(n) ((n)->decls)

extern cstring mtDefaultsNode_unparse (mtDefaultsNode p_node) /*@*/ ;

# else
# error "Multiple include"
# endif
