#include <stdio.h>

typedef int Array[10];

int fixedArrayTouch()
{
  Array buffer;

  buffer[ sizeof(Array)-1] = 1;

  return 0;
}
