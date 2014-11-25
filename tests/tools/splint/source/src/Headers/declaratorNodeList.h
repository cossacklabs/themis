/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef DECLARATORNODELIST_H
# define DECLARATORNODELIST_H

typedef /*@only@*/ declaratorNode o_declaratorNode;

abst_typedef struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_declaratorNode  *elements;
} *declaratorNodeList ;

/*@iter declaratorNodeList_elements (sef declaratorNodeList x, yield exposed declaratorNode el); @*/
# define declaratorNodeList_elements(x, m_el) \
   { int m_ind; declaratorNode *m_elements = &((x)->elements[0]); \
       for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
	 { declaratorNode m_el = *(m_elements++);

# define end_declaratorNodeList_elements }}


extern /*@only@*/ declaratorNodeList declaratorNodeList_new(void);
extern declaratorNodeList 
  declaratorNodeList_add (/*@returned@*/ declaratorNodeList p_s,
			  /*@only@*/ declaratorNode p_el);

extern /*@only@*/ cstring declaratorNodeList_unparse (declaratorNodeList p_s) ;
extern void declaratorNodeList_free (/*@only@*/ declaratorNodeList p_s) ;
extern declaratorNodeList declaratorNodeList_copy (declaratorNodeList p_s) ;

/*@constant int declaratorNodeListBASESIZE;@*/
# define declaratorNodeListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




