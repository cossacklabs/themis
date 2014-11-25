char *sp (/*@untainted@*/ char *s1, /*@untainted@*/ char *s2, /*@tainted@*/ char *s3)
{
  char *s;
  s = (char *) malloc (sizeof (*s) * 100);
  assert (s != NULL);

  sprintf (s, "%s %d %s %d", s1, 3, s2, 3);
  (void) system (s); /* okay */

  sprintf (s, "%s %d %s %d", s1, 3, s3, 3);
  (void) system (s); /* error */

  sprintf (s, "%s %d %s %d %s %s %s", s1, 3, s2, 3, s2, s3, s2);
  (void) system (s); /* error */

  return s; /* error */
}
  
