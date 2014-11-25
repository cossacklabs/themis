/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** stateInfo.h
*/

# ifndef STATEINFO_H
# define STATEINFO_H

typedef /*@null@*/ struct s_stateInfo *stateInfo ;

typedef enum {
  SA_UNKNOWN,

  /* Any type of action */
  SA_CREATED,
  SA_DECLARED,
  SA_CHANGED,

  /* Definition actions */
  SA_UNDEFINED,
  SA_MUNDEFINED,
  SA_PDEFINED,
  SA_DEFINED,
  SA_RELEASED,
  SA_ALLOCATED,
  SA_KILLED,
  SA_PKILLED,
  SA_MERGED,

  /* sharing actions */
  SA_SHARED,
  SA_ONLY,
  SA_IMPONLY,
  SA_OWNED,
  SA_DEPENDENT,
  SA_IMPDEPENDENT,
  SA_KEPT,
  SA_KEEP,
  SA_FRESH,
  SA_XSTACK, /* SA_STACK is defined in some Linux headers (but ISO does not reserve this namespace) */
  SA_TEMP,
  SA_IMPTEMP,
  SA_STATIC,
  SA_LOCAL,

  SA_REFCOUNTED,
  SA_REFS,
  SA_NEWREF,
  SA_KILLREF,

  /* exposure */
  SA_EXPOSED,
  SA_OBSERVER,

  /* nullity actions */
  SA_BECOMESNULL,
  SA_BECOMESNONNULL,
  SA_BECOMESPOSSIBLYNULL,

} stateAction;

/*@null@*/ struct s_stateInfo
{
  /*@only@*/ fileloc loc;
  stateAction action;
  /*@observer@*/ sRef ref;
  /*@null@*/ stateInfo previous;
} ;

/*@constant null stateInfo stateInfo_undefined@*/
# define stateInfo_undefined (NULL)

extern /*@falsewhennull@*/ bool stateInfo_isDefined (stateInfo p_s) /*@*/ ;
# define stateInfo_isDefined(p_s) ((p_s) != stateInfo_undefined)

extern void stateInfo_free (/*@only@*/ stateInfo p_a);

extern /*@only@*/ stateInfo stateInfo_update (/*@only@*/ stateInfo p_old, stateInfo p_newinfo);

extern /*@only@*/ stateInfo stateInfo_updateLoc (/*@only@*/ stateInfo p_old, 
						 stateAction p_action,
						 fileloc p_loc) ;

extern /*@only@*/ stateInfo 
    stateInfo_updateRefLoc (/*@only@*/ stateInfo p_old, /*@exposed@*/ sRef p_ref, 
			    stateAction p_action,
			    fileloc p_loc) ;

extern /*@only@*/ stateInfo stateInfo_copy (stateInfo p_a); 

extern /*@only@*/ /*@notnull@*/ stateInfo stateInfo_currentLoc (void) ;

extern /*@only@*/ /*@notnull@*/ stateInfo
stateInfo_makeLoc (fileloc p_loc, stateAction p_action) /*@*/ ;

extern /*@only@*/ /*@notnull@*/ stateInfo 
stateInfo_makeRefLoc (/*@exposed@*/ sRef p_ref, fileloc p_loc, stateAction p_action) /*@*/ ; 

extern /*@observer@*/ fileloc stateInfo_getLoc (stateInfo p_info) ;
extern /*@only@*/ cstring stateInfo_unparse (stateInfo p_s) /*@*/ ;

extern stateAction stateAction_fromSState (sstate p_ss) /*@*/ ;
extern stateAction stateAction_fromNState (nstate p_ns) /*@*/ ;
extern stateAction stateAction_fromExkind (exkind p_ex) /*@*/ ;
extern stateAction stateAction_fromAlkind (alkind p_ak) /*@*/ ;

extern void stateInfo_display (stateInfo p_s, /*@only@*/ cstring p_sname) 
  /*@modifies g_errorstream@*/ ;

# else
# error "Multiple include"
# endif
