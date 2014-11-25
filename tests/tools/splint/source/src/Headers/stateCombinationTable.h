/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** stateCombinationTable.h
*/

# ifndef STATECOMBINATIONTABLE_H
# define STATECOMBINATIONTABLE_H

/*
** stateCombinationTable is a mapping: int x int -> int x cstring
** The int values must be in a fixed (pre-defined) range, 0, ..., max.
*/

typedef struct {
    int value;
    cstring msg;
} *stateEntry;

typedef /*@only@*/ stateEntry o_stateEntry;

typedef struct
{
    int size;
    /*@only@*/ o_stateEntry *entries;
} *stateRow;

typedef /*@only@*/ stateRow o_stateRow;

abst_typedef struct
{
  int size;
  /*@only@*/ o_stateRow *rows;
} *stateCombinationTable;

extern /*@only@*/ stateCombinationTable stateCombinationTable_create (int p_size);

extern void stateCombinationTable_set (stateCombinationTable p_h, 
				       int p_from, int p_to, 
				       int p_value,
				       /*@only@*/ cstring p_msg);

extern void stateCombinationTable_update (stateCombinationTable p_h, 
					  int p_from, int p_to, 
					  int p_value,
					  /*@only@*/ cstring p_msg);

extern int stateCombinationTable_lookup 
               (stateCombinationTable p_h, int p_from,
		int p_to, /*@out@*/ ob_cstring *p_msg);

extern int stateCombinationTable_lookupLoseReference 
               (stateCombinationTable p_h, int p_from,
		/*@out@*/ /*@observer@*/ cstring *p_msg);

extern void stateCombinationTable_free (/*@only@*/ stateCombinationTable p_t);

extern int stateCombinationTable_size (stateCombinationTable p_h) /*@*/ ;
# define stateCombinationTable_size(h) ((h)->size)

extern cstring stateCombinationTable_unparse (stateCombinationTable) /*@*/ ;

# else
# error "Multiple include"
# endif 













