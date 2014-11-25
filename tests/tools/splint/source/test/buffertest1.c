void t1 (void)
{
  char *g;
  
  g[100] = 'f';

  {
    g++;
    
    g[0] = '1';  
    g[67] = g[70];
    g[98] = g[99];
    g[90]  = g[3];
  }
}

void t2 (void)
{
  char *g;
  g++;
  g[0] = g[1];
}
