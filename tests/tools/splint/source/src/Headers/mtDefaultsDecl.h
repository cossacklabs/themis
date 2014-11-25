/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mtDefaultsDecl.h
*/

# ifndef mtDefaultsDecl_H
# define mtDefaultsDecl_H

struct s_mtDefaultsDecl {
  mtContextNode context;
  cstring value;
  fileloc loc;
} ;

extern cstring mtDefaultsDecl_unparse (mtDefaultsDecl p_node) /*@*/ ;

extern mtDefaultsDecl 
mtDefaultsDecl_create (/*@only@*/ mtContextNode p_context, 
		       /*@only@*/ mttok p_value) /*@*/ ;

extern /*@observer@*/ fileloc mtDefaultsDecl_getLoc (mtDefaultsDecl) /*@*/ ;
# define mtDefaultsDecl_getLoc(p_dd) ((p_dd)->loc)

extern /*@observer@*/ mtContextNode mtDefaultsDecl_getContext (mtDefaultsDecl) /*@*/ ;
# define mtDefaultsDecl_getContext(p_dd) ((p_dd)->context)

extern /*@observer@*/ cstring mtDefaultsDecl_getValue (mtDefaultsDecl) /*@*/ ;
# define mtDefaultsDecl_getValue(p_dd) ((p_dd)->value)

extern void mtDefaultsDecl_free (/*@only@*/ mtDefaultsDecl) ;

# else
# error "Multiple include"
# endif
