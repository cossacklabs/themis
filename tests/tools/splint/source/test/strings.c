/*
** checks read-only strings
*/

char f1 (char *x)
{
  return *x;
}

char f2 (char *x) /*@modifies *x;@*/
{
  *x = 'a';
  return *x;
}

void f3 (/*@only@*/ char *x)
{
  free (x);
}

void main (void)
{
  (void) f1 ("hullo");
  (void) f2 ("hullo");
  f3 ("hullo");
}
