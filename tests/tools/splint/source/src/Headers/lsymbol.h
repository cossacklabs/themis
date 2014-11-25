/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

# ifndef LSYMBOL_H
# define LSYMBOL_H

/*
** Offset entries by 1, so 0 is available.
*/

/*@constant null lsymbol lsymbol_undefined; @*/
# define lsymbol_undefined ((lsymbol) 0)

extern bool lsymbol_isDefined (lsymbol p_s);
# define lsymbol_isDefined(s) ((s) != lsymbol_undefined)

extern bool lsymbol_isUndefined (lsymbol p_s);
# define lsymbol_isUndefined(s) ((s) == lsymbol_undefined)

extern lsymbol lsymbol_fromChars (/*@temp@*/ char *) /*@*/ ;
extern lsymbol lsymbol_fromString (/*@temp@*/ cstring p_s) /*@*/ ;
extern /*@exposed@*/ /*@null@*/ char *lsymbol_toChars (lsymbol) /*@*/ ;
extern /*@exposed@*/ char *lsymbol_toCharsSafe (lsymbol) /*@*/ ;
extern /*@observer@*/ cstring lsymbol_toString(lsymbol) /*@*/ ;

extern bool lsymbol_equal (lsymbol p_s1, lsymbol p_s2) /*@*/ ;
# define lsymbol_equal(s1, s2) ((s1) == (s2))

extern /*@unused@*/ void lsymbol_printStats (void);

extern void lsymbol_initMod (void) /*@modifies internalState@*/ ;
extern void lsymbol_destroyMod (void) /*@modifies internalState@*/ ;

# else
# error "Multiple include"
# endif
