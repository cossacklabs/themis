/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** lcltokentable.h
*/

# ifndef LCLTOKENTABLE_H
# define LCLTOKENTABLE_H

extern /*@exposed@*/ ltoken 
  LCLInsertToken (ltokenCode, lsymbol, lsymbol, bool)
  /*@modifies internalState@*/ ;
extern void LCLUpdateToken (ltokenCode, lsymbol, bool) /*@modifies internalState@*/ ;
extern void LCLSetTokenHasSyn(lsymbol, bool) /*@modifies internalState@*/ ;
extern /*@exposed@*/ ltoken LCLGetToken (lsymbol) /*@*/ ;
extern /*@exposed@*/ ltoken LCLReserveToken (ltokenCode, char *) 
                       /*@modifies internalState@*/ ;

extern void LCLTokenTableInit(void);
extern void LCLTokenTableCleanup(void);

# else
# error "Multiple include"
# endif
