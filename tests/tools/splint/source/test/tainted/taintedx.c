extern void checkUntainted (char **s) /*@requires untainted *s@*/;
extern void checkUntainted1 (char **s) /*@requires untainted *s@*/;
extern void checkTainted (char **s) /*@requires tainted *s@*/;
extern void checkTainted1 (char **s) /*@requires tainted *s@*/;

void test2 (char **def) /*@requires untainted *def@*/ 
{
  checkUntainted (def); 
  checkTainted (def); /* error */
}

void test (char *def2) /*@requires untainted def2@*/ 
{
  checkUntainted1 (&def2); 
  checkTainted1 (&def2); /* error */
}

