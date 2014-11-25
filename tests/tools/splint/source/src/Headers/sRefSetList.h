/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef sRefSetLIST_H
# define sRefSetLIST_H

typedef /*@dependent@*/ sRefSet d_sRefSet;

abst_typedef /*@null@*/ struct
{
  int      nelements;
  int      nspace;
  /*@reldef@*/ /*@relnull@*/ d_sRefSet *elements;
} *sRefSetList ;

/*@iter sRefSetList_elements (sef sRefSetList x, yield exposed sRefSet el); @*/
# define sRefSetList_elements(x, m_el) \
   { if (sRefSetList_isDefined (x)) { \
     int m_ind; sRefSet *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { sRefSet m_el = *(m_elements++); 

# define end_sRefSetList_elements }}}

/*@constant null sRefSetList sRefSetList_undefined; @*/
# define sRefSetList_undefined  ((sRefSetList) NULL)

extern /*@falsewhennull@*/ bool sRefSetList_isDefined (sRefSetList p_s);
# define sRefSetList_isDefined(s) ((s) != sRefSetList_undefined)

extern /*@unused@*/ /*@nullwhentrue@*/ bool sRefSetList_isUndefined (sRefSetList p_s);
# define sRefSetList_isUndefined(s) ((s) == sRefSetList_undefined)

extern sRefSetList 
  sRefSetList_add (/*@returned@*/ sRefSetList p_s, /*@exposed@*/ sRefSet p_el)
  /*@modifies p_s@*/ ;

extern void sRefSetList_free (/*@only@*/ sRefSetList p_s);
extern void sRefSetList_clear (sRefSetList p_s);

/*@constant int sRefSetListBASESIZE;@*/
# define sRefSetListBASESIZE MIDBASESIZE

# else
# error "Multiple include"
# endif




