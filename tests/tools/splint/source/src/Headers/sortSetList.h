/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
# ifndef sortSetLIST_H
# define sortSetLIST_H

typedef /*@dependent@*/ sortSet o_sortSet;

abst_typedef struct
{
  int nelements;
  int free;
  int current;
  /*@reldef@*/ /*@only@*/ /*@relnull@*/ o_sortSet  *elements;
} *sortSetList ;

/*@iter sortSetList_elements (sef sortSetList x, yield exposed sortSet el); @*/
# define sortSetList_elements(x, m_el) \
   { int m_ind; sortSet *m_elements = &((x)->elements[0]); \
       for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
	 { sortSet m_el = *(m_elements++); 

# define end_sortSetList_elements }}

extern int sortSetList_size (sortSetList);

# define sortSetList_size(s) ((s)->nelements)   

extern /*@only@*/ sortSetList sortSetList_new(void);
extern void sortSetList_addh (sortSetList p_s, /*@dependent@*/ /*@exposed@*/ sortSet p_el) ;

extern void sortSetList_reset (sortSetList p_s) ;   
extern void sortSetList_advance (sortSetList p_s) ; /* was "list_pointToNext" */

extern /*@unused@*/ /*@only@*/ cstring sortSetList_unparse (sortSetList p_s) ;
extern void sortSetList_free (/*@only@*/ sortSetList p_s) ;

extern /*@observer@*/ sortSet sortSetList_head (sortSetList p_s) ;
extern /*@observer@*/ sortSet sortSetList_current (sortSetList p_s) ;

/*@constant int sortSetListBASESIZE;@*/
# define sortSetListBASESIZE (8)

# else
# error "Multiple include"
# endif




