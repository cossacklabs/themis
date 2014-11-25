void f (/*@only@*/ char *s)
{
  char *m = realloc (s);

  free (m);
}
