/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef exprNodeSList_H
# define exprNodeSList_H

/*
** like exprNodeList, except elements are dependent.
*/

typedef /*@dependent@*/ exprNode d_exprNode;

abst_typedef struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ d_exprNode *elements;
} *exprNodeSList;

/*@iter exprNodeSList_elements (sef exprNodeSList x, yield exposed exprNode el); @*/
# define exprNodeSList_elements(x, m_el) \
   { int m_ind; exprNode *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { exprNode m_el = *(m_elements++); 

# define end_exprNodeSList_elements }}

extern /*@only@*/ exprNodeSList exprNodeSList_new(void);

extern /*@only@*/ exprNodeSList exprNodeSList_singleton (/*@exposed@*/ /*@dependent@*/ exprNode p_e) ;

extern /*@unused@*/ void 
  exprNodeSList_addh (exprNodeSList p_s, /*@exposed@*/ /*@dependent@*/ exprNode p_el);

extern /*@only@*/ /*@unused@*/ cstring exprNodeSList_unparse (exprNodeSList p_s);
extern void exprNodeSList_free (/*@only@*/ exprNodeSList p_s) ;

extern exprNodeSList 
  exprNodeSList_append (/*@returned@*/ exprNodeSList p_s1, /*@only@*/ exprNodeSList p_s2) ;

/*@constant int exprNodeSListBASESIZE;@*/
# define exprNodeSListBASESIZE SMALLBASESIZE

# endif




