/*@constant int MaxLength=20@*/
# define MaxLength 20

void foo (char *str) /*@requires maxSet(str) >= MaxLength@*/ 
{
  str[20] = 'a';
}

void foo2 (char *str) /*@requires maxSet(str) >= (MaxLength - 1)@*/ 
{
  str[20] = 'a'; /* error */
}

void foo3 ()
{
  char buf[MaxLength];

  buf[0] = '\0';

  foo (buf); /* error: off by 1 */
  foo2 (buf); /* okay */
}
