extern /*@out@*/ /*@only@*/ void *smalloc (unsigned int size);

typedef int mint;

typedef struct _st
{
  int a;
  /*@only@*/ int *b;
  /*@shared@*/ mint *c;
  int d;
} *st;

/*@only@*/ st st_create1 ()
{
  st res = (st) smalloc (sizeof(struct _st));
  int *z;

  res->a = 3; 
  z = res->b;   /* res->b not defined */
  z = (*res).c; /* (*res).c not defined */

  return res;   /* res->d not defined */
}

void f1(/*@only@*/ st x)
{
  free (x->b);
  free (x);
} /* correct */

void f2(/*@only@*/ st x) 
{
  free (x);
} /* bad --- didn't release x->b */
     
void f3(/*@only@*/ st x) 
{
  free (x->c); /* bad --- x->c is shared */
} /* bad --- didn't release x */

/*@only@*/ st st_create ()
{
  st res = (st) smalloc(sizeof(struct _st));
  res->a = 3; 
  return res; /* 6, 7, 8. res->b, res->c, res->d not defined */
}


