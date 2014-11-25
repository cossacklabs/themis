char *f (/*@tainted@*/ char *s)
{
  char t[50];
  char t2[20];

  (void) system ("test"); 
  strcpy (t, "test");
  strcpy (t2, "okay");

  (void) system (t);

  t = strcat3 (t, t2, t2);
  (void) system (t); /* okay */

  t = strcat3 (t, t2, s);
  (void) system (t); /* error */

  t = strcat3 (t, t2, t2);
  (void) system (t); /* error */

  t = strcpy (t, s);
  (void) system (t); /* error */

  return t; /* error - tainted, stack-allocated */
}
