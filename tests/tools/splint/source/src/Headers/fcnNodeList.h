/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

# ifndef FCNNODELIST_H
# define FCNNODELIST_H

typedef /*@only@*/ fcnNode o_fcnNode;

abst_typedef /*@null@*/ struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_fcnNode  *elements;
} *fcnNodeList;

/*@iter fcnNodeList_elements (sef fcnNodeList x, yield exposed fcnNode el); @*/
# define fcnNodeList_elements(x, m_el) \
   if (fcnNodeList_isDefined (x)) \
     { int m_ind; fcnNode *m_elements = &((x)->elements[0]); \
       for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
	  { fcnNode m_el = *(m_elements++); 

# define end_fcnNodeList_elements }}

/*@constant null fcnNodeList fcnNodeList_undefined; @*/
# define fcnNodeList_undefined ((fcnNodeList)0)

extern /*@falsewhennull@*/ bool fcnNodeList_isDefined (fcnNodeList p_f);
# define fcnNodeList_isDefined(f) ((f) != fcnNodeList_undefined)

extern /*@unused@*/ /*@nullwhentrue@*/ bool 
  fcnNodeList_isUndefined (fcnNodeList p_f);
# define fcnNodeList_isUndefined(f) ((f) == fcnNodeList_undefined)

extern int fcnNodeList_size (/*@sef@*/ fcnNodeList p_f);
extern bool fcnNodeList_isEmpty (/*@sef@*/ fcnNodeList p_f);

# define fcnNodeList_size(s)  (fcnNodeList_isDefined(s) ? (s)->nelements : 0)
# define fcnNodeList_isEmpty(s) (fcnNodeList_size(s) == 0)

extern /*@only@*/ fcnNodeList fcnNodeList_new(void);
extern fcnNodeList fcnNodeList_add (/*@returned@*/ fcnNodeList p_s, /*@keep@*/ fcnNode p_el) ;


extern /*@unused@*/ /*@only@*/ cstring fcnNodeList_unparse (fcnNodeList p_s) ;
extern void fcnNodeList_free (/*@null@*/ /*@only@*/ fcnNodeList p_s) ;

/*@constant int fcnNodeListBASESIZE;@*/
# define fcnNodeListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




