int f (/*@open@*/ /*@read@*/ FILE *f1, /*@open@*/ /*@write@*/ FILE *f2)
{
  int c;

  c = fgetc (f1);
  c = fgetc (f2); /* error: not readable */
  
  (void) fputc ('a', f1); /* error */
  (void) fseek (f1, 0, SEEK_CUR); 
  (void) fputc ('a', f1); /* okay */
 
  c = fgetc (f1); /* error */
  return c;
}
