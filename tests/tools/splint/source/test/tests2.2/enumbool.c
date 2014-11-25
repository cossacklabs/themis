typedef enum { false = 0, true } bool;

bool f (bool b)
{
  if (b)
    {
      return ( 3 > 4);
    }
  else
    {
      return false;
    }
}
