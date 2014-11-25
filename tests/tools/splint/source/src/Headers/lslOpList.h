/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef lslOpLIST_H
# define lslOpLIST_H

typedef /*@exposed@*/ lslOp e_lslOp;

abst_typedef struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@only@*/ /*@relnull@*/ e_lslOp *elements;
} *lslOpList ;

extern /*@only@*/ lslOpList lslOpList_new(void);
extern void lslOpList_add (lslOpList p_s, /*@exposed@*/ lslOp p_el) ;

extern /*@unused@*/ /*@only@*/ cstring lslOpList_unparse (lslOpList p_s) ;
extern void lslOpList_free (/*@only@*/ lslOpList p_s) ;

/*@constant int lslOpListBASESIZE;@*/
# define lslOpListBASESIZE SMALLBASESIZE
# else
# error "Multiple include"
# endif




