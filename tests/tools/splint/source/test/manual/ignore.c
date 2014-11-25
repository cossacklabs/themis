extern int fi (void);
extern bool fb (void);
extern int /*@alt void@*/ 
  fv (void);

int ignore (void)
{
  fi ();                
  (void) fi ();
  fb ();
  fv ();
  return fv ();
}
