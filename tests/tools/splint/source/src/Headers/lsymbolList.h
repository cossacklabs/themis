/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

# ifndef LSYMBOLLIST_H
# define LSYMBOLLIST_H

abst_typedef struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ lsymbol  *elements;
} *lsymbolList ;

/*@iter lsymbolList_elements (sef lsymbolList x, yield lsymbol el); @*/
# define lsymbolList_elements(x, m_el) \
   { int m_ind; lsymbol *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { lsymbol m_el = *(m_elements++);

# define end_lsymbolList_elements }}

extern /*@only@*/ lsymbolList lsymbolList_new(void);
extern void lsymbolList_addh (lsymbolList p_s, lsymbol p_el) ;

extern void lsymbolList_free (/*@only@*/ lsymbolList p_s) ;

/*@constant int lsymbolListBASESIZE;@*/
# define lsymbolListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




