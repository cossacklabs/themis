/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** exprChecks.h
*/

extern bool anyAbstract (ctype p_c1, /*@sef@*/ ctype p_c2);
# define anyAbstract(c1, c2) \
  (ctype_isRealAbstract(c1) || ctype_isRealAbstract(c2))

extern void exprNode_checkAllMods (sRefSet p_mods, uentry p_ue);
extern void exprNode_checkCallModifyVal (sRef p_s, exprNodeList p_args, exprNode p_f, exprNode p_err);
extern void exprChecks_checkEmptyMacroBody (void);
extern void exprChecks_checkExport (uentry p_e);

extern void exprChecks_inCompoundStatementExpression (void) /*@modifies internalState@*/ ;
extern void exprChecks_leaveCompoundStatementExpression (void) /*@modifies internalState@*/ ;

extern void exprNode_checkFunction (uentry p_ue, /*@only@*/ exprNode p_body);
extern void exprNode_checkFunctionBody (exprNode p_body);
extern void exprNode_checkIterBody (/*@only@*/ exprNode p_body);
extern void exprNode_checkIterEnd (/*@only@*/ exprNode p_body);
extern void exprNode_checkMacroBody (/*@only@*/ exprNode p_e);
extern void exprNode_checkModify (exprNode p_e, exprNode p_err);
extern void exprNode_checkModifyVal (exprNode p_e, exprNode p_err);
extern void exprChecks_checkNullReturn (fileloc p_loc);
extern void exprNode_checkPred (cstring p_c, exprNode p_e);
extern void exprNode_checkReturn (exprNode p_e);
extern void exprChecks_checkStatementEffect (exprNode p_e);
extern void exprChecks_checkUsedGlobs (globSet p_decl, globSet p_used);

