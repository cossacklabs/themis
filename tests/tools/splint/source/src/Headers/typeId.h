# ifndef TYPEID_H
# define TYPEID_H

typedef /*@numabstract@*/ usymId typeId;
/*@access usymId@*/

extern bool typeId_isInvalid (typeId p_u) /*@*/ ;
# define typeId_isInvalid(u) ((u) == typeId_invalid)

extern bool typeId_isValid (typeId p_u) /*@*/ ;
# define typeId_isValid(u)   ((u) != typeId_invalid)

extern bool typeId_equal (typeId p_u1, typeId p_u2) /*@*/ ;
# define typeId_equal(u1,u2) ((u1) == (u2))

extern typeId typeId_fromInt (int p_i) /*@*/ ;
# define typeId_fromInt(i)   ((typeId) (i))

extern typeId typeId_fromUsymId (usymId p_u) /*@*/ ;
# define typeId_fromUsymId(u) ((typeId) (u))

extern usymId typeId_toUsymId (typeId p_u) /*@*/ ;
# define typeId_toUsymId(u) ((usymId) (u))

extern int typeId_compare (/*@sef@*/ typeId p_x, /*@sef@*/ typeId p_y) /*@*/ ;
# define typeId_compare(x,y) (int_compare(x, y))

/*@constant typeId typeId_invalid;@*/
# define typeId_invalid usymId_invalid

/*@noaccess usymId@*/

# else
# error "Multiple include"
# endif
