/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/

# ifndef TYPENAMENODELIST_H
# define TYPENAMENODELIST_H

typedef /*@only@*/ typeNameNode o_typeNameNode;

abst_typedef struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_typeNameNode  *elements;
} *typeNameNodeList ;

/*@iter typeNameNodeList_elements (sef typeNameNodeList x, yield exposed typeNameNode el); @*/
# define typeNameNodeList_elements(x, m_el) \
   { int m_ind; typeNameNode *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { typeNameNode m_el = *(m_elements++);

# define end_typeNameNodeList_elements }}

extern int typeNameNodeList_size (typeNameNodeList);
# define typeNameNodeList_size(s) ((s)->nelements)   

extern bool typeNameNodeList_empty (typeNameNodeList);
# define typeNameNodeList_empty(s) (typeNameNodeList_size(s) == 0)

extern /*@only@*/ typeNameNodeList typeNameNodeList_new(void);

extern typeNameNodeList 
  typeNameNodeList_add (/*@returned@*/ typeNameNodeList p_s, /*@only@*/ typeNameNode p_el) ;

extern /*@only@*/ cstring typeNameNodeList_unparse (typeNameNodeList p_s) ;
extern void typeNameNodeList_free (/*@only@*/ typeNameNodeList p_s) ;

/*@constant int typeNameNodeListBASESIZE;@*/
# define typeNameNodeListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




