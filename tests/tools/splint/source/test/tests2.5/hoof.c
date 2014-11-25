#include <stdio.h>

static void test (long int x);
static void init_arr(/*@out@*/ float[],/*@out@*/ long int*);

int main(void)
{
  int j;
  long i;
  float arr[2];

  init_arr(arr,&j);
  test (j);

  for( i=0; i < j; i++ ) {
    printf("%ld %d: %.6e\n",i,j,arr[i]);
  }
  return(0);
}

void init_arr(float arr[],
	      long int *j)
{
  long i;
  
  *j = 2L;
  for (i=0; i < 2L; i++) {
    arr[i] = (float)(i+1);
  }
}
