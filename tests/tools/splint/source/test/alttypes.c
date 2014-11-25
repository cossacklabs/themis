# include "bool.h"

int /*@alt void@*/ f (void) ;

int f (void)
{
  return 3;
}

int g (void)
{
  f ();

  return 4;
}

int /*@alt void@*/ f2 (void) ;

int /*@alt bool@*/ f2 (void) 
{
  return 3;
}

extern int /*@alt char, bool@*/ x;
int /*@alt char@*/ x;
