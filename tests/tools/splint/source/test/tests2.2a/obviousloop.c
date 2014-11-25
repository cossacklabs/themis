int f (void)
{
  int x = 3;

  while (TRUE)
    {
      x++;

      if (x == 12) {
	return 12;
      }
    }

  x = 12; /* unreachable */
}

int f1 (void)
{
  int x = 3;

  while (TRUE)
    {
      x++;

      if (x == 12) {
	return 12;
      }

      if (x == 8) {
	break;
      }
    }

  x = 12;
  /* no return */
}

int f1b (void)
{
  int x = 3;

  while (TRUE)
    {
      x++;

      if (x == 12) {
	return 12;
      }

      if (x == 8) {
	for (x = 2; x < 2; x++) {
	  if (x == 4) {
	    break;
	  }
	}
      }
    }

  x = 12; /* unreachable */
}

int f2 (void)
{
  int x = 3;

  for (;;)
    {
      x++;

      if (x == 12) {
	return 12;
      }
    }

  x = 12; /* unreachable */
}

