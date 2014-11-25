extern void free (/*@out@*/ /*@only@*/ void *s);
extern /*@only@*/ char *string_copy (char *s);

void f(/*@only@*/ char *only1, /*@only@*/ char *only2, /*@only@*/ char *only3,
       /*@shared@*/ char *shared)
{
  char *local1, *local2, *local4;
  char **localp;

  local1 = only1;
  *only1 = 'c';      /* okay */
  free (local1);     /* okay --- kills only1 */
  *only1 = 'c';      /* 1. bad --- only1 is dead */

  if (3 > 4)
    {
      local2 = only2;
    }
  else
    {
      local2 = shared;
    } /* 2. Clauses exit with local2 referencing dependant storage in true */

  free (local2);     /* 3. bad --- could free shared2 (may kill only2) */

  localp = malloc(sizeof(char *));
  *localp = only3; /* 4. possible null deref */

  local4 = only3;
  local4 = NULL;  /* okay */

  localp = &only3;   /* 5. new storage not released */
} /* 6. only3 may not be released [[[ only2 ??? ]]] */ 








