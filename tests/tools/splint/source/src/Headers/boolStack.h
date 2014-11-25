/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** boolStack.h
*/

# ifndef boolStack_H
# define boolStack_H

typedef struct _boolStack
{
  int nelements;
  int nspace;
  int current;
  /*@reldef@*/ bool  *elements;
} _boolStack ;

typedef _boolStack *boolStack ;

# define boolStack_size(s) ((s)->nelements)   
# define boolStack_isEmpty(s) (boolStack_size(s) == 0)
# define boolStack_empty(s) (boolStack_size(s) == 0)

extern boolStack boolStack_new(void);

extern void boolStack_push (boolStack s, bool el) ;
extern void boolStack_pop (boolStack s) ;
extern bool boolStack_top (boolStack s) ;

extern cstring boolStack_unparse (boolStack s) ;
extern void boolStack_free (/*@only@*/ boolStack s) ;
extern void boolStack_switchTop (boolStack s);

# define boolStackBASESIZE MIDBASESIZE

# else
# error "Multiple include"
# endif




