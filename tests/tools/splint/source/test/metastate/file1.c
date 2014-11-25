static void checkOpen (/*@open@*/ /*@null@*/ FILE *);
static void checkClosed (/*@closed@*/ /*@null@*/ FILE *);

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

  checkClosed (fle); /* error */
  checkOpen (fle); /* okay */

  (void) fclose (fle);
  checkOpen (fle); /* error */
  checkClosed (fle); /* okay */

  return 0; 
} 

