# include "bool.h"

/*@only@*/ /*@null@*/ int *p;

extern /*@truenull@*/ bool isNull (/*@null@*/ int *p);

void f (void)
{
  if (p != NULL) return;
  else 
    {
      p = malloc (24);  
      if (p == NULL) exit (EXIT_FAILURE);
      *p = 3;
    }
}

void f1 (void)
{
  if (p != NULL) return;

  p = malloc (24);  
  if (p == NULL) exit (EXIT_FAILURE);
  *p = 3;
}

int f2 (void)
{
  if (p == NULL) return 0;
  return *p;
}

int f3 (void)
{
  if (isNull(p)) return 0;
  return *p;
}

void g (void)
{
  if (p == NULL) return;

  p = malloc (24);  /* 1. Only storage p not released before assignment */
  if (p == NULL) exit (EXIT_FAILURE);
  *p = 3;
}

