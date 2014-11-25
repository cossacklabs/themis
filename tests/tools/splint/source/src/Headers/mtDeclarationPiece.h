/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mtDeclarationPiece.h
*/

# ifndef MTDECLARATIONPIECE_H
# define MTDECLARATIONPIECE_H

/*:private:*/ typedef enum { 
  MTP_DEAD,
  MTP_CONTEXT, MTP_VALUES, MTP_DEFAULTS, MTP_DEFAULTVALUE,
  MTP_ANNOTATIONS, MTP_MERGE,
  MTP_TRANSFERS, MTP_PRECONDITIONS, MTP_POSTCONDITIONS,
  MTP_LOSERS
} mtPieceKind;

struct s_mtDeclarationPiece {
  mtPieceKind kind;
  /*@relnull@*/ void *node; /* oneof mt*Node or mttok */
} ;

/* mtDeclarationPiece defined in forwardTypes.h */

/*@constant null mtDeclarationPiece mtDeclarationPiece_undefined; @*/
# define mtDeclarationPiece_undefined NULL

extern /*@falsewhennull@*/ bool mtDeclarationPiece_isDefined(mtDeclarationPiece) /*@*/ ;
# define mtDeclarationPiece_isDefined(p_h) ((p_h) != mtDeclarationPiece_undefined)

extern /*@nullwhentrue@*/ bool mtDeclarationPiece_isUndefined(mtDeclarationPiece) /*@*/ ;
# define mtDeclarationPiece_isUndefined(p_h) ((p_h) == mtDeclarationPiece_undefined)

extern mtDeclarationPiece mtDeclarationPiece_createContext (/*@only@*/ mtContextNode) /*@*/ ;
extern mtDeclarationPiece mtDeclarationPiece_createValues (/*@only@*/ mtValuesNode) /*@*/ ;
extern mtDeclarationPiece mtDeclarationPiece_createDefaults (/*@only@*/ mtDefaultsNode) /*@*/ ;
extern mtDeclarationPiece mtDeclarationPiece_createValueDefault (/*@only@*/ mttok) /*@*/ ;
extern mtDeclarationPiece mtDeclarationPiece_createAnnotations (/*@only@*/ mtAnnotationsNode) /*@*/ ;
extern mtDeclarationPiece mtDeclarationPiece_createMerge (/*@only@*/ mtMergeNode) /*@*/ ;
extern mtDeclarationPiece mtDeclarationPiece_createTransfers (/*@only@*/ mtTransferClauseList) /*@*/ ;
extern mtDeclarationPiece mtDeclarationPiece_createPostconditions (/*@only@*/ mtTransferClauseList) /*@*/ ;
extern mtDeclarationPiece mtDeclarationPiece_createPreconditions (/*@only@*/ mtTransferClauseList) /*@*/ ;
extern mtDeclarationPiece mtDeclarationPiece_createLosers (/*@only@*/ mtLoseReferenceList) /*@*/ ;

extern /*@observer@*/ mtContextNode mtDeclarationPiece_getContext (mtDeclarationPiece) /*@*/ ;
extern /*@only@*/ mtContextNode mtDeclarationPiece_stealContext (mtDeclarationPiece p_node) /*@modifies p_node@*/ ;

extern /*@observer@*/ mtValuesNode mtDeclarationPiece_getValues (mtDeclarationPiece) /*@*/ ;
extern /*@observer@*/ mtDefaultsNode mtDeclarationPiece_getDefaults (mtDeclarationPiece) /*@*/ ;
extern /*@observer@*/ mtAnnotationsNode mtDeclarationPiece_getAnnotations (mtDeclarationPiece) /*@*/ ;
extern /*@observer@*/ mtMergeNode mtDeclarationPiece_getMerge (mtDeclarationPiece) /*@*/ ;
extern /*@observer@*/ mtTransferClauseList mtDeclarationPiece_getTransfers (mtDeclarationPiece) /*@*/ ;
extern /*@observer@*/ mtTransferClauseList mtDeclarationPiece_getPostconditions (mtDeclarationPiece) /*@*/ ;
extern /*@observer@*/ mtTransferClauseList mtDeclarationPiece_getPreconditions (mtDeclarationPiece) /*@*/ ;
extern /*@observer@*/ cstring mtDeclarationPiece_getDefaultValue (mtDeclarationPiece) /*@*/ ;

extern /*@observer@*/ mtLoseReferenceList mtDeclarationPiece_getLosers (mtDeclarationPiece) /*@*/ ;

extern bool mtDeclarationPiece_matchKind (mtDeclarationPiece p_p, mtPieceKind p_kind) /*@*/ ;

extern void mtDeclarationPiece_free (/*@only@*/ mtDeclarationPiece p_node) ;
extern /*@only@*/ cstring mtDeclarationPiece_unparse (mtDeclarationPiece p_p) /*@*/ ;

# else
# error "Multiple include"
# endif
