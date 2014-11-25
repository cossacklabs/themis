/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** warnClause.h
*/

# ifndef WARNCLAUSE_H
# define WARNCLAUSE_H
  
struct s_warnClause
{
  /*@only@*/ fileloc loc;
  /*@only@*/ flagSpec flag;
  /*@only@*/ cstring msg;
} ;

/*@constant null warnClause warnClause_undefined; @*/
# define warnClause_undefined    ((warnClause) NULL)

extern /*@falsewhennull@*/ bool warnClause_isDefined (/*@null@*/ warnClause p_f) /*@*/ ;
extern /*@nullwhentrue@*/ bool warnClause_isUndefined (/*@null@*/ warnClause p_f) /*@*/ ;

# define warnClause_isDefined(f)   ((f) != warnClause_undefined)
# define warnClause_isUndefined(f) ((f) == warnClause_undefined)

extern warnClause warnClause_create (/*@only@*/ lltok,
				     /*@only@*/ flagSpec p_flag,
				     /*@only@*/ cstring p_msg) /*@*/ ;

extern /*@only@*/ warnClause warnClause_copy (warnClause) /*@*/ ;

extern /*@observer@*/ flagSpec warnClause_getFlag (warnClause p_w) /*@*/ ;

extern /*@only@*/ cstring warnClause_dump (warnClause p_wc) /*@*/ ;
extern /*@only@*/ warnClause warnClause_undump (char **p_s) /*@modifies p_s@*/ ;

extern /*@falsewhennull@*/ bool warnClause_hasMessage (warnClause p_w) /*@*/ ;

extern /*@observer@*/ cstring warnClause_getMessage (warnClause p_w) /*@*/ ;
extern /*@only@*/ cstring warnClause_unparse (warnClause p_w) /*@*/ ;

extern void warnClause_free (/*@only@*/ warnClause p_w);

# else
# error "Multiple include"
# endif













