void f (int x)
{
  while (x < 3) /* 3 [5]. Suspected infinite loop: no condition values modified */
    {
      switch (x)
	{
	case 1:
	  /*@switchbreak@*/ 
	  break;
	case 2:
	  /*@loopbreak@*/ /* 1. Break preceded by loopbreak is breaking a switch */
	  break; 
	case 3:
	  break; /* 2. Break statement in switch inside loop */
	}
      
      while (x > 2) /* 2 [4]. Suspected infinite loop: no condition values modified */
	{
	  if (3 > 4)
	    {
	      break; /* 3. Break statement in nested loop */
	    }
	  else
	    {
	      /*@innerbreak@*/ 
	      break;
	    }
	}
    }

  while (x < 2)
    {
      x++;
      /*@innerbreak@*/ break; /* 4 [6]. Break preceded by innerbreak is not in a deep loop */
    }
}












