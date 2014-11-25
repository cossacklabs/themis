/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

# ifndef MTLoseReferenceLIST_H
# define MTLoseReferenceLIST_H

/*:private:*/ typedef /*@only@*/ mtLoseReference o_mtLoseReference;

struct s_mtLoseReferenceList
{
  int      nelements;
  int      nspace;
  /*@reldef@*/ /*@relnull@*/ o_mtLoseReference *elements;
} ;

/*@constant null mtLoseReferenceList mtLoseReferenceList_undefined;@*/
# define mtLoseReferenceList_undefined ((mtLoseReferenceList) NULL)

extern /*@falsewhennull@*/ bool mtLoseReferenceList_isDefined (mtLoseReferenceList p_s) /*@*/ ;
# define mtLoseReferenceList_isDefined(s)   ((s) != mtLoseReferenceList_undefined)

extern int mtLoseReferenceList_size (/*@sef@*/ mtLoseReferenceList) /*@*/ ;
# define mtLoseReferenceList_size(s) (mtLoseReferenceList_isDefined (s) ? (s)->nelements : 0)

extern /*@unused@*/ /*@falsewhennull@*/ bool mtLoseReferenceList_empty (/*@sef@*/ mtLoseReferenceList) /*@*/ ;
# define mtLoseReferenceList_empty(s) (mtLoseReferenceList_size(s) == 0)

extern cstring mtLoseReferenceList_unparseSep (mtLoseReferenceList p_s, cstring p_sep) /*@*/ ;

extern /*@unused@*/ /*@only@*/ mtLoseReferenceList mtLoseReferenceList_new (void) /*@*/ ;

extern /*@only@*/ mtLoseReferenceList mtLoseReferenceList_single (/*@only@*/ mtLoseReference p_el) /*@*/ ;

extern mtLoseReferenceList 
  mtLoseReferenceList_add (/*@only@*/ mtLoseReferenceList p_s, /*@only@*/ mtLoseReference p_el) 
  /*@modifies p_s@*/ ;

extern mtLoseReferenceList 
  mtLoseReferenceList_prepend (/*@only@*/ mtLoseReferenceList p_s, /*@only@*/ mtLoseReference p_el) 
  /*@modifies p_s@*/ ;

extern /*@unused@*/ /*@only@*/ cstring mtLoseReferenceList_unparse (mtLoseReferenceList p_s) ;
extern void mtLoseReferenceList_free (/*@only@*/ mtLoseReferenceList p_s) ;

/*@constant int mtLoseReferenceListBASESIZE;@*/
# define mtLoseReferenceListBASESIZE MIDBASESIZE

/*@iter mtLoseReferenceList_elements (sef mtLoseReferenceList x, yield exposed mtLoseReference el); @*/
# define mtLoseReferenceList_elements(x, m_el) \
   { if (mtLoseReferenceList_isDefined (x)) { \
       int m_ind; mtLoseReference *m_elements = &((x)->elements[0]); \
	 for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
	   { mtLoseReference m_el = *(m_elements++); 

# define end_mtLoseReferenceList_elements }}}

# else
# error "Multiple include"
# endif




