/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef LTOKENLIST_H
# define LTOKENLIST_H

abst_typedef /*@null@*/ struct
{
  int nelements;
  int nspace;
  int current;
  /*@reldef@*/ /*@relnull@*/ o_ltoken *elements;
} *ltokenList ;

/*@iter ltokenList_elements (sef ltokenList x, yield exposed ltoken el); @*/
# define ltokenList_elements(x, m_el) \
   { if (ltokenList_isDefined(x)) { \
     int m_ind; ltoken *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { ltoken m_el; m_el = *m_elements; m_elements++; 

# define end_ltokenList_elements }}}

/*
** ltokenList_elements is split up to avoid compiler bugs
** and incompatibilities in older C compilers.  This has caused problems
** on both RS/AIX and SGI IRIX machines.
*/

/*@constant null ltokenList ltokenList_undefined;@*/
# define ltokenList_undefined NULL

extern /*@falsewhennull@*/ bool ltokenList_isDefined (ltokenList p_s) /*@*/ ;
# define ltokenList_isDefined(s) ((s) != ltokenList_undefined)

extern /*@nullwhentrue@*/ bool ltokenList_isUndefined (ltokenList p_s) /*@*/ ;
# define ltokenList_isUndefined(s) ((s) == ltokenList_undefined)

extern int ltokenList_size (/*@sef@*/ ltokenList p_s) /*@*/ ;
extern bool ltokenList_empty (/*@sef@*/ ltokenList p_s) /*@*/ ;
extern /*@unused@*/ bool ltokenList_isEmpty (/*@sef@*/ ltokenList p_s) /*@*/ ;

# define ltokenList_size(s)  (ltokenList_isUndefined(s) ? 0 : (s)->nelements)
# define ltokenList_empty(s) (ltokenList_isUndefined(s) || ltokenList_size(s) == 0)
# define ltokenList_isEmpty(s) (ltokenList_empty(s))

extern /*@only@*/ /*@notnull@*/ ltokenList ltokenList_new (void) /*@*/ ;
extern void ltokenList_addh (ltokenList p_s, /*@only@*/ ltoken p_el) /*@modifies p_s@*/ ;

extern void ltokenList_reset (ltokenList p_s) /*@modifies p_s@*/ ;  
extern void ltokenList_advance (ltokenList p_s) /*@modifies p_s@*/ ;

extern /*@only@*/ cstring ltokenList_unparse (ltokenList p_s) /*@*/ ;
extern void ltokenList_free (/*@only@*/ ltokenList p_s) ;

extern /*@observer@*/ ltoken ltokenList_head (ltokenList p_s) /*@*/ ;
extern /*@observer@*/ ltoken ltokenList_current (ltokenList p_s) /*@*/ ;

extern /*@only@*/ ltokenList ltokenList_copy (ltokenList p_s) /*@*/ ; 
extern /*@only@*/ /*@notnull@*/ ltokenList ltokenList_singleton (/*@only@*/ ltoken p_l);
extern ltokenList ltokenList_push (/*@returned@*/ ltokenList p_s, /*@only@*/ ltoken p_el);
extern bool ltokenList_equal (ltokenList p_s1, ltokenList p_s2) /*@*/ ;
extern bool ltokenList_isFinished (ltokenList p_s) /*@*/ ;
extern void ltokenList_removeCurrent (ltokenList p_s) /*@modifies p_s@*/ ;

/*@constant int ltokenListBASESIZE;@*/
# define ltokenListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




