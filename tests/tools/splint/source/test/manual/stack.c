int *glob;

/*@dependent@*/
int *f (int **x)
{
  int sa[2] = { 0, 1 };
  int loc = 3;

  glob = &loc;
  *x = &sa[0];

  return &loc; 
}

