/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2000.
** See ../LICENSE for license information.
*/

# ifndef constraintLIST_H
# define constraintLIST_H

typedef /*@only@*/ constraint o_constraint;

struct _constraintList
{
  int nelements;
  int nspacehigh;
  int nspacelow;
  int current;
  /*@dependent@*/ /*@reldef@*/ /*@relnull@*/ o_constraint *elements;
  /*@only@*/ /*@reldef@*/ /*@relnull@*/ o_constraint *elementsroot;
} ;

/*@iter constraintList_elements (sef constraintList x, yield exposed constraint el); @*/
# define constraintList_elements(x, m_el) \
   { int m_ind; constraint *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { constraint m_el = *(m_elements++); 

# define end_constraintList_elements }}

extern int constraintList_size (/*@sef@*/ constraintList);
# define constraintList_size(s)  (constraintList_isDefined (s) ? (s)->nelements : 0)

extern bool constraintList_empty (/*@sef@*/ constraintList);
# define constraintList_empty(s) (constraintList_size(s) == 0)

extern /*@falsewhennull@*/ bool constraintList_isDefined (constraintList p_t);
# define constraintList_isDefined(s) ((s) != (constraintList) 0)

extern /*@only@*/ constraintList constraintList_makeNew(void);
extern constraintList constraintList_push (/*@returned@*/ constraintList p_s, 
				       /*@keep@*/ constraint p_el) ;

extern void constraintList_addh (constraintList p_s, /*@keep@*/ constraint p_el) ;
extern void constraintList_addl (constraintList p_s, /*@keep@*/ constraint p_el) ;

extern void constraintList_reset (constraintList p_s) 
   /*@modifies p_s@*/ ;   
extern void constraintList_finish (constraintList p_s) 
   /*@modifies p_s@*/ ; 
extern void constraintList_advance (constraintList p_s) 
   /*@modifies p_s@*/ ; 

extern /*@exposed@*/ constraint constraintList_getN (constraintList p_s, int p_n) /*@*/ ;

# if 0
extern /*@only@*/ cstring constraintList_unparse (constraintList p_s) ;
extern /*@only@*/ cstring constraintList_unparseTail (constraintList p_s);
extern /*@only@*/ cstring constraintList_unparseToCurrent (constraintList p_s);
extern /*@only@*/ cstring constraintList_unparseSecondToCurrent (constraintList p_s);
# endif

extern void constraintList_free (/*@only@*/ constraintList p_s) ;

extern /*@exposed@*/ constraint constraintList_head (constraintList p_s) ;
extern /*@exposed@*/ constraint constraintList_current (constraintList p_s) ;

extern /*@only@*/ constraintList constraintList_copy (constraintList p_s) ; 

/*@constant int constraintListGROWLOW;@*/
# define constraintListGROWLOW 4 /* addl used in abstract.c */

/*@constant int constraintListGROWHI;@*/
# define constraintListGROWHI 4

/*@constant int constraintListBASESIZE;@*/
# define constraintListBASESIZE (constraintListGROWLOW + constraintListGROWHI)

# else
# error "Multiple include"
# endif




