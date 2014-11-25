extern char *mtainted (char *s); 

/*@untainted@*/ char *f (/*@tainted@*/ char *s, /*@untainted@*/ char *us)
{
  char *x = f (us, s); /* Error: tainted as untainted */
  return f (x, us);
}

void test (/*@tainted@*/ char *s)
{
  char *t = malloc (sizeof (char) * strlen (s));
  (void) system (s); /* error */

  assert (t != NULL);
  strcpy (t, s);
  /* t is tainted too */
  (void) system (t); /* error */

  t = mtainted (s); /* default return is tainted! */
  (void) system (t); /* error */
}
