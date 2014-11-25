# include "iter.h"

int test (void)
{
  char c;
  int i;
  int *a;

  intFor(2,5,x,z) /* 2. Unrecognized identifier: x */
    {
      printf("%d\n", z);
    } end_intFor;

  intFor(2,c,x,i) /* 3. Yield parameter shadows local declaration: i 
		     5. Iter intFor expects arg 2 to be int gets char: c */
    {
      ;
    } ; /* 4. Iter intFor not balanced with end_intFor */

  intFor(2, 12, x, i+3) /* 6, 7. Variable i used before definition
			         Yield parameter is not simple identifier: i + 3 */
    {
      ;
    } end_arrayElements; /* 8. Iter intFor not balanced with end_intFor: end_arrayElements */

  arrayElements(1, 10, a, el)
    {
      printf("%d\n", el);
    } end_arrayElements;
  return 3;
}



