extern void f (/*@out@*/ char *s);
extern void g (/*@special@*/ char *s) /*@allocates s@*/ ;

void t (/*@only@*/ char *s1, /*@only@*/ char *s2)
{
  free (s1);
  f (s1);

  free (s2);
  g (s2);
}
