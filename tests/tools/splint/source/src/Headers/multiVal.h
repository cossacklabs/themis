/*
** multiVal.h
*/

# ifndef MULTIVAL_H
# define MULTIVAL_H

typedef enum { MVLONG, MVCHAR, MVDOUBLE, MVSTRING } mvkind;

typedef /*@null@*/ struct
{
  mvkind   kind;
  union
  {
    long int     ival;
    char         cval;
    double       fval;
    /*@only@*/ cstring sval;
  } value;
} *multiVal;

extern /*@falsewhennull@*/ bool multiVal_isDefined (multiVal p_m) /*@*/ ;
extern /*@nullwhentrue@*/ bool multiVal_isUndefined (multiVal p_m) /*@*/ ;
extern /*@nullwhentrue@*/ bool multiVal_isUnknown (multiVal p_m) /*@*/ ;

extern multiVal multiVal_add (multiVal p_m1, multiVal p_m2) /*@*/ ;
extern multiVal multiVal_subtract (multiVal p_m1, multiVal p_m2) /*@*/ ;
extern multiVal multiVal_multiply (multiVal p_m1, multiVal p_m2) /*@*/ ;
extern multiVal multiVal_divide (multiVal p_m1, multiVal p_m2) /*@*/ ;

/*@constant null multiVal multiVal_undefined; @*/
# define multiVal_undefined      (multiVal)NULL
# define multiVal_isDefined(m)   ((m) != multiVal_undefined)
# define multiVal_isUndefined(m) ((m) == multiVal_undefined)
# define multiVal_isUnknown(m)   ((m) == multiVal_undefined)

extern /*@observer@*/ /*@dependent@*/ cstring multiVal_forceString (multiVal p_m) /*@*/ ;
extern /*@unused@*/ double multiVal_forceDouble (multiVal p_m) /*@*/ ;
extern char multiVal_forceChar (multiVal p_m) /*@*/ ;
extern long multiVal_forceInt (multiVal p_m) /*@*/ ;

extern /*@only@*/ multiVal multiVal_makeString (/*@only@*/ cstring p_s) /*@*/ ;
extern /*@only@*/ multiVal multiVal_makeDouble (double p_x) /*@*/ ;
extern /*@only@*/ multiVal multiVal_makeChar (char p_x) /*@*/ ;
extern /*@only@*/ multiVal multiVal_makeInt (long p_x) /*@*/ ;
extern /*@only@*/ multiVal multiVal_unknown (void) /*@*/ ;

extern /*@only@*/ multiVal multiVal_copy (multiVal p_m) /*@*/ ;
extern void multiVal_free (/*@only@*/ multiVal p_m);

extern multiVal multiVal_invert (multiVal p_m) /*@*/ ;

extern /*@falsewhennull@*/ bool multiVal_isInt (multiVal p_m) /*@*/ ;
extern /*@falsewhennull@*/ bool multiVal_isChar (multiVal p_m) /*@*/ ;
extern /*@falsewhennull@*/ bool multiVal_isDouble (multiVal p_m) /*@*/ ;
extern /*@falsewhennull@*/ bool multiVal_isString (multiVal p_m) /*@*/ ;

extern /*@only@*/ multiVal multiVal_undump (char **p_s) /*@modifies *p_s;@*/ ;
extern /*@only@*/ cstring multiVal_dump (multiVal p_m) /*@*/ ;

extern /*@only@*/ cstring multiVal_unparse (multiVal p_m) /*@*/ ;
extern int multiVal_compare (multiVal p_m1, multiVal p_m2) /*@*/ ;

extern bool multiVal_equiv (multiVal p_m1, multiVal p_m2) /*@*/ ;
# define multiVal_equiv(m1,m2) ((multiVal_compare(m1, m2)) == 0)

# else
# error "Multiple include"
# endif
