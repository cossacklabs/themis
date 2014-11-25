/*@only@*/ /*@null@*/ int *f(void)
{
  int *x = NULL;
  int *z;

  if (3 > 4)
    {
      int *y1 = malloc (sizeof (int));
      int *y2 = malloc (sizeof (int));

      if (y1 != NULL)
	{
	  *y1 = 3;
	}

      if (y2 != NULL)
	{
	  *y2 = 3;
	}

      x = y1;
      z = y2;
    }

  return x;
}
