int f1 (x, y)
{
  int z;

  z = *y; /* 1. Dereference of non-pointer (int): *y */

  return z + x;
}

int f2 (/*@unused@*/ x) /* 2. Function f2 declared with 1 arg, specified with 2 */
{
  return 3;
}

int f3 (x, y)
   int x;
   /*@unused@*/ char y; /* 3. Parameter 2, y, of function f3 has inconsistent type */
{
  return x;
}


