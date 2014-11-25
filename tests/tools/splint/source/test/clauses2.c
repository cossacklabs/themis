extern /*@only@*/ int *new_ip (void);

/*@only@*/ int *f(void)
{
  int *x = new_ip ();
  int *y;

  if (3 > 4)
    {
      y = x;
    }
  else
    {
      free (x);
      y = new_ip ();
    }

  return y;
}
