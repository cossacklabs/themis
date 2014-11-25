/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** sigNodeSet.h
**
** based on set_template.h
*/

# ifndef SIGNODESET_H
# define SIGNODESET_H

typedef /*@owned@*/ sigNode o_sigNode;

abst_typedef /*@null@*/ struct 
{
  int entries;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ o_sigNode *elements;
} *sigNodeSet ;

/*@iter sigNodeSet_elements (sef sigNodeSet s, yield exposed sigNode el); @*/
# define sigNodeSet_elements(x, m_el) \
   { if (sigNodeSet_isDefined (x)) { \
       int m_ind; \
       for (m_ind = 0 ; m_ind < (x)->entries; m_ind++) \
          { sigNode m_el = (x)->elements[m_ind];

# define end_sigNodeSet_elements }}}

/*@constant null sigNodeSet sigNodeSet_undefined; @*/
# define sigNodeSet_undefined ((sigNodeSet) 0)

extern /*@falsewhennull@*/ bool sigNodeSet_isDefined (sigNodeSet p_s) /*@*/ ;
# define sigNodeSet_isDefined(s) \
  ((s) != sigNodeSet_undefined)

extern /*@nullwhentrue@*/ bool sigNodeSet_isUndefined (sigNodeSet p_s) /*@*/ ;
# define sigNodeSet_isUndefined(s) \
  ((s) == sigNodeSet_undefined)

extern bool sigNodeSet_isEmpty (/*@sef@*/ sigNodeSet p_s) /*@*/ ;
# define sigNodeSet_isEmpty(s) \
  (sigNodeSet_isUndefined(s) || (s)->entries == 0)

extern int sigNodeSet_size (/*@sef@*/ sigNodeSet p_s) /*@*/ ;
# define sigNodeSet_size(s) (sigNodeSet_isDefined (s) ? (s)->entries : 0) 

extern /*@only@*/ sigNodeSet sigNodeSet_new(void) /*@*/ ;
extern /*@only@*/ sigNodeSet sigNodeSet_singleton (/*@owned@*/ sigNode p_el) /*@*/ ;
extern bool sigNodeSet_insert (sigNodeSet p_s, /*@owned@*/ sigNode p_el) /*@modifies p_s@*/ ;
extern /*@only@*/ cstring sigNodeSet_unparse (sigNodeSet p_s) /*@*/ ;
extern /*@only@*/ cstring sigNodeSet_unparsePossibleAritys (sigNodeSet p_s) /*@*/ ;
extern void sigNodeSet_free (/*@only@*/ sigNodeSet p_s);
extern /*@only@*/ cstring sigNodeSet_unparseSomeSigs (sigNodeSet p_s) /*@*/ ;

/*@constant int sigNodeSetBASESIZE;@*/
# define sigNodeSetBASESIZE MIDBASESIZE

# else
# error "Multiple include"
# endif
