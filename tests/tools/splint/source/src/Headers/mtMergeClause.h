/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mtMergeClause.h
*/

# ifndef MTMERGECLAUSE_H
# define MTMERGECLAUSE_H

struct s_mtMergeClause {
  mtMergeItem item1, item2;
  mtTransferAction action;
} ;

extern cstring mtMergeClause_unparse (mtMergeClause p_node) /*@*/ ;

extern mtMergeClause 
mtMergeClause_create (/*@only@*/ mtMergeItem p_item1, /*@only@*/ mtMergeItem p_item2, 
		      /*@only@*/ mtTransferAction p_action) ;

extern /*@observer@*/ mtMergeItem mtMergeClause_getItem1 (mtMergeClause) /*@*/ ;
# define mtMergeClause_getItem1(p_merge) ((p_merge)->item1)

extern /*@observer@*/ mtMergeItem mtMergeClause_getItem2 (mtMergeClause) /*@*/ ;
# define mtMergeClause_getItem2(p_merge) ((p_merge)->item2)

extern /*@observer@*/ mtTransferAction mtMergeClause_getAction (mtMergeClause) /*@*/ ;
# define mtMergeClause_getAction(p_merge) ((p_merge)->action)

extern /*@observer@*/ fileloc mtMergeClause_getLoc (mtMergeClause) /*@*/ ;
# define mtMergeClause_getLoc(p_merge) (mtMergeItem_getLoc ((p_merge)->item1))

extern void mtMergeClause_free (/*@only@*/ mtMergeClause p_merge) ;

# else
# error "Multiple include"
# endif
