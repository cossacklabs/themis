/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mtAnnotationsNode.h
*/

# ifndef MTANNOTATIONSNODE_H
# define MTANNOTATIONSNODE_H

struct s_mtAnnotationsNode {
  mtAnnotationList annots;
} ;

extern mtAnnotationsNode mtAnnotationsNode_create (/*@only@*/ mtAnnotationList p_annots) /*@*/ ;

extern /*@observer@*/ mtAnnotationList mtAnnotationsNode_getAnnotations (mtAnnotationsNode p_node) /*@*/ ;
# define mtAnnotationsNode_getAnnotations(node) ((node)->annots)

extern cstring mtAnnotationsNode_unparse (mtAnnotationsNode p_node) /*@*/ ;
extern void mtAnnotationsNode_free (/*@only@*/ mtAnnotationsNode) ;

# else
# error "Multiple include"
# endif
