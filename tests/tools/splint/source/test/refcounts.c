extern /*@out@*/ /*@only@*/ void *smalloc (size_t);

typedef /*@refcounted@*/ struct _rp
{
  /*@refs@*/ int refs;
  /*@only@*/ int *p;
} *rp;

extern rp rp_create2 (void);

void rp_release (/*@killref@*/ rp x)
{
  x->refs--;

  if (x->refs == 0)
    {
      free (x->p);
      free (x); /* 1. Reference counted storage passed as only param: free (x) */
    }
}

/*@tempref@*/ rp rp_temp (void)
{
  return rp_create2 (); /* 2. New reference returned as temp reference: rp_create2() */
}

void rp_f (/*@killref@*/ rp r1, /*@killref@*/ rp r2)
{
  rp rt = rp_temp ();

  rp_release (r1);  
  r2 = rp_temp (); /* 3. Kill reference parameter r2 not released before assignment */
  rp_release (rt); 
}

rp rp_create (/*@only@*/ int *p)
{
  rp r = (rp) smalloc (sizeof(rp));

  r->refs = 1;
  r->p = p;

  return r;
}

rp rp_ref (rp x)
{
  return x; /* 4. Reference counted storage returned without modifying ... */
}

rp rp_refok (rp x)
{
  x->refs++;
  return x; 
}

rp rp_waste (/*@only@*/ int *p)
{
  rp z1 = rp_create (p);
  rp z2 = rp_ref (z1);

  z2->p++;
  return z1; /* 5. New reference z2 not released before return */
}

rp rp_waste2 (/*@only@*/ int *p)
{
  rp z1 = rp_create (p);
  rp z2 = rp_ref (z1);

  z2 = rp_ref (z1); /* 6. New reference z2 not released before assignment */
  return z1; /* 7. New reference z2 not released ... */
}


