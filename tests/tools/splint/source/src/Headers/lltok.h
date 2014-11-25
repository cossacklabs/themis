/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** lltok.h
*/

# ifndef LLTOK_H
# define LLTOK_H

typedef /*@abstract@*/ struct
{
  int tok;
  fileloc loc;
} *lltok;

extern lltok lltok_create  (int p_tok, /*@only@*/ fileloc p_loc) /*@modifies nothing@*/ ;
extern /*@observer@*/ cstring lltok_unparse (lltok p_tok) /*@*/ ;
extern void lltok_free (/*@only@*/ lltok p_t) ;
extern void lltok_free2 (/*@only@*/ lltok p_t1, /*@only@*/ lltok p_t2) ;
extern void lltok_free3 (/*@only@*/ lltok p_t1, /*@only@*/ lltok p_t2, /*@only@*/ lltok p_t3) ;

extern /*@dependent@*/ /*@exposed@*/ fileloc lltok_getLoc (lltok p_t) /*@*/ ;
extern int lltok_getTok (lltok p_t) /*@*/ ;

# define lltok_getTok(t)  ((t)->tok)
# define lltok_getLoc(t)  ((t)->loc)

extern bool lltok_isSemi (lltok p_tok);

/* DRL added 10/23/2000 for boolean stuff */
extern bool lltok_isEqOp (lltok p_tok);
extern bool lltok_isNotEqOp (lltok p_tok);

extern bool lltok_isMult (lltok p_tok);

extern bool lltok_isIncOp (lltok p_tok);

extern bool lltok_isAndOp (lltok p_tok);

extern bool lltok_isOrOp (lltok p_tok);

extern bool lltok_isNotOp (lltok p_tok);

/*drl7x added this function 11/20/00 */

extern bool lltok_isLt_Op (lltok p_tok);
extern bool lltok_isGt_Op (lltok p_tok);
extern bool lltok_isGe_Op (lltok p_tok);
extern bool lltok_isLe_Op (lltok p_tok);

/* end drl7x added */


/*drl7x added 11 30 2000*/
bool lltok_isPlus_Op (lltok p_tok);

bool lltok_isMinus_Op (lltok p_tok);

/*end drl added */

/*drl
  added 1/14/2001
*/
bool lltok_isDecOp  (lltok p_tok);
bool lltok_isAmpersand_Op (lltok p_tok);

extern bool lltok_isExcl_Op (lltok p_tok);
extern bool lltok_isTilde_Op (lltok p_tok);

extern bool lltok_isEnsures (lltok p_tok);
extern bool lltok_isRequires (lltok p_tok);

# else
# error "Multiple include"
# endif
