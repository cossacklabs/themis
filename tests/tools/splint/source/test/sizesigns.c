#include <stdlib.h>

int foo (size_t z)
{
  if (z / sizeof(z) == 10) 
    return 1;
  else
    return 0;
}

