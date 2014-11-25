static void checkOpen (/*@open@*/ /*@null@*/ FILE *);
static void checkClosed (/*@closed@*/ /*@null@*/ FILE *);

int main (void)
{
  FILE *fle1 = fopen ("test1", "r");

  if (3 > 4)
    {
      (void) fclose (fle1); /* possibly null */
    } /* merge inconsistent */
  
  return 0; 
} 

