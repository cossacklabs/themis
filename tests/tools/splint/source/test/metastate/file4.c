static void checkOpen (/*@open@*/ /*@null@*/ FILE *);
static void checkClosed (/*@closed@*/ /*@null@*/ FILE *);

void passOpen (/*@open@*/ FILE *f)
{
  (void) fputc (3, f);
} /* okay, still open */

int main (void)
{
  FILE *fle = NULL;
  char s[10];

  checkClosed (fle); /* okay */
  checkOpen (fle); /* error */

  fle = fopen ("test", "r");

  if (fle == NULL) 
    {
      return 0;
    }

  passOpen (fle);
  (void) fclose (fle);

  return 0; 
} 

