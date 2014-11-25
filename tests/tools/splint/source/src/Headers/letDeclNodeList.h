/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef LETDECLNODELIST_H
# define LETDECLNODELIST_H

typedef /*@only@*/ letDeclNode o_letDeclNode;

abst_typedef struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_letDeclNode  *elements;
} *letDeclNodeList ;

/*@iter letDeclNodeList_elements (sef letDeclNodeList x, yield exposed letDeclNode el); @*/
# define letDeclNodeList_elements(x, m_el) \
   { int m_ind; letDeclNode *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { letDeclNode m_el = *(m_elements++); 

# define end_letDeclNodeList_elements }}

extern /*@only@*/ letDeclNodeList letDeclNodeList_new(void);
extern letDeclNodeList 
  letDeclNodeList_add (/*@returned@*/ letDeclNodeList p_s, /*@only@*/ letDeclNode p_el);

extern /*@only@*/ cstring letDeclNodeList_unparse (letDeclNodeList p_s) ;
extern void letDeclNodeList_free (/*@only@*/ letDeclNodeList p_s) ;

/*@constant int letDeclNodeListBASESIZE;@*/
# define letDeclNodeListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




