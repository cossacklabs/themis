# include "immutable.h"

immutable immutable_create (int x) 
{
  immutable res = (immutable) malloc (sizeof (*res));
  
  res->x = x;
  return res;
}

