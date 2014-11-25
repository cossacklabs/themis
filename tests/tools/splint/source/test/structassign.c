# include <stdio.h>
# include <strings.h>
# include <stdlib.h>
# include <assert.h>

typedef struct
{
  int x;
  char *name;
} record;

record copyrecord (record x)
{
  record copy = x;
  return copy; /* 1. Released storage x.name reachable from parameter at ... */
}

record copyrecord2 (record x)
{
  record copy;

  copy = x;
  return copy; /* 2. Released storage x.name reachable from parameter at ... */
}

record copyrecord3 (record x)
{
  return x; /* 3. Released storage x.name reachable from parameter at ... */
}

int main ()
{
  record r;
  record rc;

  r.x = 3;
  r.name = (char *) malloc (sizeof (char) * 100);
  assert (r.name != NULL);
  strcpy (r.name, "yo");

  rc = r;

  printf ("rc: %s", rc.name);

  return r.x; /* 4. Only storage rc.name (type char *) derived from variable ... */
}
