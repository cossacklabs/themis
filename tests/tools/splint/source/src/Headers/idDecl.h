/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** idDecl.h
*/

# ifndef idDecl_H
# define idDecl_H

struct s_idDecl
{
  cstring id;
  qtype   typ;
  functionClauseList clauses;
} ; 

/*@constant null idDecl idDecl_undefined; @*/
# define idDecl_undefined ((idDecl) NULL)

extern /*@falsewhennull@*/ bool idDecl_isDefined (idDecl p_t) /*@*/ ;
# define idDecl_isDefined(t) ((t) != idDecl_undefined)

extern void idDecl_free (/*@only@*/ idDecl p_t);
extern /*@only@*/ idDecl 
   idDecl_create (/*@only@*/ cstring p_s, /*@only@*/ qtype p_t);

extern /*@only@*/ idDecl 
  idDecl_createClauses (/*@only@*/ cstring p_s, /*@only@*/ qtype p_t,
			/*@only@*/ functionClauseList p_clauses);

extern /*@only@*/ cstring idDecl_unparse (idDecl p_d) /*@*/ ;
extern /*@only@*/ cstring idDecl_unparseC (idDecl p_d) /*@*/ ;
extern /*@exposed@*/ qtype idDecl_getTyp (idDecl p_d) /*@*/ ;
extern void idDecl_setTyp (idDecl p_d, /*@only@*/ qtype p_c) /*@modifies p_d@*/ ;
extern idDecl idDecl_expectFunction (/*@returned@*/ idDecl p_d) /*@*/ ;
extern void idDecl_notExpectingFunction (idDecl p_d) /*@modifies p_d@*/ ;
extern idDecl idDecl_replaceCtype (/*@returned@*/ idDecl p_d, ctype p_c) /*@modifies p_d@*/ ;
extern idDecl idDecl_fixBase (/*@returned@*/ idDecl p_t, qtype p_b) /*@modifies p_t@*/ ;
extern idDecl idDecl_fixParamBase (/*@returned@*/ idDecl p_t, qtype p_b) /*@modifies p_t@*/ ;

extern void idDecl_addClauses (idDecl p_d, /*@only@*/ functionClauseList) /*@modifies p_d@*/ ;

extern ctype idDecl_getCtype (idDecl p_d) /*@*/ ;
extern /*@exposed@*/ qualList idDecl_getQuals (idDecl p_d) /*@*/ ; 
extern /*@exposed@*/ functionClauseList idDecl_getClauses (idDecl p_d) /*@*/ ;

extern /*@dependent@*/ /*@observer@*/ cstring idDecl_getName (idDecl p_t) /*@*/ ;
extern /*@observer@*/ cstring idDecl_observeId (idDecl p_d) /*@*/ ;
extern void idDecl_addQual (idDecl p_d, qual p_q);

# define idDecl_getName(t) idDecl_observeId(t)

# else
# error "Multiple include"
# endif


