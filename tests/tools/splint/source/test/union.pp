# 1 "union.c"
typedef union _ut
{
  int x;
  int y;
  int *ox;
    int *dx;
  struct { int a; int b; int *ip; } st;
} *ut;

extern     void *smalloc (size_t);

ut ut_create1 (  int a)
{
  ut u = (ut) smalloc (sizeof (*u));

  return u;  
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
  return u;  
}

ut ut_create4 (int *t)
{
  ut u = (ut) smalloc (sizeof (*u));  
  
  u->ox = t;  
  return u;
}

ut ut_create5 (int *t)
{
  ut u = (ut) smalloc (sizeof (*u));  
  
  u->dx = t;  
  return u;
}

ut ut_create6 (void)
{
  ut u = (ut) smalloc (sizeof (*u));  

  u->st.a = 3;
  return u;  
}

ut ut_create7 (int *p)
{
  ut u = (ut) smalloc (sizeof (*u));  

  u->st.a = 3;
  u->st.b = 4;
  u->st.ip = p;  
  return u;
}

void ut_mangle1 (ut u)
{
  free (u->ox);
}  

void ut_mangle2 (ut u)
{
  free (u->st.ip);
}  

void ut_mangle3 (ut u)
{
  free (u->st.ip);
  u->x = 3;  
}  
