/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef QUANTIFIERNODELIST_H
# define QUANTIFIERNODELIST_H

typedef /*@only@*/ quantifierNode o_quantifierNode;

abst_typedef struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_quantifierNode  *elements;
} *quantifierNodeList ;

/*@iter quantifierNodeList_elements (sef quantifierNodeList x, yield exposed quantifierNode el); @*/
# define quantifierNodeList_elements(x, m_el) \
   { int m_ind; quantifierNode *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { quantifierNode m_el = *(m_elements++); 

# define end_quantifierNodeList_elements }}

extern /*@only@*/ quantifierNodeList quantifierNodeList_new(void);

extern quantifierNodeList 
  quantifierNodeList_add (/*@returned@*/ quantifierNodeList p_s, /*@only@*/ quantifierNode p_el);

extern /*@only@*/ cstring quantifierNodeList_unparse (quantifierNodeList p_s) ;
extern /*@unused@*/ void quantifierNodeList_free (/*@only@*/ quantifierNodeList p_s) ;

extern /*@only@*/ quantifierNodeList quantifierNodeList_copy (quantifierNodeList p_s) ; 

/*@constant int quantifierNodeListBASESIZE;@*/
# define quantifierNodeListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




