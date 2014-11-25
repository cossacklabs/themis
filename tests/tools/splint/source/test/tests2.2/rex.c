#include <stdio.h>

struct rx_hash
{
  struct rx_hash *parent;
  struct rx_hash *children[13];
};

typedef struct rx_hash regex_t;

void regcomp (/*@out@*/ regex_t *re);

int main (int, char **)
{
  regex_t re;

  regcomp (&re);

  return 2;
}

int f (void)
{
  /*@-varuse@*/ regex_t re; /*@=varuse@*/
  return 3;
}

