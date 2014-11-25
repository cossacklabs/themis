/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** signature.h
*/

# ifndef SIGNATURE_H
# define SIGNATURE_H

# include "code.h"

# ifndef OSD_H
# include "osd.h"
# endif

/*@-declundef@*/
extern void PrintToken(ltoken p_tok);
/*@=declundef@*/

# else
# error "Multiple include"
# endif
