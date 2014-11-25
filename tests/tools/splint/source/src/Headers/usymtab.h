/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** usymtab.h
*/

# ifndef USYMTAB_H
# define USYMTAB_H

/*@constant null usymtab GLOBAL_ENV; @*/ 
# define GLOBAL_ENV usymtab_undefined

typedef enum { 
  US_GLOBAL, 
  US_NORMAL, 
  US_TBRANCH, US_FBRANCH, 
  US_CBRANCH, US_SWITCH
} uskind;

typedef struct { int level; int index; } *refentry;
typedef /*@only@*/ refentry o_refentry;
typedef o_refentry *refTable;

struct s_usymtab
{
  uskind   kind;
  int      nentries;
  int      nspace;
  int      lexlevel;
  bool     mustBreak;
  exitkind exitCode;
  /*@reldef@*/ /*@only@*/ o_uentry  *entries;
  /*@null@*/ /*@only@*/ cstringTable htable;   /* for the global environment */
  /*@null@*/ /*@only@*/ refTable  reftable; /* for branched environments */
             /*@only@*/ guardSet  guards;   /* guarded references (not null) */
  aliasTable aliases;
  /*@owned@*/ usymtab env;
} ; 

/*
** rep invariant:
**
** (left as exercise to reader)  ;) 
*/

extern void usymtab_printTypes (void) 
  /*@globals internalState@*/
  /*@modifies g_warningstream@*/ ;

extern void usymtab_setMustBreak (void) /*@modifies internalState@*/ ;

extern bool usymtab_inGlobalScope (void) /*@globals internalState@*/ ;
extern bool usymtab_inFunctionScope (void) /*@globals internalState@*/ ;
extern bool usymtab_inFileScope (void) /*@globals internalState@*/ ;
extern void usymtab_checkFinalScope (bool p_isReturn) 
  /*@globals internalState@*/ 
  /*@modifies *g_warningstream@*/ ;

extern void usymtab_allUsed (void)
   /*@globals internalState@*/ 
   /*@modifies *g_warningstream@*/ ;

extern void usymtab_allDefined (void)
   /*@globals internalState@*/
   /*@modifies *g_warningstream@*/ ;

extern void usymtab_prepareDump (void)
   /*@modifies internalState@*/ ;

extern void usymtab_dump (FILE *p_fout)
   /*@globals internalState@*/
   /*@modifies *p_fout@*/ ;


extern void usymtab_load (FILE *p_f) /*@modifies p_f, internalState@*/ ;

extern /*@exposed@*/ /*@dependent@*/ uentry 
  usymtab_getRefQuiet (int p_level, usymId p_index)
  /*@globals internalState@*/ ;

extern void usymtab_printLocal (void) 
  /*@globals internalState@*/ 
  /*@modifies stdout@*/ ;

extern /*@exposed@*/ /*@dependent@*/ uentry usymtab_getParam (int p_paramno)
   /*@globals internalState@*/;
extern void usymtab_free (void) /*@modifies internalState@*/ ;
extern bool usymtab_inDeepScope (void) /*@globals internalState@*/ ;

extern /*@exposed@*/  uentry usymtab_lookupExpose (cstring p_k)
   /*@globals internalState@*/ ;

extern /*@observer@*/ uentry usymtab_lookup (cstring p_k)
   /*@globals internalState@*/ ;

# define usymtab_lookup(s) (usymtab_lookupExpose (s))

extern /*@observer@*/ uentry usymtab_lookupGlob (cstring p_k)
   /*@globals internalState@*/ ;
extern /*@exposed@*/ uentry usymtab_lookupExposeGlob (cstring p_k)
   /*@globals internalState@*/ ;
extern /*@observer@*/ uentry usymtab_lookupUnionTag (cstring p_k)
   /*@globals internalState@*/ ;
extern /*@observer@*/ uentry usymtab_lookupStructTag (cstring p_k)
   /*@globals internalState@*/ ;
extern /*@observer@*/ uentry usymtab_lookupEither (cstring p_k)
   /*@globals internalState@*/ ;

extern ctype usymtab_lookupType (cstring p_k)
   /*@globals internalState@*/ ;

