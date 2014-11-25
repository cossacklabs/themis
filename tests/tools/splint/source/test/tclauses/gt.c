extern int glob;

int f (void) /*@globals undef glob@*/
{
  return glob;
}
