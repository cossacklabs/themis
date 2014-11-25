int f (/*@null@*/ int *x)
{
  while (3 > 4)
    {
      if (x == NULL) continue; 
      *x = 3;
    }

  *x = 3;
  return 5;
}
