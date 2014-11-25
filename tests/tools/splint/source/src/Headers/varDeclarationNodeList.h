/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/

# ifndef VARDECLARATIONNODELIST_H
# define VARDECLARATIONNODELIST_H

typedef /*@only@*/ varDeclarationNode o_varDeclarationNode;

abst_typedef struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_varDeclarationNode  *elements;
} *varDeclarationNodeList ;

/*@iter varDeclarationNodeList_elements (sef varDeclarationNodeList x, yield exposed varDeclarationNode el); @*/
# define varDeclarationNodeList_elements(x, m_el) \
   { int m_ind; varDeclarationNode *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { varDeclarationNode m_el = *(m_elements++); 

# define end_varDeclarationNodeList_elements }}

extern /*@only@*/ varDeclarationNodeList varDeclarationNodeList_new (void) /*@*/ ;
extern void 
  varDeclarationNodeList_addh (varDeclarationNodeList p_s, 
			       /*@keep@*/ varDeclarationNode p_el) 
  /*@modifies p_s@*/ ;

extern /*@only@*/ cstring varDeclarationNodeList_unparse (varDeclarationNodeList p_s)  /*@*/ ;
extern void varDeclarationNodeList_free (/*@only@*/ varDeclarationNodeList p_s) ;

/*@constant int varDeclarationNodeListBASESIZE;@*/
# define varDeclarationNodeListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




