# include "bool.h"

void g(int *y);

/*@truenull@*/ bool ptrpred (/*@out@*/ /*@null@*/ int *x)
{
  return (x == NULL);
}

/*@only@*/ int *f(/*@null@*/ int *x1, /*@null@*/ int *x2, 
		  /*@null@*/ int *x3, /*@null@*/ int *x4) 
{
  bool test;

  test = x1 && (*x1 == 3);  /* okay */
  test = !x2 && (*x2 == 3); /* 1. Possible dereference of null pointer: *x2 */
  test = x3 || (*x3 == 3);  /* 2. Possible dereference of null pointer: *x3 */
  test = !x4 || (*x4 == 3);  /* okay */

  test = ptrpred(x1) && (*x1 == 3); /* 3. Possible dereference of null pointer: *x1 */
  test = !ptrpred(x1) && (*x1 == 3); /* okay */

  if (x4 && (*x4 == 3))
    {
      *x4 = 6; /* okay */
    }

  if (!x4 || (*x4 == 6))
    {
      *x4 = 12; /* 4. Possible dereference of null pointer: *x4 */
    }

  if (!x1 || (*x1 == 6))
    {
      return (x3); /* 5, 6. Unqualified storage returned as only: (x3),
		            Possibly null storage returned as non-null: (x3) */
    }

  return (x1); /* 7. Unqualified storage returned as only: (x1) 
		     not: null as non-null (because of || semantics) */
}


