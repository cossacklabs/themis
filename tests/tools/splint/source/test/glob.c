int x;
int y;
int z;

int useGlob (int a)
{
  a = x + y;
  return (a + x + y) ;
}

int dontuseGlob (int a)
{
  return (x + a);
}

int unspec (int a)
{
  return (y + z + a);
}

int callsUseGlob (int a)
{
  return (useGlob(a));
}




