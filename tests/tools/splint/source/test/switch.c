int f1(int x)
{
  switch (x)
    {
    case 0: return 3;
    case 1: return 4;
    default: return 6;
    }
}

typedef enum _et { ONE, TWO, THREE } et;

int f2 (et x)
{
  switch (x)
    {
    case ONE: return 3;
    case TWO: return 8;
    case THREE: return 12;
    }
}

int f3 (et x)
{
  switch (x)
    {
    case ONE: return 3;
    case TWO: break;
    default:  return 12;
    }
} /* 1. Path with no return in function declared to return int */

int f4 (et x)
{
  switch (x)
    {
    case ONE: return 3;
    case TWO: return 14;
    default:  return 12;
    }
}

int f5 (et x)
{
  switch (x)
    {
    case ONE: return 3;
    case TWO: return 14;
    } /* 2. Missing case in switch: THREE */
} /* 3. Path with no return in function declared to return int */

int f6 (et x)
{
  switch (x)
    {
    case ONE: 
      if (3 > 4)
	{
	  return 3;
	}
      else
	{
	  return 12;
	}
    case TWO: 
      if (3 > 4) break;
      return 14;
    default:  return 12;
    }
} /* 4. Path with no return in function declared to return int */



