/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mstring.h
*/

# ifndef MSTRING_H
# define MSTRING_H

/* mstring is a non-abstract string (char *) */

typedef /*@concrete@*/ char *mstring;
typedef /*@observer@*/ char *ob_mstring;
typedef /*@observer@*/ /*@null@*/ char *bn_mstring;

extern /*@unused@*/ /*@notnull@*/ /*@observer@*/ char *
  mstring_safePrint (/*@returned@*/ /*@null@*/ mstring p_s) /*@*/ ;
extern char *mstring_spaces (int p_n) /*@*/ ;
extern char *mstring_concat  (const char *p_s1, const char *p_s2) /*@*/ ;
extern char *mstring_concatFree (/*@only@*/ char *p_s1, /*@only@*/ char *p_s2) /*@modifies *p_s1, *p_s2*/ ;
extern char *mstring_concatFree1 (/*@only@*/ char *p_s1, const char *p_s2);
extern char *mstring_append (/*@only@*/ char *p_s1, char p_c);
extern char *mstring_copy (/*@null@*/ char *p_s1) /*@*/  /*@ensures maxRead(result) == maxRead(p_s1) /\  maxSet(result) == maxSet(p_s1) @*/ ;
extern bool mstring_equalPrefix (const char *p_c1, const char *p_c2) /*@*/ ;
extern bool mstring_equal (/*@null@*/ const char *p_s1, /*@null@*/ const char *p_s2) /*@*/ ;
extern bool mstring_containsChar (/*@unique@*/ const char *p_s, char p_c) /*@*/ ;
extern bool mstring_containsString (/*@unique@*/ const char *p_s, /*@unique@*/ const char *p_c) /*@*/ ;

extern size_t mstring_length (/*@sef@*/ /*@null@*/ char *p_s) /*@*/ ;
# define mstring_length(s) \
  (((s) != NULL) ? strlen (s) : 0)

extern /*@falsewhennull@*/ bool mstring_isDefined (/*@sef@*/ /*@null@*/ char *p_s) /*@*/ ;
# define mstring_isDefined(s) ((s) != NULL)

extern /*@nullwhentrue@*/ bool mstring_isEmpty (/*@sef@*/ /*@null@*/ char *p_s) /*@*/ ;
# define mstring_isEmpty(s) (mstring_length(s) == 0)

extern void mstring_markFree (/*@owned@*/ char *p_s) /*@modifies *p_s;@*/ ;

extern /*@notnull@*/ /*@only@*/ char *mstring_create (size_t p_n) 
   /*@*/
   /*@ensures maxSet(result) == p_n /\ maxRead(result) == 0 @*/ ;

extern /*@notnull@*/ /*@only@*/ char *mstring_createEmpty (void) /*@*/ ;

extern void mstring_free (/*@out@*/ /*@only@*/ /*@null@*/ char *p_s);
# define mstring_free(s) sfree(s)

# define mstring_createEmpty() mstring_create(0)

# else
# error "Multiple include"
# endif





