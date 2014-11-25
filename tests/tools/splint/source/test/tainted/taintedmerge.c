char *f (/*@tainted@*/ char *s)
{
  char t[50];

  (void) system ("test"); 
  strcpy (t, "test");

  (void) system (t);

  strcat (t, s);
  (void) system (t); /* error */

  strcpy (t, s);
  (void) system (t); /* error */

  return t; /* error - tainted, stack-allocated */
}
