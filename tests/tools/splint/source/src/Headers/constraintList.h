/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2000.
** See ../LICENSE for license information.
*/
# ifndef constraintLIST_H
# define constraintLIST_H

typedef /*@only@*/ /*@notnull@*/ constraint o_constraint;

struct s_constraintList
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_constraint  *elements;
} ;

/*@constant null constraintList constraintList_undefined;@*/

# define constraintList_undefined ((constraintList)NULL)

extern /*@falsewhennull@*/ bool constraintList_isDefined (constraintList p_e) /*@*/ ;
extern /*@unused@*/ /*@nullwhentrue@*/ bool constraintList_isUndefined (constraintList p_e) /*@*/ ;
extern /*@nullwhentrue@*/ /*@unused@*/ bool constraintList_isError (constraintList p_e) /*@*/ ;

# define constraintList_isDefined(e)    ((e) != constraintList_undefined)
# define constraintList_isUndefined(e)  ((e) == constraintList_undefined)
# define constraintList_isError(e)      ((e) == constraintList_undefined)

extern bool constraintList_isEmpty (/*@sef@*/ constraintList p_e) /*@*/;

# define constraintList_isEmpty(e)      ((constraintList_isUndefined((e)) ) ||(( (e)->nelements == 0) ) )

extern constraintList constraintList_single (/*@only@*/ constraint) ;

extern constraintList constraintList_addListFree (/*@returned@*/ constraintList, /*@only@*/ constraintList) ;
extern constraintList constraintList_preserveCallInfo (/*@returned@*/ constraintList p_c, /*@observer@*/ /*@dependent@*/ exprNode p_fcn) ;

/*@iter constraintList_elements (sef constraintList x, yield exposed constraint el); @*/
# define constraintList_elements(x, m_el) \
   { if (constraintList_isDefined (x)) { int m_ind; constraint *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { constraint m_el = *(m_elements++); 

# define end_constraintList_elements }}}

extern /*@only@*/ constraintList constraintList_makeNew(void) /*@*/;
extern constraintList constraintList_add (/*@returned@*/ constraintList p_s, /*@only@*/ constraint p_el) /*@modifies p_s@*/ ;

     extern /*@only@*/ constraintList constraintList_addList (/*@only@*/ /*@returned@*/ constraintList p_s, /*@observer@*/ /*@temp@*/ constraintList p_newList) /*@modifies p_s@*/  ;


extern constraintList constraintList_copy ( /*@observer@*/ /*@temp@*/ constraintList p_s) /*@*/ ;


extern void constraintList_free (/*@only@*/ constraintList p_s) ;


extern /*@only@*/ cstring constraintList_unparse (/*@observer@*/ constraintList p_s) /*@*/;

extern cstring constraintList_unparseDetailed (/*@observer@*/ constraintList p_s) /*@*/;

extern /*@only@*/ constraintList
constraintList_logicalOr ( /*@observer@*/ constraintList p_l1, /*@observer@*/  constraintList p_l2);

extern constraintList constraintList_preserveOrig (/*@returned@*/ constraintList p_c);

/*@constant int constraintListBASESIZE;@*/

# define constraintListBASESIZE SMALLBASESIZE

extern  /*@only@*/ constraintList constraintList_doSRefFixBaseParam ( constraintList p_preconditions, /*@temp@*/ /*@observer@*/ exprNodeList p_arglist) /*@modifies p_preconditions@*/;

extern constraintList constraintList_togglePost (/*@returned@*/ constraintList p_c) /*@modifies p_c@*/;

extern  /*@only@*/ constraintList constraintList_doSRefFixConstraintParam ( /*@only@*/ constraintList p_preconditions, /*@observer@*/ /*@temp@*/ exprNodeList p_arglist) /*@modifies p_preconditions@*/;

extern constraintList exprNode_getPostConditions (/*@dependent@*/ /*@observer@*/ exprNode p_fcn, exprNodeList p_arglist, /*@dependent@*/ /*@observer@*/ exprNode p_fcnCall) /*@*/;

/*@only@*/ constraintList constraintList_doFixResult ( /*@only@*/ constraintList p_postconditions, /*@observer@*/ /*@dependent@*/ exprNode p_fcnCall) /*@modifies p_postconditions@*/;

extern constraintList constraintList_addGeneratingExpr (/*@returned@*/ constraintList p_c, /*@dependent@*/ /*@observer@*/ exprNode p_e) /*@modifies p_c@*/;
extern /*@only@*/ constraintList constraintList_makeFixedArrayConstraints ( /*@observer@*/ sRefSet p_s) ;
extern void constraintList_printErrorPostConditions (constraintList p_s, fileloc p_loc) ;
extern void constraintList_printError (constraintList p_s, /*@observer@*/ fileloc p_loc) ;

extern constraintList constraintList_sort (/*@returned@*/ constraintList p_ret)
   /*@modifies p_ret@*/ ;

extern void constraintList_dump (/*@observer@*/ constraintList p_c,  FILE * p_f);

/*@only@*/ constraintList constraintList_undump (FILE * p_f);
/*@only@*/ constraintList constraintList_removeSurpressed (/*@only@*/ constraintList p_s);

/*drl 1/6/2001: I didn't think these functions were solid enough to include in the   stable  release of splint.*/
/*drl added 12/30/01 */
/* / *@only@* / constraintList constraintList_doSRefFixStructConstraint(constraintList p_invars, sRef p_s, ctype p_ct ); */

# else
# error "Multiple include"
# endif




