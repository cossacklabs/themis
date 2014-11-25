/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef IMPORTNODELIST_H
# define IMPORTNODELIST_H

typedef /*@only@*/ importNode o_importNode;

abst_typedef struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_importNode *elements;
} *importNodeList ;

/*@iter importNodeList_elements (sef importNodeList x, yield exposed importNode el); @*/
# define importNodeList_elements(x, m_el) \
   { int m_ind; importNode *m_elements = &((x)->elements[0]); \
       for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
	 { importNode m_el = *(m_elements++);

# define end_importNodeList_elements }}

extern /*@only@*/ importNodeList importNodeList_new (void);
extern importNodeList 
  importNodeList_add (/*@returned@*/ importNodeList p_s, /*@only@*/ importNode p_el) ;

extern /*@only@*/ cstring importNodeList_unparse (importNodeList p_s) ;
extern void importNodeList_free (/*@only@*/ importNodeList p_s) ;

/*@constant int importNodeListBASESIZE;@*/
# define importNodeListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




