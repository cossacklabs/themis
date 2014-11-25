extern void nomodcall (int *x) /*@*/;
extern void mysterycall (int *x);

int noeffect (int *x, int y)
{
  y == *x;
  nomodcall (x);  
  mysterycall (x); 
  return *x;
}
