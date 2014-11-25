/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** intTable.h
*/

# ifndef INTTABLE_H
# define INTTABLE_H

/*
** intTable is used to map a limited number of integers to other integers.
** The table size is fixed, and gives the maximum value.
*/

abst_typedef struct _intTable
{
    int size;
    /*@only@*/ int *values;
} *intTable;

extern /*@only@*/ intTable intTable_create (int p_size);
extern void intTable_set (intTable p_h, int p_key, int p_value);
extern int intTable_lookup (intTable p_h, int p_key);
extern void intTable_free (/*@only@*/ intTable p_h);

# else
# error "Multiple include"
# endif 













