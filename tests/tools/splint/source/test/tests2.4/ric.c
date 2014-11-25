#include <stdio.h>

static void foo() {printf("%s()\n",__PRETTY_FUNCTION__);}

int main()
{
  if(bar() == 0)
    printf("%s()\n",__PRETTY_FUNCTION__);
  return 0;
}
