/*@null@*/ int *x;

void f1 (void)
{
  int *y = x;

  if (3 > 4)
    {
      ;
    }
  else
    {
      x = NULL;
      if (y != NULL) free (y);
    }
  /* y is unuseable...but no error yet */
}

void f2 (void)
{
  int *y = x;

  if (3 > 4)
    {
      ;
    }
  else
    {
      x = NULL;
      if (y != NULL) free (y);
    }

  *y = 23; /* 1. Variable y used in inconsistent state */
}          /* 2. Dereference of possibly null pointer y: *y */

void f3 (void)
{
  int *y = x;

  if (3 > 4)
    {
      ;
    }
  else
    {
      x = NULL;
      if (y != NULL) free (y);
    }

  y = x;
  *y = 23; /* 3. Dereference of possibly null pointer y: *y */
}



