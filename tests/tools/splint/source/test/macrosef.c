int f(/*@sef@*/ int x, int y);
# define f(x,y) ((x) + (x) + (y) + (y)) /* 1. */

int g (int *p)
{
  *p++;
  return *p;
}

int m (int *p)
{
  int x = f ((printf ("yo\n"), 3), g(p));

  return (f (g (p), x));
}
