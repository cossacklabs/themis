/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** refTable.h
*/

# ifndef REFTABLE_H
# define REFTABLE_H

/*@private@*/ typedef struct _refentry { int level; int index; } *refentry;
/*@private@*/ typedef /*@only@*/ refentry o_refentry;

typedef /*@null@*/ struct s_refTable {
  int nelements;
  int nspace;
  o_refentry *elements;
} *refTable;

extern /*@only@*/ refTable refTable_create (void) /*@*/ ;
static void refTable_free (/*@only@*/ refTable p_x);



# else
# error "Multiple include"
# endif



