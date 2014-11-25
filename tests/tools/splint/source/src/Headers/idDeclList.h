/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef idDeclLIST_H
# define idDeclLIST_H

typedef /*@only@*/ idDecl o_idDecl;

abst_typedef struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_idDecl  *elements;
} *idDeclList ;

/*@iter idDeclList_elements (sef idDeclList x, yield exposed idDecl el); @*/
# define idDeclList_elements(x, m_el) \
   { int m_ind; idDecl *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { idDecl m_el = *(m_elements++); 

# define end_idDeclList_elements }}

extern /*@only@*/ idDeclList idDeclList_singleton (/*@only@*/ idDecl p_e) ;
extern idDeclList idDeclList_add (/*@returned@*/ idDeclList p_s, /*@only@*/ idDecl p_el) ; 

extern /*@unused@*/ /*@only@*/ cstring idDeclList_unparse (idDeclList p_s) ;
extern void idDeclList_free (/*@only@*/ idDeclList p_s) ;

/*@constant int idDeclListBASESIZE;@*/
# define idDeclListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




