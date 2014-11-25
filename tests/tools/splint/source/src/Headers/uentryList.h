/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** uentryList.h (from slist_templace.h)
*/

# ifndef uentryLIST_H
# define uentryLIST_H

abst_typedef /*@null@*/ struct {
  int nelements;
  int nspace;
  int current;
  /*@reldef@*/ /*@relnull@*/ o_uentry  *elements;
} *uentryList;

/*@iter uentryList_elements (sef uentryList x, yield exposed uentry el); @*/
# define uentryList_elements(x, m_el) \
   { if (!uentryList_isUndefined(x) && !uentryList_isVoid(x))\
       { int m_ind; uentry *m_elements = &((x)->elements[0]); \
          for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
           { uentry m_el = *(m_elements++); 

# define end_uentryList_elements }}}

extern void uentryList_clear (uentryList p_s) /*@modifies p_s@*/ ;
extern int uentryList_size (uentryList p_s) /*@*/ ;
extern /*@only@*/ uentryList uentryList_makeMissingParams (void);

/*@constant null uentryList uentryList_missingParams; @*/
# define uentryList_missingParams uentryList_undefined

# define uentryList_makeMissingParams() uentryList_missingParams

extern /*@nullwhentrue@*/ bool uentryList_isMissingParams (uentryList p_s) /*@*/ ;
extern /*@nullwhentrue@*/ bool uentryList_isUndefined (uentryList p_s) /*@*/ ;
extern /*@unused@*/ /*@nullwhentrue@*/ bool uentryList_isEmpty (uentryList p_s) /*@*/ ;
extern /*@unused@*/ /*@falsewhennull@*/ bool uentryList_isDefined (uentryList p_s) /*@*/ ;

# define uentryList_isEmpty(s) (uentryList_size(s) == 0)

/*@constant null uentryList uentryList_undefined; @*/
# define uentryList_undefined      ((uentryList)0)

# define uentryList_isUndefined(c) ((c) == uentryList_undefined)
# define uentryList_isDefined(c)   ((c) != uentryList_undefined)

extern /*@notnull@*/ /*@only@*/ uentryList uentryList_new(void);
extern uentryList uentryList_add (/*@returned@*/ uentryList p_s, /*@keep@*/ uentry p_el)  /*@modifies p_s@*/ ;
extern /*@only@*/ uentryList uentryList_single (/*@keep@*/ uentry p_el) /*@*/ ;
extern /*@exposed@*/ uentry uentryList_getN (uentryList p_p, int p_n) /*@*/ ;

extern /*@unused@*/ cstring uentryList_unparseFull (uentryList p_s) /*@*/ ;
extern cstring uentryList_unparse (uentryList p_s) /*@*/ ;
extern cstring uentryList_unparseAbbrev (uentryList p_p) /*@*/ ;
extern cstring uentryList_unparseParams (uentryList p_s) /*@*/ ;

extern void uentryList_free (/*@only@*/ uentryList p_s) ;
extern void uentryList_freeShallow (/*@only@*/ uentryList p_s) ;
extern bool uentryList_isVoid (uentryList p_cl) /*@*/ ;
extern /*@only@*/ uentryList uentryList_copy (uentryList p_s) /*@*/ ;
extern void uentryList_fixMissingNames (uentryList p_cl) /*@modifies p_cl@*/ ;

extern int uentryList_compareStrict (uentryList p_s, uentryList p_t) /*@*/ ;
extern int uentryList_compareParams (uentryList p_s, uentryList p_t) /*@*/ ;
extern int uentryList_compareFields (uentryList p_s, uentryList p_t) /*@*/ ;
extern bool uentryList_equivFields (uentryList p_p1, uentryList p_p2) /*@*/ ;

extern cstring uentryList_dumpParams (uentryList p_s) /*@*/ ;
extern uentryList uentryList_undump (char **p_s) /*@modifies *p_s@*/ ;

extern bool uentryList_hasReturned (uentryList p_ul) /*@*/ ; 
extern void uentryList_advanceSafe (uentryList p_s) /*@modifies p_s@*/;
extern bool uentryList_isFinished (uentryList p_s) /*@*/ ;
extern void uentryList_reset (uentryList p_s) /*@modifies p_s@*/ ;
extern /*@exposed@*/ uentry uentryList_current (uentryList p_s) /*@*/ ;
extern int uentryList_lookupRealName (uentryList p_s, cstring p_name) /*@*/ ;
extern /*@exposed@*/ uentry 
  uentryList_lookupField (uentryList p_f, cstring p_name) /*@*/ ;
extern uentryList uentryList_mergeFields (/*@only@*/ uentryList p_f1, /*@only@*/ uentryList p_f2);
extern void uentryList_showFieldDifference (uentryList p_p1, uentryList p_p2);
extern /*@only@*/ uentryList uentryList_undumpFields (char **p_s, fileloc p_loc);
extern /*@only@*/ cstring uentryList_dumpFields (uentryList p_s);
extern void uentryList_fixImpParams (uentryList p_cl);
extern bool uentryList_sameObject (uentryList, uentryList);
extern bool uentryList_matchFields (uentryList p_p1, uentryList p_p2);
extern bool uentryList_matchParams (uentryList p_p1, uentryList p_p2, bool p_force, bool p_arg);

# define uentryList_sameObject(s,t) ((s) == (t))

/*@constant int uentryListBASESIZE;@*/
# define uentryListBASESIZE MIDBASESIZE

/*@constant int uentryList_abbrevBreadth@*/
# define uentryList_abbrevBreadth 3

# else
# error "Multiple include"
# endif




