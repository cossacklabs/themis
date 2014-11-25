/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
# ifndef VARNODELIST_H
# define VARNODELIST_H

typedef /*@only@*/ varNode o_varNode;

abst_typedef struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_varNode  *elements;
} *varNodeList;

/*@iter varNodeList_elements (sef varNodeList x, yield exposed varNode el); @*/
# define varNodeList_elements(x, m_el) \
   { int m_ind; varNode *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { varNode m_el = *(m_elements++); 

# define end_varNodeList_elements }}

extern /*@only@*/ varNodeList varNodeList_new(void);
extern varNodeList varNodeList_add (/*@returned@*/ varNodeList p_s, /*@only@*/ varNode p_el) ;

extern varNodeList varNodeList_copy (varNodeList p_s);

extern /*@only@*/ cstring varNodeList_unparse (varNodeList p_s) ;
extern void varNodeList_free (/*@only@*/ varNodeList p_s) ;

/*@constant int varNodeListBASESIZE;@*/
# define varNodeListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




