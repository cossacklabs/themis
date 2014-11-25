/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef paramNodeLIST_H
# define paramNodeLIST_H

typedef /*@null@*/ /*@only@*/ paramNode o_paramNode;

abst_typedef /*@null@*/ struct 
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_paramNode *elements;
} *paramNodeList ;

/*@iter paramNodeList_elements (sef paramNodeList x, yield exposed paramNode el); @*/
# define paramNodeList_elements(x, m_el) \
    if (paramNodeList_isDefined (x)) \
      { int m_ind; paramNode *m_elements = &((x)->elements[0]); \
        for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
          { paramNode m_el = *(m_elements++); 

# define end_paramNodeList_elements }}

extern int paramNodeList_size (/*@sef@*/ paramNodeList);

# define paramNodeList_size(s)  (paramNodeList_isDefined (s) ? (s)->nelements : 0)   

extern bool paramNodeList_empty (/*@sef@*/ paramNodeList p_s);
# define paramNodeList_empty(s) (paramNodeList_size(s) == 0)

extern /*@only@*/ paramNodeList paramNodeList_single (/*@keep@*/ paramNode p_p);

extern /*@falsewhennull@*/ bool paramNodeList_isDefined (paramNodeList p_s) /*@*/ ;
# define paramNodeList_isDefined(s) ((s) != (paramNodeList)0)
extern /*@only@*/ paramNodeList paramNodeList_new (void);

extern paramNodeList 
  paramNodeList_add (/*@returned@*/ paramNodeList p_s, /*@null@*/ /*@only@*/ paramNode p_el) ;

extern /*@only@*/  cstring paramNodeList_unparse (paramNodeList p_s) ;
extern void paramNodeList_free (/*@only@*/ /*@only@*/ paramNodeList p_s) ;

extern /*@only@*/ paramNodeList paramNodeList_copy (paramNodeList p_s) ; 
extern /*@only@*/ cstring paramNodeList_unparseComments (paramNodeList p_s);

/*@constant int paramNodeListBASESIZE;@*/
# define paramNodeListBASESIZE SMALLBASESIZE

/*@constant null paramNodeList paramNodeList_undefined; @*/
# define paramNodeList_undefined ((paramNodeList) 0)

extern /*@nullwhentrue@*/ bool paramNodeList_isNull (/*@null@*/ paramNodeList p_p);

# define paramNodeList_isNull(p) ((p) == paramNodeList_undefined)

# else
# error "Multiple include"
# endif




