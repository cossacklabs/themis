/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mtTransferAction.h
*/

# ifndef MTTRANSFERACTION_H
# define MTTRANSFERACTION_H

struct s_mtTransferAction {
  enum { MTAK_VALUE, MTAK_ERROR } kind;
  cstring value; /* for MTAK_VALUE only */
  cstring message;
  fileloc loc;
} ;

extern void mtTransferAction_free (/*@only@*/ mtTransferAction p_node) ;

extern cstring mtTransferAction_unparse (mtTransferAction p_node) /*@*/ ;

extern mtTransferAction 
mtTransferAction_createValue (/*@only@*/ mttok p_value) /*@*/ ;

extern /*@observer@*/ cstring mtTransferAction_getValue (mtTransferAction) /*@*/ ;
# define mtTransferAction_getValue(node) ((node)->value)

extern /*@observer@*/ fileloc mtTransferAction_getLoc (mtTransferAction) /*@*/ ;
# define mtTransferAction_getLoc(node) ((node)->loc)

extern /*@observer@*/ cstring mtTransferAction_getMessage (mtTransferAction) /*@*/ ;

extern bool mtTransferAction_isError (mtTransferAction) /*@*/ ;
# define mtTransferAction_isError(node) ((node)->kind == MTAK_ERROR)

extern mtTransferAction mtTransferAction_createError (/*@only@*/ mttok) /*@*/ ;

extern mtTransferAction 
mtTransferAction_createErrorMessage (/*@only@*/ mttok p_message) /*@*/ ;

# else
# error "Multiple include"
# endif
