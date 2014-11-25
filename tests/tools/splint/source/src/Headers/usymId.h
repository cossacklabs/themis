# ifndef USYMID_H
# define USYMID_H

typedef /*@numabstract@*/ int usymId;

extern usymId usymId_fromInt (int p_i) /*@*/ ;
# define usymId_fromInt(i)   ((usymId)(i))

extern int usymId_toInt (usymId p_i) /*@*/ ;
# define usymId_toInt(i)   ((int)(i))

extern bool usymId_isInvalid (usymId p_u) /*@*/ ;
# define usymId_isInvalid(u) ((u) == usymId_invalid)

extern bool usymId_isValid (usymId p_u) /*@*/ ;
# define usymId_isValid(u)   ((u) != usymId_invalid)

extern bool usymId_equal (usymId p_u1, usymId p_u2) /*@*/ ;
# define usymId_equal(u1,u2) ((u1) == (u2))

/*@constant usymId usymId_invalid;@*/
# define usymId_invalid -17

/*@constant usymId usymId_notfound;@*/
# define usymId_notfound NOT_FOUND

# else
# error "Multiple include"
# endif
