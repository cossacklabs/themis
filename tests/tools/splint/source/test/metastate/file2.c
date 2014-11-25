static void checkOpen (/*@open@*/ /*@null@*/ FILE *);

int main (void)
{
  FILE *fle1 = fopen ("test1", "r");

  if (fle1 == NULL)
    {
      FILE *fle2 = fopen ("test2", "r");
      checkOpen (fle2);
    } /* fle2 not closed */
  
  return 0; /* fle1 not closed */
} 

