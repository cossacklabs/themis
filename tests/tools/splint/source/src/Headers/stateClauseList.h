/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** stateClauseList.h
*/

# ifndef STATECLAUSELIST_H
# define STATECLAUSELIST_H

struct s_stateClauseList  
{
  int nelements;
  int nspace;
  /*@relnull@*/ /*@reldef@*/ o_stateClause *elements;
} ;

extern void stateClauseList_checkAll (uentry p_ue) 
   /*@modifies p_ue, g_warningstream@*/ ;

/*@constant null stateClauseList stateClauseList_undefined@*/
# define stateClauseList_undefined ((stateClauseList) 0)

extern /*@falsewhennull@*/ bool stateClauseList_isDefined (stateClauseList p_s) /*@*/ ;
# define stateClauseList_isDefined(s) ((s) != stateClauseList_undefined)

extern /*@nullwhentrue@*/ bool stateClauseList_isUndefined (stateClauseList p_s) /*@*/ ;
# define stateClauseList_isUndefined(s) ((s) == stateClauseList_undefined)

extern /*@unused@*/ int 
  stateClauseList_size (/*@sef@*/ stateClauseList p_s) /*@*/ ;
# define stateClauseList_size(s) (stateClauseList_isDefined (s) ? (s)->nelements : 0)

extern cstring stateClause_unparseKind (stateClause p_s) /*@*/ ;

extern stateClauseList
  stateClauseList_add (/*@returned@*/ stateClauseList p_s, 
		      /*@only@*/ stateClause p_el)  
  /*@modifies p_s@*/ ;

extern /*@unused@*/ cstring stateClauseList_unparse (stateClauseList p_s) /*@*/ ;
extern void stateClauseList_free (/*@only@*/ stateClauseList p_s) ;

extern /*@only@*/ stateClauseList stateClauseList_copy (stateClauseList p_s) /*@*/ ;

extern cstring stateClauseList_dump (stateClauseList p_s) /*@*/ ;
extern stateClauseList stateClauseList_undump (char **p_s) /*@modifies *p_s@*/ ;

extern int stateClauseList_compare (stateClauseList p_s1, stateClauseList p_s2) /*@*/ ;

/*@constant int stateClauseListBASESIZE;@*/
# define stateClauseListBASESIZE MIDBASESIZE

extern void stateClauseList_checkEqual (uentry p_old, uentry p_unew) 
  /*@modifies g_warningstream@*/ ;

/*@iter stateClauseList_elements (sef stateClauseList x, yield exposed stateClause el); @*/
# define stateClauseList_elements(x, m_el) \
   { if (!stateClauseList_isUndefined(x)) \
       { int m_ind; stateClause *m_elements = &((x)->elements[0]); \
          for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
           { stateClause m_el = *(m_elements++); 

# define end_stateClauseList_elements }}}

/*@iter stateClauseList_preElements (sef stateClauseList x, yield exposed stateClause el); @*/
# define stateClauseList_preElements(x, m_el) \
   { if (!stateClauseList_isUndefined(x)) \
       { int m_ind; stateClause *m_elements = &((x)->elements[0]); \
          for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
           { stateClause m_el = *(m_elements++); if (stateClause_isBefore (m_el)) { \

# define end_stateClauseList_preElements }}}}

/*@iter stateClauseList_postElements (sef stateClauseList x, yield exposed stateClause el); @*/
# define stateClauseList_postElements(x, m_el) \
   { if (!stateClauseList_isUndefined(x)) \
       { int m_ind; stateClause *m_elements = &((x)->elements[0]); \
          for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
           { stateClause m_el = *(m_elements++); if (stateClause_isAfter (m_el)) { \

# define end_stateClauseList_postElements }}}}

# else
# error "Multiple include"
# endif


