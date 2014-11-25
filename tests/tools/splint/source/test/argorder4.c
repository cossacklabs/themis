int glob;

int add(int x, int y) { return (x + y); }

int f()
{
  return (glob);
}

int g()
{
  return (++glob);
}

int h()
{
  if (f() > 3)
    {
      return (f() + g()); /* bad --- order of evaluation matters! */
    }
  else
    {
      if (g() < 2)
	{
	  return (f() + (++glob)); /* bad! */
	}
      else
	{
	  return (g() + h()); /* bad (twice) */
	}
    }
  
  /* unreachable code here */

  return (add ((printf("hullo"), 3), (printf("goodbye"), 4))); /* bad (wrice) */
}



