void fn (FILE *f, char *fmt)
{
  printf (fmt); 
  printf ("%s", fmt);
  fprintf (f, fmt);
  fprintf (f, fmt, fmt);
}
