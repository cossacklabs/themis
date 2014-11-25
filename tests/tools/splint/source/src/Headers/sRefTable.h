/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** sRefTable.h
**
** based on sRefSet.h
*/

# ifndef sRefTABLE_H
# define sRefTABLE_H

typedef /*@owned@*/ sRef ow_sRef;

abst_typedef /*@null@*/ struct 
{
  int entries;
  int nspace;
  /*@reldef@*/ /*@relnull@*/ ow_sRef *elements;
} *sRefTable;

/*@constant int sRefTableBASESIZE; @*/
# define sRefTableBASESIZE HUGEBASESIZE

/*@constant null sRefTable sRefTable_undefined; @*/
# define sRefTable_undefined ((sRefTable) NULL)

extern /*@nullwhentrue@*/ bool sRefTable_isNull (sRefTable p_s) /*@*/ ;
extern /*@nullwhentrue@*/ bool sRefTable_isEmpty (/*@sef@*/ sRefTable p_s) /*@*/ ;
extern /*@unused@*/ /*@falsewhennull@*/ bool 
  sRefTable_isDefined (sRefTable p_s) /*@*/ ;

# define sRefTable_isNull(s) ((s) == sRefTable_undefined)
# define sRefTable_isDefined(s) ((!sRefTable_isNull(s)))

# define sRefTable_isEmpty(s) ((s) == sRefTable_undefined || ((s)->entries == 0))

extern /*@unused@*/ /*@only@*/ cstring sRefTable_unparse (sRefTable p_s) /*@*/ ;
extern void sRefTable_free (/*@only@*/ sRefTable p_s) /*@modifies p_s@*/;
extern void sRefTable_clear (sRefTable p_s) /*@modifies p_s@*/ ;
extern sRefTable 
  sRefTable_add (/*@returned@*/ sRefTable p_s, 
		 /*@special@*/ /*@owned@*/ sRef p_el)
  /*@modifies p_s@*/ ;

# else
# error "Multiple include"
# endif



