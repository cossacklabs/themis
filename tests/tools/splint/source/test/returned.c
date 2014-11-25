extern char *returnme (/*@returned@*/ char *s);

/*@only@*/ char *f (void)
{
  char *s = (char *) malloc (sizeof (char));
  *s = 'a';

  if (3 > 4)
    {
      return s;
    }
  else
    {
      return returnme (s);
    }
}
