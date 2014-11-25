/*
** qualList.h (from slist_templace.h)
*/

# ifndef QUALLIST_H
# define QUALLIST_H

abst_typedef /*@null@*/ struct 
{
  int nelements;
  int free;
  /*@reldef@*/ /*@relnull@*/ qual *elements;
} *qualList ;

extern /*@falsewhennull@*/ bool qualList_isDefined (qualList p_s);
extern /*@unused@*/ /*@nullwhentrue@*/ bool qualList_isUndefined (qualList p_s);

/*@constant null qualList qualList_undefined; @*/
# define qualList_undefined ((qualList) NULL)

# define qualList_isDefined(q)   ((q) != qualList_undefined)
# define qualList_isUndefined(q) ((q) == qualList_undefined)

/*@iter qualList_elements (sef qualList x, yield qual el); @*/
# define qualList_elements(x, m_el) \
   { if (qualList_isDefined(x)) { \
       int m_ind; qual *m_elements = &((x)->elements[0]); \
       for (m_ind = 0; m_ind < (x)->nelements; m_ind++) \
         { qual m_el = *(m_elements++); 

# define end_qualList_elements }}}

extern int qualList_size (/*@sef@*/ qualList p_s);
# define qualList_size(s) (qualList_isDefined(s) ? (s)->nelements : 0)

extern bool qualList_isEmpty (/*@sef@*/ qualList p_s);
# define qualList_isEmpty(s) (qualList_size(s) == 0)

extern qualList qualList_new (void) /*@*/ ;

extern qualList qualList_single (qual p_el) /*@*/ ;

extern qualList qualList_add (/*@returned@*/ qualList p_s, qual p_el) 
   /*@modifies p_s@*/;

extern /*@only@*/ cstring qualList_unparse (qualList p_s) /*@*/ ;
extern void qualList_free (/*@only@*/ qualList p_s) ;
extern qualList qualList_appendList (/*@returned@*/ qualList p_s, qualList p_t);

extern qualList qualList_copy (qualList p_s);
extern /*@only@*/ cstring qualList_toCComments (qualList p_s);

extern void qualList_clear (qualList p_q);

/*@constant int qualListBASESIZE;@*/
# define qualListBASESIZE 8

extern bool qualList_hasAliasQualifier (qualList p_s);
extern bool qualList_hasExposureQualifier (qualList p_s);

/* start modifications */
extern bool qualList_hasNullTerminatedQualifier(qualList p_s);

extern bool qualList_hasBufQualifiers(qualList p_s);

# define qualList_hasBufQualifiers(p_s) \
  (qualList_hasNullTerminatedQualifier(p_s))

/* end modification/s */

# else
# error "Multiple include"
# endif




