/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

# ifndef MTMERGECLAUSELIST_H
# define MTMERGECLAUSELIST_H

/*:private:*/ typedef /*@only@*/ mtMergeClause o_mtMergeClause;

struct s_mtMergeClauseList
{
  int      nelements;
  int      nspace;
  /*@reldef@*/ /*@relnull@*/ o_mtMergeClause *elements;
} ;

/*@constant null mtMergeClauseList mtMergeClauseList_undefined;@*/
# define mtMergeClauseList_undefined ((mtMergeClauseList) NULL)

extern /*@falsewhennull@*/ bool mtMergeClauseList_isDefined (mtMergeClauseList p_s) /*@*/ ;
# define mtMergeClauseList_isDefined(s)   ((s) != mtMergeClauseList_undefined)

extern int mtMergeClauseList_size (/*@sef@*/ mtMergeClauseList) /*@*/ ;
# define mtMergeClauseList_size(s) (mtMergeClauseList_isDefined (s) ? (s)->nelements : 0)

extern /*@unused@*/ /*@falsewhennull@*/ bool mtMergeClauseList_empty (/*@sef@*/ mtMergeClauseList) /*@*/ ;
# define mtMergeClauseList_empty(s) (mtMergeClauseList_size(s) == 0)

extern cstring mtMergeClauseList_unparseSep (mtMergeClauseList p_s, cstring p_sep) /*@*/ ;

extern /*@unused@*/ /*@only@*/ mtMergeClauseList mtMergeClauseList_new (void) /*@*/ ;

extern /*@only@*/ mtMergeClauseList mtMergeClauseList_single (/*@keep@*/ mtMergeClause p_el) /*@*/ ;

extern /*@unused@*/ mtMergeClauseList 
  mtMergeClauseList_add (/*@returned@*/ mtMergeClauseList p_s, /*@keep@*/ mtMergeClause p_el) 
  /*@modifies p_s@*/ ;

extern mtMergeClauseList 
  mtMergeClauseList_prepend (/*@only@*/ mtMergeClauseList p_s, /*@only@*/ mtMergeClause p_el) 
  /*@modifies p_s@*/ ;

extern /*@unused@*/ /*@only@*/ cstring mtMergeClauseList_unparse (mtMergeClauseList p_s) ;
extern void mtMergeClauseList_free (/*@only@*/ mtMergeClauseList p_s) ;

/*@constant int mtMergeClauseListBASESIZE;@*/
# define mtMergeClauseListBASESIZE MIDBASESIZE

/*@iter mtMergeClauseList_elements (sef mtMergeClauseList x, yield exposed mtMergeClause el); @*/
# define mtMergeClauseList_elements(x, m_el) \
   { if (mtMergeClauseList_isDefined (x)) { \
       int m_ind; mtMergeClause *m_elements = &((x)->elements[0]); \
	 for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
	   { mtMergeClause m_el = *(m_elements++); 

# define end_mtMergeClauseList_elements }}}

# else
# error "Multiple include"
# endif




