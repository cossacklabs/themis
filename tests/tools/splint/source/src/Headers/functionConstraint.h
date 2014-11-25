/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** functionConstraint.h
*/

# ifndef FCNCONSTRAINT_H
# define FcNCONSTRAINT_H

typedef enum
{
  FCT_BUFFER, FCT_METASTATE, FCT_CONJUNCT
} functionConstraintKind ;

struct s_functionConstraint {
  functionConstraintKind kind;
  union {
    /*@only@*/ constraintList buffer;
    /*@only@*/ metaStateConstraint metastate;
    struct { functionConstraint op1; functionConstraint op2; } conjunct;
  } constraint;
} ;

/*@constant null functionConstraint functionConstraint_undefined; @*/
# define functionConstraint_undefined    ((functionConstraint) NULL)

extern /*@falsewhennull@*/ bool functionConstraint_isDefined (functionConstraint) /*@*/ ;
# define functionConstraint_isDefined(p_info) ((p_info) != NULL)

extern /*@falsewhennull@*/ bool functionConstraint_isBufferConstraint (/*@sef@*/ functionConstraint) /*@*/ ;
# define functionConstraint_isBufferConstraint(p_con) (((p_con) != NULL) && ((p_con)->kind == FCT_BUFFER))

extern void functionConstraint_addBufferConstraints (functionConstraint p_node, /*@only@*/ constraintList) 
  /*@modifies p_node@*/ ;

extern /*@nullwhentrue@*/ bool functionConstraint_isUndefined (functionConstraint) /*@*/ ;
# define functionConstraint_isUndefined(p_info) ((p_info) == NULL)

extern functionConstraint functionConstraint_copy (functionConstraint) /*@*/ ;

extern functionConstraint
functionConstraint_createBufferConstraint (/*@only@*/ constraintList) ;

extern functionConstraint
functionConstraint_createMetaStateConstraint (/*@only@*/ metaStateConstraint) ;

extern bool functionConstraint_hasBufferConstraint (functionConstraint) /*@*/ ;
extern bool functionConstraint_hasMetaStateConstraint (functionConstraint) /*@*/ ;

extern functionConstraint
functionConstraint_conjoin (/*@only@*/ functionConstraint, /*@only@*/ functionConstraint) ;

extern /*@only@*/ constraintList functionConstraint_getBufferConstraints (functionConstraint) /*@*/ ;
extern /*@only@*/ metaStateConstraintList functionConstraint_getMetaStateConstraints (functionConstraint) /*@*/ ;

extern cstring functionConstraint_unparse (functionConstraint) /*@*/ ;
extern void functionConstraint_free (/*@only@*/ functionConstraint) ;

# else
# error "Multiple include"
# endif 




