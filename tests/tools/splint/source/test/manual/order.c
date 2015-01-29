extern int glob;

extern int mystery (void);

extern int modglob (void)
   /*@globals glob@*/
   /*@modifies glob@*/ ;
 
int f (int x, int y[])
{
  int i = x++ * x;
 
  y[i] = i++;
  i += modglob() * glob;
  i += mystery() * glob;
  return i;
}
