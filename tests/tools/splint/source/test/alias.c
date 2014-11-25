# include "mut.h"

int glob;
int *globp;

int f(int *a, int b, int **c)
{
  int *x, *y, *z;

  x = a;   
  *x = 3;  /* 1. modifies *a */
  x = a;
  y = x;
  *y = 4;  /* 2. modifies *a */

  globp = a;  /* 3. modifies *globp */
  if (*x == 3) return 3; /* 4. returns aliasing globp */

  if (*x == 4) 
    {
      globp = z; /* 5, 6. z use before def, modifies globp */
      return 4; /* okay */
    }
  
  *globp = 4; /* 7, 8. modifies *a, *globp */

  x = globp;
  *x = 7;     /* 9, 10. modifies *globp, *a */

  x = &glob;
  *x = 4;    /* 11. modifies glob */

  x = &b;  /* okay */
  *x = 3;  /* okay */
  b = 3;   /* okay */
  *x = b;  /* okay */
  x = *c;  /* okay */
  *x = 4;  /* 12. modifies **c */
  a = *c;  /* okay */
  *a = 4;  /* 13. modifies **c (but not *a) */
  *globp = 3; /* 14, 15. modifies *globp, modifies *a */
  return 4;   /* 16. returns with globp aliasing a */
}

int h (mut a, mut b) 
{ 
  mut c = mut_create();

  mut_mod (a);  /* 17. modifies a */
  a = b;
  mut_mod (a);  /* 18. modifies b */
  b = c; 
  mut_mod (b);  /* okay */

  return 3;     /* 19. locally allocated storage c not released */
}


