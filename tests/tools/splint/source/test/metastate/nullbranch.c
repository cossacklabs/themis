void ftest (char *fname)
{
  FILE *f;

  f = fopen (fname, "w");

  if (f == NULL)
    {
      ;
    }
  else
    {
      fprintf (f, "okay\n");
      (void) fclose (f);
    }
}

void ftest2 (char *fname)
{
  FILE *f;

  f = fopen (fname, "w");

  if (f != NULL)
    {
      ;
    }
  else
    {
      (void) fclose (f); /* Possibly null */
    } 
} /* Lost open reference */


void ftest3 (char *fname)
{
  FILE *f;

  f = fopen (fname, "w");

  if (f != NULL)
    {
      fprintf (f, "okay\n");
      (void) fclose (f);
    }
  else
    {

    }
}
