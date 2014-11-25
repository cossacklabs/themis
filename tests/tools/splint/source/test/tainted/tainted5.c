typedef /*@untainted@*/ char *ucharp_t;
extern void checkUntainted (ucharp_t *s) ;
typedef /*@tainted@*/ char *tcharp_t;
extern void checkTainted (tcharp_t *s) ;

void test (/*@untainted@*/ char *def) 
{
  checkUntainted (&def); 
  checkTainted (&def); /* okay (untainted as tainted) */
}

void test2 (ucharp_t *def)  
{
  checkUntainted (def); 
  checkTainted (def); /* okay */
}
