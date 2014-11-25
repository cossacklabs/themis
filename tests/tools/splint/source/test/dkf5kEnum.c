/* dkf5k */
#include <stdlib.h>

 
int main()
{ 

  typedef enum 
    {
      apple,
      pear, 
      grapefruit
    } fruits;
  register int i;

  int array[grapefruit+1];
  array[apple] = 24;
  array[pear] = 10;
  for(i=0; i <= 1; i++)
    {
      printf("%i\n", array[i]);
    }
  return 0;
}







