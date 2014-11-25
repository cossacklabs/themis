/*
** transferChecks.h
*/

extern bool transferChecks_globalDestroyed (sRef p_fref, fileloc p_loc) 
   /*@modifies g_warningstream@*/ ;
extern void transferChecks_localDestroyed (sRef p_fref, fileloc p_loc) 
   /*@modifies g_warningstream@*/ ;
extern void transferChecks_structDestroyed (sRef p_fref, fileloc p_loc)
   /*@modifies g_warningstream@*/ ;

extern void transferChecks_assign (exprNode p_lhs, exprNode p_rhs);
extern void transferChecks_initialization (exprNode p_lhs, exprNode p_rhs);

extern void transferChecks_passParam 
              (exprNode p_fexp, uentry p_arg, bool p_isSpec,
	       /*@dependent@*/ exprNode p_fcn, int p_argno, int p_totargs);

extern void transferChecks_return (exprNode p_fexp, uentry p_rval);
extern void transferChecks_globalReturn (uentry p_glob);
extern void transferChecks_paramReturn (uentry p_actual);

extern void transferChecks_loseReference (uentry p_actual);
extern bool transferChecks_canLoseReference (/*@dependent@*/ sRef p_sr, fileloc p_loc);
