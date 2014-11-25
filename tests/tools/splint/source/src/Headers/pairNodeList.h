/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef PAIRNODELIST_H
# define PAIRNODELIST_H

typedef /*@only@*/ pairNode o_pairNode;

abst_typedef /*@null@*/ struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_pairNode *elements;
} *pairNodeList ;

/*@iter pairNodeList_elements (sef pairNodeList x, yield exposed pairNode el); @*/
# define pairNodeList_elements(x, m_el) \
   if (pairNodeList_isDefined (x)) \
     { int m_ind; pairNode *m_elements = &((x)->elements[0]); \
       for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
         { pairNode m_el = *(m_elements++); 

# define end_pairNodeList_elements }}

extern /*@falsewhennull@*/ bool pairNodeList_isDefined (pairNodeList p_p) /*@*/ ;

/*@constant null pairNodeList pairNodeList_undefined; @*/
# define pairNodeList_undefined    ((pairNodeList)0)
# define pairNodeList_isDefined(p) ((p) != pairNodeList_undefined)

extern /*@only@*/ pairNodeList pairNodeList_new(void) /*@*/ ;
extern void pairNodeList_addh (pairNodeList p_s, /*@keep@*/ pairNode p_el) 
   /*@modifies p_s@*/ ;

extern /*@only@*/ cstring pairNodeList_unparse (pairNodeList p_s)  /*@*/ ;
extern void pairNodeList_free (/*@only@*/ pairNodeList p_s) ;

/*@constant int pairNodeListBASESIZE;@*/
# define pairNodeListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




