/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef enumNameLIST_H
# define enumNameLIST_H

typedef cstring enumName ;

extern /*@only@*/ enumName enumName_create (/*@only@*/ cstring p_s) /*@*/ ;
# define enumName_create(s)  (s)

typedef /*@only@*/ enumName o_enumName;

abst_typedef struct
{
  int nelements;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_enumName  *elements;
} *enumNameList ;

/*@iter enumNameList_elements (sef enumNameList x, yield exposed enumName el); @*/
# define enumNameList_elements(x, m_el) \
   { int m_ind; enumName *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { enumName m_el = *(m_elements++); 

# define end_enumNameList_elements }}

extern int enumNameList_size (enumNameList);

# define enumNameList_size(s) ((s)->nelements)   

extern /*@only@*/ enumNameList enumNameList_new(void);
extern bool enumNameList_member (enumNameList p_s, cstring p_m);
extern enumNameList enumNameList_push (/*@returned@*/ enumNameList p_s, /*@only@*/ enumName p_el);
extern void enumNameList_addh (enumNameList p_s, /*@keep@*/ enumName p_el) 
     /*@modifies p_s@*/;

extern /*@only@*/ cstring enumNameList_unparse (enumNameList p_s) /*@*/ ;
extern void enumNameList_free (/*@only@*/ enumNameList p_s) ;

extern bool enumNameList_match (enumNameList p_e1, enumNameList p_e2) /*@*/ ;
extern /*@only@*/ enumNameList enumNameList_single (/*@keep@*/ enumName p_t) /*@*/ ;
extern /*@only@*/ enumNameList 
  enumNameList_subtract (enumNameList p_source, enumNameList p_del) /*@*/ ;
extern /*@only@*/ enumNameList enumNameList_copy (enumNameList p_s) /*@*/ ;
extern /*@only@*/ enumNameList enumNameList_undump(char **p_s);
extern /*@only@*/ cstring enumNameList_dump (enumNameList p_s);
extern /*@only@*/ cstring enumNameList_unparseBrief (enumNameList p_s);

/*@constant int enumNameListBASESIZE;@*/
# define enumNameListBASESIZE SMALLBASESIZE

# else
# error "Multiple include"
# endif




