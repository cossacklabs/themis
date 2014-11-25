void test (char *s1, int x);
void test3 (int, char *s1);
void test2 (/*@only@*/ char *s1, int x);
char *copystring (char *s);

int f (/*@only@*/ char *name) /*@modifies *name;@*/
{
  test (name, f(name));  /* 1. Variable name used after being released */
  test3 (f(name), name); /* 2. Variable name used after being released */
  test2 (copystring(name), f(name)); /* 3. Argument 2 modifies *name, used by ... */
  return 3;
}
