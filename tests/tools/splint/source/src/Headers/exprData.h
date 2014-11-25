/* ;-*-C-*-; */

/*
** freeShallow: free exprData created from exprNode_effect calls.
**    All but the innermost storage is free'd.
*/

/*@only@*/ exprData exprData_makeLiteral (/*@only@*/ cstring p_s);

/*@only@*/ exprData exprData_makeId (/*@temp@*/ uentry p_id);

/*@only@*/ exprData exprData_makePair (/*@keep@*/ exprNode p_a, /*@keep@*/ exprNode p_b);

/*@-declundef*/
/*static*/ void exprData_freeShallow (/*@only@*/ exprData p_data, exprKind p_kind);

/*static*/ void exprData_free (/*@only@*/ exprData p_data, exprKind p_kind);

/*static*/ /*@exposed@*/ exprNode exprData_getInitNode (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ idDecl exprData_getInitId (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ exprNode exprData_getOpA (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ exprNode exprData_getOpB (exprData p_data) /*@*/;

/*static*/ /*@observer@*/ lltok exprData_getOpTok (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ exprNode exprData_getPairA (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ exprNode exprData_getPairB (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ uentry exprData_getIterSname (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ exprNodeList exprData_getIterAlist (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ exprNode exprData_getIterBody (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ uentry exprData_getIterEname (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ exprNode exprData_getFcn (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ exprNodeList exprData_getArgs (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ exprNode exprData_getTriplePred (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ uentry exprData_getIterCallIter (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ exprNodeList
exprData_getIterCallArgs (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ exprNode exprData_getTripleInit (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ exprNode exprData_getTripleTrue (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ exprNode exprData_getTripleTest (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ exprNode exprData_getTripleFalse (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ exprNode exprData_getTripleInc (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ exprNode exprData_getFieldNode (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ cstring exprData_getFieldName (exprData p_data) /*@*/;

/*static*/ /*@observer@*/ lltok exprData_getUopTok (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ exprNode exprData_getUopNode (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ exprNode exprData_getCastNode (exprData p_data) /*@*/;

/*static*/ /*@observer@*/ lltok exprData_getCastTok (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ qtype exprData_getCastType (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ cstring exprData_getLiteral (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ cstring exprData_getId (exprData p_data) /*@*/;

/*static*/ /*@observer@*/ lltok exprData_getTok (exprData p_data) /*@*/;

/*static*/ /*@exposed@*/ qtype exprData_getType (exprData p_data) /*@*/ ;

/*static*/ /*@exposed@*/ qtype exprData_getOffsetType (exprData p_data) /*@*/ ;

/*static*/ /*@exposed@*/ cstringList exprData_getOffsetName (exprData p_data) /*@*/ ;

/*drl 10/18/001 added sef annotation*/
/*static*/ /*@exposed@*/ exprNode exprData_getSingle (exprData p_data) /*@*/;

/*static*/ /*@only@*/ exprData 
exprData_makeOp (/*@keep@*/ exprNode p_a, /*@keep@*/ exprNode p_b, /*@keep@*/ lltok p_op);

/*static*/ /*@only@*/ exprData exprData_makeUop (/*@keep@*/ exprNode p_a, /*@keep@*/ lltok p_op);

/*static*/ /*@only@*/ exprData exprData_makeSingle (/*@only@*/ exprNode p_a);

/*static*/ /*@only@*/ exprData exprData_makeTok (/*@only@*/ lltok p_op);

/*static*/ /*@only@*/ exprData 
exprData_makeIter (/*@exposed@*/ uentry p_sname, /*@keep@*/ exprNodeList p_args,
		   /*@keep@*/ exprNode p_body, /*@exposed@*/ uentry p_ename);

/*static*/ /*@only@*/ exprData exprData_makeTriple (/*@keep@*/ exprNode p_pred, 
						/*@keep@*/ exprNode p_tbranch, 
						/*@keep@*/ exprNode p_fbranch);

/*static*/ /*@only@*/ exprData exprData_makeCall (/*@keep@*/ exprNode p_fcn,
					      /*@keep@*/ exprNodeList p_args);


/*static*/ /*@only@*/ exprData exprData_makeIterCall (/*@dependent@*/ uentry p_iter,
						  /*@keep@*/ exprNodeList p_args);

/*static*/ /*@only@*/ exprData exprData_makeField (/*@keep@*/ exprNode p_rec, 
					       /*@keep@*/ cstring p_field);

/*static*/ /*@only@*/ exprData exprData_makeOffsetof (/*@only@*/ qtype p_q,
						  /*@keep@*/ cstringList p_s);


/*static*/ /*@only@*/ exprData exprData_makeSizeofType (/*@only@*/ qtype p_q);

/*static*/ /*@only@*/ exprData 
  exprData_makeCast (/*@keep@*/ lltok p_tok, /*@keep@*/ exprNode p_e, /*@only@*/ qtype p_q);

/*static*/ /*@only@*/ exprData 
  exprData_makeInit (/*@keep@*/ idDecl p_t, /*@keep@*/ exprNode p_e);


/*static*/ /*@only@*/ exprData exprData_makeCond (/*@keep@*/ exprNode p_pred, 
					      /*@keep@*/ exprNode p_ifclause, 
					      /*@keep@*/ exprNode p_elseclause);

/*static*/ /*@only@*/ exprData exprData_makeFor (/*@keep@*/ exprNode p_init, 
					     /*@keep@*/ exprNode p_pred, 
					     /*@keep@*/ exprNode p_inc);


/*@=declundef*/








