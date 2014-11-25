/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef replaceNodeLIST_H
# define replaceNodeLIST_H

typedef /*@only@*/ replaceNode o_replaceNode;

abst_typedef struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_replaceNode  *elements;
} *replaceNodeList ;

/*@iter replaceNodeList_elements (sef replaceNodeList x, yield exposed replaceNode el); @*/
# define replaceNodeList_elements(x, m_el) \
   { int m_ind; replaceNode *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { replaceNode m_el = *(m_elements++); 

# define end_replaceNodeList_elements }}

extern int replaceNodeList_size (replaceNodeList);

# define replaceNodeList_size(s)  ((s)->nelements)   

extern bool replaceNodeList_isDefined (replaceNodeList);
# define replaceNodeList_isDefined(s) ((s) != (replaceNodeList)0)

extern /*@only@*/ replaceNodeList replaceNodeList_new(void);

extern replaceNodeList 
  replaceNodeList_add (/*@returned@*/ replaceNodeList p_s, /*@only@*/ replaceNode p_el) ;

extern /*@only@*/ cstring replaceNodeList_unparse (replaceNodeList p_s) ;
extern void replaceNodeList_free (/*@only@*/ replaceNodeList p_s) ;

/*@constant int replaceNodeListBASESIZE;@*/
# define replaceNodeListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




