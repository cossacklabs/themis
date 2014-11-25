/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** qtype.h
**
** Qualified types: a type qualifier list, and a ctype.
*/

# ifndef QTYPEH
# define QTYPEH

abst_typedef /*@null@*/ struct
{
  ctype    type;
  qualList quals;
} *qtype;

/*@constant null qtype qtype_undefined;@*/
# define qtype_undefined ((qtype) NULL)

extern /*@nullwhentrue@*/ bool qtype_isUndefined (qtype p_q);
# define qtype_isUndefined(q) ((q) == qtype_undefined)

extern /*@falsewhennull@*/ bool qtype_isDefined (qtype p_q);
# define qtype_isDefined(q)   ((q) != qtype_undefined)

extern ctype qtype_getType (/*@sef@*/ qtype p_q);
# define qtype_getType(q) \
     (qtype_isDefined(q) ? (q)->type : ctype_unknown)

extern /*@exposed@*/ qualList qtype_getQuals (/*@sef@*/ qtype p_q);
# define qtype_getQuals(q) \
  (qtype_isDefined(q) ? (q)->quals : qualList_undefined)

extern void qtype_setType (/*@sef@*/ qtype p_q, ctype p_c);
# define qtype_setType(q, c) (qtype_isDefined(q) ? (q)->type = (c) : (c))

extern qtype qtype_addQualList (/*@returned@*/ qtype p_qt, qualList p_ql);
extern qtype qtype_mergeImplicitAlt (/*@returned@*/ qtype p_q1, /*@only@*/ qtype p_q2);
extern /*@only@*/ qtype qtype_copy (qtype p_q);

extern /*@notnull@*/ qtype qtype_create (ctype p_c) /*@*/ ;
extern /*@only@*/ qtype qtype_unknown(void) /*@*/ ;
extern qtype qtype_addQual (/*@returned@*/ qtype p_qt, qual p_q);
extern qtype qtype_combine (/*@returned@*/ qtype p_q1, ctype p_ct);
extern qtype qtype_mergeAlt (/*@returned@*/ qtype p_q1, /*@only@*/ qtype p_q2);

extern qtype qtype_resolve (/*@returned@*/ qtype p_q);
extern void qtype_adjustPointers (/*@only@*/ pointers p_n, /*@returned@*/ qtype p_q);
extern /*@only@*/ cstring qtype_unparse (qtype p_q) /*@*/ ;
extern qtype qtype_newBase (/*@returned@*/ qtype p_q, ctype p_ct);
extern qtype qtype_newQbase (/*@returned@*/ qtype p_q1, qtype p_q2);
extern void qtype_free (/*@only@*/ qtype p_q);

# else
# error "Multiple include"
# endif
