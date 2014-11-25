void g (void)
{
  char *x;
  char **y;

  f1(&x); 
  printf("%s\n", x); 

  f1(y); /* 1. Unallocated storage y passed as out parameter: y */
  printf("%s\n", *y);
}

void h (void)
{
  int z;
  char *x;
  char **y;

  f3 (&z); /* okay! */
  f2 (&x); /* 2. Value &x used before definition, */
  printf ("%s\n", x);  

  f2 (y);  /* 3. Variable y used before definition */
}
