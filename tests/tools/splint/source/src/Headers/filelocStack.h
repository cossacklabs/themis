/*
** filelocStack.h (from slist_templace.h)
*/

# ifndef FILELOCSTACK_H
# define FILELOCSTACK_H

abst_typedef /*@null@*/ struct
{
  int nelements;
  int free;
  /*@reldef@*/ /*@relnull@*/ o_fileloc *elements;
} *filelocStack ;

/*@constant null filelocStack filelocStack_undefined; @*/
# define filelocStack_undefined (NULL)

extern /*@falsewhennull@*/ bool filelocStack_isDefined (filelocStack p_f) /*@*/ ;
# define filelocStack_isDefined(f)   ((f) != filelocStack_undefined)

extern int filelocStack_size (/*@sef@*/ filelocStack p_s) /*@*/ ;
# define filelocStack_size(s)    (filelocStack_isDefined (s) ? (s)->nelements : 0)

extern int filelocStack_includeDepth (filelocStack p_s);
extern void filelocStack_printIncludes (filelocStack p_s) /*@modifies g_warningstream@*/ ;

extern void filelocStack_clear (filelocStack p_s) /*@modifies p_s@*/ ;

extern /*@only@*/ filelocStack filelocStack_new (void) /*@*/ ;
extern /*@observer@*/ fileloc filelocStack_nextTop (filelocStack p_s) /*@*/ ;

extern bool
  filelocStack_popPushFile (filelocStack p_s, /*@only@*/ fileloc p_el) 
  /*@modifies p_s@*/ ;

extern /*@unused@*/ /*@only@*/ cstring filelocStack_unparse (filelocStack p_s) /*@*/ ;
extern void filelocStack_free (/*@only@*/ filelocStack p_s) ;

/*@constant int filelocStackBASESIZE;@*/
# define filelocStackBASESIZE MIDBASESIZE

# endif




