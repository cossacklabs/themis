/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** storeRef.h
*/

# ifndef STOREREF_H
# define STOREREF_H

/*
** note: forwardTypes defines sRef
*/

/*
** kinds of storage references 
*/

typedef enum {
  SR_NOTHING,
  SR_INTERNAL,
  SR_SPECSTATE,
  SR_SYSTEM,
  SR_GLOBALMARKER
} speckind;
    
typedef enum { 
  SK_PARAM, 
  SK_ARRAYFETCH, 
  SK_FIELD,
  SK_PTR, 
  SK_ADR, 
  SK_CONST,
  SK_CVAR, 
  SK_UNCONSTRAINED,
  SK_OBJECT, 
  SK_CONJ, 
  SK_EXTERNAL,
  SK_DERIVED,
  SK_NEW, 
  SK_TYPE, 
  SK_RESULT,
  SK_SPECIAL,
  SK_UNKNOWN 
} skind;

typedef struct
{
  int    lexlevel;
  usymId index;
} *cref;

typedef struct
{
  /*@exposed@*/ /*@notnull@*/ sRef arr;
  bool indknown;
  int  ind;
} *ainfo;

typedef struct
{
  /*@exposed@*/ /*@notnull@*/  sRef rec;
  /*@observer@*/ cstring field;
} *fldinfo ;

typedef struct
{
  /*@exposed@*/ /*@notnull@*/ sRef a;
  /*@exposed@*/ /*@notnull@*/ sRef b;
} *cjinfo ;

typedef union
{
  /*@only@*/ cref     cvar;
             int      paramno;
  /*@only@*/ ainfo    arrayfetch;
  /*@only@*/ fldinfo  field;
             ctype    object;
  /*@observer@*/ cstring fname; /* unconstrained, new */
  /*@exposed@*/ /*@notnull@*/ sRef ref;
  /*@only@*/ cjinfo   conj;
             speckind   spec;
} *sinfo;
  
struct s_sRef
{
  /* does it need to be inside parens (macros) */
  bool safe;

  /* has it been modified */
  bool modified; /*  BOOLBITS; */

  /* for debugging: make this sRef as immutable. */
  bool immut; /*  BOOLBITS; */

  skind kind;
  ctype type;

  multiVal val; /* Some sRef's have known values */

  sstate defstate;
  nstate nullstate;

  /* start modifications: We keep a track of the buf state(nullterm info)*/
  struct s_bbufinfo bufinfo;
  /* end modifications */

  alkind aliaskind;
  alkind oaliaskind;
  
  exkind expkind;     /* exposed, observer, normal */
  exkind oexpkind;
  
  /* where it becomes observer/exposed */
  /*@null@*/ /*@only@*/ stateInfo expinfo;  

  /* where it changed alias kind */
  /*@null@*/ /*@only@*/ stateInfo aliasinfo;

  /* where it is defined/allocated */
  /*@null@*/ /*@only@*/ stateInfo definfo;  

  /* where it is null */
  /*@null@*/ /*@only@*/ stateInfo nullinfo;  

  /*@only@*/ /*@relnull@*/ sinfo info;
  
  /* stores fields for structs, elements for arrays, derefs for pointers */
  /*@only@*/ sRefSet deriv; 

  /* sRef's include a lookup table of state variables. */
  /*@only@*/ valueTable state;
} ;

extern bool sRef_perhapsNull (sRef p_s);
extern bool sRef_possiblyNull (sRef p_s);
extern bool sRef_definitelyNull (sRef p_s);

extern bool sRef_definitelyNullContext (sRef p_s);
extern bool sRef_definitelyNullAltContext (sRef p_s);

extern void sRef_setNullError (sRef p_s);
extern void sRef_setNullUnknown (sRef p_s, fileloc p_loc);
extern void sRef_setNotNull (sRef p_s, fileloc p_loc);
extern void sRef_setNullState (sRef p_s, nstate p_n, fileloc p_loc);
extern void sRef_setNullStateInnerComplete (sRef p_s, nstate p_n, fileloc p_loc);
extern void sRef_setPosNull (sRef p_s, fileloc p_loc);
extern void sRef_setDefNull (sRef p_s, fileloc p_loc);

extern /*@nullwhentrue@*/ bool sRef_isInvalid (sRef p_s) /*@*/ ;
extern /*@falsewhennull@*/ bool sRef_isValid (sRef p_s) /*@*/ ;
extern /*@falsewhennull@*/ bool sRef_isReasonable (sRef p_s) /*@*/ ;

/*@constant null sRef sRef_undefined; @*/
# define sRef_undefined    ((sRef) NULL)
# define sRef_isInvalid(s) ((s) == NULL)
# define sRef_isValid(s)   ((s) != NULL)

extern bool sRef_isRecursiveField (sRef p_s) /*@*/ ;
extern void sRef_copyRealDerivedComplete (sRef p_s1, sRef p_s2) /*@modifies p_s1@*/ ;
extern nstate sRef_getNullState (/*@sef@*/ sRef p_s) /*@*/ ;
extern bool sRef_isNotNull (sRef p_s) /*@*/ ;

