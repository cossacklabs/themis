/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** paramNode.h
*/

# ifndef PARAMNODEH
# define PARAMNODEH

typedef enum { PNORMAL, PYIELD, PELIPSIS } paramkind ;  
/* add PREF later?   PYIELD is for iterators only (~ POUT) */

typedef struct {
  /*@null@*/ lclTypeSpecNode type;
  /*@null@*/ struct s_typeExpr *paramdecl;
  paramkind kind;
} *paramNode;

extern void paramNode_free (/*@only@*/ /*@null@*/ paramNode p_x);

extern /*@null@*/ paramNode paramNode_copy (/*@null@*/ paramNode p_p);
extern /*@only@*/ cstring paramNode_unparse (paramNode p_x);
extern /*@only@*/ cstring paramNode_unparseComments (paramNode p_x);

extern bool paramNode_isElipsis (paramNode p_p);
extern bool paramNode_isYield (paramNode p_p);

# define paramNode_isElipsis(p) ((p)->kind == PELIPSIS)
# define paramNode_isYield(p) ((p)->kind == PYIELD)

# else
# error "Multiple include"
# endif


