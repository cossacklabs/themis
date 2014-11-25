#include <stdio.h>
#include <stdlib.h>
 
char *foo1(void)
{
  static char buf[1000];
  
  strcpy(buf, "hello");
  return buf;
}

/*@observer@*/ char *foo2(void)
{
  static char buf[1000];
  
  strcpy(buf, "hello");
  return buf;
}

char *f (char outstr[])
{
  return outstr; 
}

char *g (char *outstr)
{
  return outstr; 
}
