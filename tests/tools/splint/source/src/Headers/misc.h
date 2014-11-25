/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** misc.h
**
** (general.c)
*/

# ifndef MISC_H
# define MISC_H

# include "lclMisc.h"

extern void assertSet (/*@special@*/ /*@sef@*/ /*@unused@*/ void *p_x) 
   /*@sets p_x, *p_x@*/ ;
# define assertSet(x) ;

extern void assertDefined (/*@sef@*/ /*@unused@*/ void *p_x) ;
# define assertDefined(x) ;


/*@-czechfcns@*/
extern int size_toInt (size_t p_x) /*@*/ /*@ensures result == p_x@*/;
extern long size_toLong (size_t p_x) /*@*/ /*@ensures result == p_x@*/ ;
extern size_t size_fromInt (int p_x) /*@*/ /*@ensures result == p_x@*/ ;
extern size_t size_fromLong (long p_x) /*@*/ /*@ensures result == p_x@*/ ;
extern size_t size_fromLongUnsigned (long unsigned p_x) /*@*/ /*@ensures result == p_x@*/ ;
extern int longUnsigned_toInt (long unsigned int p_x) /*@*/ /*@ensures result == p_x@*/ ;
extern int long_toInt (long p_x) /*@*/ /*@ensures result == p_x@*/;
extern long unsigned longUnsigned_fromInt (int p_x) /*@*/  /*@ensures result == p_x@*/ ;
/*@=czechfcns@*/

# include "mstring.h"

extern int int_compare (/*@sef@*/ int p_x, /*@sef@*/ int p_y) /*@*/ ;
# define int_compare(x,y) (((x) > (y)) ? 1 : (((x) < (y)) ? -1 : 0))

/*@-macroparams@*/
/*@-macrofcndecl@*/ /* works for lots of types */
# define generic_compare(x,y) (((x) > (y)) ? 1 : (((x) < (y)) ? -1 : 0)) 
/*@=macrofcndecl@*/
/*@=macroparams@*/

/*@notfunction@*/
# define GET(s) ((s *)smalloc(sizeof(s)))

/*@-czechfcns@*/
extern bool isHeaderFile (cstring) /*@*/ ;

extern void fputline (FILE *p_out, char *p_s) /*@modifies p_out@*/;

extern int int_log (int p_x) /*@*/ ;

extern char char_fromInt (int p_x) /*@*/ ;

extern /*@exposed@*/ cstring removePreDirs (cstring p_s);

/* These are defined by the bison library (?) */
extern /*@external@*/ int isatty (int);
extern /*@external@*/ int yywrap (void);
/*@=czechfcns@*/

# else
# error "Multiple include"
# endif





