/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/

# ifndef STRUCTDECLNODELIST_H
# define STRUCTDECLNODELIST_H

typedef /*@only@*/ stDeclNode o_stDeclNode;

abst_typedef struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_stDeclNode  *elements;
} *stDeclNodeList ;

/*@iter stDeclNodeList_elements (sef stDeclNodeList x, yield exposed stDeclNode el); @*/
# define stDeclNodeList_elements(x, m_el) \
   { int m_ind; stDeclNode *m_elements = &((x)->elements[0]); \
       for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { stDeclNode m_el = *(m_elements++);

# define end_stDeclNodeList_elements }}

extern int stDeclNodeList_size (stDeclNodeList);

# define stDeclNodeList_size(s) ((s)->nelements)   

extern /*@only@*/ stDeclNodeList stDeclNodeList_new(void);
extern stDeclNodeList 
  stDeclNodeList_add (/*@returned@*/ stDeclNodeList p_s, /*@only@*/ stDeclNode p_el);

extern /*@only@*/ cstring stDeclNodeList_unparse (stDeclNodeList p_s) ;
extern void stDeclNodeList_free (/*@only@*/ stDeclNodeList p_s) ;

extern /*@only@*/ stDeclNodeList stDeclNodeList_copy (stDeclNodeList p_s) ; 

/*@constant int stDeclNodeListBASESIZE;@*/
# define stDeclNodeListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




