extern int f (void) 
     /*@warn bufferoverflow|statemerge "g is a very dangerous function!" @*/ ;

int wflags ()
{
  return f ();
}
