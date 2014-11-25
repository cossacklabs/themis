/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef EKIND_H
# define EKIND_H

/* need to maintain compatibility with libraries */

immut_typedef enum {
  KINVALID = -1, 
  KDATATYPE = 0, /* must dump datatypes first, so ctypes are ok */
  KCONST, KENUMCONST,
  KVAR, KFCN, KITER, KENDITER,
  KSTRUCTTAG, KUNIONTAG, KENUMTAG,
  KELIPSMARKER
  } ekind;

/*@constant ekind KELAST;@*/
# define KELAST KELIPSMARKER

/*@constant int KGLOBALMARKER;@*/
# define KGLOBALMARKER ((int) KELAST + 1)

extern bool ekind_equal (ekind, ekind) /*@*/ ;
# define ekind_equal(e1, e2) ((e1) == (e2))

extern ekind ekind_fromInt (int p_i) /*@*/ ;
extern int ekind_toInt (ekind p_k) /*@*/ ;

extern bool ekind_isFunction (ekind p_k) /*@*/ ;
# define ekind_isFunction(k) ((k) == KFCN)

extern /*@unused@*/ bool ekind_isVariable (ekind p_k) /*@*/ ;
# define ekind_isVariable(k) ((k) == KVAR)

extern bool ekind_isElipsis (ekind p_k) /*@*/ ;
# define ekind_isElipsis(k)  ((k) == KELIPSMARKER)

extern bool ekind_isConst (ekind p_k) /*@*/ ;
# define ekind_isConst(k) ((k) == KCONST)

extern bool ekind_isEnumConst (ekind p_k) /*@*/ ;
# define ekind_isEnumConst(k) ((k) == KENUMCONST)

# define ekind_toInt(k)     ((int)(k))

/*@constant ekind ekind_variable; @*/
# define ekind_variable ((ekind)KVAR)

/*@constant ekind ekind_function; @*/
# define ekind_function ((ekind)KFCN)

extern /*@observer@*/ cstring ekind_capName (ekind p_k) /*@*/ ;
extern /*@observer@*/ cstring ekind_unparse (ekind p_k) /*@*/ ;
extern /*@observer@*/ cstring ekind_unparseLong (ekind p_k) /*@*/ ;

# else
# error "Multiple include"
# endif
