/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mtLoseReference.h
*/

# ifndef MTLoseReference_H
# define MTLoseReference_H

struct s_mtLoseReference {
  fileloc loc;
  cstring fromname;
  mtTransferAction action;
} ;

extern cstring mtLoseReference_unparse (mtLoseReference p_node) /*@*/ ;

extern mtLoseReference 
mtLoseReference_create (/*@only@*/ mttok p_fromname, 
			 /*@only@*/ mtTransferAction p_action) /*@*/ ;

extern /*@observer@*/ cstring mtLoseReference_getFrom (mtLoseReference) /*@*/ ;
# define mtLoseReference_getFrom(p_trans) ((p_trans)->fromname)

extern /*@observer@*/ mtTransferAction mtLoseReference_getAction (mtLoseReference) /*@*/ ;
# define mtLoseReference_getAction(p_trans) ((p_trans)->action)

extern /*@observer@*/ fileloc mtLoseReference_getLoc (mtLoseReference) /*@*/ ;
# define mtLoseReference_getLoc(p_trans) ((p_trans)->loc)

extern void mtLoseReference_free (/*@only@*/ mtLoseReference p_transfer) ;

# else
# error "Multiple include"
# endif
