void ftest (char *fname)
{
  char *s;

  s = malloc (sizeof (*s) * 20);
  
  if (s == NULL)
    {
      ;
    }
  else
    {
      *s = '4';
      free (s);
    }
}
