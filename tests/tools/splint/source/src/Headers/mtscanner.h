/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mtscanner.h
*/

# ifndef MTSCANNER_H
# define MTSCANNER_H

/*@-declundef@*/
extern void mtscanner_reset (inputStream p_sourceFile) 
     /*@modifies internalState@*/ ;

/*@-redecl@*/
extern int mtlex (YYSTYPE *p_mtlval)
     /*@modifies internalState, p_mtlval@*/ ;
/*@=redecl@*/

extern ctype mtscanner_lookupType (mttok p_tok) /*@modifies p_tok@*/ ;
   /* Can steal p_tok's location. */
/*@=declundef@*/

# else
# error "Multiple include"
# endif
