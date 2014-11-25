extern int glob1;

int f (void) /*@globals glob1@*/
{
  return glob1;
}
	    
int g (void) /*@globals glob2@*/ /*@globals glob1@*/
{
  return f();
}