extern bool usymtab_isDefinitelyNull (sRef p_s)
   /*@globals internalState@*/ ;
extern bool usymtab_isDefinitelyNullDeep (sRef p_s)
   /*@globals internalState@*/ ;

extern usymId usymtab_supExposedTypeEntry (/*@only@*/ uentry p_e, bool p_dodef)
   /*@modifies internalState, p_e@*/ ;

extern ctype usymtab_supTypeEntry (/*@only@*/ uentry p_e)
   /*@modifies internalState, p_e@*/ ;

extern /*@exposed@*/ uentry usymtab_supReturnTypeEntry (/*@only@*/ uentry p_e)
   /*@modifies internalState@*/ ;

extern /*@observer@*/ uentry usymtab_lookupSafe (cstring p_k)
   /*@globals internalState@*/ ;

extern /*@observer@*/ uentry
  usymtab_lookupSafeScope (cstring p_k, int p_lexlevel)
     /*@globals internalState@*/ ;

extern /*@observer@*/ uentry usymtab_getGlobalEntry (usymId p_uid)
  /*@globals internalState@*/ ;

extern bool usymtab_exists (cstring p_k)
   /*@globals internalState@*/ ;

extern bool usymtab_existsVar (cstring p_k)
   /*@globals internalState@*/ ;

extern bool usymtab_existsGlob (cstring p_k)
   /*@globals internalState@*/ ;

extern bool usymtab_existsType (cstring p_k)
   /*@globals internalState@*/ ;

extern bool usymtab_existsEither (cstring p_k)
   /*@globals internalState@*/ ;

extern bool usymtab_existsTypeEither (cstring p_k)
   /*@globals internalState@*/ ;

extern usymId usymtab_getId (cstring p_k) /*@globals internalState@*/ ;
extern typeId usymtab_getTypeId (cstring p_k) /*@globals internalState@*/ ;

extern void usymtab_supEntry (/*@only@*/ uentry p_e)
  /*@modifies internalState, p_e@*/ ;

extern void usymtab_replaceEntry (/*@only@*/ uentry p_s)
  /*@modifies internalState, p_s@*/ ;

extern void usymtab_supEntrySref (/*@only@*/ uentry p_e)
  /*@modifies internalState, p_e@*/ ;

extern void usymtab_supGlobalEntry (/*@only@*/ uentry p_e)
  /*@modifies internalState@*/ ;

extern void usymtab_addGlobalEntry (/*@only@*/ uentry p_e) 
  /*@modifies internalState, p_e@*/ ;

extern /*@exposed@*/ uentry 
  usymtab_supEntryReturn (/*@only@*/ uentry p_e)
  /*@modifies internalState, p_e@*/ ;

extern usymId usymtab_addEntry (/*@only@*/ uentry p_e)
  /*@modifies internalState, p_e@*/ ;

extern ctype usymtab_lookupAbstractType (cstring p_k) 
     /*@globals internalState@*/ /*@modifies nothing@*/ ;

extern bool usymtab_matchForwardStruct (typeId p_u1, typeId p_u2)
     /*@globals internalState@*/ ;

extern bool usymtab_existsEnumTag (cstring p_k)
  /*@globals internalState@*/ ;
extern bool usymtab_existsUnionTag (cstring p_k) 
  /*@globals internalState@*/ ;
extern bool usymtab_existsStructTag (cstring p_k) 
  /*@globals internalState@*/ ;

