/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** sortSet.h
**
** based on set_template.h
*/

# ifndef SORTSET_H
# define SORTSET_H

abst_typedef /*@null@*/ struct 
{
  int entries;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ sort  *elements;
} *sortSet ;

/*@iter sortSet_elements(sef sortSet s, yield sort el); @*/
# define sortSet_elements(x, m_el) \
    if (sortSet_isDefined (x)) { int m_ind; \
       for (m_ind = 0 ; m_ind < (x)->entries; m_ind++) \
         { sort m_el = (x)->elements[m_ind];

# define end_sortSet_elements }}

/*@constant null sortSet sortSet_undefined; @*/
# define sortSet_undefined ((sortSet) NULL)
extern /*@falsewhennull@*/ bool sortSet_isDefined (sortSet p_s) /*@*/ ;
# define sortSet_isDefined(s) ((s) != sortSet_undefined)

extern int sortSet_size (/*@sef@*/ sortSet p_s);
# define sortSet_size(s) (sortSet_isDefined (s) ? (s)->entries : 0)

extern /*@only@*/ sortSet sortSet_new(void);
extern bool sortSet_insert (sortSet p_s, sort p_el);
extern bool sortSet_member (sortSet p_s, sort p_el);
extern /*@only@*/ cstring sortSet_unparse (sortSet p_s);
extern /*@only@*/ cstring sortSet_unparseClean (sortSet p_s);
extern /*@only@*/ cstring sortSet_unparseOr (sortSet p_s);
extern void sortSet_free (/*@only@*/ sortSet p_s);
extern sort sortSet_choose (sortSet p_s);
extern /*@only@*/ sortSet sortSet_copy (sortSet p_s);

/*@constant int sortSetBASESIZE;@*/
# define sortSetBASESIZE MIDBASESIZE

# else
# error "Multiple include"
# endif

