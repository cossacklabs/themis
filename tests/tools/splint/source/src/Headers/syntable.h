/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** syntable.h
*/

extern void LSLAddSyn(lsymbol, lsymbol) /*@modifies internalState@*/ ;
extern /*@exposed@*/ ltoken LSLGetTokenForSyn(lsymbol) 
  /*@globals internalState@*/ ;
extern bool LSLIsSyn(lsymbol) /*@globals internalState@*/ ;
extern void lsynTableInit (void) /*@modifies internalState@*/ ;
extern void lsynTableReset (void)  /*@modifies internalState@*/ ;
extern void lsynTableCleanup (void) /*@modifies internalState@*/ ;
