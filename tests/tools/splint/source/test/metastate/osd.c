void check (int b);

int osd_fileIsReadable (char *f)
{
  FILE *fl = fopen (f, "r");

  if (fl != NULL)
    {
      fclose (fl);
      return true;
    }
  else
    {
      return false;
    }
}
