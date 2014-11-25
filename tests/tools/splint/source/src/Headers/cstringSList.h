/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef cstringSList_H
# define cstringSList_H

typedef /*@observer@*/ cstring ob_cstring;

struct s_cstringSList
{
  int      nelements;
  int      nspace;
  /*@reldef@*/ /*@relnull@*/ ob_cstring *elements;
} ;

/*@constant null cstringSList cstringSList_undefined;@*/
# define cstringSList_undefined ((cstringSList) NULL)

extern /*@falsewhennull@*/ bool cstringSList_isDefined (cstringSList p_s) /*@*/ ;
# define cstringSList_isDefined(s)   ((s) != cstringSList_undefined)

extern int cstringSList_size (/*@sef@*/ cstringSList) /*@*/ ;
# define cstringSList_size(s) (cstringSList_isDefined (s) ? (s)->nelements : 0)

extern /*@falsewhennull@*/ bool cstringSList_empty (/*@sef@*/ cstringSList) /*@*/ ;
# define cstringSList_empty(s) (cstringSList_size(s) == 0)

extern /*@unused@*/ cstring cstringSList_unparseSep (cstringSList p_s, cstring p_sep) /*@*/ ;

extern /*@only@*/ cstringSList cstringSList_new (void) /*@*/ ;

extern /*@unused@*/ /*@only@*/ cstringSList cstringSList_single (/*@exposed@*/ cstring p_el) /*@*/ ;

extern cstringSList 
  cstringSList_add (/*@returned@*/ cstringSList p_s, /*@exposed@*/ cstring p_el) 
  /*@modifies p_s@*/ ;

extern void cstringSList_alphabetize (cstringSList p_s);

extern /*@observer@*/ cstring cstringSList_get (cstringSList p_s, int p_index) /*@*/ ;

extern /*@only@*/ cstring cstringSList_unparseAbbrev (cstringSList p_s) /*@*/ ;
extern /*@unused@*/ /*@only@*/ cstring cstringSList_unparse (cstringSList p_s) /*@*/ ;
extern void cstringSList_free (/*@only@*/ cstringSList p_s) ;

extern void cstringSList_printSpaced (cstringSList p_s, size_t p_indent, size_t p_gap, int p_linelen);

/*@constant int cstringSListBASESIZE;@*/
# define cstringSListBASESIZE MIDBASESIZE

/*@iter cstringSList_elements (sef cstringSList x, yield exposed cstring el); @*/
# define cstringSList_elements(x, m_el) \
   { if (cstringSList_isDefined (x)) { \
       int m_ind; cstring *m_elements = &((x)->elements[0]); \
	 for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
	   { cstring m_el = *(m_elements++); 

# define end_cstringSList_elements }}}

# else
# error "Multiple include"
# endif




