void
foo(void)
{
  size_t                      n;
  char                        *p;

  for (p = ""; (p != (char*) 0 ) && (*p); )
    {
      switch (*p)
	{
	case '9':
	  for (n = 0; n < 3; ++n) ;
	  p += n;
	}
    }
}

/*
        % lclint -nof -boolops -branchstate foo11.c
*/
