/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef enumNameSLIST_H
# define enumNameSLIST_H

abst_typedef enumNameList enumNameSList;

extern int enumNameSList_size (enumNameSList) /*@*/ ;
# define enumNameSList_size(s) enumNameList_size(s)

extern /*@only@*/ enumNameSList enumNameSList_subtract (enumNameList, enumNameSList);
# define enumNameSList_subtract(s,t) enumNameList_subtract(s,t)

extern /*@only@*/ enumNameSList enumNameSList_new(void);
# define enumNameSList_new() enumNameList_new()

extern bool enumNameSList_member (enumNameSList p_s, cstring p_m);
# define enumNameSList_member(s,m) enumNameList_member(s,m)

extern void enumNameSList_addh (enumNameSList p_s, /*@dependent@*/ enumName p_el) ;
/*@-dependenttrans@*/
# define enumNameSList_addh(s,el) enumNameList_addh(s, el)
/*@=dependenttrans@*/

extern void enumNameSList_free (/*@only@*/ enumNameSList p_s);

extern /*@only@*/ cstring enumNameSList_unparse (enumNameSList p_s) /*@*/ ;
# define enumNameSList_unparse(s) enumNameList_unparse(s)

# else
# error "Multiple include"
# endif




