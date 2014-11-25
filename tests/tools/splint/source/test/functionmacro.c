#include <stdio.h>

void blah()
{
  int len;
  
  len = strlen(__FUNCTION__);
  
  printf("this function (%s) has a name that is %d characters long\n",
	 __FUNCTION__, len);
}

int
main()
{
  printf("hello from %s\n",
	 __FUNCTION__);
  blah();
}
