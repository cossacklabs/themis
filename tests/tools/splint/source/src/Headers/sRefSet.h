/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** sRefSet.h
**
** based on set_template.h
*/

# ifndef sRefSET_H
# define sRefSET_H

typedef /*@exposed@*/ sRef ex_sRef;

struct s_sRefSet
{
  int entries;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ ex_sRef *elements;
} ;

/* in forwardTypes: typedef _sRefSet *sRefSet; */

/*
** realElements --- only non-objects 
*/

/*@iter sRefSet_realElements (sef sRefSet s, yield exposed sRef el)@*/
# define sRefSet_realElements(x, m_el) \
   { int m_ind; if (sRefSet_isDefined (x)) \
     { for (m_ind = 0 ; m_ind < (x)->entries; m_ind++) \
       { sRef m_el = (x)->elements[m_ind]; if (!(sRef_isExternal(m_el))) {
# define end_sRefSet_realElements }}}}

/*@iter sRefSet_elements (sef sRefSet s, yield exposed sRef el)@*/
# define sRefSet_elements(s,m_el) sRefSet_allElements (s, m_el)
# define end_sRefSet_elements   end_sRefSet_allElements

/*@iter sRefSet_allElements (sef sRefSet s, yield exposed sRef el)@*/
# define sRefSet_allElements(x, m_el) \
   { int m_ind; if (sRefSet_isDefined (x)) { \
     for (m_ind = 0 ; m_ind < (x)->entries; m_ind++) \
       { sRef m_el = (x)->elements[m_ind];  

# define end_sRefSet_allElements }}}

/*@constant int sRefSetBASESIZE;@*/
# define sRefSetBASESIZE SMALLBASESIZE

/*@constant null sRefSet sRefSet_undefined;@*/
# define sRefSet_undefined ((sRefSet) 0)

extern /*@nullwhentrue@*/ bool sRefSet_isUndefined (sRefSet p_s) /*@*/ ;
extern /*@nullwhentrue@*/ bool sRefSet_isEmpty (/*@sef@*/ sRefSet p_s) /*@*/ ;
extern /*@falsewhennull@*/ bool sRefSet_isDefined (sRefSet p_s) /*@*/ ;

# define sRefSet_isUndefined(s) ((s) == sRefSet_undefined)
# define sRefSet_isDefined(s)   ((s) != sRefSet_undefined)

# define sRefSet_isEmpty(s) \
  ((s) == sRefSet_undefined || ((s)->entries == 0))

extern bool sRefSet_equal (sRefSet p_s1, sRefSet p_s2) /*@*/ ;
extern bool sRefSet_hasRealElement (sRefSet p_s) /*@*/ ;
extern bool sRefSet_hasUnconstrained (sRefSet p_s) /*@*/ ;
extern cstring sRefSet_unparsePlain (sRefSet p_s) /*@*/ ;
extern cstring sRefSet_unparseUnconstrained (sRefSet p_s) /*@*/ ;
extern cstring sRefSet_unparseUnconstrainedPlain (sRefSet p_s) /*@*/ ;
extern void sRefSet_fixSrefs (sRefSet p_s);
extern bool sRefSet_delete (sRefSet p_s, sRef p_el);
extern /*@exposed@*/ sRef sRefSet_lookupMember (sRefSet p_s, sRef p_el);
extern bool sRefSet_isSameMember (sRefSet p_s, sRef p_el) /*@*/ ;
extern bool sRefSet_isSameNameMember (sRefSet p_s, sRef p_el) /*@*/ ;
extern /*@only@*/ sRefSet sRefSet_newCopy (/*@exposed@*/ /*@temp@*/ sRefSet p_s);
extern /*@only@*/ sRefSet sRefSet_newDeepCopy (sRefSet p_s);
extern int sRefSet_size(sRefSet p_s) /*@*/ ;
extern sRefSet sRefSet_unionFree (/*@returned@*/ sRefSet p_s1, /*@only@*/ sRefSet p_s2);
extern /*@only@*/ sRefSet sRefSet_new (void) /*@*/ ;
extern /*@only@*/ sRefSet sRefSet_single (/*@exposed@*/ sRef);
extern sRefSet sRefSet_insert (/*@returned@*/ sRefSet p_s, /*@exposed@*/ sRef p_el);
extern bool sRefSet_member (sRefSet p_s, sRef p_el) /*@*/ ;
extern bool sRefSet_containsSameObject (sRefSet p_s, sRef p_el) /*@*/ ;
extern /*@only@*/ cstring sRefSet_unparse (sRefSet p_s) /*@*/ ;
extern void sRefSet_free (/*@only@*/ sRefSet p_s) /*@modifies p_s@*/;
extern void sRefSet_clear (sRefSet p_s) /*@modifies p_s@*/;
extern /*@only@*/ sRefSet sRefSet_addIndirection (sRefSet p_s) /*@*/ ;
extern /*@only@*/ sRefSet sRefSet_removeIndirection (sRefSet p_s) /*@*/ ;
extern sRefSet 
  sRefSet_union (/*@returned@*/ sRefSet p_s1, /*@exposed@*/ sRefSet p_s2) 
  /*@modifies p_s1@*/ ;
extern void sRefSet_levelPrune (sRefSet p_s, int p_lexlevel)
  /*@modifies p_s@*/ ;
extern void sRefSet_clearStatics (sRefSet p_s)
  /*@modifies p_s@*/ ;
extern sRefSet sRefSet_levelUnion (/*@returned@*/ sRefSet p_sr, sRefSet p_s, int p_lexlevel);
extern /*@only@*/ sRefSet sRefSet_intersect (sRefSet p_s1, sRefSet p_s2);
extern /*@only@*/ sRefSet sRefSet_fetchKnown (sRefSet p_s, int p_i);
extern /*@only@*/ sRefSet sRefSet_fetchUnknown (sRefSet p_s);
extern /*@only@*/ sRefSet sRefSet_accessField (sRefSet p_s, /*@observer@*/ cstring p_f);
extern /*@only@*/ sRefSet sRefSet_realNewUnion (sRefSet p_s1, sRefSet p_s2);
extern /*@only@*/ cstring sRefSet_unparseDebug (sRefSet p_s) /*@*/ ;
extern /*@unused@*/ cstring sRefSet_unparseFull (sRefSet p_s) /*@*/ ;
extern int sRefSet_compare (sRefSet p_s1, sRefSet p_s2) /*@*/ ;
extern bool sRefSet_modifyMember (sRefSet p_s, sRef p_m) /*@modifies p_m@*/ ;
extern /*@only@*/ sRefSet sRefSet_undump (char **p_s) /*@modifies *p_s@*/ ;
extern /*@only@*/ cstring sRefSet_dump (sRefSet p_sl) /*@*/ ;
extern bool sRefSet_deleteBase (sRefSet p_s, sRef p_base) /*@modifies p_s@*/ ;
extern /*@exposed@*/ sRef sRefSet_choose (sRefSet p_s) /*@*/ ;
extern /*@exposed@*/ sRef sRefSet_mergeIntoOne (sRefSet p_s) /*@*/ ;
extern /*@only@*/ sRefSet 
  sRefSet_levelCopy (/*@exposed@*/ sRefSet p_s, int p_lexlevel) /*@*/ ;
extern sRefSet 
  sRefSet_unionExcept (/*@returned@*/ sRefSet p_s1, sRefSet p_s2, sRef p_ex)
     /*@modifies p_s1@*/ ;

sRefSet sRefSet_copyInto (/*@returned@*/ sRefSet p_s1, /*@exposed@*/ sRefSet p_s2)
     /*@modifies p_s1@*/ ;

extern bool sRefSet_hasStatic (sRefSet p_s) /*@*/ ;

extern void sRefSet_markImmutable (sRefSet p_s) /*@modifies p_s@*/ ;

# else
# error "Multiple include"
# endif



