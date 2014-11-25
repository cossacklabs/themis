/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** functionClause.h
*/

# ifndef FUNCTIONCLAUSE_H
# define FUNCTIONCLAUSE_H

/*:private:*/ typedef enum { 
  FCK_GLOBALS,
  FCK_MODIFIES,
  FCK_WARN,
  FCK_STATE,
  FCK_ENSURES,
  FCK_REQUIRES,
  FCK_DEAD
} functionClauseKind;

struct s_functionClause {
  functionClauseKind kind;
  union {
    /*@null@*/ globalsClause globals;
    /*@null@*/ modifiesClause modifies;
    /*@null@*/ warnClause warn;
    /*@null@*/ stateClause state;
    /*@null@*/ functionConstraint constraint;
  } val;
} ;

/* functionClause defined in forwardTypes.h */

/*@constant null functionClause functionClause_undefined; @*/
# define functionClause_undefined NULL

extern /*@falsewhennull@*/ bool functionClause_isDefined(functionClause) /*@*/ ;
# define functionClause_isDefined(p_h) ((p_h) != functionClause_undefined)

extern bool functionClause_isGlobals (functionClause) /*@*/ ;
# define functionClause_isGlobals(p_h) (functionClause_matchKind(p_h, FCK_GLOBALS))

extern bool functionClause_isNoMods (/*@sef@*/ functionClause) /*@*/ ;
# define functionClause_isNoMods(p_h) (functionClause_matchKind(p_h, FCK_MODIFIES) && modifiesClause_isNoMods (functionClause_getModifies (p_h)))
extern bool functionClause_isModifies (functionClause) /*@*/ ;
# define functionClause_isModifies(p_h) (functionClause_matchKind(p_h, FCK_MODIFIES))

extern bool functionClause_isState (functionClause) /*@*/ ;
# define functionClause_isState(p_h) (functionClause_matchKind(p_h, FCK_STATE))

extern bool functionClause_isWarn (functionClause) /*@*/ ;
# define functionClause_isWarn(p_h) (functionClause_matchKind(p_h, FCK_WARN))

extern bool functionClause_isEnsures (functionClause) /*@*/ ;
# define functionClause_isEnsures(p_h) (functionClause_matchKind(p_h, FCK_ENSURES))

extern bool functionClause_isRequires (functionClause) /*@*/ ;
# define functionClause_isRequires(p_h) (functionClause_matchKind(p_h, FCK_REQUIRES))

extern /*@nullwhentrue@*/ bool functionClause_isUndefined(functionClause) /*@*/ ;
# define functionClause_isUndefined(p_h) ((p_h) == functionClause_undefined)

extern functionClause functionClause_createGlobals (/*@only@*/ globalsClause) /*@*/ ;
extern functionClause functionClause_createModifies (/*@only@*/ modifiesClause) /*@*/ ;
extern functionClause functionClause_createWarn (/*@only@*/ warnClause) /*@*/ ;
extern functionClause functionClause_createState (/*@only@*/ stateClause) /*@*/ ;
extern functionClause functionClause_createEnsures (/*@only@*/ functionConstraint) /*@*/ ;
extern functionClause functionClause_createRequires (/*@only@*/ functionConstraint) /*@*/ ;

extern /*@exposed@*/ globalsClause functionClause_getGlobals (functionClause) /*@*/ ;
extern /*@exposed@*/ modifiesClause functionClause_getModifies (functionClause) /*@*/ ;
extern /*@exposed@*/ stateClause functionClause_getState (functionClause) /*@*/ ;
extern /*@exposed@*/ warnClause functionClause_getWarn (functionClause) /*@*/ ;
extern /*@exposed@*/ functionConstraint functionClause_getEnsures (functionClause) /*@*/ ;
extern /*@exposed@*/ functionConstraint functionClause_getRequires (functionClause) /*@*/ ;

extern /*@only@*/ stateClause functionClause_takeState (functionClause p_fc) /*@modifies p_fc@*/ ;
extern /*@only@*/ functionConstraint functionClause_takeEnsures (functionClause p_fc) /*@modifies p_fc@*/ ;
extern /*@only@*/ functionConstraint functionClause_takeRequires (functionClause p_fc) /*@modifies p_fc@*/ ;
extern /*@only@*/ warnClause functionClause_takeWarn (functionClause p_fc) /*@modifies p_fc@*/ ;

extern bool functionClause_matchKind (functionClause p_p, functionClauseKind p_kind) /*@*/ ;

extern void functionClause_free (/*@only@*/ functionClause p_node) ;
extern /*@only@*/ cstring functionClause_unparse (functionClause p_p) /*@*/ ;

# else
# error "Multiple include"
# endif
