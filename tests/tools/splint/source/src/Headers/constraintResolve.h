
extern /*@only@*/ constraintList constraintList_reflectChanges(/*@observer@*/ /*@temp@*/ constraintList p_pre2, /*@observer@*/ /*@temp@*/ constraintList p_post1);


/* tries to resolve constraints in List p_pre2 using post1 */
/*@only@*/ extern constraintList constraintList_reflectChangesFreePre (/*@only@*/ constraintList p_pre2, /*@observer@*/ /*@temp@*/ constraintList p_post1);

extern /*@only@*/ constraint constraint_substitute (/*@temp@*/ /*@observer@*/ constraint p_c, /*@temp@*/ /*@observer@*/ constraintList p_p);

/* extern constraint constraint_searchandreplace (constraint p_c, constraintExpr old, constraintExpr new); */

extern bool constraintList_resolve (/*@temp@*/ /*@observer@*/ constraint p_c,  /*@temp@*/ /*@observer@*/ constraintList p_p);

extern constraint constraint_simplify ( /*@returned@*/ constraint p_c);

extern /*@only@*/ constraintList constraintList_fixConflicts (constraintList p_list1, constraintList p_list2);

extern constraintList constraintList_subsumeEnsures (constraintList p_list1, constraintList p_list2);

extern constraintList constraintList_mergeEnsures (/*observer@*/ /*@temp@*/ constraintList p_list1, /*@observer@*/ /*@temp@*/ constraintList p_list2);

/*@only@*/ constraintList constraintList_mergeEnsuresFreeFirst (/*@only@*/ constraintList p_list1, /*@observer@*/ /*@temp@*/ constraintList p_list2);


bool constraint_isAlwaysTrue (/*@observer@*/ /*@temp@*/ constraint p_c);

/*@only@*/ constraintList constraintList_mergeRequires (/*@observer@*/ /*@temp@*/ constraintList p_list1,/*@observer@*/ /*@temp@*/ constraintList p_list2);

/*@only@*/ constraintList constraintList_mergeRequiresFreeFirst (/*@only@*/constraintList p_list1,/*@observer@*/ /*@temp@*/ constraintList p_list2);



constraintList constraintList_reflectChangesOr (constraintList p_pre2, constraintList p_post1);

/*@only@*/ constraintList constraintList_substitute (constraintList p_target, /*2observer@*/ constraintList p_subList);

/*@only@*/ constraintList constraintList_substituteFreeTarget (/*@only@*/ constraintList p_target, /*@observer@*/ /*@temp@*/ constraintList p_subList);

extern void exprNode_mergeResolve (exprNode p_parent, exprNode p_child1, exprNode p_child2);

