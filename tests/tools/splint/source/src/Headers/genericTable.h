/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** genericTable.h
*/

# ifndef GHTABLE_H
# define GHTABLE_H

/*@constant int GHBUCKET_BASESIZE; @*/
# define GHBUCKET_BASESIZE 2

/* in forwardTypes:
   abst_typedef null struct _genericTable *genericTable;
*/

/*:private:*/ typedef struct
{
  /*@only@*/ cstring key;
  /*@only@*/ void *val;
} *ghentry;

typedef /*@only@*/ ghentry o_ghentry;

typedef /*@null@*/ struct
{
  int size;
  int nspace;
  /*@only@*/ o_ghentry *entries;
} *ghbucket;

typedef /*@only@*/ ghbucket o_ghbucket;

struct s_genericTable
{
  int size;
  int nentries;
  /*@only@*/ o_ghbucket *buckets;
} ;

/*@constant null genericTable genericTable_undefined; @*/
# define genericTable_undefined      ((genericTable) NULL)

extern /*@falsewhennull@*/ bool genericTable_isDefined(genericTable) /*@*/ ;
# define genericTable_isDefined(p_h) ((p_h) != genericTable_undefined)

extern /*@nullwhentrue@*/ /*@unused@*/ bool genericTable_isUndefined(genericTable) /*@*/ ;
# define genericTable_isUndefined(p_h) ((p_h) == genericTable_undefined)

extern /*@only@*/ genericTable genericTable_create (int p_size);

extern int genericTable_size (genericTable p_h);

extern void genericTable_insert (genericTable p_h, /*@only@*/ cstring p_key,
				 /*@only@*/ void *p_value);
extern /*@null@*/ /*@exposed@*/ void *genericTable_lookup (genericTable p_h, cstring p_key);

extern bool genericTable_contains (genericTable p_h, cstring p_key) /*@*/ ;

extern /*@unused@*/ /*@only@*/ cstring genericTable_stats (genericTable p_h);
extern void genericTable_free (/*@only@*/ genericTable p_h);
extern void genericTable_remove (genericTable p_h, cstring p_key) /*@modifies p_h@*/ ;

extern /*@unused@*/ void genericTable_update (genericTable p_h, cstring p_key,
					      /*@only@*/ void *p_newval) /*@modifies p_h@*/ ;

/*@iter genericTable_elements (sef genericTable p_g, yield exposed cstring m_key, yield exposed void *m_el)@*/

# define genericTable_elements(p_g, m_key, m_el) \
   { int m_ind; if (genericTable_isDefined (p_g)) \
     { for (m_ind = 0 ; m_ind < (p_g)->size; m_ind++) \
       { ghbucket m_hb; m_hb = (p_g)->buckets[m_ind]; \
         if (m_hb != NULL) { \
           int m_j; \
           for (m_j = 0; m_j < (m_hb)->size; m_j++) { \
             cstring m_key; void *m_el; m_key = (m_hb)->entries[m_j]->key; \
	     m_el = (m_hb)->entries[m_j]->val;
# define end_genericTable_elements }}}}}


# else
# error "Multiple include"
# endif 













