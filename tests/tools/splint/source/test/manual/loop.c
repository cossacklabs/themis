extern int glob1, glob2;

extern int f (void) 
  /*@globals glob1@*/ 
  /*@modifies nothing@*/ ;

extern void g (void) 
  /*@modifies glob2@*/ ;

extern void h (void) ; 

void upto (int x) 
{
  while (x > f()) g();
  while (f() < 3) h();
}
