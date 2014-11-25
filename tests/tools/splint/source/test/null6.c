# include "bool.h"

typedef /*@null@*/ int *mnull;

extern /*@notnull@*/ mnull mnull_create (void);

extern int f1 (/*@notnull@*/ mnull x); /* 1. Function f1 declared with notnull ... */

int f (mnull x)
{
  return *x; /* 2. Possible dereference of null pointer: *x */
}

static /*@unused@*/ int f2 (/*@notnull@*/ mnull x)
{
  return *x;
}

extern /*@falsenull@*/ bool isThree (mnull x);

static /*@unused@*/ int f3 (/*@notnull@*/ mnull x)
{
  if (isThree (x)) /* the parameter was missing before 2.4! */
    {
      *x = 4;
    }
  else
    {
      *x = 5;
    }

  return (*x);
}

/*@notnull@*/ mnull f4 (void)
{
  mnull x = NULL;

  if (x == NULL)
    {
      x = mnull_create ();
    } 

  return x;
}

/*@notnull@*/ mnull f5 (void)
{
  static /*@only@*/ mnull x = NULL;

  if (x == NULL)
    {
      x = mnull_create ();
    } 

  return x;
}

/*@notnull@*/ mnull f6 (void)
{
  static /*@only@*/ mnull x = NULL;

  if (x != NULL)
    {
      x = mnull_create ();
    } 

  return x; /* 3. Possibly null storage returned as non-null */
}

/*@notnull@*/ mnull f7 (void)
{
  static /*@only@*/ mnull x = NULL;

  if (x == NULL)
    {
      x = mnull_create ();
    } 
  else
    {
      x = NULL;
    }

  return x; /* 4. Possibly null storage returned as non-null */
}
