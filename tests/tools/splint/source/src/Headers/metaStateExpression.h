/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** metaStateExpression.h
*/

# ifndef METASTATEEXPRESSION_H
# define METASTATEEXPRESSION_H

struct s_metaStateExpression {
  metaStateSpecifier spec;
  /*@null@*/ metaStateExpression rest;
} ;

/*@constant null metaStateExpression metaStateExpression_undefined; @*/
# define metaStateExpression_undefined    ((metaStateExpression) NULL)

extern /*@falsewhennull@*/ bool metaStateExpression_isDefined (metaStateExpression) /*@*/ ;
# define metaStateExpression_isDefined(p_info) ((p_info) != NULL)

extern /*@nullwhentrue@*/ bool metaStateExpression_isUndefined (metaStateExpression) /*@*/ ;
# define metaStateExpression_isUndefined(p_info) ((p_info) == NULL)

extern /*@notnull@*/ metaStateExpression
metaStateExpression_create (/*@only@*/ metaStateSpecifier) ;

extern /*@notnull@*/ metaStateExpression
metaStateExpression_createMerge (/*@only@*/ metaStateSpecifier, /*@only@*/ metaStateExpression) ;

extern /*@observer@*/ metaStateSpecifier
metaStateExpression_getSpecifier (metaStateExpression) /*@*/ ;

extern bool metaStateExpression_isMerge (metaStateExpression) /*@*/ ;
extern /*@observer@*/ metaStateExpression metaStateExpression_getRest (metaStateExpression) /*@*/ ;

extern metaStateExpression metaStateExpression_copy (metaStateExpression) ;

extern cstring metaStateExpression_unparse (metaStateExpression) /*@*/ ;

extern void metaStateExpression_free (/*@only@*/ metaStateExpression) ;

# else
# error "Multiple include"
# endif 




