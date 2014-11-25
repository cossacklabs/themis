/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mttok.h
*/

# ifndef MTTOK_H
# define MTTOK_H

struct s_mttok
{
  int tok;
  cstring text;
  fileloc loc;
} ;

/* in forwardTypes.h: abst_typedef struct _mttok *mttok; */

extern mttok mttok_create  (int p_tok,
			    /*@only@*/ cstring p_text,
			    /*@only@*/ fileloc p_loc) /*@modifies nothing@*/ ;
extern /*@only@*/ cstring mttok_unparse (mttok p_tok) /*@*/ ;

extern void mttok_free (/*@only@*/ mttok p_t) ;

extern int mttok_getTok (mttok p_t) /*@*/ ;
# define mttok_getTok(t)   ((t)->tok)

extern /*@dependent@*/ /*@exposed@*/ fileloc mttok_getLoc (mttok p_t) /*@*/ ;
# define mttok_getLoc(t)   ((t)->loc)

extern /*@only@*/ fileloc mttok_stealLoc (mttok p_t) /*@modifies p_t@*/ ;

extern /*@only@*/ cstring mttok_getText (mttok p_t) /*@*/ ;
# define mttok_getText(t)  (cstring_copy ((t)->text))

extern /*@observer@*/ cstring mttok_observeText (mttok p_t) /*@*/ ;
# define mttok_observeText(t)  ((t)->text)

extern bool mttok_isIdentifier (mttok p_t) /*@*/ ;
extern bool mttok_isError (mttok p_t) /*@*/ ;

# else
# error "Multiple include"
# endif
