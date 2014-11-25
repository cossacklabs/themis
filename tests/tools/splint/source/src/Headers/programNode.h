/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
typedef enum 
{
  ACT_SELF, ACT_ITER, ACT_ALTERNATE, 
  ACT_SEQUENCE
} actionKind;

typedef struct {
  int wrapped; /* number of balanced parentheses around this program node */
  actionKind kind;
  union {
    stmtNode self;
    programNodeList args;
  } content;
} *programNode;

extern void programNode_free (/*@only@*/ /*@null@*/ programNode p_x);
extern /*@only@*/ cstring programNode_unparse (programNode p_p);

