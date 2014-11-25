/*@notfunction@*/
# define NUMTYPES int /*@alt unsigned int, char, float, double@*/
extern NUMTYPES square (NUMTYPES x);
 
int f (int x, char c, float f, double d, char *s)
{
  x = square (x);
  c = square (c);
  square (f);
  square (d);
  square (s);

  return (square (d));
}
