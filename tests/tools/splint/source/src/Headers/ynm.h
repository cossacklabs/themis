/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/

# if !defined(YNM_H)
# define YNM_H

typedef enum { NO, YES, MAYBE } ynm;

extern /*@observer@*/ cstring ynm_unparse (/*@sef@*/ ynm p_b) /*@*/ ;
extern /*@observer@*/ cstring ynm_unparseCode (/*@sef@*/ ynm p_b) /*@*/ ;
extern bool ynm_toBoolStrict (ynm p_b) /*@*/ ;
extern bool ynm_toBoolRelaxed (ynm p_b) /*@*/ ;
extern ynm ynm_fromBool (bool p_b) /*@*/ ;
extern bool ynm_isOff (ynm p_b) /*@*/ ;
extern bool ynm_isOn (ynm p_b) /*@*/ ;
extern bool ynm_isMaybe (ynm p_b) /*@*/ ;
extern int ynm_compare (ynm p_x, ynm p_y) /*@*/ ;
extern ynm ynm_fromCodeChar (char p_c) /*@*/ ;

# define ynm_unparse(b) \
  (cstring_makeLiteralTemp (((b) == NO) ? "no" : ((b) == YES) ? "yes" : "maybe"))
# define ynm_unparseCode(b) \
  (cstring_makeLiteralTemp (((b) == NO) ? "-" : ((b) == YES) ? "+" : "="))

# define ynm_toBoolStrict(y) ((y) == YES)
# define ynm_toBoolRelaxed(y) ((y) != NO)
# define ynm_fromBool(b)   ((b) ? YES : NO)
# define ynm_isOff(y)      ((y) == NO)
# define ynm_isOn(y)       ((y) == YES)
# define ynm_isMaybe(y)    ((y) == MAYBE)

# else
# error "Multiple include"
# endif

