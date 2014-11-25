/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

# ifndef SORTLIST_H
# define SORTLIST_H

abst_typedef struct
{
  int nelements;
  int nspace;
  int current;
  /*@reldef@*/ /*@relnull@*/ sort *elements;
} *sortList ;

extern /*@only@*/ sortList sortList_new (void);
extern void sortList_addh (sortList p_s, sort p_el) ;

extern void sortList_reset (sortList p_s) ;   
extern void sortList_advance (sortList p_s) ; /* was "list_pointToNext" */

extern /*@only@*/ cstring sortList_unparse (sortList p_s) ;
extern void sortList_free (/*@only@*/ sortList p_s) ;

extern sort sortList_current (sortList p_s) ;

/*@constant int sortListBASESIZE;@*/
# define sortListBASESIZE SMALLBASESIZE
# else
# error "Multiple include"
# endif




