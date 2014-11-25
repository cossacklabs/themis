typedef int mapping;
typedef int mappair;

int smalloc();

mapping *
mapping_create (void)
{
  mapping **t;
  int x;

  t = (mapping **) smalloc (sizeof (mapping *));
  x = sizeof(t);
  x = sizeof *t;
  return *t;
}

