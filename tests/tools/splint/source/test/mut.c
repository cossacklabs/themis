# include "mut.h"

void mut_mod (mut a)
{
  *a = 3;
}

mut mut_create (void)
{
  int *x = (int *) malloc(sizeof(int));

  if (x == NULL)
    {
      exit (EXIT_FAILURE);
    }
  *x = 3;    
  return x;
}
