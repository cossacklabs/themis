/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** intSet.h
**
** based on set_template.h
*/

# ifndef intSET_H
# define intSET_H

abst_typedef struct
{
  int entries;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ int  *elements;
} *intSet;

/*@iter intSet_elements (sef intSet s, yield int el)@*/
# define intSet_elements(x, m_el) \
   { int m_ind; for (m_ind = 0 ; m_ind < (x)->entries; m_ind++) \
       { int m_el = (x)->elements[m_ind];  

# define end_intSet_elements }}

extern /*@only@*/ intSet intSet_new (void);

extern bool intSet_isEmpty (intSet p_s);
# define intSet_isEmpty(s) ((s)->entries == 0)

extern int intSet_size (intSet p_s);
# define intSet_size(s) ((s)->entries)

extern bool intSet_insert (intSet p_s, int p_el);
extern /*@unused@*/ bool intSet_member (intSet p_s, int p_el);
extern /*@only@*/ /*@unused@*/ cstring intSet_unparse (intSet p_s);
extern void intSet_free (/*@only@*/ intSet p_s);

extern /*@only@*/ cstring intSet_unparseText (intSet p_s);

/*@constant int intSetBASESIZE;@*/
# define intSetBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif



