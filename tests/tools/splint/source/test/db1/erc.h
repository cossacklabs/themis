# ifndef ERC_H
# define ERC_H

# include "eref.h"

typedef struct _elem { eref val; struct _elem *next; } ercElem;
typedef ercElem *ercList;
typedef struct { ercList vals; int size; } ercInfo;
typedef ercInfo *erc;
typedef ercList *ercIter;

# include "erc.lh"

# define erc_size(c) ((c)->size)
# define erc_choose(c) ((c->vals)->val)
# define erc_initMod() \
    do { bool_initMod(); employee_initMod();\
         eref_initMod(); } while (FALSE)

# define erc_iterFinal(it) (free(it)) 

# define erc_iterReturn(it, result) \
    do { erc_iterFinal(it); return result; } while (FALSE)

# define for_ercElems(er, it, c)\
    for (er = erc_yield (it = erc_iterStart (c)); \
         !eref_equal (er, erefNIL); \
         er = erc_yield (it))

# endif
