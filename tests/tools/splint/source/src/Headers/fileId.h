/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** fileId.h
*/

# ifndef fileId_H
# define fileId_H

immut_typedef int fileId;

/*@constant fileId fileId_invalid; @*/
# define fileId_invalid -1

extern bool fileId_isValid (fileId) /*@*/ ;
# define fileId_isValid(f) ((f) > fileId_invalid)

extern bool fileId_isInvalid (fileId) /*@*/ ;
# define fileId_isInvalid(f) ((f) == fileId_invalid)

extern bool fileId_equal (fileId p_t1, fileId p_t2) /*@*/ ;
# define fileId_equal(p_t1, p_t2) (fileTable_sameBase (context_fileTable (),  p_t1, p_t2))
/* evs 2000-07-27: was
# define fileId_equal(t1,t2) ((t1) == (t2))
*/

/* fileId_baseEqual moved to fileTable.h */

extern /*@unused@*/ int 
  fileId_compare (/*@sef@*/ fileId p_t1, /*@sef@*/ fileId p_t2) /*@*/ ;
# define fileId_compare(t1,t2) (int_compare (t1, t2))

# else
# error "Multiple include"
# endif
