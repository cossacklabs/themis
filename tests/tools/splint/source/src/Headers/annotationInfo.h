/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** annotationInfo.h
**
** A record that keeps information on a user-defined annotations including:
**
*/

# ifndef ANNOTINFO_H
# define ANNOTINFO_H

struct s_annotationInfo {
  cstring name;
  /*@dependent@*/ /*@observer@*/ metaStateInfo state;
  /* associated metaStateInfo entry */
  fileloc loc;
  int value;
  mtContextNode context;
} ;

/* typedef struct _annotationInfo *annotationInfo; */

/*@constant null annotationInfo annotationInfo_undefined; @*/
# define annotationInfo_undefined    ((annotationInfo) NULL)

extern /*@falsewhennull@*/ bool annotationInfo_isDefined (annotationInfo) /*@*/ ;
# define annotationInfo_isDefined(p_info) ((p_info) != annotationInfo_undefined)

extern /*@nullwhentrue@*/ bool annotationInfo_isUndefined (annotationInfo) /*@*/ ;
# define annotationInfo_isUndefined(p_info) ((p_info) == annotationInfo_undefined)

extern bool annotationInfo_equal (annotationInfo, annotationInfo) /*@*/ ;
# define annotationInfo_equal(p_info1, p_info2) ((p_info1) == (p_info2))

extern bool annotationInfo_matchesContext (annotationInfo, uentry) /*@*/ ;
extern bool annotationInfo_matchesContextRef (annotationInfo, sRef) /*@*/ ;

extern /*@observer@*/ metaStateInfo annotationInfo_getState (annotationInfo) /*@*/ ;
extern int annotationInfo_getValue (annotationInfo) /*@*/ ;
extern /*@observer@*/ cstring annotationInfo_getName (annotationInfo) /*@*/ ;

extern /*@only@*/ annotationInfo 
annotationInfo_create (/*@only@*/ cstring p_name,
		       /*@dependent@*/ /*@exposed@*/ metaStateInfo p_state, 
		       /*@only@*/ mtContextNode p_context,
		       int p_value, /*@only@*/ fileloc p_loc) /*@*/ ;

extern /*@observer@*/ cstring annotationInfo_unparse (annotationInfo p_a);
extern /*@observer@*/ fileloc annotationInfo_getLoc (annotationInfo p_a) /*@*/ ;

extern void annotationInfo_free (/*@only@*/ annotationInfo) ;

extern /*@observer@*/ cstring annotationInfo_dump (annotationInfo) ;
extern /*@observer@*/ annotationInfo annotationInfo_undump (char **p_s) /*@modifies *p_s@*/ ;

extern void annotationInfo_showContextRefError (annotationInfo, sRef) /*@*/ ;

# else
# error "Multiple include"
# endif 




