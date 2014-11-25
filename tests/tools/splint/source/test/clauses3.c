typedef struct { /*@null@*/ /*@only@*/ int *x; } *st;

void f(/*@only@*/ st x)
{
  if (x->x != NULL)
    {
      free (x->x);
    } ;

  free (x);
}

void f2 (/*@only@*/ st x)
{
  if (x->x != NULL)
    {
      free (x->x);
    }
  else
    {
      ;
    }

  free (x);
}

void g (/*@only@*/ st x)
{
  if (x->x == NULL)
    {
    }
  else
    {
      free (x->x);
    }

  free (x);
}

void h(/*@only@*/ st x)
{
  if (7 > 3)
    {
      if (x->x != NULL)
	{
	  free (x->x); 
	}
    } /* 1. Storage x->x is released in one path, but live in another. */

  free (x);
}

void m(/*@only@*/ st x)
{
  if (7 > 3)
    {
    }
  else
    {
      free (x->x); /* 2. Possibly null storage passed as non-null param: x->x */
    } /* 3. Storage x->x is released in one path, but live in another. */

  free (x);
}
  




