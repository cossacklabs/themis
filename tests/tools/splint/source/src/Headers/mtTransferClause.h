/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mtTransferClause.h
*/

# ifndef MTTRANSFERCLAUSE_H
# define MTTRANSFERCLAUSE_H

struct s_mtTransferClause {
  fileloc loc;
  cstring fromname;
  cstring toname;
  mtTransferAction action;
} ;

extern cstring mtTransferClause_unparse (mtTransferClause p_node) /*@*/ ;

extern mtTransferClause 
mtTransferClause_create (/*@only@*/ mttok p_fromname, /*@only@*/ mttok p_toname,
			 /*@only@*/ mtTransferAction p_action) /*@*/ ;

extern /*@observer@*/ cstring mtTransferClause_getFrom (mtTransferClause) /*@*/ ;
# define mtTransferClause_getFrom(p_trans) ((p_trans)->fromname)

extern /*@observer@*/ cstring mtTransferClause_getTo (mtTransferClause) /*@*/ ;
# define mtTransferClause_getTo(p_trans) ((p_trans)->toname)

extern /*@observer@*/ mtTransferAction mtTransferClause_getAction (mtTransferClause) /*@*/ ;
# define mtTransferClause_getAction(p_trans) ((p_trans)->action)

extern /*@observer@*/ fileloc mtTransferClause_getLoc (mtTransferClause) /*@*/ ;
# define mtTransferClause_getLoc(p_trans) ((p_trans)->loc)

extern void mtTransferClause_free (/*@only@*/ mtTransferClause p_transfer) ;

# else
# error "Multiple include"
# endif
