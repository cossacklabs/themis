/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
# ifndef traitRefNodeLIST_H
# define traitRefNodeLIST_H

typedef /*@only@*/ traitRefNode o_traitRefNode;

abst_typedef struct 
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_traitRefNode  *elements;
} *traitRefNodeList ;

/*@iter traitRefNodeList_elements (sef traitRefNodeList x, yield exposed traitRefNode el); @*/
# define traitRefNodeList_elements(x, m_el) \
   { int m_ind; traitRefNode *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { traitRefNode m_el = *(m_elements++); 

# define end_traitRefNodeList_elements }}

extern /*@only@*/ traitRefNodeList traitRefNodeList_new(void);
extern traitRefNodeList 
  traitRefNodeList_add (/*@returned@*/ traitRefNodeList p_s, /*@only@*/ traitRefNode p_el);

extern /*@only@*/ cstring traitRefNodeList_unparse (traitRefNodeList p_s) ;
extern void traitRefNodeList_free (/*@only@*/ traitRefNodeList p_s) ;

/*@constant int traitRefNodeListBASESIZE;@*/
# define traitRefNodeListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




