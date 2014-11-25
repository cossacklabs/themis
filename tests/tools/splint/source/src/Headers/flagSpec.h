/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** flagSpec.h
*/

# ifndef flagSpec_H
# define flagSpec_H

/*:private:*/ typedef struct
{
  cstring name;
  flagcode code; 
} *flagSpecItem;

abst_typedef /*@null@*/ struct s_flagSpec *flagSpec;

struct s_flagSpec
{
  flagSpecItem tspec;
  /* combination logic if necessary */
  /*@null@*/ /*@only@*/ flagSpec trest;
} ; 

/*@constant null flagSpec flagSpec_undefined; @*/
# define flagSpec_undefined  ((flagSpec) NULL)

extern /*@falsewhennull@*/ bool flagSpec_isDefined (flagSpec p_e) /*@*/ ;
# define flagSpec_isDefined(e)        ((e) != flagSpec_undefined)

extern /*@only@*/ flagSpec flagSpec_createPlain (/*@only@*/ cstring) ;
extern /*@only@*/ flagSpec 
   flagSpec_createOr (/*@only@*/ cstring, /*@only@*/ flagSpec) ;

extern /*@only@*/ flagSpec flagSpec_copy (flagSpec) /*@*/ ;
extern /*@unused@*/ /*@only@*/ cstring flagSpec_unparse (flagSpec) /*@*/ ;
extern void flagSpec_free (/*@only@*/ flagSpec) ;

extern /*@only@*/ cstring flagSpec_dump (flagSpec) /*@*/ ;
extern /*@only@*/ flagSpec flagSpec_undump (char **p_s) /*@modifies p_s@*/ ;

extern flagcode flagSpec_getDominant (flagSpec) /*@*/ ;
extern flagcode flagSpec_getFirstOn (flagSpec, fileloc) /*@*/ ;

extern bool flagSpec_isOn (flagSpec, fileloc) /*@*/ ;

# else
# error "Multiple include"
# endif




