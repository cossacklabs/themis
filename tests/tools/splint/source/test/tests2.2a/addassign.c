int hash (char *name)
{
  int h = 0;
  
  while (*name != '\0') 
    {
      h += *name++; /* error in 2.1b */
    }

  return h % 137;
}
