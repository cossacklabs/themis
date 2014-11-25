extern char *mtainted (char *s); 

void test (/*@untainted@*/ char *s)
{
  (void) system (mtainted (s));
}
