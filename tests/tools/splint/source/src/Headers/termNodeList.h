/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/

# ifndef termNodeLIST_H
# define termNodeLIST_H

typedef /*@only@*/ termNode o_termNode;

struct s_termNodeList
{
  int nelements;
  int nspacehigh;
  int nspacelow;
  int current;
  /*@dependent@*/ /*@reldef@*/ /*@relnull@*/ o_termNode *elements;
  /*@only@*/ /*@reldef@*/ /*@relnull@*/ o_termNode *elementsroot;
} ;

/*@iter termNodeList_elements (sef termNodeList x, yield exposed termNode el); @*/
# define termNodeList_elements(x, m_el) \
   { int m_ind; termNode *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { termNode m_el = *(m_elements++); 

# define end_termNodeList_elements }}

extern int termNodeList_size (/*@sef@*/ termNodeList);
# define termNodeList_size(s)  (termNodeList_isDefined (s) ? (s)->nelements : 0)

extern bool termNodeList_empty (/*@sef@*/ termNodeList);
# define termNodeList_empty(s) (termNodeList_size(s) == 0)

extern /*@falsewhennull@*/ bool termNodeList_isDefined (termNodeList p_t);
# define termNodeList_isDefined(s) ((s) != (termNodeList) 0)

extern /*@only@*/ termNodeList termNodeList_new(void);
extern termNodeList termNodeList_push (/*@returned@*/ termNodeList p_s, 
				       /*@keep@*/ termNode p_el) ;

extern void termNodeList_addh (termNodeList p_s, /*@keep@*/ termNode p_el) ;
extern void termNodeList_addl (termNodeList p_s, /*@keep@*/ termNode p_el) ;

extern void termNodeList_reset (termNodeList p_s) 
   /*@modifies p_s@*/ ;   
extern void termNodeList_finish (termNodeList p_s) 
   /*@modifies p_s@*/ ; 
extern void termNodeList_advance (termNodeList p_s) 
   /*@modifies p_s@*/ ; 

extern /*@exposed@*/ termNode termNodeList_getN (termNodeList p_s, int p_n) /*@*/ ;

extern /*@only@*/ cstring termNodeList_unparse (termNodeList p_s) ;
extern /*@only@*/ cstring termNodeList_unparseTail (termNodeList p_s);
extern /*@only@*/ cstring termNodeList_unparseToCurrent (termNodeList p_s);
extern /*@only@*/ cstring termNodeList_unparseSecondToCurrent (termNodeList p_s);

extern void termNodeList_free (/*@only@*/ termNodeList p_s) ;

extern /*@exposed@*/ termNode termNodeList_head (termNodeList p_s) ;
extern /*@exposed@*/ termNode termNodeList_current (termNodeList p_s) ;

extern /*@only@*/ termNodeList termNodeList_copy (termNodeList p_s) ; 

/*@constant int termNodeListGROWLOW;@*/
# define termNodeListGROWLOW 4 /* addl used in abstract.c */

/*@constant int termNodeListGROWHI;@*/
# define termNodeListGROWHI 4

/*@constant int termNodeListBASESIZE;@*/
# define termNodeListBASESIZE (termNodeListGROWLOW + termNodeListGROWHI)

# else
# error "Multiple include"
# endif




