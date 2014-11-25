/*dkf5k*/
#include <stdlib.h>
#include <stdio.h>
 
int main()
{ 
  char buffer[12] = "hello world";
  int i;
  printf("Enter the character you want: ");
  scanf("%i", &i);
  if ( i >= sizeof(buffer))
    {
      fprintf(stderr, "Error: %i is greater than or equal to the range which is %i", i, sizeof(buffer));
      exit(0);
    }
  printf("You asked for character %i which is %c", i, buffer[i]);
  return 0;
}
