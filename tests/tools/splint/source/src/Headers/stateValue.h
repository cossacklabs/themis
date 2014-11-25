/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** stateValue.h
*/

# ifndef STATEVALUE_H
# define STATEVALUE_H

/*
** Keeps track of the value of a state variable, as well as tracking
** information about its history.
*/

struct s_stateValue
{
  int value;
  bool implicit;
  /*@only@*/ stateInfo info;
};

extern /*@notnull@*/ stateValue stateValue_create (int p_value, /*@only@*/ stateInfo p_info) /*@*/ ;
extern /*@notnull@*/ stateValue stateValue_createImplicit (int p_value, /*@only@*/ stateInfo p_info) /*@*/ ;

/*@constant null stateValue stateValue_undefined@*/
# define stateValue_undefined (NULL)

extern /*@nullwhentrue@*/ bool stateValue_isUndefined (stateValue) /*@*/ ;
# define stateValue_isUndefined(p_s) ((p_s) == stateValue_undefined)

extern /*@falsewhennull@*/ bool stateValue_isDefined (stateValue) /*@*/ ;
# define stateValue_isDefined(p_s) ((p_s) != NULL)

extern bool stateValue_isImplicit (stateValue) /*@*/ ;

extern int stateValue_getValue (stateValue p_s) /*@*/ ;

extern void stateValue_update (stateValue p_res, stateValue p_val) /*@modifies p_res@*/ ;

extern /*@observer@*/ fileloc stateValue_getLoc (stateValue p_s) /*@*/ ;
# define stateValue_getLoc(p_s) (stateInfo_getLoc (stateValue_getInfo (p_s)))

extern bool stateValue_hasLoc (stateValue p_s) /*@*/ ;

extern /*@observer@*/ stateInfo stateValue_getInfo (stateValue p_s) /*@*/ ;

extern void stateValue_updateValue (/*@sef@*/ stateValue p_s, int p_value, /*@only@*/ stateInfo p_info) /*@modifies p_s@*/ ;

extern void stateValue_updateValueLoc (stateValue p_s, int p_value, fileloc p_loc) /*@modifies p_s@*/ ;

extern void stateValue_show (stateValue p_s, metaStateInfo p_msinfo) ;

extern stateValue stateValue_copy (stateValue p_s) /*@*/ ;

extern /*@only@*/ cstring 
   stateValue_unparseValue (stateValue p_s, metaStateInfo p_msinfo) /*@*/ ;

extern cstring stateValue_unparse (stateValue p_s) /*@*/ ;

extern bool stateValue_sameValue (stateValue p_s1, stateValue p_s2) /*@*/ ;

extern bool stateValue_isError (/*@sef@*/ stateValue p_s) /*@*/ ;
# define stateValue_isError(p_s) (stateValue_isDefined (p_s) && (stateValue_getValue (p_s) == stateValue_error))

/*@constant int stateValue_error@*/ 
# define stateValue_error -1

# else
# error "Multiple include"
# endif
