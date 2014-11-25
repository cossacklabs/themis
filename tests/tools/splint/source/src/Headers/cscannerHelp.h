/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** cscannerHelp.h
*/

# ifndef CSCANNERHELP_H
# define CSCANNERHELP_H

extern void cscannerHelp_swallowMacro (void) ;
extern bool cscannerHelp_processMacro (void) ;
extern void cscannerHelp_handleMacro (void) ;

extern void cscannerHelp_exitSpecPart (void) /*@modifies internalState@*/ ;

extern void cscannerHelp_setContinueLine (void) /*@modifies internalState@*/ ;

extern int cscannerHelp_returnToken (int p_t) 
   /*@modifies g_currentloc, internalState@*/ ;

extern int cscannerHelp_returnTokenLength (int p_t, int p_length) 
   /*@modifies g_currentloc, internalState@*/ ;

extern int cscannerHelp_returnString (/*@only@*/ cstring p_s) 
   /*@modifies g_currentloc, internalState@*/ ;

extern int cscannerHelp_returnInt (ctype p_ct, long p_val)
   /*@modifies g_currentloc, internalState@*/ ;

extern int cscannerHelp_returnFloat (ctype p_ct, double p_f)
   /*@modifies g_currentloc, internalState@*/ ;

extern int cscannerHelp_returnChar (char p_c)
   /*@modifies g_currentloc, internalState@*/ ;

extern int cscannerHelp_returnType (int p_tok, ctype p_ct) 
   /*@modifies g_currentloc, internalState@*/ ;

extern int cscannerHelp_returnExpr (/*@only@*/ exprNode p_e)
   /*@modifies g_currentloc, internalState@*/ ;

extern void cscannerHelp_setTokLength (int) /*@modifies g_currentloc, internalState@*/ ;
extern void cscannerHelp_setTokLengthT (size_t) /*@modifies g_currentloc, internalState@*/ ;
extern void cscannerHelp_advanceLine (void) /*@modifies g_currentloc, internalState@*/ ;

extern /*@observer@*/ cstring cscannerHelp_observeLastIdentifier (void) ;
extern int cscannerHelp_handleLlSpecial (void) /*@modifies g_currentloc, internalState@*/ ; 
extern bool cscannerHelp_handleSpecial (char *) /*@modifies g_currentloc, internalState@*/ ;
extern /*@only@*/ cstring cscannerHelp_makeIdentifier (char *);

extern bool cscannerHelp_isConstraintToken (int p_tok) /*@*/ ;

extern int cscannerHelp_handleNewLine (void) /*@modifies g_currentloc, internalState@*/ ;

extern int cscannerHelp_processTextIdentifier (char *) /*@modifies internalState@*/ ;
extern int cscannerHelp_processIdentifier (/*@only@*/ cstring) /*@modifies internalState@*/ ;
extern bool cscannerHelp_processHashIdentifier (/*@only@*/ cstring) /*@modifies internalState@*/ ;

extern int cscannerHelp_processSpec (int p_tok) /*@modifies internalState@*/ ;
extern char cscannerHelp_processChar (void) /*@modifies internalState@*/ ;
extern double cscannerHelp_processFloat (void) /*@modifies internalState@*/ ;
extern /*@only@*/ exprNode cscannerHelp_processString (void)  /*@modifies internalState@*/ ;
extern /*@only@*/ exprNode cscannerHelp_processWideString (void)  /*@modifies internalState@*/ ;
extern long cscannerHelp_processDec (void) /*@modifies internalState@*/ ;
extern long cscannerHelp_processHex (void) /*@modifies internalState@*/ ;
extern long cscannerHelp_processOctal (void) /*@modifies internalState@*/ ;

extern void cscannerHelp_expectingMetaStateName (void) 
   /*@modifies internalState@*/ ;

extern void cscannerHelp_clearExpectingMetaStateName (void)
   /*@modifies internalState@*/ ;

extern /*@observer@*/ uentry cscannerHelp_coerceId (cstring p_cn);
extern /*@observer@*/ uentry cscannerHelp_coerceIterId (cstring p_cn);

extern void cscannerHelp_setExpectingTypeName (void) /*@modifies internalState@*/ ;
extern void cscannerHelp_clearExpectingTypeName (void) /*@modifies internalState@*/ ;
extern bool cscannerHelp_isExpectingTypeName (void) /*@globals internalState@*/ ;

extern int cscannerHelp_ninput (void) /*@modifies internalState, g_currentloc@*/;

# else
# error "Multiple include."
# endif


