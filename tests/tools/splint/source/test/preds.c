# include "bool.h"

int f (bool b1, bool b2, char c, int *p)
{
  if (!p)
    {
      return 3;
    }

  if (*p)
    {
      return 4;
    }

  if (!(*p))
    {
      return 4;
    }

  if (b1 = b2)
    {
      return 5;
    }
  
  if ((c = 'a'))
    {
      return 5;
    }  

  if (b1 == b2)
    {
      return 5;
    }
  
  if (c)
    {
      return 2;
    }

  return 12;
}








