/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** sRefList.h (from slist_templace.h)
*/

# ifndef sRefLIST_H
# define sRefLIST_H

typedef /*@dependent@*/ sRef d_sRef ;

struct s_sRefList
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ d_sRef  *elements;
} ;

/*@iter sRefList_elements (sef sRefList x, yield exposed sRef el); @*/
# define sRefList_elements(x, m_el) \
   { if (!sRefList_isUndefined(x)) \
       { int m_ind; sRef *m_elements = &((x)->elements[0]); \
          for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
           { sRef m_el = *(m_elements++); 

# define end_sRefList_elements }}}

extern int sRefList_size (sRefList p_s) /*@*/ ;

extern /*@nullwhentrue@*/ bool sRefList_isUndefined (sRefList p_s) /*@*/ ;
extern /*@unused@*/ /*@nullwhentrue@*/ bool sRefList_isEmpty (sRefList p_s) /*@*/ ;
extern /*@unused@*/ /*@falsewhennull@*/ bool sRefList_isDefined (sRefList p_s) /*@*/ ;

# define sRefList_isEmpty(s) (sRefList_size(s) == 0)

/*@constant null sRefList sRefList_undefined; @*/
# define sRefList_undefined      ((sRefList)0)

# define sRefList_isUndefined(c) ((c) == sRefList_undefined)
# define sRefList_isDefined(c)   ((c) != sRefList_undefined)

extern /*@only@*/ sRefList sRefList_new (void);
extern /*@only@*/ sRefList sRefList_single (/*@dependent@*/ sRef p_el);

extern /*@notnull@*/ sRefList 
   sRefList_add (/*@returned@*/ sRefList p_s, /*@exposed@*/ sRef p_el)  /*@modifies p_s@*/ ;

extern cstring sRefList_unparse (sRefList p_s) /*@*/ ;
extern void sRefList_free (/*@only@*/ sRefList p_s) ;
extern /*@only@*/ sRefList sRefList_copy (sRefList p_s) /*@*/ ;

/*@constant int sRefListBASESIZE;@*/
# define sRefListBASESIZE MIDBASESIZE

# else
# error "Multiple include"
# endif




