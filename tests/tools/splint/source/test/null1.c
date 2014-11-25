# include "bool.h"

void g(/*@temp@*/ int *y);

/*@truenull@*/ bool ptrpred (/*@out@*/ /*@null@*/ int *x)
{
  return (x == NULL); /* new error detected by out undef */
}

/*@only@*/ int *f(/*@null@*/ int *x)
{
  int *y = (int *)malloc (sizeof (int));
  int *z;
  int *z2;
  int *z3;
  int *z4;
  int *z5;

  if (!x) { return x; /* 1. Unqualified storage returned as only: x,
		      ** 2. Possibly null storage returned as non-null: x
		      ** 3. Fresh storage y not released before return 
                      */
	  } 

  z = (int *) malloc (sizeof (int));
  z2 = (int *)malloc (sizeof (int));
  z3 = (int *)malloc (sizeof (int));

  *x = 7; 

  *y = 3; /* 4. Possible dereference of null pointer: *y */
  free (y);

  g(z);   /* 5. Possibly null storage passed as non-null param: z
          ** 6. Passed storage z not completely defined (allocated only): z
          */
  if (z) { *z = 3; } /* okay */

  if (!z) { *z = 3; } /* 7. Possible dereference of null pointer: *z */
  else { *z = 4; }    /* okay */

  z4 = (int *)malloc (sizeof (int));
  if (z4 == NULL) { *z4 = 3; } /* 8. Possible dereference of null pointer: *z4 */
  else { free (z4); } /* [ not any more: 12. Clauses exit... ] */

  if (!(z2 != NULL)) { *z2 = 3; } /* 9. Possible dereference of null pointer: *z2 */

  if (z3 != NULL) { *z3 = 5; /* okay */ }
  else { *z3 = 3; } /* 10. Possible dereference of null pointer: *z3 */
  if (z2) free (z2);
  z2 = (int *)malloc (sizeof (int));
  
  if (z2 != NULL) { *z2 = 5; } ;

  *z2 = 7; /* 11. Possible dereference of null pointer: *z2 */

  z5 = (int *) malloc (sizeof (int));

  if (ptrpred(z5)) { *z5 = 3; }  /* 12. Possible dereference of null pointer: *z5 */
  if (!ptrpred(z5)) { *z5 = 3; } 

  if (ptrpred(z5)) { *z5 = 3; } 
  else { free (z5); } 

  free (z2);

  return z; /* 13. Fresh storage z3 not released */
}



