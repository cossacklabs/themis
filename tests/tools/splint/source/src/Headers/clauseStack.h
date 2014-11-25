/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** clauseStack.h
*/

# ifndef clauseStack_H
# define clauseStack_H

abst_typedef struct
{
  int nelements;
  int nspace;
  int current;
  /*@reldef@*/ clause *elements;
} *clauseStack ;

/*@iter clauseStack_elements (sef clauseStack s, yield clause el); @*/
# define clauseStack_elements(s, m_el) \
  { int m_i; for (m_i = (s)->nelements - 1; m_i >= 0; m_i--) { clause m_el = (s)->elements[m_i]; 

# define end_clauseStack_elements }} 

extern int clauseStack_size (clauseStack p_s) /*@*/ ;
extern bool clauseStack_isEmpty (clauseStack p_s) /*@*/ ;

# define clauseStack_size(s) ((s)->nelements)   
# define clauseStack_isEmpty(s) (clauseStack_size(s) == 0)

extern /*@only@*/ clauseStack clauseStack_new (void) /*@*/ ;

extern void clauseStack_push (clauseStack p_s, clause p_el) /*@modifies p_s@*/ ;
extern void clauseStack_pop (clauseStack p_s) /*@modifies p_s@*/ ;
extern clause clauseStack_top (clauseStack p_s) /*@*/ ;

extern /*@only@*/ cstring clauseStack_unparse (clauseStack p_s) /*@*/ ;
extern void clauseStack_free (/*@only@*/ clauseStack p_s) ;
extern void clauseStack_clear (clauseStack p_s) /*@modifies p_s@*/ ;
extern void clauseStack_switchTop (clauseStack p_s, clause p_x) /*@modifies p_s@*/ ;
extern void clauseStack_removeFirst (clauseStack p_s, clause p_key)
            /*@modifies p_s@*/ ;

extern int clauseStack_controlDepth (clauseStack p_s) /*@*/ ;

/*@constant int clauseStackBASESIZE;@*/
# define clauseStackBASESIZE MIDBASESIZE

# else
# error "Multiple include"
# endif




