/** fixed, 8 july 1997 */

/* test file for lclint */

# include <stdlib.h>

static void func1 (char *x)
{
  char *s = x;
  int i1;
  float f;
  
  s += 5;
  s += 'c'; /* bad types */
  i1 += f; /* i1 used before def, f used before def */
}

static void func2()
{
  int i1;
  int i2;
  
  i1 = i2; /* i2 used before def */
}

int main ()
{
  func1 (NULL); /* null passed */
  func2 ();
  return (EXIT_SUCCESS);
}




