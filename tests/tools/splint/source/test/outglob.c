int x1, x2, x3;

int g()
{
  return x1 + x2 + x3; /* 1. uses x3 before definition */
}

int f()
{
  int loc;

  if (3 > 4)
    {
      loc = 3;
      return x1; /* 2, 3, 4. bad --- x1 not defined, x2, x3 not defined */ 
    }
  else  
    { 
      if (4 > 6)
	{
	  loc = x1;  /* 5. x1 not defined */
	  loc = g(); /* 6. bad --- x1, x2 not defined before call (defines x2 and x3) */
	  loc = x3; 
	}
      else if (2 > 3)
	{
	  loc = x3; /* 7. x3 not defined */
	  x1 = 6;
	  x2 = 7;
	  return g(); 
	}
      else
	{
	  x1 = 6;
	  x2 = 7;
	  
	  return 12; /* 8. returns with x3 not defined */
	}
    }

  return 12; 
  /* No errors to report.  Previously, 
        [9, 10. returns with x2 and x3 undefined (x1 IS defined on all branches!)] 
     but this is not correct; all branches that can reach the return do define 
     x1, x2 and x3.
  */
}

int h (void)
{
  return x1; /* okay */
}







