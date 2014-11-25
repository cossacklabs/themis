int g;

int f(int a, int b)
{
  int x, y, z, z2, z3, z4, z5, z6;

  x = a; /* okay */
  x = y; /* bad */
  y = b; /* okay */
  z = z; /* bad */
  z = g; /* okay */

  if (z == g)
      z2 = 3;
  else
      z2 = 4;

  x = z2; /* okay */

  if (z == g)
    z3 = 3;
  else
    z4 = 4;

  x = z3; /* bad */
  x = z4; /* bad */

  if (z == g)
    z6 = 4;

  x = z6; /* bad */

  if (z == g)
    {
      int i, j;
      
      i = x;  /* okay */
      i = j;  /* bad */
      z2 = i; /* okay */
      z3 = i; /* okay */
    }
  else
    {
      int i, j;

      {
	int m;

	j = i;  /* bad */
	i = z5; /* bad */
	z3 = j; /* okay */
	m = x;
      }
    }
  
  x = z5; /* bad */
  x = z3; /* okay */

  if (x == z3)
    {
      z4 = 7;
    }

  x = z4; /* < not reported> */
  return 5;
}



