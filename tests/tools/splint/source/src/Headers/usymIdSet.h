/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** usymIdSet.h
**
** based on set_template.h
*/

# ifndef USYMIDSET_H
# define USYMIDSET_H

abst_typedef /*@null@*/ struct
{
  int entries;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ usymId  *elements;
} *usymIdSet ;

extern usymIdSet usymIdSet_new(void);
extern bool usymIdSet_member (usymIdSet p_s, usymId p_el) /*@*/ ;
extern usymIdSet usymIdSet_subtract (usymIdSet p_s, usymIdSet p_t);
extern void usymIdSet_free (/*@only@*/ /*@only@*/ usymIdSet p_s);
extern /*@only@*/ cstring usymIdSet_unparse (usymIdSet p_ll);
extern /*@only@*/ cstring usymIdSet_dump (usymIdSet p_lset);
extern usymIdSet usymIdSet_undump (char **p_s);
extern /*@only@*/ usymIdSet usymIdSet_single (usymId p_t);

extern int usymIdSet_compare (usymIdSet p_l1, usymIdSet p_l2);

/*@constant int usymIdSetBASESIZE;@*/
# define usymIdSetBASESIZE SMALLBASESIZE

extern usymIdSet usymIdSet_newUnion (usymIdSet p_s1, usymIdSet p_s2) /*@*/ ;
extern usymIdSet usymIdSet_add (usymIdSet p_s, usymId p_el) /*@*/ ;

extern /*@only@*/ usymIdSet 
  usymIdSet_removeFresh (/*@temp@*/ usymIdSet p_s, usymId p_el);

/*@constant null usymIdSet usymIdSet_undefined; @*/
# define usymIdSet_undefined    ((usymIdSet) NULL)

extern /*@falsewhennull@*/ bool usymIdSet_isDefined (usymIdSet p_s) /*@*/ ;
# define usymIdSet_isDefined(s) ((s) != usymIdSet_undefined)

extern /*@nullwhentrue@*/ bool usymIdSet_isUndefined (usymIdSet p_s) /*@*/ ;
# define usymIdSet_isUndefined(s) ((s) == usymIdSet_undefined)

/*@iter usymIdSet_elements (sef usymIdSet u, yield usymId el); @*/ 
# define usymIdSet_elements(x, m_el) \
   { int m_ind; if (usymIdSet_isDefined (x)) { \
     for (m_ind = 0 ; m_ind < usymIdSet_size(x); m_ind++) \
       { usymId m_el = (x)->elements[m_ind];

# define end_usymIdSet_elements }}}

extern int usymIdSet_size (/*@sef@*/ usymIdSet p_s);
# define usymIdSet_size(s) (usymIdSet_isUndefined(s) ? 0 : (s)->entries)

# else
# error "Multiple include"
# endif

