# include "iter2.h"

int test (void)
{
  char c;
  int i;
  int *a;

  intFor(2,5,x,z) /* 1. unrecognized identifier: x */
    {
      printf("%d\n", z);
    } end_intFor;

  intFor(2,c,x,i) /* 2, 3, 5. yield param shadows local, variable i declared but not used, type mismatch c */
    {
      ;
    } ; /* 4. not balanced with end_intFor */

  intFor(2,12,x,i+3) /* 6, 7. use i before def, yield param not simple */
    {
      ;
    } end_arrayElements; /* 8. not balanced with end_intFor */

  arrayElements(1, 10, a, el)
    {
      printf("%d\n", el);
    } end_arrayElements;
  return 3;
}



