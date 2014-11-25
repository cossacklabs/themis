/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** cstringTable.h
*/

# ifndef CSTRINGTABLE_H
# define CSTRINGTABLE_H

/*@constant int HBUCKET_BASESIZE; @*/
# define HBUCKET_BASESIZE 2

/*@constant int HBUCKET_DNE; @*/
# define HBUCKET_DNE NOT_FOUND

/* in forwardTypes:
   abst_typedef null struct _cstringTable *cstringTable;
*/

/*:private:*/ typedef struct 
{
  /*@only@*/ cstring key;
  int val;
} *hentry;

/*:private:*/ typedef /*@only@*/ hentry o_hentry;

typedef /*@null@*/ struct
{
  int size;
  int nspace;
  /*@only@*/ o_hentry *entries;
} *hbucket;

typedef /*@only@*/ hbucket o_hbucket;

struct s_cstringTable
{
  unsigned long size;
  unsigned long nentries;
  /*@only@*/ o_hbucket *buckets;
} ;


/*@constant null cstringTable cstringTable_undefined; @*/
# define cstringTable_undefined      ((cstringTable) NULL)

extern /*@falsewhennull@*/ bool cstringTable_isDefined(cstringTable) /*@*/ ;
# define cstringTable_isDefined(p_h) ((p_h) != cstringTable_undefined)

extern /*@nullwhentrue@*/ /*@unused@*/ bool cstringTable_isUndefined(cstringTable) /*@*/ ;
# define cstringTable_isUndefined(p_h) ((p_h) == cstringTable_undefined)

extern /*@only@*/ cstringTable cstringTable_create(unsigned long p_size) /*@*/ ;
extern void cstringTable_insert (cstringTable p_h, 
				 /*@only@*/ cstring p_key, 
				 int p_value) /*@modifies p_h@*/ ;

extern int cstringTable_lookup (cstringTable p_h, cstring p_key);
extern /*@unused@*/ /*@only@*/ cstring cstringTable_stats(cstringTable p_h);
extern void cstringTable_free (/*@only@*/ cstringTable p_h);
extern void cstringTable_remove (cstringTable p_h, cstring p_key) /*@modifies p_h@*/ ;

extern /*@unused@*/ cstring cstringTable_unparse (cstringTable) /*@*/ ;

extern /*@unused@*/ void cstringTable_update (cstringTable p_h, cstring p_key, int p_newval) /*@modifies p_h@*/ ;

extern void 
  cstringTable_replaceKey (cstringTable p_h, cstring p_oldkey,
			   /*@only@*/ cstring p_newkey);

# else
# error "Multiple include"
# endif 













