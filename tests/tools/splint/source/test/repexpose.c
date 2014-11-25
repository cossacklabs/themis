# include "repexpose.h"

abst abst_create (/*@dependent@*/ abst p, /*@only@*/ char *x, int i)
{
  abst a = (abst) malloc (sizeof (struct _abst));
  abst b = p;

  a->name = x;            /* 1. Arrow access from possibly null pointer a */
  a->val = i;
  a->parent = p;          /* [3. exposes rep] */
  a->parent = b;          /* [4. exposes rep through alias] */
  a->parent = p->parent;  /* [5, 6. exposes rep] */
  a->parent = b->parent;  /* 2. Suspect modification of p->parent through alias a->parent */
                          /* [7, 8. exposes rep through alias] 3. modifies p */
  b = a;                 
  a->name = *globstring;  /* [9, 10.] exposes rep through global */
  b->name = *globstring;  /* ??? NO? 3. Suspect modification of *globstring through alias a->name */
                          /* [11, 12.] exposes rep through global and alias */
  return a; /* 4. Storage a->name reachable from return value is kept */
            /* 5. Returned storage *a contains 1 undefined field: im */
            /* 6. Storage *globstring reachable from global is kept */
}

/*@only@*/ char *abst_name (abst a)
{
  return a->name;   /* 7. Released storage a->name reachable from parameter */
      	            /* [15, 16.] reference to parameter, exposes rep */
}

int abst_val (abst a)
{
  return a->val;
}

int *abst_aval(abst a)
{
  return (&(a->val));  /* 9. [17, 18] reference to parameter, exposes rep */
}

abst abst_parent (abst a)
{
  abst b = a;

  if (TRUE) return b;     /* [19] reference to parameter a */
  else return b->parent;  /* 8. Dependent storage b->parent returned as only */
                          /* 9. Only storage a not released before return */
}

/*@dependent@*/ char *immut_name (immut im)
{
  return (im->name);  /* 10. Only storage im->name returned as dependent: (im->name) */ 
                      /* [23, 24] exposes rep, reference to parameter */
}

void abst_setIm (abst a, immut im)
{
  a->im = im;  /* 11. Suspect modification of a->im: a->im = im */
               /* 13. [25] modifies a, NO rep exposure since it is immutable! */
}

immut abst_getIm (abst a)
{
  return (a->im); /* NO rep exposure since it is immutable */
}


