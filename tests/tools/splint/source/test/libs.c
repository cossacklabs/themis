/*
** test of standard library
*/

int compare (int x, int y)
{
  x = y;
  return (3);
}

char *compare2 (int x, int y)
{
  x = y;
  return "ok";
}

void
leave (int i)
{
  exit ("hullo"); /* 1. Function exit expects arg 1 to be int gets char * */
  exit (i);       /* 2. Unreachable code */
} 

void
print (char *s, FILE *f)
{
  char c;

  fprintf (f, s);
  printf(s);
  fprintf (stderr, s);

  c = fgetc(f); /* 3. Assignment of int to char: c = fgetc(f) */
  c = getc (f); /* 4. Assignment of int to char: c = getc(f) */
}

int
main (void)
{
  unsigned int x;

  x = NULL;
  
  /*@-null@*/ /* suppress errors for passing NULL's */
  /*@-noeffect@*/
  (void) bsearch (NULL, NULL, sizeof(int), compare) ;  /* 5, 6  */
  (void) bsearch (NULL, NULL, sizeof(int), sizeof(int), (int (*) ()) compare) ; /* ok */
  bsearch (NULL, NULL, sizeof(int), sizeof(int), (char (*) ()) compare2) ; /* 7, 8 */
  /*@=noeffect@*/

  qsort (NULL, x, x, (int (*)()) compare);
  qsort (x, x, x, (char (*)()) compare2); /* 9, 10. */

  signal (SIGHUP, compare); /* 11. */
  signal (SIGHUP, leave);

  return 23;
}


