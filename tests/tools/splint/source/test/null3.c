# include "bool.h"

typedef /*@null@*/ int *nip;
/*@only@*/ nip gnip;
/*@only@*/ int *gip;

void g(int *y);

/*@truenull@*/ bool ptrpred (nip x)
{
  return (x == NULL);
}

void f3 (/*@only@*/ nip x)
{
  *gnip = 3; /* 1. Possible dereference of null pointer: *gnip */
  *gip = 3;  /* okay */

  if (x) free (x); 
}

/*@only@*/ int *f (nip arg0, nip arg1, nip arg2, /*@only@*/ int *aip)
{
  int *y = (int *) malloc (sizeof (int));
  int *z = (int *) malloc (sizeof (int));

  *arg0 = 3; /* 2. Possible dereference of null pointer: *arg0 */

  if (arg1)
    {
      *arg1 = 7; /* okay */
    }
  else
    {
      free (y); /* 3. Possibly null storage passed as non-null param: y */
      
      *z = 3;   /* 4. Possible dereference of null pointer: *z */
      return z; /* 5. Only storage not released before return: aip */
    }

  /* arg1 is guarded */

  *arg1 = 3; /* okay */
  *arg2 = 5; /* 6. Possible dereference of null pointer: *arg2 */
  *gip = 6;  /* okay */

  if (z) { *z = 3; }

  if (gnip) { free (gnip); } else { ; } /* okay */

  gnip = z;  /* okay */
  *gnip = 3; /* 7. Possible dereference of null pointer: *gnip */
  gip = z;   /* 8, 9. uses z after release, only z not released */
             /* Note: gip is possibly null now  +++ kept*/
  gnip = aip; /* 10. Only storage gnip not released before assignment: gnip = aip */
  *gnip = 3; /* okay */

  if (y)
    {
      return y; /* 11, 12. Returned storage y not completely defined, 
                   Function returns with non-null global gip referencing null */
    }
  else
    {
      return y; /* 13, 14, 15. Possibly null storage returned as non-null: y,
		   Returned storage y not completely defined,
		   Function returns with non-null global gip referencing null */
    }
}

void f2 (void)
{
  *gnip = 3; /* 16. Possible dereference of null pointer: *gnip */
  *gip = 3;  /* okay */
}




