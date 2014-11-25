typedef int *abst;

/*@noaccess abst*/
int main (void)
{
  abst a;
  int b = 3;
  abst *ap = (abst *) NULL;
  abst *ap2 = NULL;
  void *vp;
  int *ip;

  vp = ap ; /* 1. Assignment of abst * to void *: vp = ap */
  ip = ap2; /* 2. Assignment of abst * to int *: ip = ap2 */
  ap = ip ; /* 3. Assignment of int * to abst *: ap = ip */
  vp = (void *) ap ; /* 4. Cast from underlying abstract type abst *: (void *)ap */
  a = *ap ; /* 5. Possible dereference of null pointer: *ap */    
  *ap = a ;
  vp = (void *)&a ; /* 6. Cast from underlying abstract type abst *: (void *)&a */
  ap = (abst *)&b ; /* 7. Cast to underlying abstract type abst *: (abst *)&b */
  ap = &b; /* 8. Assignment of int * to abst *: ap = &b */
  *ap = b; /* 9. Assignment of int to abst: *ap = b */
  return 12;
}
