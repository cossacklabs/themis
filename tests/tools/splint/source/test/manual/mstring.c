static /*@notnull@*/ char *mstring_createNew (int x);

char *mstring_space (void)
{
  char *m = mstring_createNew (2);
  *m = ' '; 
  *(m + 1) = '\0';  
  return m;
}
