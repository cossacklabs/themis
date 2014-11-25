/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

# ifndef GENERAL_H
# define GENERAL_H

/* sgi's don't let you undef NULL */
# ifndef IRIX 
# ifdef NULL
# undef NULL
# endif
# endif

# include "forwardTypes.h"

# ifdef USEDMALLOC
# define sfree(x) do { if (x != NULL) free(x); } while (FALSE)
# else
extern void sfree (/*@out@*/ /*@only@*/ /*@null@*/ void *p_x) /*@modifies p_x@*/; 
# endif

# include "misc.h"
# include "cstring.h"
# include "bool.h"

extern /*@out@*/ /*@only@*/ void *dimalloc  (size_t p_size, const char *p_name, int p_line) /*@ensures maxSet(result) == (p_size - 1); @*/ ;
extern /*@only@*/ void *dicalloc  (size_t p_num, size_t p_size, const char *p_name, int p_line);
extern /*@notnull@*/ /*@out@*/ /*@only@*/ void *
  direalloc (/*@returned@*/ /*@only@*/ /*@out@*/ /*@null@*/ void *p_x, 
	     size_t p_size, char *p_name, int p_line);

extern /*@only@*/ void *
  drealloc (/*@special@*/ /*@null@*/ /*@sef@*/ void *p_x,
	    /*@sef@*/ size_t p_size)
  /*@releases p_x@*/
  /*@modifies *p_x@*/ ;

extern /*@out@*/ /*@only@*/ void *dmalloc  (/*@sef@*/ size_t p_size) /*@*/
        /*@ensures maxSet(result) == (p_size - 1); @*/ ;
	  

     /*drl 12/28/01 Work around for self checking */
#ifndef LINTBUFFERCHECK

# ifdef USEDMALLOC
# define dmalloc(s)    (malloc(s))
# define drealloc(s,l) (realloc(s,l))
# else
# define dmalloc(s)    (dimalloc(s, __FILE__, __LINE__))
# define drealloc(s,l) (direalloc(s, l, __FILE__, __LINE__))
# endif

#endif
     
# include "system_constants.h"

# ifdef USEGC
# include <gc.h>
# define NOFREE
# define free(s) ; /* nothing */ 
# else
# endif

# ifdef USEDMALLOC
# include "dmalloc.h"
# endif

/*
** no file except general.c should use primitive
** memory operations:
*/

/*@-exportlocal@*/
# ifndef USEDMALLOC
# undef malloc
# undef realloc
# undef calloc
# define malloc(s)     (dimalloc(s, __FILE__, __LINE__))
# define calloc(n, s)  (dicalloc(n, s, __FILE__, __LINE__))
# define realloc(v, s) (direalloc(v, s, __FILE__, __LINE__))
# endif
/*@=exportlocal@*/

# ifndef NULL
# define NULL 0
# endif 

extern void sfreeEventually (/*@owned@*/ /*@null@*/ void *p_x) 
   /*@modifies internalState@*/; 

typedef /*@dependent@*/ char *d_char;

/*@constant int NOT_FOUND;@*/
# define NOT_FOUND (-23)

unsigned int int_toNonNegative (int p_x) /*@*/;

# else
# error "Multiple include"
# endif






