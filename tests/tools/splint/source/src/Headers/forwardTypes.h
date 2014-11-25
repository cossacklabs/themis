# ifndef FORWARDTYPES_H
# define FORWARDTYPES_H

# include "lclForwardTypes.h"

# if defined (WIN32) || defined (OS2) && defined (__IBMC__)
/* Microsoft doesn't support ISO C99 yet */
/*@-namechecks@*/
typedef int bool; 
/*@=namechecks@*/
# endif

abst_typedef /*@null@*/ struct s_sRef *sRef;
abst_typedef /*@null@*/ struct s_uentry *uentry;
typedef struct s_hashNode *hashNode;
immut_typedef int typeIdSet;
typedef /*@only@*/ uentry o_uentry;
abst_typedef struct s_mttok *mttok;
abst_typedef /*@null@*/ struct s_idDecl *idDecl;
abst_typedef /*@null@*/ struct s_usymtab  *usymtab;
abst_typedef /*@null@*/ struct s_exprNode *exprNode;
abst_typedef /*@null@*/ struct s_guardSet *guardSet;
abst_typedef /*@null@*/ struct s_sRefSet *sRefSet;
abst_typedef /*@null@*/ struct s_sRefList *sRefList ;
abst_typedef /*@null@*/ struct s_aliasTable *aliasTable;
abst_typedef /*@null@*/ struct s_fileloc *fileloc;
abst_typedef /*@null@*/ struct s_cstringTable *cstringTable;
abst_typedef /*@null@*/ struct s_genericTable *genericTable;
abst_typedef /*@null@*/ struct s_annotationInfo *annotationInfo;
abst_typedef /*@null@*/ struct s_inputStream *inputStream;
abst_typedef /*@null@*/ struct s_stateValue *stateValue;
abst_typedef /*@null@*/ struct s_pointers *pointers;
abst_typedef /*@null@*/ genericTable valueTable;
abst_typedef /*@null@*/ genericTable metaStateTable;
abst_typedef /*@null@*/ genericTable annotationTable;
abst_typedef /*@null@*/ struct s_metaStateInfo *metaStateInfo;

abst_typedef /*@null@*/ struct s_functionConstraint *functionConstraint;
abst_typedef struct s_metaStateConstraint *metaStateConstraint;
abst_typedef /*@null@*/ struct s_metaStateConstraintList *metaStateConstraintList ;
abst_typedef struct s_metaStateSpecifier *metaStateSpecifier;
abst_typedef /*@null@*/ struct s_metaStateExpression *metaStateExpression;

abst_typedef /*@null@*/ struct s_functionClause *functionClause;
abst_typedef /*@null@*/ struct s_functionClauseList *functionClauseList;

abst_typedef struct s_globalsClause *globalsClause;
abst_typedef struct s_modifiesClause *modifiesClause;
abst_typedef /*@null@*/ struct s_warnClause *warnClause;
abst_typedef struct s_stateClause *stateClause;

abst_typedef /*@null@*/ struct s_stateClauseList *stateClauseList;

/* The mt grammar nodes: */
abst_typedef struct s_mtDeclarationNode *mtDeclarationNode;
abst_typedef /*@null@*/ struct s_mtDeclarationPiece *mtDeclarationPiece;
abst_typedef /*@null@*/ struct s_mtDeclarationPieces *mtDeclarationPieces;
abst_typedef /*@null@*/ struct s_mtContextNode *mtContextNode;
abst_typedef struct s_mtValuesNode *mtValuesNode;
abst_typedef struct s_mtDefaultsNode *mtDefaultsNode;
abst_typedef /*@null@*/ struct s_mtDefaultsDeclList *mtDefaultsDeclList;
abst_typedef struct s_mtDefaultsDecl *mtDefaultsDecl;
abst_typedef struct s_mtAnnotationsNode *mtAnnotationsNode;
abst_typedef /*@null@*/ struct s_mtAnnotationList *mtAnnotationList;
abst_typedef struct s_mtAnnotationDecl *mtAnnotationDecl;
abst_typedef struct s_mtMergeNode *mtMergeNode;
abst_typedef struct s_mtMergeItem *mtMergeItem;
abst_typedef /*@null@*/ struct s_mtMergeClauseList *mtMergeClauseList;
abst_typedef struct s_mtMergeClause *mtMergeClause;
abst_typedef /*@null@*/ struct s_mtTransferClauseList *mtTransferClauseList;
abst_typedef struct s_mtTransferClause *mtTransferClause;
abst_typedef /*@null@*/ struct s_mtLoseReferenceList *mtLoseReferenceList;
abst_typedef struct s_mtLoseReference *mtLoseReference;
abst_typedef struct s_mtTransferAction *mtTransferAction;
abst_typedef sRefSet globSet;

abst_typedef /*@null@*/ struct s_constraint *constraint;
abst_typedef /*@null@*/ struct s_constraintList *constraintList;

abst_typedef /*@null@*/ struct s_ctypeList *ctypeList;
abst_typedef /*@null@*/ ctypeList fileIdList;

/* DRL modified 9 26 00 */

abst_typedef /*@null@*/ struct s_constraintExpr *constraintExpr;

abst_typedef /*@null@*/ char *cstring;
typedef /*@only@*/ cstring o_cstring;

abst_typedef /*@null@*/ struct s_cstringSList *cstringSList;

typedef enum e_LSLInitRuleCode LSLInitRuleCode;

immut_typedef int ctype;

/* sRef -> bool */
typedef bool (*sRefTest) (sRef);

/* sRef, fileloc -> void; modifies sRef */
typedef void (*sRefMod) (sRef, fileloc);

/* sRef, int, fileloc -> void; modifies sRef */
typedef void (*sRefModVal) (sRef, int, fileloc);

/* sRef -> void */
typedef void (*sRefShower) (sRef);

# else
# error "Multiple include"
# endif




