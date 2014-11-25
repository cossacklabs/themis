# include "bool.h"

extern /*@truenull@*/ bool isnull(/*@null@*/ int *x);
extern /*@falsenull@*/ bool notnull(/*@null@*/ int *x);
extern /*@falsenull@*/ char badnull (/*@null@*/ int *x);
extern /*@falsenull@*/ bool worsenull (/*@null@*/ int *x, int t);

int g1 (/*@null@*/ int *y)
{
  if (isnull (y))
    {
      return 0;
    }

  return *y;
}

int g2 (/*@null@*/ int *y)
{
  if (notnull (y))
    {
      return 0;
    }
  else
    {
      return *y;
    }
}

int g3 (/*@null@*/ int *y)
{
  if (notnull (y))
    {
      return 0;
    }

  return *y;
}
