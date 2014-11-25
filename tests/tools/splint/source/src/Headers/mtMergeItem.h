/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mtMergeItem.h
*/

# ifndef MTMERGEITEM_H
# define MTMERGEITEM_H

struct s_mtMergeItem {
  enum { MTMK_VALUE, MTMK_STAR } kind;
  cstring value; /* for MTAK_VALUE only */
  fileloc loc;
} ;

extern void mtMergeItem_free (/*@only@*/ mtMergeItem p_node) ;

extern cstring mtMergeItem_unparse (mtMergeItem p_node) /*@*/ ;

extern mtMergeItem 
mtMergeItem_createValue (/*@only@*/ mttok p_value) /*@*/ ;

extern mtMergeItem 
mtMergeItem_createStar (/*@only@*/ mttok p_value) /*@*/ ;

extern bool mtMergeItem_isStar (mtMergeItem) /*@*/ ;
# define mtMergeItem_isStar(p_mi) ((p_mi)->kind == MTMK_STAR)

extern /*@observer@*/ cstring mtMergeItem_getValue (mtMergeItem) /*@*/ ;
# define mtMergeItem_getValue(node) ((node)->value)

extern /*@observer@*/ fileloc mtMergeItem_getLoc (mtMergeItem) /*@*/ ;
# define mtMergeItem_getLoc(node) ((node)->loc)

# else
# error "Multiple include"
# endif
