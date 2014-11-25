int glob;
int *globp;

int f(int *a, int b, int **c)
{
  int *x, *y, *z;

  {
    {
      int *lx;

      x = a;
      lx = a;
      {
	/*@-shadow@*/ int *lx; /*@=shadow@*/
	
	*lx = 3; /* 1. use before def */
	y = *c;
      }
      *lx = 4; /* 2. modifies a */
    }
    x = y; /* x aliases *c */
    x = *c;
  }

  if (b == *a)
    {
      x = a;   /* okay - x alias a */
      *x = 3;  /* 3. modifies *a */
      y = &glob;
    }
  else
    {
      *x = 3; /* 4. modifies **c */
      y = globp;
    }

  *x = 4; /* 5, 6. bad - may modify *a or **c */
  z = a;

  if ((**c = b) == 3) /* 7. modifies **c */
    {
      *y = 3;    /* 8, 9. may modify glob and *globp */
      globp = a; /* 10. modifies globp */
      a = y; 
      z = globp;
    }
  else
    {
      a = z;
    }

  *a = 3;     /* 11, 12. modifies glob, *globp through y and z */
  *z = 6;     /* 13, 14. modifies *a, *globp */
  *globp = 5; /* 15, 16. modifies *globp, *a */
  return 3;   /* 17. leaves globp aliasing a */
}







