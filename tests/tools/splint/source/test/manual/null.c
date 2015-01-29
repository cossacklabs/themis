char firstChar1 (/*@null@*/ char *s)
{
  return *s;
}

char firstChar2 (/*@null@*/ char *s)
{
  if (s == NULL) return '\0';
  return *s;
}
