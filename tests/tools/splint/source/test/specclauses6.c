/*@-paramuse@*/ 

typedef struct 
{
  int id;
  char *name;
} *record;

void usename (/*@special@*/ char **name)
   /*@uses *name@*/
   /*@requires isnull *name@*/
   /*@ensures notnull *name@*/
{
  **name = 'a'; /* 1. Dereference of null pointer *name: **name */
}

void callname (void)
{
  char **s;

  s = (char **) malloc (sizeof (char *));
  assert (s != NULL);
  *s = (char *) malloc (sizeof (char));
  assert (*s != NULL);

  **s = 'a';

  usename (s); /* 2. Non-null storage *s corresponds to storage listed in ... */
  **s = 'a'; /* 3. null! */
  free (*s);
  free (s);
}

void nullname (/*@special@*/ char **name)
   /*@post:isnull *name@*/
{
  *name = NULL;
}

void nullname2 (/*@special@*/ char **name)
   /*@post:isnull *name@*/
{
  ;
} /* 3. Non-null storage *name corresponds to storage listed in ... */





