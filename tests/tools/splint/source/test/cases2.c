extern /*@only@*/ int *g(/*@only@*/ int *y);

/*@null@*/ /*@only@*/ int *f(/*@only@*/ int *x)
{
  switch (*x)
    {
    case 1:
      return g(x);
    case 2:
      return g(x);
    case 3:
      return g(x);
    default:
      return g(x);
    }
}

/*@null@*/ /*@only@*/ int *f2(/*@only@*/ int *x)
{
  switch (*x)
    {
    case 1:
      return g(x);
    case 2:
      return g(x);
    } /* 1. Variable x is released in one possible execution, but live ... */

  return g(x);
}

/*@null@*/ /*@only@*/ int *f3(/*@only@*/ int *x)
{
  switch (*x)
    {
    case 1:
      return g(x);
    } 

  return g(x);
}
