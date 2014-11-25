/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef exprNodeList_H
# define exprNodeList_H

typedef /*@only@*/ exprNode o_exprNode;

abst_typedef struct
{
  int nelements;
  int nspace;
  int current;
  /*@reldef@*/ /*@relnull@*/ o_exprNode *elements
  /*:invariant maxUse(elements) = nspace /\ maxDefined(elements) = nelements@*/;
} *exprNodeList;

/*@iter exprNodeList_elements (sef exprNodeList s, yield exposed exprNode el); @*/ 
# define exprNodeList_elements(x, m_el) \
   { int m_ind; exprNode *m_elements = &((x)->elements[0]); \
     for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
       { exprNode m_el = *(m_elements++); 

# define end_exprNodeList_elements }}

extern int exprNodeList_size (exprNodeList p_s) /*@*/ ;
# define exprNodeList_size(s) ((s)->nelements)   

extern bool exprNodeList_isEmpty (exprNodeList p_s) /*@*/ ;
# define exprNodeList_isEmpty(s) (exprNodeList_size(s) == 0)

extern /*@only@*/ exprNodeList exprNodeList_new(void);
extern /*@exposed@*/  exprNode exprNodeList_nth (exprNodeList p_args, int p_n)  /*@*/ ;
extern exprNodeList exprNodeList_push (/*@returned@*/ exprNodeList p_args,
				       /*@only@*/ exprNode p_e) ;
extern /*@only@*/ exprNodeList exprNodeList_singleton (/*@only@*/ exprNode p_e) ;

extern void exprNodeList_addh (exprNodeList p_s, /*@only@*/ exprNode p_el) ;

extern void exprNodeList_reset (exprNodeList p_s) ;   
extern void exprNodeList_advance (exprNodeList p_s) ; /* was "list_pointToNext" */

extern /*@only@*/ cstring exprNodeList_unparse (exprNodeList p_s)  /*@*/ ;
extern void exprNodeList_free (/*@only@*/ exprNodeList p_s) ;
extern void exprNodeList_freeShallow (/*@only@*/ exprNodeList p_s);

extern /*@observer@*/ exprNode exprNodeList_head (exprNodeList p_s) ;
extern /*@observer@*/ exprNode exprNodeList_current (exprNodeList p_s) /*@*/ ;

extern /*@exposed@*/ exprNode exprNodeList_getN (exprNodeList p_s, int p_n) /*@*/ ;

/*@constant int exprNodeListBASESIZE;@*/
# define exprNodeListBASESIZE SMALLBASESIZE

# endif




