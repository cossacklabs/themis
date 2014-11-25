/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** shift.h
*/

extern void LSLGenShift (ltoken p_tok);
extern void LSLGenShiftOnly (/*@only@*/ ltoken p_tok);

extern /*@only@*/ ltoken LSLGenTopPopShiftStack(void);

extern void LSLGenInit(bool p_LSLParse);
