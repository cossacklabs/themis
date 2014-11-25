/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** tokentable.h
*/

extern /*@exposed@*/ ltoken
  LSLInsertToken(ltokenCode, lsymbol, lsymbol, bool) 
  /*@modifies internalState@*/ ;
extern void LSLUpdateToken(ltokenCode, lsymbol, bool)
   /*@modifies internalState@*/ ;
extern /*@exposed@*/ ltoken LSLGetToken(lsymbol) /*@*/ ;
extern /*@exposed@*/ ltoken LSLReserveToken(ltokenCode, char *) 
   /*@modifies internalState@*/ ;

extern void LSLSetTokenHasSyn (lsymbol p_sym, bool p_syn) 
   /*@modifies internalState@*/ ;

extern void ltokenTableInit(void) /*@modifies internalState@*/ ;
extern void ltokenTableCleanup(void) /*@modifies internalState@*/ ;
