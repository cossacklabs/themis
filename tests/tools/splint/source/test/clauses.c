int f(/*@only@*/ int *x, /*@only@*/ int *y, /*@only@*/ int *z, /*@only@*/ int *z2)
{
  if (3 > 4)
    {
      free (x);
    } /* 1. Variable x is released in true branch, but live in continuation. */

  while (3 < 4)
    {
      free (y);
    } /* 2. Variable y is released in while body, but live if loop is not taken. */

  if (3 > 4)
    {
      free (z);
    }
  else
    {
      free (z2);
    } /* 3. Variable z2 is released in false branch, but live in true branch. */
      /* 4. Variable z is released in true branch, but live in false branch. */
  return 3;
}

