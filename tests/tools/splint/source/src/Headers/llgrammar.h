# ifndef LLGRAMMAR_H
# define LLGRAMMAR_H

# include "llgrammar_gen.h"

/*@-redecl@*/ /*@-declundef@*/
extern void ylerror (char *p_s) /*@modifies *g_warningstream@*/ ;
/*@=redecl@*/ /*@=declundef@*/

# else
# error "Multiple includes"
# endif
