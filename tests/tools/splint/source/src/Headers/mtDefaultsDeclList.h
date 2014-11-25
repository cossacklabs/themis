/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

# ifndef mtDefaultsDeclLIST_H
# define mtDefaultsDeclLIST_H

/*:private:*/ typedef /*@only@*/ mtDefaultsDecl o_mtDefaultsDecl;

struct s_mtDefaultsDeclList
{
  int      nelements;
  int      nspace;
  /*@reldef@*/ /*@relnull@*/ o_mtDefaultsDecl *elements;
} ;

/*@constant null mtDefaultsDeclList mtDefaultsDeclList_undefined;@*/
# define mtDefaultsDeclList_undefined ((mtDefaultsDeclList) NULL)

extern /*@falsewhennull@*/ bool mtDefaultsDeclList_isDefined (mtDefaultsDeclList p_s) /*@*/ ;
# define mtDefaultsDeclList_isDefined(s)   ((s) != mtDefaultsDeclList_undefined)

extern int mtDefaultsDeclList_size (/*@sef@*/ mtDefaultsDeclList) /*@*/ ;
# define mtDefaultsDeclList_size(s) (mtDefaultsDeclList_isDefined (s) ? (s)->nelements : 0)

extern /*@unused@*/ /*@falsewhennull@*/ bool mtDefaultsDeclList_empty (/*@sef@*/ mtDefaultsDeclList) /*@*/ ;
# define mtDefaultsDeclList_empty(s) (mtDefaultsDeclList_size(s) == 0)

extern cstring mtDefaultsDeclList_unparseSep (mtDefaultsDeclList p_s, cstring p_sep) /*@*/ ;

extern /*@unused@*/ /*@only@*/ mtDefaultsDeclList mtDefaultsDeclList_new (void) /*@*/ ;

extern /*@only@*/ mtDefaultsDeclList mtDefaultsDeclList_single (/*@keep@*/ mtDefaultsDecl p_el) /*@*/ ;

extern mtDefaultsDeclList 
  mtDefaultsDeclList_add (/*@returned@*/ mtDefaultsDeclList p_s, /*@keep@*/ mtDefaultsDecl p_el) 
  /*@modifies p_s@*/ ;

extern mtDefaultsDeclList 
  mtDefaultsDeclList_prepend (/*@only@*/ mtDefaultsDeclList p_s, /*@keep@*/ mtDefaultsDecl p_el) 
  /*@modifies p_s@*/ ;

extern /*@unused@*/ /*@only@*/ cstring 
   mtDefaultsDeclList_unparse (mtDefaultsDeclList p_s) ;

extern void mtDefaultsDeclList_free (/*@only@*/ mtDefaultsDeclList p_s) ;

/*@constant int mtDefaultsDeclListBASESIZE;@*/
# define mtDefaultsDeclListBASESIZE MIDBASESIZE

/*@iter mtDefaultsDeclList_elements (sef mtDefaultsDeclList x, yield exposed mtDefaultsDecl el); @*/
# define mtDefaultsDeclList_elements(x, m_el) \
   { if (mtDefaultsDeclList_isDefined (x)) { \
       int m_ind; mtDefaultsDecl *m_elements = &((x)->elements[0]); \
	 for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
	   { mtDefaultsDecl m_el = *(m_elements++); 

# define end_mtDefaultsDeclList_elements }}}

# else
# error "Multiple include"
# endif




