# include "../bool.h"

int f (bool a, bool b)
{
  if (a == b) /* 1. */
    {
      return 3;
    }
  else if ((a == b) /* 2. */
	   == 
	   (a != b)) /* 3. */
    {
      return 5;
    }
  else if ((a == FALSE) == 
	   (b == TRUE)) /* 4. */
    {
      return 7;
    }
}
