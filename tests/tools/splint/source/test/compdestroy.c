typedef /*@only@*/ int *oip;

typedef /*@abstract@*/ struct
{
  oip *ips;
  int size;
} *sip;

void sip_free (/*@only@*/ sip x)
{
  int i;

  for (i = 0; i < x->size; i++)
    {
      free (x->ips[i]);
      free (x->ips[i]);
    }

  free (x->ips);
  free (x);
}

void sip_free2 (/*@only@*/ sip x)
{
  free (x->ips); /* 1. Only storage *(x->ips) (type oip) derived from released */
  free (x);
}

