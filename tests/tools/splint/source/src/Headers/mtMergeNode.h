/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mtMergeNode.h
*/

# ifndef MTMERGENODE_H
# define MTMERGENODE_H

struct s_mtMergeNode {
  mtMergeClauseList mlist;
} ;

extern mtMergeNode mtMergeNode_create (/*@only@*/ mtMergeClauseList) /*@*/ ;
extern void mtMergeNode_free (/*@only@*/ mtMergeNode) ;

extern cstring mtMergeNode_unparse (mtMergeNode p_node) /*@*/ ;

extern /*@observer@*/ mtMergeClauseList mtMergeNode_getClauses (mtMergeNode p_node) /*@*/ ;
# define mtMergeNode_getClauses(p_node) ((p_node)->mlist)

# else
# error "Multiple include"
# endif
