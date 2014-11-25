/*@null@*/ char * f()
{
  char *buf = malloc(sizeof ("1234") );
  if (buf == NULL)
    {
      return NULL;
    }
  
  strcpy(buf, "1234");
  return buf;
}

