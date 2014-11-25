#include <stdio.h>
#include <stdlib.h>

int fn(int arr[/*7*/][10]) /* better without the 7 */
{
  return 0;
}

int fn2(int arr[10][]) /* bad */
{
  return 0;
}

int fn3(int arr[][]) /* bad */
{
  return 0;
}

int fn4(int arr[][10][7][4]) 
{
  return 0;
}

int fn5(int arr[][10][][4]) /* bad */
{
  return 0;
}

int main()
{
  int array[10][]; /* incomplete type bad */
  int array2[][10]; /* array size missing */
  return fn(array);
}
