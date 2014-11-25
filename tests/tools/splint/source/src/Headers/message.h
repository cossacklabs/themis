/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef MESSAGE_H
# define MESSAGE_H

# if USEVARARGS
/*@-usevarargs@*/ /* suppress error about varargs.h */
# include <varargs.h>
/*@=usevarargs@*/
extern cstring message ();
# else
# include <stdarg.h>
/*@messagelike@*/
extern /*@only@*/ cstring message(/*@temp@*/ char *p_fmt, ...) /*@*/ ;
# endif

# else
# error "Multiple include"
# endif
