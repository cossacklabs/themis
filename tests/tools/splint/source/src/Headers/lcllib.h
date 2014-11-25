/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef LCLLIB_H
# define LCLLIB_H

extern void dumpState (cstring p_cfname);
extern void loadState (cstring p_cfname);
extern bool loadStandardState (void);
extern bool lcllib_isSkipHeader (cstring p_sname);

# else
# error "Multiple include"
# endif
