static void checkOpen (/*@open@*/ /*@null@*/ FILE *);
static void checkClosed (/*@closed@*/ /*@null@*/ FILE *);

int main (void)
{
  FILE *fle = NULL;
  char s[10];

  checkClosed (fle); /* okay */
  checkOpen (fle); /* error */

  fle = fopen ("test", "r");
  checkClosed (fle); /* error */
  checkOpen (fle); /* okay */


  (void) fclose (fle);
  checkOpen (fle); /* error */
  checkClosed (fle); /* okay */

  return 0; /* error: f is not closed */
} 



# if 0

@.S
  (void) fgets (s, 3, fle);
  (void) fclose (fle);
  (void) fgets (s, 3, fle); /* error: f is not open */
  (void) freopen ("test", "r", fle);
  (void) fgets (s, 3, fle);
# endif
