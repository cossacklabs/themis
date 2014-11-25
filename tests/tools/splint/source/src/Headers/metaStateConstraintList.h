/*
** metaStateConstraintList.h (from slist_templace.h)
*/

# ifndef metaStateConstraintLIST_H
# define metaStateConstraintLIST_H

typedef /*@observer@*/ metaStateConstraint b_metaStateConstraint;

struct s_metaStateConstraintList 
{
  int nelements;
  int free;
  /*@reldef@*/ /*@relnull@*/ b_metaStateConstraint *elements;
} ;

extern /*@unused@*/ /*@nullwhentrue@*/ bool
  metaStateConstraintList_isUndefined (metaStateConstraintList p_f) /*@*/ ;
extern /*@falsewhennull@*/ bool metaStateConstraintList_isDefined (metaStateConstraintList p_f) /*@*/ ;

/*@constant null metaStateConstraintList metaStateConstraintList_undefined; @*/
# define metaStateConstraintList_undefined (NULL)
# define metaStateConstraintList_isDefined(f)   ((f) != metaStateConstraintList_undefined)
# define metaStateConstraintList_isUndefined(f) ((f) == metaStateConstraintList_undefined)

/*@iter metaStateConstraintList_elements (sef metaStateConstraintList x, yield exposed metaStateConstraint el); @*/
# define metaStateConstraintList_elements(x, m_el) \
   { if (metaStateConstraintList_isDefined (x)) { \
      int m_ind; metaStateConstraint *m_elements = &((x)->elements[0]); \
      for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
         { metaStateConstraint m_el = *(m_elements++); 

# define end_metaStateConstraintList_elements }}}

extern int metaStateConstraintList_size (/*@sef@*/ metaStateConstraintList p_s) /*@*/ ;
# define metaStateConstraintList_size(s)    (metaStateConstraintList_isDefined (s) ? (s)->nelements : 0)

extern bool metaStateConstraintList_isEmpty (/*@sef@*/ metaStateConstraintList p_s);
# define metaStateConstraintList_isEmpty(s) (metaStateConstraintList_size(s) == 0)

extern metaStateConstraintList 
  metaStateConstraintList_append (/*@returned@*/ metaStateConstraintList p_s, /*@only@*/ metaStateConstraintList p_t);

extern /*@observer@*/ metaStateConstraint metaStateConstraintList_getFirst (metaStateConstraintList p_s) /*@*/ ;

extern /*@only@*/ metaStateConstraintList metaStateConstraintList_new (void) /*@*/ ;

extern metaStateConstraintList 
metaStateConstraintList_add (/*@returned@*/ metaStateConstraintList p_s, /*@observer@*/ metaStateConstraint p_el)
     /*@modifies p_s@*/ ;

extern metaStateConstraintList 
metaStateConstraintList_single (/*@observer@*/ metaStateConstraint p_el) /*@*/ ;

extern /*@unused@*/ /*@only@*/ cstring metaStateConstraintList_unparse (metaStateConstraintList p_s) ;
extern void metaStateConstraintList_free (/*@only@*/ metaStateConstraintList p_s) ;

/*@constant int metaStateConstraintListBASESIZE;@*/
# define metaStateConstraintListBASESIZE MIDBASESIZE

# endif




