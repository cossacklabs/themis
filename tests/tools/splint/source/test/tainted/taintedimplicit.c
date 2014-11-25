char *taintme (char *s)
     /*@ensures result:taintedness = s:taintedness@*/
{
  char *res = (char *) malloc (sizeof (*res) * strlen (s));
  assert (res != NULL);
  strcpy (res, s);
  return res;
}

void safecall (/*@untainted@*/ char *s)
{
  (void) system (taintme (s)); /* okay */
}

void dangerouscall (/*@tainted@*/ char *s)
{
  (void) system (taintme (s)); /* error */
}
