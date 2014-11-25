/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

# ifndef FUNCTIONCLAUSELIST_H
# define FUNCTIONCLAUSELIST_H

/*:private:*/ typedef /*@only@*/ functionClause o_functionClause;

struct s_functionClauseList
{
  int      nelements;
  int      nspace;
  /*@reldef@*/ /*@relnull@*/ o_functionClause *elements;
} ;

/*@constant null functionClauseList functionClauseList_undefined;@*/
# define functionClauseList_undefined ((functionClauseList) NULL)

extern /*@falsewhennull@*/ bool functionClauseList_isDefined (functionClauseList p_s) /*@*/ ;
# define functionClauseList_isDefined(s)   ((s) != functionClauseList_undefined)

extern /*@nullwhentrue@*/ bool functionClauseList_isUndefined (functionClauseList p_s) /*@*/ ;
# define functionClauseList_isUndefined(s)   ((s) == functionClauseList_undefined)

extern int functionClauseList_size (/*@sef@*/ functionClauseList) /*@*/ ;
# define functionClauseList_size(s) (functionClauseList_isDefined (s) ? (s)->nelements : 0)

extern /*@unused@*/ /*@falsewhennull@*/ bool functionClauseList_empty (/*@sef@*/ functionClauseList) /*@*/ ;
# define functionClauseList_empty(s) (functionClauseList_size(s) == 0)

extern cstring functionClauseList_unparseSep (functionClauseList p_s, cstring p_sep) /*@*/ ;

extern /*@unused@*/ /*@only@*/ functionClauseList functionClauseList_new (void) /*@*/ ;

extern /*@only@*/ functionClauseList functionClauseList_single (/*@keep@*/ functionClause p_el) /*@*/ ;

extern /*@unused@*/ functionClauseList 
  functionClauseList_add (/*@returned@*/ functionClauseList p_s, /*@keep@*/ functionClause p_el) 
  /*@modifies p_s@*/ ;

extern /*@only@*/ functionClauseList 
  functionClauseList_prepend (/*@only@*/ functionClauseList p_s, /*@keep@*/ functionClause p_el) 
  /*@modifies p_s@*/ ;

extern /*@unused@*/ /*@only@*/ cstring functionClauseList_unparse (functionClauseList p_s) ;
extern void functionClauseList_free (/*@only@*/ functionClauseList p_s) ;

functionClauseList 
functionClauseList_setImplicitConstraints (/*@returned@*/ functionClauseList p_s);
     
/*@constant int functionClauseListBASESIZE;@*/
# define functionClauseListBASESIZE MIDBASESIZE

/*@iter functionClauseList_elements (sef functionClauseList x, yield exposed functionClause el); @*/
# define functionClauseList_elements(x, m_el) \
   { if (functionClauseList_isDefined (x)) { \
       int m_ind; functionClause *m_elements = &((x)->elements[0]); \
	 for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
	   { functionClause m_el = *(m_elements++); 

# define end_functionClauseList_elements }}}

# else
# error "Multiple include"
# endif




