extern FILE *f1;

FILE *f (FILE *fin, FILE *fout)
     /*@ensures closed fin@*/
{
  FILE *res;
  int x;

  x = fgetc (fin);

  if (x > 65)
    {
      (void) fclose (fout);
    } /* merge incompatible */

  if (x > 65)
    {
      ;
    }
  else
    {
      (void) fclose (fin);
    } /* merge incompatible */

  return res;
}
