/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** lsymbolSet.h
**
** based on set_template.h
*/

# ifndef LSYMBOLSET_H
# define LSYMBOLSET_H

abst_typedef /*@null@*/ struct
{
  int entries;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ lsymbol  *elements;
} *lsymbolSet ;

/*@constant null lsymbolSet lsymbolSet_undefined; @*/
# define lsymbolSet_undefined (NULL)

extern /*@falsewhennull@*/ bool lsymbolSet_isDefined (lsymbolSet p_l) /*@*/ ;
# define lsymbolSet_isDefined(l) ((l) != lsymbolSet_undefined)
 
/*@iter lsymbolSet_elements (sef lsymbolSet s, yield lsymbol el); @*/
# define lsymbolSet_elements(x, m_el) \
     if (lsymbolSet_isDefined (x)) \
      { int m_ind; \
        for (m_ind = 0 ; m_ind < (x)->entries; m_ind++) \
         { lsymbol m_el = (x)->elements[m_ind]; 

# define end_lsymbolSet_elements }}

extern /*@only@*/ lsymbolSet lsymbolSet_new(void) /*@*/ ;
extern bool lsymbolSet_insert (lsymbolSet p_s, lsymbol p_el) /*@modifies p_s@*/ ;
extern bool lsymbolSet_member (lsymbolSet p_s, lsymbol p_el) /*@*/ ;
extern /*@unused@*/ /*@only@*/ cstring lsymbolSet_unparse (lsymbolSet p_s) /*@*/ ;
extern void lsymbolSet_free (/*@only@*/ lsymbolSet p_s);

/*@constant int lsymbolSetBASESIZE;@*/
# define lsymbolSetBASESIZE MIDBASESIZE

# else
# error "Multiple include"
# endif
