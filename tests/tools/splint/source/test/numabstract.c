typedef /*@numabstract@*/ int apples;
typedef /*@numabstract@*/ int oranges;

/*@noaccess apples@*/
/*@noaccess oranges@*/

int adding (apples a, oranges o)
{
  int i;
  apples a2;
  a++; /* Okay */
  a2 = 13; /* error (unless +numabstractlit) ? */
  a2 = 'a'; /* error */
  i = 'c'; /* error */
  a2 = (apples) 13; /* warning if +numabstractcast */
  a2 = a + 5; /* okay */
  a2 = o; /* error */
  a2 = a2 - a; /* okay */
  i = o; /* error */
  i = a2 + a; /* error */
  return a + o; /* error */
}

int comparing (apples a, oranges o, apples a2)
{
  if (a < 3) { /* error unless +numabstractlit */
    return 3;
  } 

  if (a < o) { /* error */
    return 5;
  }

  if (a == o) { /* error */
    return 6;
  }

  if (a == a2) { /* okay */
    return 23;
  }

  --a2;

  if (a >= a2) { 
    return 523;
  }

  return 7;
}
