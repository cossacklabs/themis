void setx (int *x, int *y)
  /*@modifies *x@*/
{
  *y = *x;
}

void sety (int *x, int *y)   
   /*@modifies *y@*/
{   
   setx (y, x); 
}

