/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** stateClause.h
*/

# ifndef STATECLAUSE_H
# define STATECLAUSE_H

typedef enum
{
  SP_USES,
  SP_DEFINES,
  SP_ALLOCATES,
  SP_RELEASES,
  SP_SETS,
  SP_QUAL,
  SP_GLOBAL
} stateClauseKind ;

typedef enum
{
  TK_BEFORE,
  TK_AFTER,
  TK_BOTH
} stateConstraint;

struct s_stateClause
{
  stateConstraint state;
  stateClauseKind kind;
  qual squal; /* only for SP_QUAL and SP_GLOBAL */
  sRefSet refs;
  fileloc loc;
} ;

/* in forwardTypes.h: abst_typedef struct _stateClause *stateClause; */

typedef /*@only@*/ stateClause o_stateClause;

extern /*@unused@*/ cstring stateClause_unparse (stateClause p_s) /*@*/ ;

extern /*@null@*/ sRefMod 
  stateClause_getEffectFunction (stateClause p_cl) /*@*/ ;

extern /*@null@*/ sRefModVal
  stateClause_getEnsuresFunction (stateClause p_cl) /*@*/ ;

extern /*@null@*/ sRefModVal
  stateClause_getRequiresBodyFunction (stateClause p_cl) /*@*/ ;

extern int
  stateClause_getStateParameter (stateClause p_cl) /*@*/ ;

extern /*@null@*/ sRefMod 
  stateClause_getReturnEffectFunction (stateClause p_cl) /*@*/ ;

extern /*@null@*/ sRefMod 
  stateClause_getEntryFunction (stateClause p_cl) /*@*/ ;

extern bool stateClause_isGlobal (stateClause p_cl) /*@*/ ;
# define stateClause_isGlobal(cl) ((cl)->kind == SP_GLOBAL)

extern bool stateClause_isBefore (stateClause p_cl) /*@*/ ;
extern bool stateClause_isBeforeOnly (stateClause p_cl) /*@*/ ;
extern bool stateClause_isAfter (stateClause p_cl) /*@*/ ;
extern bool stateClause_isEnsures (stateClause p_cl) /*@*/ ;

extern bool stateClause_sameKind (stateClause p_s1, stateClause p_s2) /*@*/ ;

extern /*@observer@*/ sRefSet stateClause_getRefs (stateClause p_cl) /*@*/ ;
# define stateClause_getRefs(cl) ((cl)->refs)

extern flagcode stateClause_preErrorCode (stateClause p_cl) /*@*/ ;
extern /*@observer@*/ cstring 
  stateClause_preErrorString (stateClause p_cl, sRef p_sr) /*@*/ ;

extern flagcode stateClause_postErrorCode (stateClause p_cl) /*@*/ ;
extern /*@observer@*/ cstring 
  stateClause_postErrorString (stateClause p_cl, sRef p_sr) /*@*/ ;

extern sRefTest stateClause_getPreTestFunction (stateClause p_cl) /*@*/ ;
extern sRefTest stateClause_getPostTestFunction (stateClause p_cl) /*@*/ ;
extern sRefShower stateClause_getPostTestShower (stateClause p_cl) /*@*/ ;

extern stateClause 
  stateClause_create (lltok p_tok, qual p_q, /*@only@*/ sRefSet p_s) /*@*/ ;

extern stateClause 
  stateClause_createPlain (lltok p_tok, /*@only@*/ sRefSet p_s) /*@*/ ;

extern stateClause stateClause_createDefines (/*@only@*/ sRefSet p_s) /*@*/ ;
extern stateClause stateClause_createUses (/*@only@*/ sRefSet p_s) /*@*/ ;
extern stateClause stateClause_createAllocates (/*@only@*/ sRefSet p_s) /*@*/ ;
extern stateClause stateClause_createReleases (/*@only@*/ sRefSet p_s) /*@*/ ;
extern stateClause stateClause_createSets (/*@only@*/ sRefSet p_s) /*@*/ ;

extern /*@observer@*/ fileloc stateClause_loc (stateClause) /*@*/ ;
extern bool stateClause_isMemoryAllocation (stateClause p_cl) /*@*/ ;
extern bool stateClause_isQual (stateClause p_cl) /*@*/ ;

extern void stateClause_free (/*@only@*/ stateClause p_s) ;
extern cstring stateClause_dump (stateClause p_s) /*@*/ ;
extern stateClause stateClause_undump (char **p_s) /*@modifies *p_s@*/ ;
extern stateClause stateClause_copy (stateClause p_s) /*@*/ ;
extern bool stateClause_matchKind (stateClause p_s1, stateClause p_s2) /*@*/ ;

extern bool stateClause_hasEnsures (stateClause p_cl) /*@*/ ;
extern bool stateClause_hasRequires (stateClause p_cl) /*@*/ ;

extern bool stateClause_setsMetaState (stateClause p_cl) /*@*/ ;
extern qual stateClause_getMetaQual (stateClause p_cl) /*@*/ ;

extern bool stateClause_hasEmptyReferences (stateClause p_s);

extern bool stateClause_isMetaState (stateClause p_s);

# else
# error "Multiple include"
# endif


