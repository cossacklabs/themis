typedef /*@untainted@*/ char *ucharp_t;
extern void checkUntainted (ucharp_t *s) ;
typedef /*@tainted@*/ char *tcharp_t;
extern void checkTainted (tcharp_t *s) ;

void test (/*@tainted@*/ char *def)
{
  checkTainted (&def); /* okay */
  checkUntainted (&def); /* error */
}
