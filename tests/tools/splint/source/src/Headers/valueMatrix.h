/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** valueMatrix.h
*/

/*
**          key -> (int x int -> int x cstring)
** e.g., "null" -> null x notnull -> error x "attempt to ..."
*/

# ifndef VALUEMATRIX_H
# define VALUEMATRIX_H

/* in forward types:
abst_typedef genericTable valueMatrix;
*/

/*@constant null valueMatrix valueMatrix_undefined; @*/
# define valueMatrix_undefined genericTable_undefined

extern /*@falsewhennull@*/ bool valueMatrix_isDefined(valueMatrix) /*@*/ ;
# define valueMatrix_isDefined(p_h) (genericTable_isDefined ((genericTable) (p_h)))

extern /*@nullwhentrue@*/ bool valueMatrix_isUndefined(valueMatrix) /*@*/ ;
# define valueMatrix_isUndefined(p_h) (genericTable_isDefined ((genericTable) (p_h)))

extern /*@only@*/ valueMatrix valueMatrix_create(int p_size);
# define valueMatrix_create(p_s) ((valueMatrix) genericTable_create (p_s))

extern void valueMatrix_insert (valueMatrix p_h, /*@dependent@*/ cstring p_key, 
				/*@only@*/ stateCombinationTable p_value);

# define valueMatrix_insert(p_h,p_key,p_value) \
  (genericTable_insert ((valueMatrxi) (p_h), p_key, (void *) (p_value)))

extern /*@null@*/ /*@dependent@*/ /*@exposed@*/ stateCombinationTable valueMatrix_lookup (valueMatrix p_h, cstring p_key) /*@*/ ;
# define valueMatrix_lookup(p_h,p_key) \
  ((stateCombinationTable) genericTable_lookup ((genericTable) (p_h), p_key))

extern /*@unused@*/ /*@only@*/ cstring valueMatrix_stats(valueMatrix p_h);
# define valueMatrix_stats(p_h) genericTable_stats ((genericTable) (p_h))

extern void valueMatrix_free (/*@only@*/ valueMatrix p_h);
# define valueMatrix_free(p_h) (genericTable_free ((genericTable) (p_h)))

extern void valueMatrix_remove (valueMatrix p_h, cstring p_key) /*@modifies p_h@*/ ;
# define valueMatrix_remove(p_h,p_key) (genericTable_remove ((genericTable) (p_h), p_key))

/*@iter valueMatrix_elements (sef valueMatrix p_g, yield exposed cstring m_key, yield exposed stateCombinationTable m_el)@*/

# define valueMatrix_elements(p_g,m_key,m_el) genericTable_elements((genericTable) (p_g), m_key, m_el) 
# define end_valueMatrix_elements end_genericTable_elements

extern int valueMatrix_size (valueMatrix p_h);
# define valueMatrix_size(p_h) (genericTable_size(p_h))

# else
# error "Multiple include"
# endif 




