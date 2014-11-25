/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** aliasStack.h
*/

# ifndef aliasStack_H
# define aliasStack_H

typedef struct _aliasStack *aliasStack ;

typedef struct _aliasStack
{
  aliasTable elements;
  aliasStack thisbranch;
  aliasStack lastbranch;
  aliasStack parent;
  int lexlevel;
} _aliasStack ;

extern aliasStack aliasStack_new(void);

extern void aliasStack_newAlias (aliasStack s, sRef el, sRef al);
extern void aliasStack_clearAlias (aliasStack s, sRef el);
extern sRefSet aliasStack_canAlias (aliasStack s, sRef sr);

extern void aliasStack_push (aliasStack s, sRef el) ;
extern aliasStack aliasStack_pop (aliasStack s) ;
extern sRef aliasStack_top (aliasStack s) ;

extern cstring aliasStack_unparse (aliasStack s) ;
extern void aliasStack_free (/*@only@*/ aliasStack s) ;

extern sRefSet aliasStack_aliasedBy (aliasStack s, sRef sr);
extern aliasStack aliasStack_branch (aliasStack s);
extern aliasStack aliasStack_trueBranch (aliasStack s);
extern aliasStack aliasStack_altBranch (aliasStack s);
extern aliasStack aliasStack_popBranches (aliasStack alt);
extern aliasStack aliasStack_popTrueBranch (aliasStack s);
extern void aliasStack_checkGlobs (aliasStack s);

# define aliasStackBASESIZE MIDBASESIZE
# define aliasStackNULL ((aliasStack)0)
# else
# error "Multiple include"
# endif




