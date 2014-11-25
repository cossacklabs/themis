/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** messageLog.h (from slist_templace.h)
*/

# ifndef messageLog_H
# define messageLog_H

typedef struct
{
  fileloc loc;
  cstring msg;
} *msgentry ;

typedef /*@only@*/ msgentry o_msgentry;

abst_typedef /*@null@*/ struct
{
  int        nelements;
  int        nspace;
  /*@reldef@*/ /*@only@*/ o_msgentry  *elements;
} *messageLog ;

/*@constant null messageLog messageLog_undefined; @*/
# define messageLog_undefined ((messageLog)0)

extern /*@unused@*/ /*@falsewhennull@*/ bool 
  messageLog_isDefined (messageLog p_s) /*@*/ ;
# define messageLog_isDefined(c) ((c) != messageLog_undefined)

extern /*@only@*/ messageLog messageLog_new (void) /*@*/ ;
extern bool messageLog_add (messageLog p_s, fileloc p_fl, cstring p_mess) 
            /*@modifies p_s@*/ ;

extern /*@only@*/ /*@unused@*/ cstring messageLog_unparse (messageLog p_s)  /*@*/ ;
extern void messageLog_free (/*@only@*/ messageLog p_s) ;

/*@constant int messageLogBASESIZE; @*/
# define messageLogBASESIZE MIDBASESIZE

# else
# error "Multiple include"
# endif




