/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef cstringList_H
# define cstringList_H

abst_typedef /*@null@*/ struct s_cstringList
{
  int      nelements;
  int      nspace;
  /*@reldef@*/ /*@relnull@*/ o_cstring *elements;
} *cstringList ;

/*@constant null cstringList cstringList_undefined;@*/
# define cstringList_undefined ((cstringList) NULL)

extern /*@falsewhennull@*/ bool cstringList_isDefined (cstringList p_s) /*@*/ ;
# define cstringList_isDefined(s)   ((s) != cstringList_undefined)

extern int cstringList_size (/*@sef@*/ cstringList) /*@*/ ;
# define cstringList_size(s) (cstringList_isDefined (s) ? (s)->nelements : 0)

extern /*@unused@*/ /*@falsewhennull@*/ bool cstringList_empty (/*@sef@*/ cstringList) /*@*/ ;
# define cstringList_empty(s) (cstringList_size(s) == 0)

extern cstring cstringList_unparseSep (cstringList p_s, cstring p_sep) /*@*/ ;

extern /*@exposed@*/ /*@null@*/ ob_cstring *cstringList_getElements (cstringList) /*@*/ ;

extern /*@unused@*/ /*@only@*/ cstringList cstringList_new (void) /*@*/ ;

extern /*@only@*/ cstringList cstringList_single (/*@keep@*/ cstring p_el) /*@*/ ;

extern cstringList 
  cstringList_add (/*@returned@*/ cstringList p_s, /*@keep@*/ cstring p_el) 
  /*@modifies p_s@*/ ;

extern /*@only@*/ cstringList 
  cstringList_prepend (/*@only@*/ /*@returned@*/ cstringList p_s,
		       /*@keep@*/ cstring p_el) 
  /*@modifies p_s@*/ ;

extern bool cstringList_contains (cstringList p_s, cstring p_key) /*@*/ ;
extern int cstringList_getIndex (cstringList p_s, cstring p_key) /*@*/ ;
extern /*@observer@*/ cstring cstringList_get (cstringList p_s, int p_index) /*@*/ ;

extern /*@unused@*/ void cstringList_alphabetize (cstringList p_s);
extern /*@unused@*/ /*@only@*/ cstring cstringList_unparseAbbrev (cstringList p_s) /*@*/ ;
extern /*@unused@*/ /*@only@*/ cstring cstringList_unparse (cstringList p_s) ;
extern void cstringList_free (/*@only@*/ cstringList p_s) ;

extern /*@unused@*/ void cstringList_printSpaced (cstringList p_s, size_t p_indent, size_t p_gap, int p_linelen);

extern /*@only@*/ cstringList cstringList_copy (cstringList p_s) /*@*/ ;

/*@constant int cstringListBASESIZE;@*/
# define cstringListBASESIZE MIDBASESIZE

/*@iter cstringList_elements (sef cstringList x, yield exposed cstring el); @*/
# define cstringList_elements(x, m_el) \
   { if (cstringList_isDefined (x)) { \
       int m_ind; cstring *m_elements = &((x)->elements[0]); \
	 for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
	   { cstring m_el = *(m_elements++); 

# define end_cstringList_elements }}}

# else
# error "Multiple include"
# endif