/*@iter usymtab_entries (sef usymtab u, yield exposed uentry el); @*/
# define usymtab_entries(x, m_i)   \
    { int m_ind; \
      if (usymtab_isDefined (x)) \
        for (m_ind = 0; m_ind < (x)->nentries; m_ind++) \
	   { uentry m_i = (x)->entries[m_ind]; 

# define end_usymtab_entries }}

extern /*@unused@*/ void usymtab_displayAllUses (void)
   /*@globals internalState@*/ 
   /*@modifies *g_warningstream@*/ ;

extern /*@unused@*/ void usymtab_printOut (void)
   /*@globals internalState@*/ 
   /*@modifies *g_warningstream@*/ ;

extern /*@unused@*/ void usymtab_printAll (void)
   /*@globals internalState@*/ 
   /*@modifies *g_warningstream@*/ ;

extern void usymtab_enterScope (void) 
  /*@modifies internalState;@*/ ;
extern void usymtab_enterFunctionScope (uentry p_fcn) 
  /*@modifies internalState;@*/ ;
extern void usymtab_quietExitScope (fileloc p_loc)
  /*@modifies internalState;@*/ ;
extern void usymtab_exitScope (exprNode p_expr) /*@modifies internalState@*/ ;
extern void usymtab_addGuards (guardSet p_guards) /*@modifies internalState@*/ ;
extern void usymtab_setExitCode (exitkind p_ex) /*@modifies internalState@*/ ;
extern void usymtab_exitFile (void) /*@modifies internalState@*/ ;
extern void usymtab_enterFile (void) /*@modifies internalState@*/ ;

extern /*@observer@*/ uentry usymtab_lookupEnumTag (cstring p_k)
  /*@globals internalState@*/ ; 

extern usymId usymtab_convertId (usymId p_uid) /*@globals internalState@*/ ;
extern typeId usymtab_convertTypeId (typeId p_uid) /*@globals internalState@*/ ;
extern void usymtab_initMod (void) /*@modifies internalState@*/ ;
extern void usymtab_destroyMod (void) /*@modifies internalState@*/ ;
extern void usymtab_initBool (void) /*@modifies internalState@*/ ;
extern void usymtab_initGlobalMarker (void) /*@modifies internalState@*/ ;

extern void usymtab_exportHeader (void)
   /*@modifies internalState@*/ ;

extern ctype usymtab_structFieldsType (uentryList p_f)
   /*@globals internalState@*/ ;

extern ctype usymtab_unionFieldsType (uentryList p_f)
   /*@globals internalState@*/ ;

extern ctype usymtab_enumEnumNameListType (enumNameList p_f)
   /*@globals internalState@*/ ;

extern /*@exposed@*/ uentry usymtab_getTypeEntrySafe (typeId p_uid)
   /*@globals internalState@*/ ;

extern void usymtab_popOrBranch (exprNode p_pred, exprNode p_expr)
  /*@modifies internalState@*/ ;
extern void usymtab_popAndBranch (exprNode p_pred, exprNode p_expr)
  /*@modifies internalState@*/ ;

extern void usymtab_trueBranch (/*@only@*/ guardSet p_guards)
  /*@modifies internalState@*/ ;
extern void usymtab_altBranch (/*@only@*/ guardSet p_guards)
  /*@modifies internalState@*/ ;

extern void usymtab_popTrueBranch (exprNode p_pred, exprNode p_expr, clause p_cl)
  /*@modifies internalState@*/ ;

extern void
  usymtab_popTrueExecBranch (exprNode p_pred, exprNode p_expr, clause p_cl)
  /*@modifies internalState@*/ ;

extern void 
  usymtab_popBranches (exprNode p_pred, exprNode p_tbranch, exprNode p_fbranch, 
		       bool p_isOpt, clause p_cl)
   /*@modifies internalState@*/ ;

extern void usymtab_unguard (sRef p_s) /*@modifies internalState@*/ ;
extern bool usymtab_isGuarded (sRef p_s) /*@globals internalState@*/ ;
extern void usymtab_printGuards (void) /*@globals internalState@*/ /*@modifies *g_warningstream@*/ ;
extern void usymtab_quietPlainExitScope (void) /*@modifies internalState@*/ ;
extern void usymtab_printComplete (void) /*@globals internalState@*/ /*@modifies *stdout@*/ ;

extern bool usymtab_existsGlobEither (cstring p_k) /*@globals internalState@*/ ;

extern bool usymtab_isBoolType (typeId p_uid) /*@globals internalState@*/ ;
extern /*@only@*/ cstring usymtab_getTypeEntryName (typeId p_uid)
  /*@globals internalState@*/ ;
extern /*@exposed@*/ uentry usymtab_getTypeEntry (typeId p_uid)
  /*@globals internalState@*/ ;

extern typeId
  usymtab_supAbstractTypeEntry (/*@only@*/ uentry p_e, bool p_dodef)
  /*@modifies internalState, p_e@*/ ;
extern ctype usymtab_supForwardTypeEntry (/*@only@*/ uentry p_e)
  /*@modifies internalState, p_e@*/ ;

extern /*@exposed@*/ uentry 
  usymtab_supGlobalEntryReturn (/*@only@*/ uentry p_e)
  /*@modifies internalState, p_e@*/ ;

extern /*@exposed@*/ uentry 
  usymtab_supEntrySrefReturn (/*@only@*/ uentry p_e)
  /*@modifies internalState, p_e@*/ ;

extern usymId usymtab_directParamNo (uentry p_ue)
  /*@globals internalState@*/ ;

extern bool usymtab_newCase (exprNode p_pred, exprNode p_last)
  /*@modifies internalState@*/ ;

extern void usymtab_switchBranch (exprNode p_s) 
  /*@modifies internalState@*/ ;

extern /*@only@*/ cstring usymtab_unparseStack (void)
  /*@globals internalState@*/ ;
extern void usymtab_exitSwitch (exprNode p_sw, bool p_allpaths)
  /*@modifies internalState@*/ ;

extern /*@observer@*/ uentry usymtab_lookupGlobSafe (cstring p_k)
  /*@globals internalState@*/ ;

extern /*@only@*/ sRefSet usymtab_aliasedBy (sRef p_s)
  /*@globals internalState@*/ ;

extern /*@only@*/ sRefSet usymtab_canAlias (sRef p_s)
  /*@globals internalState@*/ ;

extern void usymtab_clearAlias (sRef p_s)
  /*@modifies internalState, p_s@*/ ;

extern void usymtab_addMustAlias (/*@exposed@*/ sRef p_s, /*@exposed@*/ sRef p_al)
  /*@modifies internalState@*/ ;

extern void usymtab_addForceMustAlias (/*@exposed@*/ sRef p_s, /*@exposed@*/ sRef p_al)
  /*@modifies internalState@*/ ;

extern /*@only@*/ cstring usymtab_unparseAliases (void)
  /*@globals internalState@*/ ;

extern /*@exposed@*/ uentry
  usymtab_supReturnFileEntry (/*@only@*/ uentry p_e)
   /*@modifies internalState@*/ ;

extern bool usymtab_isAltDefinitelyNullDeep (sRef p_s)
   /*@globals internalState@*/ ;

extern bool usymtab_existsReal (cstring p_k)
   /*@globals internalState@*/ ;

extern /*@only@*/ sRefSet usymtab_allAliases (sRef p_s)
   /*@globals internalState@*/ ;

extern void usymtab_exportLocal (void)
   /*@modifies internalState@*/ ;

extern void usymtab_popCaseBranch (void)
     /*@modifies internalState@*/ ;

/* special scopes */

/*@constant int globScope;@*/
# define globScope 0  /* global variables */

/*@constant int fileScope;@*/
# define fileScope   1  /* file-level static variables */

/*@constant int paramsScope;@*/
# define paramsScope 2  /* function parameters */

/*@constant int functionScope;@*/
# define functionScope 3

extern /*@falsewhennull@*/ bool usymtab_isDefined (usymtab p_u) /*@*/ ;

/*@constant null usymtab usymtab_undefined; @*/
# define usymtab_undefined ((usymtab)NULL)
# define usymtab_isDefined(u) ((u) != usymtab_undefined)

extern void usymtab_checkDistinctName (uentry p_e, int p_scope)
  /*@globals internalState@*/
  /*@modifies *g_warningstream, p_e@*/ ;

extern /*@exposed@*/ sRef usymtab_lookupGlobalMarker (void) /*@globals internalState@*/ ;
extern void usymtab_addReallyForceMustAlias (/*@exposed@*/ sRef p_s, /*@exposed@*/ sRef p_al) /*@modifies internalState@*/ ;
extern int usymtab_getCurrentDepth (void) /*@globals internalState@*/ ;

# ifdef DEBUGSPLINT
extern void usymtab_checkAllValid (void) /*@modifies g_errorstream@*/ ; 
# endif

# else
# error "Multiple include"
# endif



