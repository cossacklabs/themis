typedef /*@integraltype@*/ a_int;
typedef /*@unsignedintegraltype@*/ u_int;
typedef /*@signedintegraltype@*/ s_int;

int f (a_int a, u_int u, s_int s)
{
  unsigned long ul;
  long l;
  
  printf ("the values are: %d %ld %ud", a, u, s); /* expect 3 errors */
  ul = a; /* Assignment of a_int to unsigned long int: ul = a */

  /*@+longunsignedintegral@*/
  ul = a;

  /*@+longintegral@*/
  ul = a;

  if (u == a) printf ("yo!");

  /*@+matchanyintegral@*/
  if (u == l) printf ("yo!");
  if (u == a) printf ("yo!");
  /*@=matchanyintegral@*/

  /*@+longsignedintegral@*/
  printf ("i'm okay: %ld", s);

  /*@+longunsignedunsignedintegral@*/
  printf ("you're not! %ld", u);
  printf ("you're not! %ud", u);

  return a;
}
