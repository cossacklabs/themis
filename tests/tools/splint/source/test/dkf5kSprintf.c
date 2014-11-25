/*dkf5k*/
#include <stdlib.h>

 
int main()
{ 
  char na[1000];
  sprintf(na, "%s",  "hello world");
  printf("%s\n", &(na[0]));
  return 0;
}
