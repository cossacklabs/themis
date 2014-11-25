#ifndef BOOL_H
#define BOOL_H

/*@-cppnames@*/
/*@-exporttype@*/
typedef /*@abstract@*/ int bool;
/*@=exporttype@*/
/*@=cppnames@*/

#ifndef FALSE
/*@constant unused bool FALSE@*/
#define FALSE false
#endif

#ifndef TRUE
/*@constant unused bool TRUE@*/
#define TRUE true
#endif

/*@-slovakfcns@*/
extern /*@unused@*/ void bool_initMod (void);
# define bool_initMod()

extern /*@unused@*/ /*@observer@*/ char *bool_unparse (bool);
# define bool_unparse(b) ((b) ? "true" : "false" )

extern /*@unused@*/ bool bool_not (bool);
# define bool_not(b) ((b) ? FALSE : TRUE)

extern /*@unused@*/ bool bool_equal (bool, bool);
# define bool_equal(a,b) ((a) ? (b) : !(b))
/*@=slovakfcns@*/

# endif
