/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
# ifndef STOREREFNODELIST_H
# define STOREREFNODELIST_H

typedef /*@only@*/ storeRefNode o_storeRefNode;

abst_typedef struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_storeRefNode  *elements;
} *storeRefNodeList ;

/*@iter storeRefNodeList_elements (sef storeRefNodeList x, yield exposed storeRefNode el); @*/
# define storeRefNodeList_elements(x, m_el) \
   { int m_ind; storeRefNode *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { storeRefNode m_el = *(m_elements++); 

# define end_storeRefNodeList_elements }}

extern /*@only@*/ storeRefNodeList storeRefNodeList_new(void);
extern storeRefNodeList 
  storeRefNodeList_add (/*@returned@*/ storeRefNodeList p_s, /*@only@*/ storeRefNode p_el);

extern /*@only@*/ cstring storeRefNodeList_unparse (storeRefNodeList p_s) ;
extern void storeRefNodeList_free (/*@only@*/ storeRefNodeList p_s) ;

extern /*@only@*/ storeRefNodeList storeRefNodeList_copy (storeRefNodeList p_s) ; 
/*@constant int storeRefNodeListBASESIZE;@*/
# define storeRefNodeListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




