extern int g(int x);

int f(int j)
{
  int i;

  switch (j) 
    {
    case 3: /* okay */
      i = g(j);
      printf("3");
    case 4:  /* 2. Fall through case (no preceeding break) */
      if (i == 3) /* 1. Variable i used before definition */
	{
	  printf("hullo");
	  return 3;
	}
      else
	{
	  printf("yabba");
	  break;
	}
    case 5: /* okay */
      i = g(j++);
      while (i < 3)
	{
	  i++;
	  if (j == 4) break;
	}
    case 6: /* 3. Fall through case (no preceeding break) */
      printf("high");
      return 3;
    case 7: /* okay */
    case 8: /* okay */
    case 9:
      printf("nope");
    default: /* 4. Fall through case (no preceeding break) */
      printf("splat");
   }
} /* 5. Path with no return in function declared to return int */
  
