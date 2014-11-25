void f (char *fname)
{
  FILE *f;

  if (fname != NULL)
    {
      f = fopen (fname, "r");
      if (f != NULL) {
	(void) fclose (f);
      }
    }
}
