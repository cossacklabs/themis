/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** metaStateTable.h
**
** A global table that keeps information on the user-defined states.
**
** For each state definition, we need:
**
**    o A name
**    o State Type (what kinds of things have this state value)
**    o Number of values, and their mnemonics
**    o A combination table (stateCombinationTable) for how different states combine as l/rvalues
**    o Annotations (and map between annotation and value)
**    o Context information (where can annotations be used)
*/

# ifndef MSTABLE_H
# define MSTABLE_H

/*@constant null metaStateTable metaStateTable_undefined; @*/
# define metaStateTable_undefined genericTable_undefined

extern /*@falsewhennull@*/ bool metaStateTable_isDefined(metaStateTable) /*@*/ ;
# define metaStateTable_isDefined(p_h) (genericTable_isDefined ((genericTable) (p_h)))

extern /*@nullwhentrue@*/ bool metaStateTable_isUndefined(metaStateTable) /*@*/ ;
# define metaStateTable_isUndefined(p_h) (genericTable_isDefined ((genericTable) (p_h)))

/*@constant int DEFAULT_MSTABLE_SIZE@*/
# define DEFAULT_MSTABLE_SIZE 32

extern /*@only@*/ metaStateTable metaStateTable_create (void) /*@*/ ;
# define metaStateTable_create() ((metaStateTable) genericTable_create (DEFAULT_MSTABLE_SIZE))

extern void metaStateTable_insert (metaStateTable p_h, 
				   /*@only@*/ cstring p_key, 
				   /*@only@*/ metaStateInfo p_metaState)
     /*@modifies p_h@*/ ;

extern /*@null@*/ /*@dependent@*/ /*@exposed@*/ metaStateInfo
   metaStateTable_lookup (metaStateTable p_h, cstring p_key) /*@*/ ;

/*@access metaStateInfo@*/ 
# define metaStateTable_lookup(p_h,p_key) \
  ((metaStateInfo) genericTable_lookup ((genericTable) (p_h), p_key))
/*@noaccess metaStateInfo@*/

extern bool metaStateTable_contains (metaStateTable p_h, cstring p_key) /*@*/ ;
# define metaStateTable_contains(p_h,p_key) \
  (genericTable_contains ((genericTable) (p_h), p_key))

extern /*@unused@*/ /*@only@*/ cstring metaStateTable_stats(metaStateTable p_h);
# define metaStateTable_stats(p_h) genericTable_stats ((genericTable) (p_h))

extern void metaStateTable_free (/*@only@*/ metaStateTable p_h);
# define metaStateTable_free(p_h) (genericTable_free ((genericTable) (p_h)))

/*@iter metaStateTable_elements (sef metaStateTable p_g, yield exposed cstring m_key, yield exposed metaStateInfo m_el)@*/

/*@access genericTable@*/
# define metaStateTable_elements(p_g, m_key, m_el) \
   { int m_ind; if (metaStateTable_isDefined (p_g)) \
     { for (m_ind = 0 ; m_ind < (p_g)->size; m_ind++) \
       { ghbucket m_hb; m_hb = (p_g)->buckets[m_ind]; \
         if (m_hb != NULL) { \
           int m_j; \
           for (m_j = 0; m_j < (m_hb)->size; m_j++) { \
             cstring m_key; metaStateInfo m_el; m_key = (m_hb)->entries[m_j]->key; \
	      /*@access metaStateInfo@*/ \
              m_el = (metaStateInfo) ((m_hb)->entries[m_j]->val); \
              /*@noaccess metaStateInfo@*/
# define end_metaStateTable_elements }}}}}
/*@noaccess genericTable@*/ 

extern cstring metaStateTable_unparse (metaStateTable p_h) ;

extern int metaStateTable_size (metaStateTable p_h);
# define metaStateTable_size(p_h) (genericTable_size(p_h))

# else
# error "Multiple include"
# endif 




