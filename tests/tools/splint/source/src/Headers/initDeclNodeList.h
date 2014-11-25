/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

# ifndef INITDECLNODELIST_H
# define INITDECLNODELIST_H

typedef /*@only@*/ initDeclNode o_initDeclNode;

abst_typedef struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_initDeclNode  *elements;
} *initDeclNodeList ;

/*@iter initDeclNodeList_elements (sef initDeclNodeList x, yield exposed initDeclNode el); @*/
# define initDeclNodeList_elements(x, m_el) \
   { int m_ind; initDeclNode *m_elements = &((x)->elements[0]); \
       for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
	 { initDeclNode m_el = *(m_elements++); 

# define end_initDeclNodeList_elements }}

extern /*@only@*/ initDeclNodeList initDeclNodeList_new(void);
extern initDeclNodeList 
  initDeclNodeList_add (/*@returned@*/ initDeclNodeList p_s, 
			/*@only@*/ initDeclNode p_el) ;

extern /*@only@*/ cstring initDeclNodeList_unparse (initDeclNodeList p_s) ;
extern void initDeclNodeList_free (/*@only@*/ initDeclNodeList p_s) ;

/*@constant int initDeclNodeListBASESIZE;@*/
# define initDeclNodeListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




