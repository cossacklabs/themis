/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef interfaceNodeLIST_H
# define interfaceNodeLIST_H

typedef /*@only@*/ interfaceNode o_interfaceNode;

abst_typedef struct
{
  int nelements;
  int nspacehigh;
  int nspacelow;
  /*@dependent@*/ /*@reldef@*/ /*@relnull@*/ o_interfaceNode *elements;
  /*@only@*/ /*@reldef@*/ /*@relnull@*/ o_interfaceNode *elementsroot;
} *interfaceNodeList ;

/*@iter interfaceNodeList_elements (sef interfaceNodeList x, yield exposed interfaceNode el); @*/
# define interfaceNodeList_elements(x, m_el) \
   { int m_ind; interfaceNode *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
         { interfaceNode m_el = *(m_elements++); 

# define end_interfaceNodeList_elements }}

extern /*@only@*/ interfaceNodeList interfaceNodeList_new(void);

extern interfaceNodeList
  interfaceNodeList_addh (/*@returned@*/ interfaceNodeList p_s, /*@only@*/ interfaceNode p_el);
extern void interfaceNodeList_addl (interfaceNodeList p_s, /*@keep@*/ interfaceNode p_el) ;

extern void interfaceNodeList_free (/*@only@*/ interfaceNodeList p_s) ;

/*@constant int interfaceNodeListGROWLOW;@*/
# define interfaceNodeListGROWLOW 4 /* addl used in abstract.c */

/*@constant int interfaceNodeListGROWHI;@*/
# define interfaceNodeListGROWHI 4

/*@constant int interfaceNodeListBASESIZE;@*/
# define interfaceNodeListBASESIZE \
  (interfaceNodeListGROWLOW + interfaceNodeListGROWHI)

# else
# error "Multiple include"
# endif




