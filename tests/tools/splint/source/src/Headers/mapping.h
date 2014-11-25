/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mapping.h
*/

# ifndef MAPPING_H
# define MAPPING_H

typedef struct s_mappair {
  lsymbol domain, range;
  /*@null@*/ struct s_mappair *next;
} mappair;

typedef /*@relnull@*/ /*@only@*/ mappair *o_mappair;

typedef /*@abstract@*/ struct {
  unsigned int count;
  /*@relnull@*/ /*@only@*/ o_mappair *buckets;  
} *mapping;


extern /*@only@*/ mapping mapping_create (void);
extern lsymbol mapping_find (mapping p_t, lsymbol p_domain);
extern void mapping_bind (mapping p_t, lsymbol p_domain, lsymbol p_range);
extern void mapping_free (/*@only@*/ mapping p_m);

# else
# error "Multiple include"
# endif
