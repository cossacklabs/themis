/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** scanline.h
** 
**	The header files
**	    lsl.h
**	    string.h
**	    scan.h
**	must be included prior to including
**	    scanline.h
**	
**  MODIFICATION HISTORY:
**
**  X0.1-1	GAF	90.11.08	Created from LSL version
**  X0.1-2	JPW	91.05.03	Add LSLReportCommentTokens.
**  X0.1-3	JPW	91.06.26	Update copyright notice.
**  3.2   ymtan 92.11.14  conditionalize defn of types, needed for
**                        merging scanline.h and lclscanline.h
**                        Hence added SLASHCHAR (from LSL's scanline.h). 
**  2.4     ymtan  93.03.16  -- adapted original for release 2.4
**  Version Who    yy.mm.dd  -- did what?
*/

# ifndef LCLSCANLINE_H
# define LCLSCANLINE_H

extern void LCLScanLine (char *p_line) /*@modifies internalState, p_line@*/ ;
/*@exposed@*/ ltoken LCLScanEofToken (void) /*@*/ ;
extern void LCLReportEolTokens (bool p_setting) /*@modifies internalState@*/ ;

extern void LCLScanLineInit(void) /*@modifies internalState@*/ ;
extern void LCLScanLineReset(void) /*@modifies internalState@*/ ;
extern void LCLScanLineCleanup(void) /*@modifies internalState@*/ ;

extern charCode LCLScanCharClass (char p_c) /*@*/ ;
extern bool LCLIsEndComment(char p_c) /*@*/ ;

extern void LCLSetCharClass (char p_c, charCode p_cod) 
   /*@modifies internalState@*/ ;

extern void LCLSetEndCommentChar(char p_c, bool p_flag)
   /*@modifies internalState@*/ ;

# else
# error "Multiple include"
# endif





