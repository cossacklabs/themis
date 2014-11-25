extern int glob;

/*@function int fg(void) globals glob;@*/
# define fg() (glob + 1)

/*@function int fm(void) globals glob; modifies glob;@*/
# define fm() (glob++)

int f (void) /*@globals undef glob@*/ ;

int f (void) /*@globals undef glob@*/
{
  return glob;
}
# if 0
int f2 (void) /*@globals int glob;@*/
{
  return glob;
}
# endif
