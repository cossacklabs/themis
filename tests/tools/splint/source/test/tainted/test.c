void test (/*@tainted@*/ char *s)
{
  char *t = malloc (sizeof (char) * strlen (s));
  assert (t != NULL);
  strcpy (t, s);
  /* t is tainted too */
  (void) system (t); /* error */
}
