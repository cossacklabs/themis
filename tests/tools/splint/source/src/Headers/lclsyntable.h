/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** lclsyntable.h
*/

# ifndef LCLSYNTABLE_H
# define LCLSYNTABLE_H

extern void LCLAddSyn(lsymbol, lsymbol);
extern /*@exposed@*/ ltoken LCLGetTokenForSyn(lsymbol);
extern bool LCLIsSyn(lsymbol);

extern void LCLSynTableInit(void);
extern void LCLSynTableReset(void);
extern void LCLSynTableCleanup(void);

# else
# error "Multiple include"
# endif
