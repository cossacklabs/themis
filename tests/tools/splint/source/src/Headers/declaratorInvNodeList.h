/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef DECLARATORINVNODELIST_H
# define DECLARATORINVNODELIST_H

typedef /*@only@*/ declaratorInvNode o_declaratorInvNode;

abst_typedef struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_declaratorInvNode  *elements;
} *declaratorInvNodeList ;

/*@iter declaratorInvNodeList_elements (sef declaratorInvNodeList x, yield exposed declaratorInvNode el); @*/
# define declaratorInvNodeList_elements(x, m_el) \
   { int m_ind; declaratorInvNode *m_elements = &((x)->elements[0]); \
       for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
	 { declaratorInvNode m_el = *(m_elements++); 

# define end_declaratorInvNodeList_elements }}

extern int declaratorInvNodeList_size (declaratorInvNodeList);
# define declaratorInvNodeList_size(d) ((d)->nelements)

extern /*@only@*/ declaratorInvNodeList declaratorInvNodeList_new(void);

extern declaratorInvNodeList 
  declaratorInvNodeList_add (/*@returned@*/ declaratorInvNodeList p_s,
			      /*@only@*/ declaratorInvNode p_el);

extern /*@only@*/ cstring declaratorInvNodeList_unparse (declaratorInvNodeList p_s) ;
extern void declaratorInvNodeList_free (/*@only@*/ declaratorInvNodeList p_s) ;

/*@constant int declaratorInvNodeListBASESIZE;@*/
# define declaratorInvNodeListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




