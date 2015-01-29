extern int square (/*@sef@*/ int x);
# define square(x) ((x) *(x))

extern int sumsquares (int x, int y);
# define sumsquares(x,y) (square(x) + square(y))
 
int f (int i)
{
  return square (i++);
}

