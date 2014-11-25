typedef struct
{
  /*@only@*/ /*@open@*/ FILE *file; 
} *source;

void source_badClose (source s)
{
  // free (s->file);
  (void) fclose (s->file);
} /* error - scope exits with file closed */
