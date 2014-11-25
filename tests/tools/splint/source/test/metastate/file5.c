static void checkOpen (/*@open@*/ /*@null@*/ FILE *);
static void checkClosed (/*@closed@*/ /*@null@*/ FILE *);

void passOpen (/*@open@*/ FILE *f)
     /*@ensures closed f@*/
{
  (void) fputc (3, f);
} /* error: ensures not satisfied, didn't close */

/*@dependent@*/ FILE *returnOpen (char *s)
     /*@ensures open result@*/
{
  FILE *fle = fopen (s, "r");
  assert (fle != NULL);
  (void) fclose (fle);
  return fle; /* error: ensures not satisfied */
} 

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
  (void) fclose (fle); /* error: already closed */ 

  return 0; 
} 

