int *mmod (int *);
int *umod (int *);

int f (int *x)
{
  (void) umod (mmod (umod (x)));
  return *x;
}

int g (int *x) /*@*/
{
  (void) umod (x);
  (void) umod (mmod (umod (x)));
  return *x;
}
