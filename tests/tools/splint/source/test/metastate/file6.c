static void checkOpen (/*@open@*/ /*@null@*/ FILE *);
static void checkClosed (/*@closed@*/ /*@null@*/ FILE *);

/*@open@*/ /*@dependent@*/ FILE *newOpen (char *s)
{
  FILE *res;

  res = fopen (s, "r");
  assert (res != NULL);
  return res; /* okay: return open as open */
}

/*@open@*/ /*@dependent@*/ FILE *newOpenBad (char *s)
{
  FILE *res;

  res = fopen (s, "r");
  assert (res != NULL);
  (void) fclose (res);
  return res; /* error: return closed as open */
}

int main (void)
{
  FILE *fle;

  fle = newOpen ("test");
  checkOpen (fle); /* okay */

  return 0;  /* error: fle not closed */
} 

