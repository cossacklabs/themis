typedef struct 
{
  int *x;
  int *y;
} *pair;

void pair_free (/*@only@*/ pair p)
{
  int i = 3;

  free (p->x);

  if (i < 2)
    {
      *(p->y) = 3;
      i++;
    }

  free (p->y);
  free (p);
}

void pair_free2 (/*@only@*/ pair p)
{
  int i = 3;

  free (p->x);

  if (i < 2)
    {
      *(p->y) = 3;
    }
  else
    {
      *(p->y) = 12;
    }

  free (p->y);
  free (p);
}
