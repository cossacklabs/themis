/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** flag_codes.h
*/

# ifndef FLAGCODES_H
# define FLAGCODES_H

/*
** attempt to keep order consistent with that in flags.c 
*/

typedef enum  
{ 
  MODENAME_FLAG = -3,
  SKIP_FLAG = -2,
  INVALID_FLAG = -1,
# include "flag_codes.gen"
  LAST_FLAG
} flagcode;

/*@constant flagcode NUMFLAGS; @*/
# define NUMFLAGS       (LAST_FLAG)

/*@constant int NUMVALUEFLAGS; @*/
# define NUMVALUEFLAGS       15

/*@constant int NUMSTRINGFLAGS; @*/
# define NUMSTRINGFLAGS      28

/*@iter allFlagCodes (yield flagcode f); @*/
# define allFlagCodes(m_code) \
  { /*@+enumint@*/ flagcode m_code; for (m_code = 0; m_code < NUMFLAGS; m_code++) \
      /*@=enumint@*/ { 

# define end_allFlagCodes }}

# else
# error "Multiple include"
# endif
