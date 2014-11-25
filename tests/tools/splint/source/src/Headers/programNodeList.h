/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef PROGRAMNODELIST_H
# define PROGRAMNODELIST_H

typedef /*@only@*/ programNode o_programNode;

struct s_programNodeList 
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_programNode  *elements;
} ;

/*@iter programNodeList_elements (sef programNodeList x, yield exposed programNode el); @*/
# define programNodeList_elements(x, m_el) \
   { int m_ind; programNode *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { programNode m_el = *(m_elements++);

# define end_programNodeList_elements }}

extern /*@only@*/ programNodeList programNodeList_new(void);

extern void programNodeList_addh (programNodeList p_s, /*@keep@*/ programNode p_el) ;

extern /*@only@*/ cstring programNodeList_unparse (programNodeList p_s) ;
extern void programNodeList_free (/*@only@*/ programNodeList p_s) ;

/*@constant int programNodeListBASESIZE;@*/
# define programNodeListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




