/*
** filelocList.h (from slist_templace.h)
*/

# ifndef filelocLIST_H
# define filelocLIST_H

abst_typedef /*@null@*/ struct
{
  int nelements;
  int free;
  /*@reldef@*/ /*@relnull@*/ o_fileloc *elements;
} *filelocList ;

extern /*@unused@*/ /*@nullwhentrue@*/ bool
  filelocList_isUndefined (filelocList p_f) /*@*/ ;
extern /*@falsewhennull@*/ bool filelocList_isDefined (filelocList p_f);

/*@constant null filelocList filelocList_undefined; @*/
# define filelocList_undefined (NULL)
# define filelocList_isDefined(f)   ((f) != filelocList_undefined)
# define filelocList_isUndefined(f) ((f) == filelocList_undefined)

/*@iter filelocList_elements (sef filelocList x, yield exposed fileloc el); @*/
# define filelocList_elements(x, m_el) \
   { if (filelocList_isDefined (x)) { \
      int m_ind; fileloc *m_elements = &((x)->elements[0]); \
      for (m_ind = 0 ; m_ind < (x)->nelements; m_ind++) \
         { fileloc m_el = *(m_elements++); 

# define end_filelocList_elements }}}

extern int filelocList_realSize (filelocList p_s) /*@*/ ;

extern int filelocList_size (/*@sef@*/ filelocList p_s) /*@*/ ;
# define filelocList_size(s)    (filelocList_isDefined (s) ? (s)->nelements : 0)

extern bool filelocList_isEmpty (/*@sef@*/ filelocList p_s);
# define filelocList_isEmpty(s) (filelocList_size(s) == 0)

extern filelocList 
  filelocList_append (/*@returned@*/ filelocList p_s, /*@only@*/ filelocList p_t);

extern /*@only@*/ filelocList filelocList_new (void) /*@*/ ;
extern filelocList 
  filelocList_add (/*@returned@*/ filelocList p_s, /*@only@*/ fileloc p_el)
   /*@modifies p_s@*/ ;

extern filelocList 
  filelocList_addDifferentFile (/*@returned@*/ filelocList p_s, 
				fileloc p_where, fileloc p_loc)
   /*@modifies p_s@*/ ;

extern filelocList filelocList_addUndefined (/*@returned@*/ filelocList p_s)
   /*@modifies p_s@*/ ;

extern /*@only@*/ cstring filelocList_unparseUses (filelocList p_s);
extern /*@unused@*/ /*@only@*/ cstring filelocList_unparse (filelocList p_s) ;
extern void filelocList_free (/*@only@*/ filelocList p_s) ;

/*@constant int filelocListBASESIZE;@*/
# define filelocListBASESIZE MIDBASESIZE

# endif




