/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** lslOpSet.h
**
** based on set_template.h
*/

# ifndef lslOpSET_H
# define lslOpSET_H

abst_typedef /*@null@*/ struct
{
  int entries;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_lslOp *elements;
} *lslOpSet ;

/*@iter lslOpSet_elements (sef lslOpSet s, yield exposed lslOp el); @*/
# define lslOpSet_elements(x, m_el) \
   { if (lslOpSet_isDefined (x)) { int m_ind; \
     for (m_ind = 0 ; m_ind < (x)->entries; m_ind++) \
       { lslOp m_el = (x)->elements[m_ind];

# define end_lslOpSet_elements }}}

/*@constant null lslOpSet lslOpSet_undefined;@*/
# define lslOpSet_undefined ((lslOpSet) NULL)

extern /*@falsewhennull@*/ bool lslOpSet_isDefined (lslOpSet p_s) /*@*/ ;
# define lslOpSet_isDefined(s) ((s) != lslOpSet_undefined)

extern /*@unused@*/ int 
  lslOpSet_size (/*@sef@*/ lslOpSet p_s) /*@*/ ;

# define lslOpSet_size(s) \
  (lslOpSet_isDefined(s) ? (s)->entries : 0)

extern /*@only@*/ lslOpSet lslOpSet_new (void) /*@*/ ;
extern bool lslOpSet_insert (lslOpSet p_s, /*@only@*/ lslOp p_el)
   /*@modifies p_s@*/ ;

extern /*@unused@*/ /*@only@*/ cstring lslOpSet_unparse (lslOpSet p_s) /*@*/ ;
extern void lslOpSet_free (/*@only@*/ lslOpSet p_s);
extern /*@only@*/ lslOpSet lslOpSet_copy (lslOpSet p_s) /*@*/ ;

/*@constant int lslOpSetBASESIZE;@*/
# define lslOpSetBASESIZE MIDBASESIZE

# else
# error "Multiple include"
# endif
