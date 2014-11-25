/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mtAnnotationDecl.h
*/

# ifndef MTANNOTATIONDECL_H
# define MTANNOTATIONDECL_H

struct s_mtAnnotationDecl {
  cstring name;
  cstring value;
  mtContextNode context;
  fileloc loc;
} ;

extern cstring mtAnnotationDecl_unparse (mtAnnotationDecl p_node) /*@*/ ;

extern mtAnnotationDecl 
mtAnnotationDecl_create (/*@only@*/ mttok p_id, /*@only@*/ mtContextNode p_c, /*@only@*/ mttok p_value) /*@*/ ;

extern /*@observer@*/ cstring mtAnnotationDecl_getName (mtAnnotationDecl p_annot) /*@*/ ;
# define mtAnnotationDecl_getName(annot) ((annot)->name)

extern /*@observer@*/ cstring mtAnnotationDecl_getValue (mtAnnotationDecl p_annot) /*@*/ ;
# define mtAnnotationDecl_getValue(annot) ((annot)->value)

extern /*@only@*/ mtContextNode mtAnnotationDecl_stealContext (mtAnnotationDecl p_annot) /*@modifies p_annot@*/ ;

extern /*@observer@*/ mtContextNode mtAnnotationDecl_getContext (mtAnnotationDecl p_annot) /*@*/ ;
# define mtAnnotationDecl_getContext(annot) ((annot)->context)

extern /*@observer@*/ fileloc mtAnnotationDecl_getLoc (mtAnnotationDecl p_annot) /*@*/ ;
# define mtAnnotationDecl_getLoc(annot) ((annot)->loc) /*@i25@*/

# else
# error "Multiple include"
# endif
