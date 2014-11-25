/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef RANDOMNUMBERS_H
# define RANDOMNUMBERS_H

/*@constant int NUM_RANDOM=256@*/
# define NUM_RANDOM 256

extern unsigned int g_randomNumbers[NUM_RANDOM];

# else
# error "Multiple include"
# endif