extern bool sRef_isDefinitelyNull (sRef p_s) /*@*/ ;
						      
extern /*@falsewhennull@*/ bool sRef_isLocalVar (sRef p_s) /*@*/ ;
extern /*@falsewhennull@*/ bool sRef_isNSLocalVar (sRef p_s) /*@*/ ;
extern /*@falsewhennull@*/ bool sRef_isRealLocalVar (sRef p_s) /*@*/ ;
extern /*@falsewhennull@*/ bool sRef_isLocalParamVar (sRef p_s) /*@*/ ;
extern /*@falsewhennull@*/ bool sRef_isKnown (/*@sef@*/ sRef p_s) /*@*/ ;

extern bool sRef_hasLastReference (sRef p_s) /*@*/ ;
# define sRef_hasLastReference(s) (sRef_hasAliasInfoRef (s))

# define sRef_isKnown(s)          (sRef_isValid(s) && (s)->kind != SK_UNKNOWN)

extern bool sRef_isMeaningful (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isMeaningful(s)     (sRef_isValid(s) && sRef_isKnown(s) \
				   && (s)->kind != SK_NEW && (s)->kind != SK_TYPE)

extern bool sRef_isSomewhatMeaningful (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isSomewhatMeaningful(s)     (sRef_isValid(s) && sRef_isKnown(s) \
				           && (s)->kind != SK_TYPE)
extern bool sRef_isNew (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isNew(s)            (sRef_isValid(s) && (s)->kind == SK_NEW)

extern bool sRef_isType (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isType(s)           (sRef_isValid(s) && (s)->kind == SK_TYPE)

extern bool sRef_isSafe (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isSafe(s)           (sRef_isValid(s) && (s)->safe)

extern bool sRef_isUnsafe (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isUnsafe(s)         (sRef_isValid(s) && !(s)->safe)

extern void sRef_clearAliasKind (/*@sef@*/ sRef p_s) /*@modifies p_s@*/ ;
# define sRef_clearAliasKind(s)   (sRef_isValid(s) ? (s)->aliaskind = AK_UNKNOWN : AK_ERROR)

extern bool sRef_stateKnown (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_stateKnown(s)       (sRef_isValid(s) && (s)->defstate != SS_UNKNOWN)

extern alkind sRef_getAliasKind (/*@sef@*/ sRef p_s) /*@*/ ;

extern alkind sRef_getOrigAliasKind (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_getOrigAliasKind(s) (sRef_isValid(s) ? (s)->oaliaskind : AK_ERROR)

extern /*@falsewhennull@*/ bool sRef_isConj (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isConj(s)           (sRef_isValid(s) && (s)->kind == SK_CONJ)

extern /*@exposed@*/ sRef sRef_buildArrow (sRef p_s, /*@dependent@*/ cstring p_f);
extern /*@exposed@*/ sRef sRef_makeArrow (sRef p_s, /*@dependent@*/ cstring p_f);

extern bool sRef_isAllocIndexRef (sRef p_s) /*@*/ ;
extern void sRef_setAliasKind (sRef p_s, alkind p_kind, fileloc p_loc) /*@modifies p_s@*/ ;
extern void sRef_setPdefined (sRef p_s, fileloc p_loc) /*@modifies p_s@*/ ;

extern /*@unused@*/ bool sRef_hasDerived (sRef p_s) /*@*/ ;
extern void sRef_clearDerived (sRef p_s);
extern void sRef_clearDerivedComplete (sRef p_s);
extern /*@exposed@*/ sRef sRef_getBaseSafe (sRef p_s);

extern /*@observer@*/ sRefSet sRef_derivedFields (/*@temp@*/ sRef p_rec) /*@*/ ;
extern bool sRef_sameName (sRef p_s1, sRef p_s2) /*@*/ ;
extern bool sRef_isDirectParam (sRef p_s) /*@*/ ;
extern /*@exposed@*/ sRef sRef_makeAnyArrayFetch (/*@exposed@*/ sRef p_arr);
extern bool sRef_isUnknownArrayFetch (sRef p_s) /*@*/ ;

extern void sRef_setPartialDefinedComplete (sRef p_s, fileloc p_loc);
extern bool sRef_isMacroParamRef (sRef p_s) /*@*/ ;

extern void sRef_destroyMod (void) /*@modifies internalState@*/ ;

extern bool sRef_deepPred (bool (p_predf) (sRef), sRef p_s);

extern bool sRef_aliasCompleteSimplePred (bool (p_predf) (sRef), sRef p_s);

extern void sRef_clearExKindComplete (sRef p_s, fileloc p_loc);

extern /*@falsewhennull@*/ bool sRef_isKindSpecial (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isKindSpecial(s) (sRef_isValid (s) && (s)->kind == SK_SPECIAL)

extern /*@observer@*/ cstring sRef_nullMessage (sRef p_s) /*@*/ ;

extern bool sRef_isSystemState (sRef p_s) /*@*/ ;
extern bool sRef_isGlobalMarker (sRef p_s) /*@*/ ;
extern bool sRef_isInternalState (sRef p_s) /*@*/ ;
extern bool sRef_isResult (sRef p_s) /*@*/ ;
extern bool sRef_isSpecInternalState (sRef p_s) /*@*/ ;
extern bool sRef_isSpecState (sRef p_s) /*@*/ ;
extern bool sRef_isNothing (sRef p_s) /*@*/ ;

extern bool sRef_isFileOrGlobalScope (sRef p_s) /*@*/ ;
extern bool sRef_isReference (sRef p_s) /*@*/ ;

extern ctype sRef_deriveType (sRef p_s, uentryList p_cl) /*@*/ ;
extern ctype sRef_getType (sRef p_s) /*@*/ ;

extern void sRef_markImmutable (sRef p_s) /*@modifies p_s@*/ ;

extern /*@falsewhennull@*/ bool sRef_isAddress (sRef p_s) /*@*/ ;
extern /*@falsewhennull@*/ bool sRef_isArrayFetch (sRef p_s) /*@*/ ;
extern /*@falsewhennull@*/ bool sRef_isConst (sRef p_s) /*@*/ ;
extern /*@falsewhennull@*/ bool sRef_isCvar (sRef p_s) /*@*/ ; 
extern /*@falsewhennull@*/ bool sRef_isField (sRef p_s) /*@*/ ;
extern /*@falsewhennull@*/ bool sRef_isParam (sRef p_s) /*@*/ ;
extern /*@falsewhennull@*/ bool sRef_isPointer (sRef p_s) /*@*/ ;

extern void sRef_setType (sRef p_s, ctype p_t);
extern void sRef_setTypeFull (sRef p_s, ctype p_t);
extern void sRef_mergeNullState (sRef p_s, nstate p_n);
extern void sRef_setLastReference (/*@temp@*/ sRef p_s, /*@exposed@*/ sRef p_ref, fileloc p_loc);
extern bool sRef_canModify (sRef p_s, sRefSet p_sl) /*@modifies p_s@*/ ;
extern bool sRef_canModifyVal (sRef p_s, sRefSet p_sl) /*@modifies p_s@*/ ;
extern bool sRef_isIReference (sRef p_s) /*@*/ ;
extern bool sRef_isIndexKnown (sRef p_arr) /*@*/ ;
extern bool sRef_isModified (sRef p_s) /*@*/ ;

extern bool sRef_isExternallyVisible (sRef p_s) /*@*/ ;
extern int sRef_compare (sRef p_s1, sRef p_s2) /*@*/ ;
extern bool sRef_realSame (sRef p_s1, sRef p_s2) /*@*/ ;
extern bool sRef_sameObject (sRef p_s1, sRef p_s2) /*@*/ ;
extern bool sRef_same (sRef p_s1, sRef p_s2) /*@*/ ;
extern bool sRef_similar (sRef p_s1, sRef p_s2) /*@*/ ;
extern /*@observer@*/ cstring sRef_getField (sRef p_s) /*@*/ ;
extern /*@only@*/ cstring sRef_unparse (sRef p_s) /*@*/ ;
extern /*@observer@*/ cstring sRef_stateVerb (sRef p_s) /*@*/ ;
extern /*@observer@*/ cstring sRef_stateAltVerb (sRef p_s) /*@*/ ;
extern /*@only@*/ cstring sRef_unparseOpt (sRef p_s) /*@*/ ;
extern /*@only@*/ cstring sRef_unparseDebug (sRef p_s) /*@*/ ;
extern void sRef_killComplete (sRef p_s, fileloc p_loc) /*@modifies p_s@*/ ;
extern int sRef_getIndex (sRef p_arr) /*@*/ ;
extern /*@dependent@*/ sRef sRef_fixOuterRef (/*@returned@*/ sRef p_s);

/* Use this one to clear after error */
extern void sRef_setDefinedCompleteDirect (sRef p_s, fileloc p_loc) ;

extern void sRef_setDefinedComplete (sRef p_s, fileloc p_loc) ;
extern void sRef_setDefinedNCComplete (sRef p_s, fileloc p_loc) ;
extern int sRef_getParam (sRef p_s) /*@*/ ;
extern int sRef_lexLevel (sRef p_s) /*@*/ ;
extern void sRef_setOrigAliasKind (sRef p_s, alkind p_kind);

extern /*@exposed@*/ sRef 
  sRef_fixBase (/*@returned@*/ sRef p_s, /*@returned@*/ sRef p_base) 
  /*@modifies p_s, p_base@*/ ;

extern void sRef_showNotReallyDefined (sRef p_s) /*@modifies g_warningstream@*/ ;

extern void sRef_enterFunctionScope (void) /*@modifies internalState@*/ ;
extern void sRef_setGlobalScope (void) /*@modifies internalState@*/ ;
extern bool sRef_inGlobalScope (void) /*@*/ ;
extern void sRef_exitFunctionScope (void) /*@modifies internalState@*/ ;
extern void sRef_clearGlobalScopeSafe (void) /*@modifies internalState@*/ ;
extern void sRef_setGlobalScopeSafe (void) /*@modifies internalState@*/ ;

extern /*@notnull@*/ /*@exposed@*/ sRef 
  sRef_buildArrayFetch (/*@exposed@*/ sRef p_arr);
extern /*@notnull@*/ /*@exposed@*/ sRef sRef_buildArrayFetchKnown (/*@exposed@*/ sRef p_arr, int p_i);
extern /*@exposed@*/ sRef
  sRef_buildField (/*@exposed@*/ sRef p_rec, /*@dependent@*/ cstring p_f) 
  /*@modifies p_rec@*/ ;
extern /*@exposed@*/ sRef sRef_buildPointer (/*@exposed@*/ sRef p_t) 
  /*@modifies p_t@*/ ;

extern /*@exposed@*/ sRef sRef_makeAddress (/*@exposed@*/ sRef p_t);

/* evans 2002-07-12: the parameter was exposed (but this led to invalid reads, reported by valgrind */
extern /*@notnull@*/ /*@dependent@*/ sRef sRef_makeUnconstrained (/*@temp@*/ cstring) /*@*/ ;

extern /*@falsewhennull@*/ bool sRef_isUnconstrained (sRef p_s) /*@*/ ;

extern /*@observer@*/ cstring sRef_unconstrainedName (sRef p_s) /*@*/ ;

extern /*@notnull@*/ /*@exposed@*/ sRef sRef_makeArrayFetch (/*@exposed@*/ sRef p_arr) /*@*/ ;
extern /*@notnull@*/ /*@exposed@*/ sRef
  sRef_makeArrayFetchKnown (/*@exposed@*/ sRef p_arr, int p_i);
extern /*@notnull@*/ /*@dependent@*/ sRef
  sRef_makeConj (/*@exposed@*/ /*@returned@*/ sRef p_a, /*@exposed@*/ sRef p_b);
extern /*@notnull@*/ /*@dependent@*/ sRef
  sRef_makeCvar (int p_level, usymId p_index, ctype p_ct, /*@only@*/ stateInfo p_stinfo);
extern /*@notnull@*/ /*@dependent@*/ sRef
  sRef_makeConst (ctype p_ct);
extern /*@exposed@*/ sRef
  sRef_makeField (sRef p_rec, /*@dependent@*/ cstring p_f);
extern /*@notnull@*/ /*@dependent@*/ sRef 
  sRef_makeGlobal (usymId p_l, ctype p_ct, /*@only@*/ stateInfo);
extern /*@exposed@*/ sRef
  sRef_makeNCField (/*@exposed@*/ sRef p_rec, /*@dependent@*/ cstring p_f) /*@*/ ;
extern void sRef_maybeKill (sRef p_s, fileloc p_loc) /*@modifies p_s@*/ ;
extern /*@unused@*/ /*@notnull@*/ /*@dependent@*/ sRef 
  sRef_makeObject (ctype p_o) /*@*/ ;
extern /*@notnull@*/ /*@dependent@*/ sRef sRef_makeType (ctype p_ct) /*@*/ ;
extern /*@notnull@*/ /*@dependent@*/ sRef sRef_makeParam (int p_l, ctype p_ct, /*@only@*/ stateInfo p_stinfo) /*@*/ ;
extern /*@exposed@*/ sRef sRef_makePointer (/*@exposed@*/ sRef p_s) /*@modifies p_s@*/ ;
extern void sRef_makeSafe (sRef p_s) /*@modifies p_s@*/ ;
extern void sRef_makeUnsafe (sRef p_s) /*@modifies p_s@*/ ;

extern /*@dependent@*/ sRef sRef_makeUnknown (void) /*@*/ ;
extern /*@dependent@*/ sRef sRef_makeNothing (void) /*@*/ ;
extern /*@dependent@*/ sRef sRef_makeInternalState (void) /*@*/ ;
extern /*@dependent@*/ sRef sRef_makeSpecState (void) /*@*/ ;
extern /*@dependent@*/ sRef sRef_makeGlobalMarker (void) /*@*/ ;
extern /*@dependent@*/ sRef sRef_makeSystemState (void) /*@*/ ;

extern /*@dependent@*/ /*@notnull@*/ sRef sRef_makeResult (ctype) /*@*/ ;
extern /*@exposed@*/ sRef 
  sRef_fixResultType (/*@returned@*/ sRef p_s, ctype p_typ, uentry p_ue)
  /*@modifies p_s@*/;

extern void sRef_setParamNo (sRef p_s, int p_l) /*@modifies p_s;@*/;

extern /*@notnull@*/ /*@dependent@*/ sRef 
  sRef_makeNew (ctype p_ct, sRef p_t, /*@exposed@*/ cstring p_name) ;

extern usymId sRef_getScopeIndex (sRef p_s) /*@*/ ;
extern /*@exposed@*/ uentry sRef_getBaseUentry (sRef p_s);

extern /*@exposed@*/ sRef 
  sRef_fixBaseParam (/*@returned@*/ sRef p_s, exprNodeList p_args)
  /*@modifies p_s@*/ ;

  /*
    drl7x
    added function
    12/24/2000
  */
  
/*@only@*/ constraintExpr sRef_fixConstraintParam (/*@observer@*/  sRef p_s, /*@observer@*/ /*@temp@*/ exprNodeList p_args);
  
extern bool sRef_isUnionField (sRef p_s);
extern void sRef_setModified (sRef p_s);

extern void sRef_resetState (sRef p_s);
extern void sRef_resetStateComplete (sRef p_s);

extern void sRef_storeState (sRef p_s);
extern /*@exposed@*/ sRef sRef_getBase (sRef p_s) /*@*/ ;
extern /*@exposed@*/ sRef sRef_getRootBase (sRef p_s) /*@*/ ;

extern /*@observer@*/ uentry sRef_getUentry (sRef p_s);

extern cstring sRef_dump (sRef p_s) /*@*/ ;
extern cstring sRef_dumpGlobal (sRef p_s) /*@*/ ;
extern /*@exposed@*/ sRef sRef_undump (char **p_c) /*@modifies *p_c@*/ ;
extern /*@exposed@*/ sRef sRef_undumpGlobal (char **p_c) /*@modifies *p_c@*/ ;

extern /*@only@*/ sRef sRef_saveCopy (sRef p_s);
extern /*@dependent@*/ sRef sRef_copy (sRef p_s);

extern cstring sRef_unparseState (sRef p_s) /*@*/ ;
extern ynm sRef_isWriteable (sRef p_s) /*@*/ ;
extern ynm sRef_isValidLvalue (sRef p_s) /*@*/ ;
extern bool sRef_isStrictReadable (sRef p_s) /*@*/ ;
extern bool sRef_hasNoStorage (sRef p_s) /*@*/ ;
extern void sRef_showExpInfo (sRef p_s) /*@modifies g_warningstream*/ ;
extern void sRef_setDefined (sRef p_s, fileloc p_loc) /*@modifies p_s@*/ ;
extern void sRef_setUndefined (sRef p_s, fileloc p_loc) /*@modifies p_s@*/ ;
extern void sRef_setOnly (sRef p_s, fileloc p_loc) /*@modifies p_s@*/ ;
extern void sRef_setDependent (sRef p_s, fileloc p_loc) /*@modifies p_s@*/ ;
extern void sRef_setOwned (sRef p_s, fileloc p_loc) /*@modifies p_s@*/ ;
extern void sRef_setKept (sRef p_s, fileloc p_loc) /*@modifies p_s@*/ ;
extern void sRef_setKeptComplete (sRef p_s, fileloc p_loc) /*@modifies p_s@*/ ;
extern void sRef_setFresh (sRef p_s, fileloc p_loc) /*@modifies p_s@*/ ;
extern void sRef_setShared (sRef p_s, fileloc p_loc) /*@modifies p_s@*/ ;
extern void sRef_showAliasInfo (sRef p_s) /*@modifies g_warningstream@*/ ;
extern void sRef_showMetaStateInfo (sRef p_s, cstring p_key) /*@modifies g_warningstream@*/ ; 
extern void sRef_showNullInfo (sRef p_s) /*@modifies g_warningstream@*/ ;
extern void sRef_showStateInfo (sRef p_s) /*@modifies g_warningstream@*/ ;
extern void sRef_setStateFromType (sRef p_s, ctype p_ct) /*@modifies p_s@*/ ;
extern void sRef_kill (sRef p_s, fileloc p_loc) /*@modifies p_s@*/ ;
extern void sRef_setAllocated (sRef p_s, fileloc p_loc) /*@modifies p_s@*/ ;
extern void sRef_setAllocatedShallowComplete (sRef p_s, fileloc p_loc) /*@modifies p_s@*/ ;
extern void sRef_setAllocatedComplete (sRef p_s, fileloc p_loc) /*@modifies p_s@*/ ;
extern /*@only@*/ cstring sRef_unparseKindNamePlain (sRef p_s) /*@*/ ;
extern /*@falsewhennull@*/ bool sRef_isRealGlobal(sRef p_s) /*@*/ ;
extern /*@falsewhennull@*/ bool sRef_isFileStatic (sRef p_s) /*@*/ ;

extern int sRef_getScope (sRef p_s) /*@*/ ;
extern /*@observer@*/ cstring sRef_getScopeName (sRef p_s) /*@*/ ;

/* must be real function (passed as function param) */
extern /*@falsewhennull@*/ bool sRef_isDead (sRef p_s) /*@*/ ;
extern /*@falsewhennull@*/ bool sRef_isDeadStorage (sRef p_s) /*@*/ ;
extern bool sRef_isStateLive (sRef p_s) /*@*/ ;
extern /*@falsewhennull@*/ bool sRef_isPossiblyDead (sRef p_s) /*@*/ ;
extern /*@nullwhentrue@*/ bool sRef_isStateUndefined (sRef p_s) /*@*/ ;
extern bool sRef_isUnuseable (sRef p_s) /*@*/ ;

extern /*@exposed@*/ sRef sRef_constructDeref (sRef p_t) 
   /*@modifies p_t@*/ ;

extern /*@exposed@*/ sRef sRef_constructDeadDeref (sRef p_t) 
   /*@modifies p_t@*/ ;

extern bool sRef_isJustAllocated (sRef p_s) /*@*/ ;

extern /*@falsewhennull@*/ bool sRef_isAllocated (sRef p_s) /*@*/ ;

extern bool sRef_isUndefGlob (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isUndefGlob(s) \
    ((sRef_isValid(s)) \
     && ((s)->defstate == SS_UNDEFGLOB || (s)->defstate == SS_UNDEFKILLED))

extern bool sRef_isKilledGlob (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isKilledGlob(s) \
    ((sRef_isValid(s)) \
     && ((s)->defstate == SS_KILLED || (s)->defstate == SS_UNDEFKILLED))

extern bool sRef_isRelDef (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isRelDef(s) \
  ((sRef_isValid(s)) && ((s)->defstate == SS_RELDEF))

extern bool sRef_isPartial (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isPartial(s) \
  ((sRef_isValid(s)) && ((s)->defstate == SS_PARTIAL))

extern bool sRef_isStateSpecial (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isStateSpecial(s) \
  ((sRef_isValid(s)) && ((s)->defstate == SS_SPECIAL))

extern bool sRef_makeStateSpecial (sRef p_s) 
     /*@modifies p_s@*/ 
     /* returns false is sRef already has an inconsistend defstate */ ;

extern bool sRef_isStateDefined (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isStateDefined(s) \
  ((sRef_isValid(s)) && (((s)->defstate == SS_DEFINED) \
			 || (s)->defstate == SS_RELDEF))

extern bool sRef_isAnyDefined (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isAnyDefined(s)   ((sRef_isValid(s)) && \
				 (((s)->defstate == SS_DEFINED) \
				  || ((s)->defstate == SS_RELDEF) \
				  || ((s)->defstate == SS_PARTIAL)))

extern /*@falsewhennull@*/ bool sRef_isPdefined (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isPdefined(s) \
  ((sRef_isValid(s)) && ((s)->defstate == SS_PDEFINED))

extern bool sRef_isReallyDefined (sRef p_s) /*@*/ ;

extern bool sRef_isStateUnknown (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isStateUnknown(s) \
  ((sRef_isValid(s)) && ((s)->defstate == SS_UNKNOWN))

extern /*@falsewhennull@*/ bool sRef_isRefCounted (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isRefCounted(s) \
  ((sRef_isValid(s)) && ((s)->aliaskind == AK_REFCOUNTED))

extern /*@falsewhennull@*/ bool sRef_isNewRef (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isNewRef(s) \
  ((sRef_isValid(s)) && ((s)->aliaskind == AK_NEWREF))

extern /*@falsewhennull@*/ bool sRef_isKillRef (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isKillRef(s) \
  ((sRef_isValid(s)) && ((s)->aliaskind == AK_KILLREF))

extern bool sRef_isOnly (sRef p_s) /*@*/ ;
extern bool sRef_isDependent (sRef p_s) /*@*/ ;
extern bool sRef_isOwned (/*@sef@*/ sRef p_s) /*@*/ ;
extern bool sRef_isKeep (/*@sef@*/ sRef p_s) /*@*/ ;

extern bool sRef_isKept (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isKept(s) \
  ((sRef_isValid(s)) && ((s)->aliaskind == AK_KEPT))

extern /*@unused@*/ bool sRef_isTemp (sRef p_s) /*@*/ ;

extern bool sRef_isStack (sRef p_s) /*@*/ ;
extern bool sRef_isLocalState (sRef p_s) /*@*/ ;
extern bool sRef_isUnique (sRef p_s) /*@*/ ;
extern bool sRef_isShared (sRef p_s) /*@*/ ;
extern bool sRef_isExposed (sRef p_s) /*@*/ ;
extern bool sRef_isObserver (sRef p_s) /*@*/ ;
extern bool sRef_isFresh (sRef p_s) /*@*/ ;

extern bool sRef_isRefsField (/*@sef@*/ sRef p_s) /*@*/ ;
# define sRef_isRefsField(s) \
  ((sRef_isValid(s)) && ((s)->aliaskind == AK_REFS))

extern void sRef_protectDerivs (void) /*@modifies internalState@*/ ;
extern void sRef_clearProtectDerivs (void) /*@modifies internalState@*/ ;

extern exkind sRef_getExKind (sRef p_s) /*@*/ ;
extern exkind sRef_getOrigExKind (sRef p_s) /*@*/ ;
extern void sRef_setExKind (sRef p_s, exkind p_exp, fileloc p_loc) /*@modifies p_s@*/ ;
extern void sRef_setExposed (sRef p_s, fileloc p_loc) /*@modifies p_s@*/;

extern bool sRef_isAnyParam (sRef p_s) /*@*/ ;
extern /*@observer@*/ sRef sRef_getAliasInfoRef (/*@temp@*/ sRef p_s) /*@*/ ;
extern bool sRef_hasAliasInfoRef (sRef p_s) /*@*/ ;

extern /*@exposed@*/ sRef sRef_constructPointer (/*@exposed@*/ sRef p_t) /*@modifies p_t*/ ;
extern bool sRef_isAliasCheckedGlobal (sRef p_s) /*@*/ ;
extern bool sRef_includedBy (sRef p_small, sRef p_big) /*@*/ ;
extern /*@dependent@*/ /*@exposed@*/ sRef sRef_makeExternal (/*@exposed@*/ sRef p_t) /*@*/ ;
extern bool sRef_similarRelaxed (sRef p_s1, sRef p_s2) /*@*/ ;
extern /*@only@*/ cstring sRef_unparseKindName (sRef p_s) /*@*/ ;
extern void sRef_copyState (sRef p_s1, sRef p_s2) /*@modifies p_s1@*/ ;
extern /*@unused@*/ bool sRef_isObject (sRef p_s) /*@*/ ;
extern bool sRef_isNotUndefined (sRef p_s) /*@*/ ;
extern bool sRef_isExternal (sRef p_s) /*@*/ ;
extern cstring sRef_unparseDeep (sRef p_s) /*@*/ ;
extern cstring sRef_unparseFull (sRef p_s) /*@*/ ;
extern /*@observer@*/ cstring sRef_unparseScope (sRef p_s) /*@*/ ;
extern void sRef_mergeState (sRef p_res, sRef p_other, clause p_cl, fileloc p_loc)
   /*@modifies p_res, p_other@*/ ;
extern void sRef_mergeOptState (sRef p_res, sRef p_other, clause p_cl, fileloc p_loc)
   /*@modifies p_res, p_other@*/ ;
extern void sRef_mergeStateQuiet (sRef p_res, sRef p_other)
   /*@modifies p_res@*/ ;
extern void sRef_mergeStateQuietReverse (/*@dependent@*/ sRef p_res, /*@dependent@*/ sRef p_other)
   /*@modifies p_res@*/ ;
extern void sRef_setStateFromUentry (sRef p_s, uentry p_ue)
   /*@modifies p_s@*/ ;
extern bool sRef_isStackAllocated (sRef p_s) /*@*/ ;
extern bool sRef_modInFunction (void) /*@*/ ;
extern void sRef_clearAliasState (sRef p_s, fileloc p_loc)
   /*@modifies p_s@*/ ;
extern void sRef_setPartial (sRef p_s, fileloc p_loc)
   /*@modifies p_s@*/ ;
extern void sRef_setDerivNullState (sRef p_set, sRef p_guide, nstate p_ns)
   /*@modifies p_set@*/ ;

extern void sRef_clearGlobalScope (void) /*@modifies internalState@*/ ;

extern /*@dependent@*/ sRef sRef_makeDerived (/*@exposed@*/ sRef p_t);

extern sstate sRef_getDefState (sRef p_s) /*@*/ ;
extern void sRef_setDefState (sRef p_s, sstate p_defstate, fileloc p_loc);
extern void sRef_showRefLost (sRef p_s);
extern void sRef_showRefKilled (sRef p_s);
extern /*@exposed@*/ sRef sRef_updateSref (sRef p_s);

extern void sRef_reflectAnnotation (sRef p_s, annotationInfo p_a, fileloc p_loc);
extern /*@observer@*/ valueTable sRef_getValueTable (sRef p_s) /*@*/ ;

extern void sRef_aliasCheckPred (bool (p_predf) (sRef, exprNode, sRef, exprNode),
				 /*@null@*/ bool (p_checkAliases) (sRef),
				 sRef p_s, exprNode p_e, exprNode p_err);
extern bool sRef_aliasCheckSimplePred (sRefTest p_predf, sRef p_s);

extern void sRef_showStateInconsistent (sRef p_s);

extern void sRef_setDependentComplete (sRef p_s, fileloc p_loc);

extern void sRef_setAliasKindComplete (sRef p_s, alkind p_kind, fileloc p_loc);

extern bool sRef_isThroughArrayFetch (sRef p_s) /*@*/ ;

extern /*@exposed@*/ /*@notnull@*/ sRef sRef_getConjA (sRef p_s) /*@*/ ; 
extern /*@exposed@*/ /*@notnull@*/ sRef sRef_getConjB (sRef p_s) /*@*/ ;  

extern /*@only@*/ cstring sRef_unparsePreOpt (sRef p_s) /*@*/ ;

extern bool sRef_hasName (sRef p_s) /*@*/ ;
extern void sRef_free (/*@only@*/ sRef p_s);
extern void sRef_setObserver (sRef p_s, fileloc p_loc) /*@modifies p_s@*/ ;

/* start modifications */
/* functions for making modification to null-term info */
extern void sRef_setNullTerminatedStateInnerComplete(sRef p_s, struct s_bbufinfo p_b, fileloc p_loc);
extern struct s_bbufinfo sRef_getNullTerminatedState(sRef p_s);
extern void sRef_setNullTerminatedState (sRef p_s);
extern void sRef_setPossiblyNullTerminatedState (sRef p_s);
extern void sRef_setNotNullTerminatedState (sRef p_s);
extern void sRef_setSize(sRef p_s, int p_size);
extern void sRef_setLen(sRef p_s, int p_len);

extern int sRef_getSize(sRef p_s);

/*@-nullderef@*/

#define sRef_getSize(p_s) \
   ((p_s)->bufinfo.size)

extern int sRef_getLen(sRef p_s);
#define sRef_getLen(p_s) \
   ((p_s)->bufinfo.len)

extern /*@falsewhennull@*/ bool sRef_hasExpInfoLoc (sRef) /*@*/ ;
extern /*@falsewhennull@*/ bool sRef_hasStateInfoLoc (sRef) /*@*/ ;
extern /*@falsewhennull@*/ bool sRef_hasAliasInfoLoc (sRef) /*@*/ ;

extern bool sRef_hasBufStateInfo(sRef p_s);
# define sRef_hasBufStateInfo(p_s) \
    (sRef_isValid(p_s)) 
 
extern bool sRef_isNullTerminated(/*@sef@*/sRef p_s);
# define sRef_isNullTerminated(p_s) \
   ( sRef_hasBufStateInfo((p_s)) ? ((p_s)->bufinfo.bufstate \
               == BB_NULLTERMINATED) : FALSE)

extern bool sRef_isPossiblyNullTerminated(/*@sef@*/sRef p_s);
# define sRef_isPossiblyNullTerminated(p_s) \
   ( sRef_hasBufStateInfo((p_s)) ? ((p_s)->bufinfo.bufstate \
               == BB_POSSIBLYNULLTERMINATED) : FALSE)

extern bool sRef_isNotNullTerminated(/*@sef@*/sRef p_s);
# define sRef_isNotNullTerminated(p_s) \
   ( sRef_hasBufStateInfo((p_s)) ? ((p_s)->bufinfo.bufstate \
               == BB_NOTNULLTERMINATED) : FALSE)

/*@=nullderef@*/


     /*drl7x 11/28/00*/
extern bool sRef_isFixedArray (sRef p_s) /*@*/;
extern size_t sRef_getArraySize (sRef p_s) /*@*/;

extern /*@observer@*/ cstring sRef_ntMessage (sRef p_s);     
extern void sRef_resetLen (sRef p_s) /*@modifies p_s@*/ ;

/* end modifications */

extern void sRef_setMetaStateValueComplete (sRef p_s, cstring p_key, int p_value, fileloc p_loc)
   /*@modifies p_s@*/ ; 

extern void sRef_setMetaStateValue (sRef p_s, cstring p_key, int p_value, fileloc p_loc) 
   /*@modifies p_s@*/ ;

extern /*@observer@*/ stateValue sRef_getMetaStateValue (sRef p_s, cstring p_key) /*@*/ ;

extern bool sRef_checkMetaStateValue (sRef p_s, cstring p_key, int p_value) 
   /*@modifies p_s@*/ ;

extern void sRef_setValue (sRef p_s, /*@only@*/ multiVal p_val) /*@modifies p_s@*/ ; 
extern bool sRef_hasValue (sRef p_s) /*@*/ ;
extern /*@observer@*/ multiVal sRef_getValue (sRef p_s) /*@*/ ;

extern /*@maynotreturn@*/ void sRef_checkValid (/*@temp@*/ sRef p_s) /*@modifies stderr@*/ ;

extern void
sRef_aliasSetComplete (void (p_predf) (sRef, fileloc), sRef p_s, fileloc p_loc)
     /*@modifies p_s@*/ ;

extern void
sRef_aliasSetCompleteParam (void (p_predf) (sRef, int, fileloc), sRef p_s, 
			    int p_kind, fileloc p_loc)
   /*@modifies p_s@*/ ;

extern void
sRef_aliasSetCompleteAlkParam (void (p_predf) (sRef, alkind, fileloc), sRef p_s, 
			       alkind p_kind, fileloc p_loc)
     /*@modifies p_s@*/ ;

# ifdef DEBUGSPLINT
extern void sRef_checkCompletelyReasonable (sRef p_s) /*@modifies g_errorstream@*/ ;
# endif

# else
# error "Multiple include"
# endif













