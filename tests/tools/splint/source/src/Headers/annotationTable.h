/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** annotationTable.h
**
** A global table that keeps information on the user-defined annotations.
**
** For each annotation, we need:
**
**    o [key] A name (matches @name@ in code)
**    o State - index into metaStateTable that identifies the state associated with this annotation
**    o Value 
**    o Context information (where can annotation be used)
*/

# ifndef ANNOTTABLE_H
# define ANNOTTABLE_H

/*@constant null annotationTable annotationTable_undefined; @*/
# define annotationTable_undefined genericTable_undefined

extern /*@falsewhennull@*/ bool annotationTable_isDefined(annotationTable) /*@*/ ;
# define annotationTable_isDefined(p_h) (genericTable_isDefined ((genericTable) (p_h)))

extern /*@nullwhentrue@*/ bool annotationTable_isUndefined(annotationTable) /*@*/ ;
# define annotationTable_isUndefined(p_h) (genericTable_isDefined ((genericTable) (p_h)))

/*@constant int DEFAULT_ANNOTTABLE_SIZE@*/
# define DEFAULT_ANNOTTABLE_SIZE 32

extern /*@only@*/ annotationTable annotationTable_create (void) /*@*/ ;
# define annotationTable_create() ((annotationTable) genericTable_create (DEFAULT_ANNOTTABLE_SIZE))

extern void annotationTable_insert (annotationTable p_h, /*@only@*/ annotationInfo p_annotation);

extern /*@null@*/ /*@dependent@*/ /*@exposed@*/ annotationInfo
   annotationTable_lookup (annotationTable p_h, cstring p_key) /*@*/ ;
/*@access annotationInfo@*/
# define annotationTable_lookup(p_h,p_key) \
  ((annotationInfo) genericTable_lookup ((genericTable) (p_h), p_key))
/*@noaccess annotationInfo@*/

extern bool annotationTable_contains (annotationTable p_h, cstring p_key) /*@*/ ;
# define annotationTable_contains(p_h,p_key) \
  (genericTable_contains ((genericTable) (p_h), p_key))

extern /*@unused@*/ /*@only@*/ cstring annotationTable_stats(annotationTable p_h);
# define annotationTable_stats(p_h) genericTable_stats ((genericTable) (p_h))

extern /*@only@*/ cstring annotationTable_unparse (annotationTable p_h);

extern void annotationTable_free (/*@only@*/ annotationTable p_h);
# define annotationTable_free(p_h) (genericTable_free ((genericTable) (p_h)))

/*@iter annotationTable_elements (sef annotationTable p_g, 
                                  yield exposed cstring m_key, 
				  yield exposed annotationInfo m_el) @*/

# define annotationTable_elements(p_g,m_key,m_el) \
            genericTable_elements((genericTable) (p_g), m_key, m_el) 
# define end_annotationTable_elements end_genericTable_elements

extern int annotationTable_size (annotationTable p_h);
# define annotationTable_size(p_h) (genericTable_size(p_h))

# else
# error "Multiple include"
# endif 




