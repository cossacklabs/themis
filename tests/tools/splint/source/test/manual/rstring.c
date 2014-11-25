# include "rstring.h"

static rstring rstring_ref (rstring r)
{  
  r->refs++;
  return r;
}

rstring rstring_first (rstring r1, rstring r2)
{
  if (strcmp (r1->contents, r2->contents) < 0)    
    { 
      return r1;    
    }
  else 
    { 
      return rstring_ref (r2);    
    }
}
