/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** typeIdSet.h
**
** based on set_template.h
*/

# ifndef typeIdSET_H
# define typeIdSET_H

/* in forwardTypes.h: immut_typedef int typeIdSet;  */

extern typeIdSet typeIdSet_emptySet (void) 
   /*@modifies internalState@*/ ;

extern bool typeIdSet_member (typeIdSet p_t, typeId p_el) /*@*/ ;
extern bool typeIdSet_isEmpty (/*@sef@*/ typeIdSet p_t) /*@*/ ;

extern typeIdSet typeIdSet_single (typeId p_t)
   /*@modifies internalState@*/ ;

extern typeIdSet typeIdSet_singleOpt (typeId p_t)
   /*@modifies internalState@*/ ;

extern typeIdSet typeIdSet_insert (typeIdSet p_t, typeId p_el) 
   /*@modifies internalState@*/ ;

extern typeIdSet typeIdSet_removeFresh (typeIdSet p_t, typeId p_el) 
  /*@modifies internalState@*/ ;

extern cstring typeIdSet_unparse (typeIdSet p_t) /*@*/ ;

extern typeIdSet typeIdSet_subtract (typeIdSet p_s, typeIdSet p_t) 
   /*@modifies internalState*/ ;

extern int typeIdSet_compare (typeIdSet p_t1, typeIdSet p_t2);
extern cstring typeIdSet_dump (typeIdSet p_t);
extern typeIdSet typeIdSet_undump (char **p_s) 
   /*@modifies internalState, *p_s@*/ ;
extern typeIdSet typeIdSet_union (typeIdSet p_t1, typeIdSet p_t2) /*@*/ ; 

extern void typeIdSet_initMod (void) /*@modifies internalState@*/ ;
extern void typeIdSet_destroyMod (void) /*@modifies internalState@*/ ;

extern void typeIdSet_dumpTable (FILE *p_fout) /*@modifies *p_fout@*/ ;
extern void 
  typeIdSet_loadTable (FILE *p_fin) /*@modifies *p_fin, internalState@*/ ;

/*@constant typeIdSet typeIdSet_undefined;@*/
# define typeIdSet_undefined 0

/*@constant typeIdSet typeIdSet_empty;@*/
# define typeIdSet_empty 0

# else
# error "Multiple include"
# endif

