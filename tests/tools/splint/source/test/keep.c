/*@only@*/ int *oglob;
/*@keep@*/ int *kglob;

void f1 (/*@keep@*/ int *x)
{
  kglob = x;
  return;
}

void f2 (/*@keep@*/ int *x)
{
  free (x); /* 1. Keep storage passed as only param: x */
}

int f3 (/*@keep@*/ int *x)
{
  int *y = malloc (sizeof (int));

  if (y == NULL) return 3; /* 2. Keep storage not transferred before return: x */

  *y = 3;

  f1 (x);
  f1 (y);

  return *x;
}

int f4 (/*@only@*/ int *x)
{
  return (f3 (x)); 
}

void f5 (/*@unused@*/ /*@keep@*/ int *x)
{
  return; /* 3. Keep storage not transferred before return: x */
}

void f6 (/*@keep@*/ int *x)
{
  if (*x > 3)
    {
      f2 (x);
    } /* 4. Variable x is kept in true branch, but live in continuation. */

  f2 (x); /* 5. Kept storage passed as keep: f2 (x) */
}

/*@null@*/ int *f7 (/*@null@*/ /*@keep@*/ int *x)
{
  if (x == NULL)
    {
      ;
    }
  else
    {
      f2 (x);
    } 

  return x; /* 6. Kept storage x returned as implicitly only: x */
}





