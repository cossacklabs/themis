/* ;-*-C-*-; 
**    Copyright (c) Massachusetts Institute of Technology 1994, 1995.
**          All Rights Reserved.
**          Unpublished rights reserved under the copyright laws of
**          the United States.
**
** THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY EXPRESSED
** OR IMPLIED.  ANY USE IS AT YOUR OWN RISK.
**
** This code is distributed freely and may be used freely under the 
** following conditions:
**
**     1. This notice may not be removed or altered.
**
**     2. This code may not be re-distributed or modified
**        without permission from MIT (contact 
**        lclint-request@larch.lcs.mit.edu.)  
**
**        Modification and re-distribution are encouraged,
**        but we want to keep track of changes and
**        distribution sites.
*/

# ifndef LCLINTMACROS_H
# define LCLINTMACROS_H


/*
** This file contains notfunction macros (hence, the .nf extension to
** prevent it being skipped when +neverinclude is used.)
*/

#ifndef PARAMS
#ifdef __STDC
/*@notfunction@*/
#define PARAMS(P) P
#else
/*@notfunction@*/
#define PARAMS(P) ()
#endif
#endif /* !PARAMS */

/*@notfunction@*/
# define BADEXIT \
 /*@notreached@*/ do { llassertprint(FALSE, ("Reached dead code!")); \
                       exit(EXIT_FAILURE); } while (FALSE) 

/*@notfunction@*/
# define BADBRANCH \
  /*@notreached@*/ BADBRANCHCONT

/*@notfunction@*/
# define BADBRANCHNULL \
  /*@notreached@*/ BADBRANCHCONT ; /*@notreached@*/ /*@-mustfree@*/ /*@-globstate@*/ return NULL; /*@=mustfree@*/ /*@=globstate@*/

/*@notfunction@*/
# define BADBRANCHCONT \
  do { llassertprint (FALSE, ("Bad branch taken!")); } while (FALSE) 

/*@notfunction@*/
# define BADBRANCHRET(r) \
  /*@notreached@*/ BADBRANCHCONT ; /*@notreached@*/ /*@-mustfree@*/ /*@-globstate@*/ return (r); /*@=mustfree@*/ /*@=globstate@*/

/*@notfunction@*/
# define BADDEFAULT \
  default: llassertprint (FALSE, ("Unexpected default case reached!")); \
           exit (EXIT_FAILURE);

/*@-namechecks@*/
/*@notfunction@*/
# define llassertprint(tst,p) \
   do { \
     if (!(tst)) { \
      llbug (message("%q:%d: llassert failed: " #tst ": %q", \
		     cstring_makeLiteral (__FILE__), __LINE__,\
		     /*@-mustfree@*/ message p) /*@=mustfree@*/ ); \
     }} while (FALSE)

/*@notfunction@*/
# define llassertprintret(tst,p,r) \
    do { if (!(tst)) \
         { llbug (message("%q:%d: %q", cstring_makeLiteral (__FILE__), __LINE__, message p)); \
           /*@-unreachable@*/ return (r); /*@=unreachable@*/ \
       } } while (FALSE)

/*@notfunction@*/
# define abst_typedef typedef /*@abstract@*/ 

/*@notfunction@*/
# define immut_typedef typedef /*@abstract@*/ /*@immutable@*/

/*@=namechecks@*/

/*
** SunOS4 can't handle bit fields correctly.
*/

# ifdef SYSSunOS
/*@notfunction@*/
# define BOOLBITS
# else 
/*@notfunction@*/
# define BOOLBITS : 1
# endif

/*
** some stupid proprocessors replace the s in %s...had
** to change s to arg.
*/

/*@notfunction@*/
# define NOALIAS(s,t) (/*@ignore@*/ (s == NULL) || (s != t) /*@end@*/)

/* evans 2002-02-24: got rid of -formatconst */

/*@notfunction@*/
# define TPRINTF(arg) \
  do { /*@-mustfree@*/ /*@-null@*/ (void) fflush (stderr); (void) fflush (stdout); \
	printf ("%s:%d [%s]: >> ", __FILE__, __LINE__, cstring_toCharsSafe (fileloc_unparse(g_currentloc))); \
	(void)printf arg; printf("\n"); /*@=mustfree@*/ /*@=null@*/ (void) fflush (stdout); \
	} while (FALSE)

/*
** DPRINTF does nothing, just a marker to save TPRINTF's
*/

/*@notfunction@*/
# define DPRINTF(s)   

/*@notfunction@*/
# define INTCOMPARERETURN(x,y) \
   do { if ((x) > (y)) { return 1; } \
        else { if ((x) < (y)) { return -1; }}} while (FALSE);

/*@notfunction@*/
# define COMPARERETURN(z) \
   do { if (z != 0) { return z; } } while (FALSE);

# else
# error "Multiple include"
# endif


