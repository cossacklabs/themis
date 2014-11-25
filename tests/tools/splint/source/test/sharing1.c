/*@-paramuse@*/

/*@only@*/   int *globonly;
/*@shared@*/ int *globshared1;
/*@shared@*/ int *globshared2;
/*@only@*/   int *zonly;

void f(/*@only@*/ int *x, /*@temp@*/ int *y, /*@shared@*/ int *z)
{
  *x = 3;
  if (3 > *x)
    return; /* 1. bad x not released */
} /* 2. bad x not released */

int f2(/*@temp@*/ int *x, /*@only@*/ int *y)
{
  *x = 3;
  *y = 6;
  return 3; /* 3. bad y not released */
}

void f3(/*@only@*/ int *x)
{
  globshared1 = x; /* 4. bad shared globshared1 <- only x */
} /* 5. only not released */

void f4(/*@only@*/ int *x)
{
  zonly = x; /* 6. bad - didn't release zonly */ 
} /* okay */

int g(int *imp) 
{
  int x = 3;
  int *y = malloc(sizeof(int));
  int *y2 = malloc(sizeof(int));
  int *y3 = malloc(sizeof(int));

  if (y2) *y2 = 3;

  f3 (imp); /* 7. bad if +memimplicit --- unqualified as only */
  *imp = 5; /* 8. uses released */

  (void) f(&x,  /* 9, 10. pass immediate as only, only parameter aliased */ 
	   &x, 
	   globshared1);  

  (void) f2 (y3, y3); /* 11-15. --- 2 * null passed as nonnull, 2 * not completely def
                          only parameter y3 aliased */
  *y3 = 6; /* 16, 17. bad --- y3 was released, null */
  (void) f(y, globshared1, globshared1); /* 18, 19. null as non-null, y not completely def */
  (void) f(globshared1, /* 20. pass shared as only */
	   globshared2, 
	   globshared2);

  free (globshared2); /* 21. bad --- free's shared! (pass shared as only) */
  free (globonly);    

  return *y; /* 22-25. y used after release, possible null, 
                       locally allocated y2 not released, 
       		       globonly is released */
} 



