#include <stdlib.h>

typedef struct ipp_s *ipp;

struct ipp_s
{
  /*@only@*/ /*@notnull@*/
  int *ip;
};

extern void ipp_delete(/*@special@*/ /*@only@*/ /*@notnull@*/ ipp This) 
  /*@releases *This@*/;

void ipp_delete(ipp This)
{
  free(This);
}
