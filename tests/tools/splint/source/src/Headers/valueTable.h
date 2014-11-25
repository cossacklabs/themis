/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** valueTable.h
*/

/*
** valueTable is a table of stateValue's.
** Based on genericTable
*/

# ifndef VTABLE_H
# define VTABLE_H

/*@constant null valueTable valueTable_undefined; @*/
# define valueTable_undefined genericTable_undefined

extern /*@falsewhennull@*/ bool valueTable_isDefined(valueTable) /*@*/ ;
# define valueTable_isDefined(p_h) (genericTable_isDefined ((genericTable) (p_h)))

extern /*@nullwhentrue@*/ bool valueTable_isUndefined(valueTable) /*@*/ ;
# define valueTable_isUndefined(p_h) (genericTable_isUndefined ((genericTable) (p_h)))

extern /*@only@*/ valueTable valueTable_create(int p_size);
# define valueTable_create(p_s) ((valueTable) genericTable_create (p_s))

extern void valueTable_insert (valueTable p_h, 
			       /*@only@*/ cstring p_key, 
			       /*@only@*/ stateValue p_value);

/*@access stateValue@*/
extern /*@null@*/ /*@dependent@*/ /*@exposed@*/ stateValue 
   valueTable_lookup (valueTable p_h, cstring p_key) /*@*/ ;
# define valueTable_lookup(p_h,p_key) \
  ((stateValue) genericTable_lookup ((genericTable) (p_h), p_key))
/*@noaccess stateValue@*/

extern bool valueTable_contains (valueTable p_h, cstring p_key) /*@*/ ;
# define valueTable_contains(p_h,p_key) \
 (stateValue_isDefined (valueTable_lookup (p_h, p_key)))

extern /*@unused@*/ /*@only@*/ cstring valueTable_stats(valueTable p_h);
# define valueTable_stats(p_h) genericTable_stats ((genericTable) (p_h))

extern void valueTable_free (/*@only@*/ valueTable p_h);
# define valueTable_free(p_h) (genericTable_free ((genericTable) (p_h)))

extern /*@only@*/ cstring valueTable_unparse (valueTable p_h) /*@*/ ;

extern void valueTable_update (valueTable p_h, cstring p_key, /*@owned@*/ stateValue p_newval) /*@modifies p_h@*/ ;

extern /*@only@*/ valueTable valueTable_copy (valueTable p_s) ;

/*@iter valueTable_elements (sef valueTable p_g, yield exposed cstring m_key, yield exposed stateValue m_el)@*/

# define valueTable_elements(p_g, m_key, m_el) genericTable_elements (p_g, m_key, m_el)
# define end_valueTable_elements end_genericTable_elements

extern int valueTable_size (valueTable p_h) /*@*/ ;
# define valueTable_size(p_h) (genericTable_size(p_h))


# else
# error "Multiple include"
# endif 




