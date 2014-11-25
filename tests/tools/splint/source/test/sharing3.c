extern /*@only@*/ char *string_copyext (char *s) ;

void f (void)
{
  char *s;

  if (3 < 4)
    {
      s = string_copyext ("asdf");
      free (s);
    }
}


/*@only@*/ char *string_copy (char *s) 
{ 
  return s; /* 1. returns temp as only! */
}

/*@only@*/ char *copy_string1 (char *s)
{
  return string_copy (s); /* okay */
}

/*@only@*/ char *copy_string2 (char *s)
{
  return string_copyext (s); /* okay */
}

void string_free1 (char *s)
{
  free (s); /* 2. unqualified as only */
}

void string_free2 (/*@only@*/ char *s)
{
  free (s);
}

void string_free3 (/*@only@*/ char *s)
{
  char *t = string_copy (s);
  string_free2 (s);
  *t = 'a';
} /* 3. bad, t not released */

void string_free4 (/*@only@*/ char *s)
{
  char *t;
  int i;

  for (i = 0; i < 3; i++)
    {
      t = string_copy (s);
      *t = 'a';
      free (t);
    }

  free (s);
} /* okay */

