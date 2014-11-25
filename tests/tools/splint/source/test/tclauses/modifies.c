int f (int *x, int *y)
     /*@modifies *x;@*/
     /*@modifies *y;@*/
{
  *x = 3;
  *y = 7;
  return 3;
}
