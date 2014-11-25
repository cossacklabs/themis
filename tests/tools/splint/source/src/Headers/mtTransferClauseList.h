/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

# ifndef MTTRANSFERCLAUSELIST_H
# define MTTRANSFERCLAUSELIST_H

/*:private:*/ typedef /*@only@*/ mtTransferClause o_mtTransferClause;

struct s_mtTransferClauseList
{
  int      nelements;
  int      nspace;
  /*@reldef@*/ /*@relnull@*/ o_mtTransferClause *elements;
} ;

/*@constant null mtTransferClauseList mtTransferClauseList_undefined;@*/
# define mtTransferClauseList_undefined ((mtTransferClauseList) NULL)

extern /*@falsewhennull@*/ bool mtTransferClauseList_isDefined (mtTransferClauseList p_s) /*@*/ ;
# define mtTransferClauseList_isDefined(s)   ((s) != mtTransferClauseList_undefined)

extern int mtTransferClauseList_size (/*@sef@*/ mtTransferClauseList) /*@*/ ;
# define mtTransferClauseList_size(s) (mtTransferClauseList_isDefined (s) ? (s)->nelements : 0)

extern /*@unused@*/ /*@falsewhennull@*/ bool mtTransferClauseList_empty (/*@sef@*/ mtTransferClauseList) /*@*/ ;
# define mtTransferClauseList_empty(s) (mtTransferClauseList_size(s) == 0)

extern cstring mtTransferClauseList_unparseSep (mtTransferClauseList p_s, cstring p_sep) /*@*/ ;

extern /*@unused@*/ /*@only@*/ mtTransferClauseList mtTransferClauseList_new (void) /*@*/ ;

extern /*@only@*/ mtTransferClauseList mtTransferClauseList_single (/*@only@*/ mtTransferClause p_el) /*@*/ ;

extern mtTransferClauseList 
  mtTransferClauseList_add (/*@only@*/ mtTransferClauseList p_s, /*@only@*/ mtTransferClause p_el) 
  /*@modifies p_s@*/ ;

extern /*@only@*/ mtTransferClauseList 
  mtTransferClauseList_prepend (/*@only@*/ mtTransferClauseList p_s, /*@only@*/ mtTransferClause p_el) 
  /*@modifies p_s@*/ ;

extern /*@unused@*/ /*@only@*/ cstring mtTransferClauseList_unparse (mtTransferClauseList p_s) ;
extern void mtTransferClauseList_free (/*@only@*/ mtTransferClauseList p_s) ;

/*@constant int mtTransferClauseListBASESIZE;@*/
# define mtTransferClauseListBASESIZE MIDBASESIZE

/*@iter mtTransferClauseList_elements (sef mtTransferClauseList x, yield exposed mtTransferClause el); @*/
# define mtTransferClauseList_elements(x, m_el) \
   { if (mtTransferClauseList_isDefined (x)) { \
       int m_ind; mtTransferClause *m_elements = &((x)->elements[0]); \
	 for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
	   { mtTransferClause m_el = *(m_elements++); 

# define end_mtTransferClauseList_elements }}}

# else
# error "Multiple include"
# endif




