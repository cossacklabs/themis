typedef struct 
{
  /*@owned@*/ int *x;
  /*@owned@*/ int *y;
  /*@dependent@*/ int *z;
} *pair;

extern /*@only@*/ /*@out@*/ void *smalloc (size_t);

/*@only@*/ pair pair_create (void)
{
  pair p = (pair) smalloc (sizeof (*p));

  p->x = smalloc (sizeof (int));
  p->y = smalloc (sizeof (int));
  p->z = p->y;

  *(p->x) = 3;
  *(p->y) = 12;

  return p;
}

/*@only@*/ pair pair_create1 (void)
{
  pair p = (pair) smalloc (sizeof (*p));

  p->x = smalloc (sizeof (int));
  p->y = smalloc (sizeof (int));
  p->z = p->y;
  *(p->y) = 12;

  *(p->x) = 3;
  p->y = smalloc (sizeof (int));
  *(p->y) = 12;

  return p; /* 1. Returned storage p->z is only (should be dependant): p */
}

void mangle (/*@temp@*/ pair p)
{
  free (p->x);
} /* 2. Released storage p->x reachable from parameter */

void mangle2 (/*@temp@*/ pair p)
{
  free (p->x);
  p->x = p->y; 
} /* 3. Storage p->y reachable from parameter is dependant */

/*@only@*/ pair mangle3 (/*@only@*/ pair p)
{
  free (p->x);
  p->x = p->y;
  return p; /* 4. Storage p->y reachable from return value is dependant (should be */
}

int f (pair p)
{
  p->x = NULL; /* 5. Only storage p->x not released before assignment: p->x = NULL */

  return *(p->y);
}

void swap (pair p)
{
  int *tmp = p->x;

  p->x = p->y;
  p->y = tmp;
}

void mangleok (/*@temp@*/ pair p)
{
  int *loc = p->x;
  *loc = 8;
} 
