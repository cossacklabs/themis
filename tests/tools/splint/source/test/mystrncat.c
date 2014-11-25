void  mystrncat (/*@unique@*/ /*@returned@*/ char *s1, char *s2, size_t n)
     /*@modifies *s1@*/ /*@requires MaxSet(s1) >= ( MaxRead(s1) + n); @*/
     /*@ensures MaxRead(result) >= (MaxRead(s1) + n); @*/;

     void func(char *str)
{
  char buffer[256];
  char *b;

  b = malloc(256);
  assert(b != NULL);
  mystrncat(buffer, str, sizeof(buffer) - 1);
  mystrncat(b, str, sizeof(buffer) - 1);

  free (b);
  return;
}

