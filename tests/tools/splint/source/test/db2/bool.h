#ifndef BOOL_H
#define BOOL_H

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

typedef int bool;

/*
** bool_initMod has no real effect
** Declared with modifies internalState, so no noeffect errors are 
** reported when it is called.)
*/

extern /*@unused@*/ void bool_initMod (void) /*@modifies internalState@*/ ;
/*@-mustmod@*/
# define bool_initMod()
/*@=mustmod@*/

extern /*@unused@*/ /*@observer@*/ char *bool_unparse (bool b) /*@*/ ;
# define bool_unparse(b) ((b) ? "true" : "false" )

extern /*@unused@*/ bool bool_not (bool b) /*@*/ ;
# define bool_not(b) ((b) ? FALSE : TRUE)

extern /*@unused@*/ bool bool_equal (bool b1, bool b2) /*@*/ ;
# define bool_equal(a,b) ((a) ? (b) : !(b))

extern /*@falseexit@*/ void check (bool x);
# define check(x) \
  do { bool m_res = x; assert (m_res); } while (FALSE)

# endif
