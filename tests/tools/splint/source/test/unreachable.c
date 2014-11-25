/*@exits@*/ void f (void)
{
  int x = 3;

  exit(x);
  x++; /* 1. Unreachable code */
}

void tu(int y)
{
  int x = 16;

  if (x == y)
    {
      return;
    }
  else
    {
      x++;
      f();
      return; /* 2. Unreachable code */
    }

  y++; /* 3. Unreachable code: no longer reported because of previous error. */
}

int s(int z)
{
  switch (z)
    {
    case 1: return 3;
    case 2: return 12;
    case 3: z++; return z; z--; /* 4. Unreachable code */
    case 4: /* 5. Fall through case */
      switch (z + 1)
	{
	case 1: return 18;
	case 2: z++;
	}
      return z;
    default:
      return 12;
    }
  
  z++; /* 6. Unreachable code */
}


