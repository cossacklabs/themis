typedef /*@null@*/ char *ncp;

void notnullname (ncp *name)
   /*@ensures notnull *name@*/
{
  **name = 'a';  /* ensures.c:6:4: Dereference of possibly null pointer *name: **name */
} /* Possibly null storage *name corresponds to storage listed in */

void nullname (/*@unused@*/ char **name)
   /*@ensures isnull *name@*/
{
  ;
} /* ensures.c:13:2: Non-null storage *name corresponds to storage listed in ensures */

void nullname2 (char **name)
   /*@ensures isnull *name@*/
{
  *name = NULL;
} 

void callname (void)
{
  char **s;

  s = (char **) malloc (sizeof (char *));
  assert (s != NULL);
  *s = NULL;

  notnullname (s); 
  **s = 'a'; /* okay! */

  nullname (s); 
  **s = 'a'; /* ensures.c:33:4: Dereference of null pointer *s: **s */

  free (*s);
  free (s);
}

