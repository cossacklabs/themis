/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** metaStateSpecifier.h
*/

# ifndef METASTATESPECIFIER_H
# define METASTATESPECIFIER_H

struct s_metaStateSpecifier {
  bool elipsis;
  sRef sr;
  /*@observer@*/ metaStateInfo msinfo;
} ;

extern metaStateSpecifier
metaStateSpecifier_create (/*@only@*/ sRef, /*@observer@*/ metaStateInfo) ;

extern metaStateSpecifier
metaStateSpecifier_createElipsis (/*@observer@*/ metaStateInfo) ;

extern bool metaStateSpecifier_isElipsis (metaStateSpecifier) /*@*/ ;
extern /*@exposed@*/ sRef metaStateSpecifier_getSref (metaStateSpecifier) /*@*/ ;
extern /*@observer@*/ metaStateInfo metaStateSpecifier_getMetaStateInfo (metaStateSpecifier) /*@*/ ;

extern cstring metaStateSpecifier_unparse (metaStateSpecifier) /*@*/ ;
extern metaStateSpecifier metaStateSpecifier_copy (metaStateSpecifier) /*@*/ ;
extern void metaStateSpecifier_free (/*@only@*/ metaStateSpecifier) ;

# else
# error "Multiple include"
# endif 




