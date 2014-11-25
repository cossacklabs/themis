int *glob;

void stack1 (int **x)
{
  int sa[3] = { 0, 1, 2 } ;
  int loc = 3;

  glob = &loc;
  *x = &sa[0];
} /* 1. Stack-allocated storage *x reachable from parameter x,
     2. Stack-allocated storage glob reachable from global glob
  */

/*@dependent@*/ int *f (int c)
{
  int x = 3;
  
  if (c == 0)
    {
      return &x; /* 3. Stack-allocated storage &x reachable from return value */ 
    }
  else
    {
      int sa[10];

      sa[0] = 35;
      sa[2] = 37;      

      if (c == 1)
	{
	  return sa; /* 4. Stack-allocated storage sa reachable ... */
	}
      else
	{
	  return &sa[0]; /* 5. Stack-allocated storage sa reachable ... */
	}
    }
}


