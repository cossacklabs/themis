/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** rcfiles.h
*/

# ifndef RCFILES_H
# define RCFILES_H

extern bool rcfiles_read (cstring p_fname,
			  cstringList *p_passThroughArgs,
			  bool p_report) 
  /*@modifies fileSystem, internalState, *p_passThroughArgs@*/ ;
   
# endif
