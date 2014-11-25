extern void 
  setVal (/*@out@*/ int *x);
extern int 
  getVal (/*@in@*/ int *x);
extern int mysteryVal (int *x);

int 
dumbfunc (/*@out@*/ int *x, int i)
{
  if (i > 3) 
    return *x;
  else if (i > 1) 
    return getVal (x);
  else if (i == 0) 
    return mysteryVal (x);
  else
    { 
      setVal (x);
      return *x;
    }     
}
