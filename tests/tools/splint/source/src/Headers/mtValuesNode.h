/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mtValuesNode.h
*/

# ifndef MTVALUESNODE_H
# define MTVALUESNODE_H

struct s_mtValuesNode {
  cstringList values;
} ;

extern mtValuesNode mtValuesNode_create (/*@only@*/ cstringList p_values) /*@*/ ;
extern void mtValuesNode_free (/*@only@*/ mtValuesNode) ;

extern cstring mtValuesNode_unparse (mtValuesNode p_node) /*@*/ ;

extern /*@observer@*/ cstringList mtValuesNode_getValues (mtValuesNode p_node) /*@*/ ;
# define mtValuesNode_getValues(node) ((node)->values)

# else
# error "Multiple include"
# endif
