# include <stdarg.h>
# include "bool.h"
int sumn (int x, ...)
{
  int y = 0;
  void *yaba;
  va_list args;

  va_start (args, (void *)x); /* okay (args counts as out param) */
  while (x > 0)
    {
      x--;
      y = va_arg (args, int); /* okay */
      y = va_arg (args, char *); /* type error */
      y = va_arg (yaba, int); /* error */
    }
  return y;
}

int test (int x, char *s)
{
  x = sumn(); /* bad */
  x = sumn(x); /* okay */
  x = sumn(s); /* bad */
  x = sumn(x, s); /* okay */

  x = test (x, s, x); /* bad */
  return x;
}

int missingargs (int x, int y)  /* this is okay */
{
  y = x;
  return x;
}

int severalargs (char c, int y, bool b) /* first arg: int, second char *, third extra */
{
  if (b) { c = 'a'; }
  return y;
}

int severalargs2 (int x)  /* bad */
{
  return x;
}

int voidargs (char c) /* bad */
{
  c = 'a'; 
  return 3;
}

int any (...) /* ok */
{
  return 3;
}

int many1 (int x, char c, float f) /* bad */
{
  x;
  c;
  f;
  return x;
}

int many2 (int x, char c, ...)
{
  c;
  return x;
}

int many3 (int x) /* bad */
{
  return x;
}



