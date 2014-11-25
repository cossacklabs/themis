/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** cprim.h
*/

# ifndef CPRIM_H
# define CPRIM_H

/*
** These numbers are not enums, since we need to extend it with all other 
** types.  They index entries in cttable.
*/

immut_typedef int cprim;

/*@constant int CTX_UNKNOWN;@*/
# define CTX_UNKNOWN 0

/*@constant int CTX_VOID;@*/
# define CTX_VOID    1

/*@constant int CTX_BOOL;@*/
# define CTX_BOOL    2

/*@constant int CTX_UCHAR;@*/
# define CTX_UCHAR   3 /* unsigned char */

/*@constant int CTX_CHAR;@*/
# define CTX_CHAR    4

extern bool cprim_isUnsignedChar (/*@sef@*/ cprim p_c);
# define cprim_isUnsignedChar(c) ((c) == CTX_UCHAR)

extern bool cprim_isSignedChar (/*@sef@*/ cprim p_c);
# define cprim_isSignedChar(c) ((c) == CTX_CHAR)

extern bool cprim_isAnyChar (/*@sef@*/ cprim p_c);
# define cprim_isAnyChar(c) ((c) >= CTX_UCHAR && (c) <= CTX_CHAR)

/*
** only a few combiniations of type qualifiers and base types are
** recognized in the ANSI standard.  Splint treats these as distinct
** types.
*/

/*@constant int CTX_INT;@*/
# define CTX_INT      5

/*@constant int CTX_UINT;@*/ /* unsigned int */
# define CTX_UINT     6


/*@constant int CTX_SINT;@*/ /* short int */
# define CTX_SINT     7 

/*@constant int CTX_USINT;@*/ /* unsigned short int */
# define CTX_USINT    8 

/*@constant int CTX_LINT;@*/ /* long int */
# define CTX_LINT     9 

/*@constant int CTX_ULINT;@*/ /* unsigned long int */  
# define CTX_ULINT   10 

/*@constant int CTX_LLINT;@*/ /* long long int (WG14/N843) */  
# define CTX_LLINT   11 

/*@constant int CTX_ULLINT;@*/ /* unsigned long long int (WG14/N843) */  
# define CTX_ULLINT   12

/*@constant int CTX_ANYINTEGRAL;@*/
# define CTX_ANYINTEGRAL 13

/*@constant int CTX_UNSIGNEDINTEGRAL;@*/
# define CTX_UNSIGNEDINTEGRAL 14

/*@constant int CTX_SIGNEDINTEGRAL;@*/
# define CTX_SIGNEDINTEGRAL 15

/*@constant int CTX_FLOAT;@*/
# define CTX_FLOAT 16

/*@constant int CTX_DOUBLE;@*/
# define CTX_DOUBLE 17

/*@constant int CTX_LDOUBLE;@*/ /* long double */
# define CTX_LDOUBLE 18 

extern bool cprim_isAnyInt (/*@sef@*/ cprim p_c);
extern bool cprim_isAnyReal (/*@sef@*/ cprim p_c);

# define cprim_isAnyInt(c)         ((c) >= CTX_INT && (c) <= CTX_SIGNEDINTEGRAL)

extern bool cprim_isAnyUnsignedInt (/*@sef@*/ cprim p_c);
# define cprim_isAnyUnsignedInt(c) ((c) == CTX_UINT || (c) == CTX_USINT \
				    || (c) == CTX_ULINT || (c) == CTX_ULLINT \
				    || (c) == CTX_UNSIGNEDINTEGRAL)
# define cprim_isAnyReal(c)        ((c) >= CTX_FLOAT && (c) <= CTX_LDOUBLE)

/*@constant int CTX_LAST;@*/
# define CTX_LAST CTX_LDOUBLE

/*@constant cprim cprim_int@*/
# define cprim_int CTX_INT

extern bool cprim_equal (cprim, cprim);
# define cprim_equal(p1,p2) ((p1) == (p2))

extern bool cprim_closeEnough (cprim, cprim) /*@*/ ;
extern bool cprim_closeEnoughDeep (cprim, cprim) /*@*/ ;
extern /*@only@*/ cstring cprim_unparse (cprim) /*@*/ ;
extern cprim cprim_fromInt (int p_i);
extern bool cprim_isInt(cprim p_c);
extern int cprim_getExpectedBits (cprim p_c) /*@*/ ;

# else
# error "Multiple include"
# endif


