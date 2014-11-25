/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** aliasTable.h
*/

# ifndef aliasTable_H
# define aliasTable_H

typedef /*@only@*/ sRefSet o_sRefSet;
typedef /*@exposed@*/ sRef e_sRef;

struct s_aliasTable 
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@only@*/ e_sRef *keys;
  /*@reldef@*/ /*@only@*/ o_sRefSet *values;
} ; 

extern /*@unused@*/ /*@nullwhentrue@*/ bool aliasTable_isUndefined (aliasTable p_s);
extern /*@unused@*/ /*@nullwhentrue@*/ bool 
  aliasTable_isEmpty (/*@sef@*/ aliasTable p_s);
extern /*@falsewhennull@*/ bool aliasTable_isDefined (aliasTable p_s);

/*@constant null aliasTable aliasTable_undefined; @*/
# define aliasTable_undefined      ((aliasTable) NULL)
# define aliasTable_isDefined(s)   ((s) != aliasTable_undefined)
# define aliasTable_isUndefined(s) ((s) == aliasTable_undefined)
# define aliasTable_isEmpty(s)     (aliasTable_size(s) == 0)

extern int aliasTable_size (/*@sef@*/ aliasTable p_s);
# define aliasTable_size(s)  (aliasTable_isDefined (s) ? (s)->nelements : 0)

/*@iter aliasTable_elements (sef aliasTable t, yield exposed sRef key, yield exposed sRefSet values); @*/
# define aliasTable_elements(t, m_key, m_value) \
   { if (aliasTable_isDefined (t)) \
     { int m_ind; sRef *m_keys = &((t)->keys[0]); \
       sRefSet *m_values = &((t)->values[0]); \
       for (m_ind = 0 ; m_ind < (t)->nelements; m_ind++) \
         { sRef m_key = *(m_keys++); sRefSet m_value = *(m_values++);

# define end_aliasTable_elements }}}

extern aliasTable aliasTable_new (void) /*@*/ ;

extern void aliasTable_clearAliases (aliasTable p_s, sRef p_sr) 
   /*@modifies p_s, p_sr@*/ ;

extern /*@only@*/ sRefSet aliasTable_canAlias (aliasTable p_s, sRef p_sr) /*@*/ ;
extern aliasTable aliasTable_copy (aliasTable p_s) /*@*/ ;
 
extern /*@only@*/ cstring aliasTable_unparse (aliasTable p_s) /*@*/ ;
extern void aliasTable_free (/*@only@*/ aliasTable p_s) ;

extern aliasTable 
  aliasTable_addMustAlias (/*@returned@*/ aliasTable p_s, /*@exposed@*/ sRef p_sr, /*@exposed@*/ sRef p_al)
  /*@modifies p_s@*/ ;

extern aliasTable
  aliasTable_levelUnion (/*@returned@*/ aliasTable p_t1, aliasTable p_t2, int p_level) 
  /*@modifies p_t1@*/ ;

extern aliasTable 
  aliasTable_levelUnionNew (aliasTable p_t1, aliasTable p_t2, int p_level) 
  /*@modifies nothing*/ ;

extern void aliasTable_checkGlobs (aliasTable p_t) /*@modifies g_warningstream@*/ ;
extern /*@only@*/ sRefSet aliasTable_aliasedBy (aliasTable p_s, sRef p_sr) /*@*/ ;
extern void aliasTable_fixSrefs (aliasTable p_s);
extern aliasTable aliasTable_levelUnionSeq (/*@returned@*/ aliasTable p_t1, 
					    /*@only@*/ aliasTable p_t2, int p_level);

/*@constant int aliasTableBASESIZE; @*/
# define aliasTableBASESIZE MIDBASESIZE

/*
** For debugging only
*/

# ifdef DEBUGSPLINT
extern void aliasTable_checkValid (aliasTable) /*@modifies g_errorstream@*/ ;
# endif

# else
# error "Multiple include"
# endif




