/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# if !defined(BOOL_H)

# define BOOL_H

# if defined (WIN32) || defined (OS2) && defined (__IBMC__)
# ifndef FALSE
/*@constant bool FALSE=false@*/
# define FALSE 0
# endif

# ifndef TRUE
/*@constant bool TRUE=true@*/
# define TRUE (!FALSE)
# endif
# else
# ifndef FALSE
/*@constant bool FALSE=false@*/
# define FALSE false
# endif

# ifndef TRUE
/*@constant bool TRUE=true@*/
# define TRUE true
# endif
# endif

extern /*@observer@*/ cstring bool_unparse (bool p_b) /*@*/ ;
extern /*@observer@*/ cstring bool_dump (bool p_b) /*@*/ ;
extern /*@unused@*/ bool bool_not (bool p_b) /*@*/ ;
extern bool bool_equal (bool p_b1, bool p_b2) /*@*/ ;
extern int bool_compare (bool p_b1, bool p_b2) /*@*/ ;
extern bool bool_fromInt (int p_i) /*@*/ ;
extern int bool_toInt (bool p_b) /*@*/ ;

extern bool bool_isReasonable (/*@sef@*/ bool p_b) /*@*/ ;
/*@-boolcompare@*/
# define bool_isReasonable(b) (((b) == TRUE) || ((b) == FALSE))
/*@=boolcompare@*/

# define bool_unparse(b)   (cstring_makeLiteralTemp ((b) ? "true" : "false" ))
# define bool_dump(b)      (cstring_makeLiteralTemp ((b) ? "1" : "0" ))
# define bool_not(b)       ((b) ? FALSE : TRUE)
# define bool_equal(a,b)   ((a) ? (b) : !(b))
# define bool_compare(a,b) ((a) ? ((b) ? 0 : 1) : ((b) ? -1 : 0))
# define bool_fromInt(i)   (((i) != 0) ? TRUE : FALSE)
# define bool_toInt(b)     ((b) ? 1 : 0)

# else
# error "Multiple include"
# endif




