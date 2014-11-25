typedef struct 
{
  /*@only@*/ int *x;
  /*@only@*/ int *y;
} *pair;  

# include "alias5.lh"

void incx1 (pair p)
{
  pair p2 = p;

  p2->x++; /* 1. Suspect modification of p->x through alias p2->x: p2->x++ */
}

void incx2 (pair p)
{
  pair p2 = p;

  p2 = pair_create ();
  p2->x++;
} /* 2. Fresh storage p2 not released before return */

void incx3 (pair p)
{
  pair p2 = pair_create ();
  p2->x = p->x; /* 3. Only storage p2->x not released before assignment: p2->x = p->x */
  *(p2->x) = 3; /* 4. Suspect modification of *(p->x) through alias *(p2->x): */
  pair_free (p2);
} /* 5. Storage p->x reachable from parameter is kept (should be only) */ 



