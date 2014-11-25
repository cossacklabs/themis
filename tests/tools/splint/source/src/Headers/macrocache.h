/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** macrocache.h
*/

# ifndef MACROCACHE_H
# define MACROCACHE_H

typedef struct
{
  fileloc fl;
  cstring def;
  bool defined;
  bool scomment;
} *mce;

typedef /*@only@*/ mce o_mce;

typedef struct
{
  int nspace;
  int entries;
  /*@reldef@*/ /*@only@*/ o_mce *contents;
} *macrocache;

extern void 
  macrocache_processUndefinedElements (macrocache p_m) ; /* modifies externalState */

extern /*@observer@*/ fileloc
  macrocache_processFileElements (macrocache p_m, cstring p_base) ;

extern /*@only@*/ /*@unused@*/ cstring macrocache_unparse (macrocache p_m) /*@*/ ;
extern /*@only@*/ macrocache macrocache_create (void) /*@*/ ;

extern void 
  macrocache_addEntry (macrocache p_s, /*@only@*/ fileloc p_fl, /*@only@*/ cstring p_def);

extern void 
  macrocache_addComment (macrocache p_s, /*@only@*/ fileloc p_fl, /*@only@*/ cstring p_def);

extern void macrocache_free (/*@only@*/ macrocache p_s);
extern void macrocache_finalize (void) /*@modifies internalState@*/ ;

# else
# error "Multiple include"
# endif
