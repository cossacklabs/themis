extern void checkUntainted (/*@untainted@*/ char *s) ;
extern void checkTainted (/*@tainted@*/ char *s) ;

void test (/*@tainted@*/ char *def)
{
  checkTainted (def);
  checkUntainted (def); /* error */
}
