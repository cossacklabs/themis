typedef struct 
{
  int *x;
  int *y;
  /*@dependent@*/ int *z;
} *pair;

extern void pair_keep (/*@keep@*/ pair p);

extern /*@only@*/ /*@out@*/ void *smalloc (size_t);
extern /*@partial@*/ pair pair_part (void);

/*@only@*/ pair pair_copy (pair p)
{
  pair ret = (pair) smalloc (sizeof (*ret));

  ret->x = p->x;
  ret->y = p->y;
  ret->z = p->z;

  return (ret); /* 1. Storage p->x reachable from parameter is kept (should be only) */
}               /* 2. Storage p->y reachable from parameter is kept (should be only) */

/*@only@*/ pair pair_create (void)
{
  pair p = (pair) smalloc (sizeof (*p));

  p->x = smalloc (sizeof (int));
  p->y = smalloc (sizeof (int));
  p->z = p->y; /* 3. Only storage p->y assigned to dependent: p->z = p-y */

  *(p->x) = 3;
  *(p->y) = 12;

  return p; /* 4. Storage p->y reachable from return value is unqualified */
}

pair pair_swankle (/*@keep@*/ pair p)
{
  pair ret = pair_part ();
 
  ret->x = p->x;
  pair_keep (p); /* 5. Storage p->x reachable from passed parameter is kept */ 
  p->x = smalloc (sizeof (int));
  *p->x = 3;
  return ret;
}
