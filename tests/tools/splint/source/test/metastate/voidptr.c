void *direalloc (/*@out@*/ /*@null@*/ void *x, size_t size, 
		 char *name, int line)
{
  void *ret;

  if (x == NULL)
    {				       
      ret = (void *) malloc (size);
    }
  else
    {
      ret = (void *) realloc (x, size);
    }

  return ret;
}
