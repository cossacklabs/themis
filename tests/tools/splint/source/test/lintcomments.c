int f (int x)
{
  int i = 0;

  switch (x)
    {
    case 1: i++; /*@fallthrough@*/ /*FALLTHROUGH*/
    case 2: i++; /*FALLTHROUGH*/
    case 3: i++; /*FALLTHRU*/
    case 4: i++;
    default: i++;
    }

  if (i > 2)
    {
      return i;
      /*@notreached@*/
      i++;
    }
  else
    {
      return i;
      /*NOTREACHED*/
      i++;
    }

  /*@notreached@*/ return 3; 
}
