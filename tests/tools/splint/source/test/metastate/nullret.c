void *direalloc (/*@out@*/ /*@null@*/ void *x, size_t size, 
		 char *name, int line)
{
  void *ret;

  if (x == NULL)
    {				       
      ret = (void *) dmalloc (size);
    }
  else
    {
      ret = (void *) realloc (x, size);
    }

  if (ret == NULL)
    {
      llfatalerrorLoc
	(message ("Out of memory.  Allocating %w bytes at %s:%d.", 
		  size_toLongUnsigned (size),
		  cstring_fromChars (name), line));
    }
  
  return ret;
}
