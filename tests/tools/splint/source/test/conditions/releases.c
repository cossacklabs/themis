#include <stdlib.h>

typedef struct ipp_s *ipp;

struct ipp_s
{
  /*@only@*/ /*@notnull@*/
  int *ip;
} ;

extern void ipp_delete(/*@only@*/ /*@notnull@*/ ipp This) ;

void ipp_delete(ipp This)
{
  free(This); /* should be error for not deleting This->ip! */
}
