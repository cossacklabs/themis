typedef union _ut
{
  int x;
  int y;
  int *ox;
  /*@dependent@*/ int *dx;
  struct { int a; int b; int *ip; } st;
} *ut;

extern /*@only@*/ /*@out@*/ void *smalloc (size_t);

ut ut_create1 (/*@unused@*/ int a)
{
  ut u = (ut) smalloc (sizeof (*u));

  return u; /* 1. Returned union u contains no defined field */
}

ut ut_create2 (int a)
{
  ut u = (ut) smalloc (sizeof (*u));

  u->x = a;
  return u; 
}

ut ut_create3 (int a)
{
  ut u = (ut) smalloc (sizeof (*u));

  u->x = a;
  u->y = a;
  return u; /* [Not anymore. Returned union u has 2 defined fields: x, y */
}

ut ut_create4 (int *t)
{
  ut u = (ut) smalloc (sizeof (*u));  
  
  u->ox = t; /* 2. Implicitly temp storage t assigned to implicitly only */
  return u;
}

ut ut_create5 (int *t)
{
  ut u = (ut) smalloc (sizeof (*u));  
  
  u->dx = t; /* 3. Implicitly temp storage t assigned to dependent: u->dx = t */
  return u;
}

ut ut_create6 (void)
{
  ut u = (ut) smalloc (sizeof (*u));  

  u->st.a = 3;
  return u; /* 4. Returned storage u->st contains 2 undefined fields: b, ip */
}

ut ut_create7 (int *p)
{
  ut u = (ut) smalloc (sizeof (*u));  

  u->st.a = 3;
  u->st.b = 4;
  u->st.ip = p; /* 5. Implicitly temp storage p assigned to implicitly only */
  return u;
}

void ut_mangle1 (ut u)
{
  free (u->ox);
} /* 6. Released storage u->ox reachable from parameter */

void ut_mangle2 (ut u)
{
  free (u->st.ip);
} /* 7. Released storage u->st.ip reachable from parameter */

void ut_mangle3 (ut u)
{
  free (u->st.ip);
  u->x = 3; /* This one's a toughy... */
} /* 8. Released storage u->st.ip reachable from parameter */
