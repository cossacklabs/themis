# include <time.h>

struct tm { int x; }

int f (struct tm s)
{
  return s.x;
}
