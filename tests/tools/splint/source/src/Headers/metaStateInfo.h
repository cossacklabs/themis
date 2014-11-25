/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** metaStateInfo.h
**
** A record that keeps information on a user-defined state including:
**
**    o A name
**    o State Type (what kinds of things have this state value)
**    o Number of values, and their mnemonics
**    o A combination table (stateCombinationTable) for how different states combine as l/rvalues
**    o Annotations (and map between annotation and value)
**    o Context information (where can annotations be used)
*/

# ifndef MSINFO_H
# define MSINFO_H

/*@constant int metaState_error@*/
# define metaState_error -1

struct s_metaStateInfo {
  /*@only@*/ cstring name;
  fileloc loc;
  /* metaStateKind type; */
  cstringList valueNames;
  stateCombinationTable sctable;
  stateCombinationTable mergetable;

  /* Default values */
  int defaultValue [MTC_NUMCONTEXTS];

  /* context */
  mtContextNode context;
} ;

/* in forwardTypes: abst_typedef null struct _metaStateInfo *metaStateInfo; */

/*@constant null metaStateInfo metaStateInfo_undefined; @*/
# define metaStateInfo_undefined    ((metaStateInfo) NULL)

extern /*@falsewhennull@*/ bool metaStateInfo_isDefined (metaStateInfo) /*@*/ ;
# define metaStateInfo_isDefined(p_info) ((p_info) != NULL)

extern /*@nullwhentrue@*/ bool metaStateInfo_isUndefined (metaStateInfo) /*@*/ ;
# define metaStateInfo_isUndefined(p_info) ((p_info) == NULL)

extern /*@notnull@*/ metaStateInfo 
metaStateInfo_create (/*@only@*/ cstring p_name,
		      /*@only@*/ cstringList p_valueNames,
		      /*@only@*/ mtContextNode p_context,
		      /*@only@*/ stateCombinationTable p_sctable,
		      /*@only@*/ stateCombinationTable p_mergetable,
		      /*@only@*/ fileloc p_loc) ;

extern bool metaStateInfo_equal (metaStateInfo p_m1, metaStateInfo p_m2) /*@*/ ;
# define metaStateInfo_equal(m1,m2) ((m1) == (m2))

extern int metaStateInfo_getDefaultValueContext (metaStateInfo p_info, mtContextKind p_context) /*@*/ ;
extern void metaStateInfo_setDefaultValueContext (metaStateInfo p_info, mtContextKind p_context, int p_val) /*@modifies p_info@*/ ;

extern void metaStateInfo_setDefaultRefValue (metaStateInfo p_info, int p_val) 
     /*@modifies p_info@*/ ;

extern void metaStateInfo_setDefaultParamValue (metaStateInfo p_info, int p_val) 
     /*@modifies p_info@*/ ;

extern void metaStateInfo_setDefaultResultValue (metaStateInfo p_info, int p_val) 
     /*@modifies p_info@*/ ;

extern int metaStateInfo_getDefaultValue (metaStateInfo p_info, sRef p_s) /*@*/ ;

extern int metaStateInfo_getDefaultRefValue (metaStateInfo p_info) /*@*/ ;
extern int metaStateInfo_getDefaultParamValue (metaStateInfo p_info) /*@*/ ;
extern int metaStateInfo_getDefaultResultValue (metaStateInfo p_info) /*@*/ ;
extern int metaStateInfo_getDefaultGlobalValue (metaStateInfo p_info) /*@*/ ;

extern /*@observer@*/ mtContextNode metaStateInfo_getContext (metaStateInfo p_info) /*@*/ ;
extern /*@observer@*/ cstring metaStateInfo_getName (metaStateInfo p_info) /*@*/ ;
extern /*@observer@*/ fileloc metaStateInfo_getLoc (metaStateInfo p_info) /*@*/ ;

extern /*@exposed@*/ stateCombinationTable metaStateInfo_getTransferTable (metaStateInfo p_info) /*@*/ ;

extern /*@exposed@*/ stateCombinationTable metaStateInfo_getMergeTable (metaStateInfo p_info) /*@*/ ;

extern /*@only@*/ cstring metaStateInfo_unparse (metaStateInfo p_info) /*@*/ ;

extern /*@observer@*/ cstring metaStateInfo_unparseValue (metaStateInfo p_info, int p_value) /*@*/ ;

extern void metaStateInfo_free (/*@only@*/ metaStateInfo) ;

# else
# error "Multiple include"
# endif 




