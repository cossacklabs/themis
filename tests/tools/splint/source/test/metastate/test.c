static void checkOpen (/*@open@*/ /*@null@*/ FILE *);
static void checkClosed (/*@closed@*/ /*@null@*/ FILE *);

int main (void)
{
  FILE *fle = NULL;

  fle = fopen ("test", "r");
  checkOpen (fle); /* okay */
  checkClosed (fle); /* error */

  return 0; /* error: f is not closed */
} 

