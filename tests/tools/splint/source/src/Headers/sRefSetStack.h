/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** sRefSetStack.h
*/

# ifndef sRefSetStack_H
# define sRefSetStack_H

typedef struct _sRefSetStack *sRefSetStack ;

typedef struct _sRefSetStack
{
  sRefSet elements;
  sRefSet allElements;
  sRefSetStack thisbranch;
  sRefSetStack lastbranch;
  sRefSetStack parent;
  int lexlevel;
} _sRefSetStack ;

extern sRefSetStack sRefSetStack_new(void);

extern void sRefSetStack_push (sRefSetStack s, sRef el) ;
extern sRefSetStack sRefSetStack_pop (sRefSetStack s) ;
extern sRefSet sRefSetStack_top (sRefSetStack s) ;

extern cstring sRefSetStack_unparse (sRefSetStack s) ;
extern void sRefSetStack_free (/*@only@*/ sRefSetStack s) ;
extern sRefSetStack sRefSetStack_init (void);

extern sRefSetStack sRefSetStack_branch (sRefSetStack s);
extern sRefSetStack sRefSetStack_trueBranch (sRefSetStack s);
extern sRefSetStack sRefSetStack_altBranch (sRefSetStack s);
extern sRefSetStack sRefSetStack_popBranches (sRefSetStack alt);
extern sRefSetStack sRefSetStack_popTrueBranch (sRefSetStack s);

# define sRefSetStackBASESIZE    MIDBASESIZE
# define sRefSetStackNULL        ((sRefSetStack)0)
# define sRefSetStack_undefined  (sRefSetStackNULL)

# else
# error "Multiple include"
# endif




