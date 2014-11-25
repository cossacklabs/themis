# include "alias4.lh"

mut f(mut a, mut b)
{
  mut c;

  if (1 == 3)
    {
      c = a;
    }
  else
    {
      c = b;
    }
  return c;  /* 2 * bad: may reference parameter a, b */
}

int g (void)
{
  return glob;
}

int *h (void)
{
  if (1 > 3)
    return &glob; /* returns ref to global glob */
  else if (2 > 3)
    return globp; /* return reference to global globp */
  else if (3 > 3)
    return (globs.field); /* returns reference to globs */
  else 
    return (&(globs.val)); /* returns reference to globs */
}

