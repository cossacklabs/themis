extern int f (void) 
     /*@warn wusef "f is a very dangerous function!" @*/ ;

extern int g (void) 
     /*@warn bufferoverflow|its4 "g is a very dangerous function!" @*/ ;

extern int h (void)
     /*@warn bufferoverflow|its4|hfuncs@*/ ;

int wfunc ()
{
  return f () + g () + h ();
}
