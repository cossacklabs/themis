/*
** tests aliasing of arrays, structs and unions
*/

# include "alias3.lh"

st gst;
ut *gu;

void model (int ia[])
{
  int *ip;

  ip = &(ia[6]);
  *ip = 3;
  gst.a = 3; /* 1. Suspect modification of gst.a */
  gu->a = 5; /* 2. Suspect modification of gu->a */
}

void modst (st s1)
{
  s1.a = 3;
  *(s1.b) = 6;
}

void modu (ut *u)
{
  u->a = 5;
}

int f (int ia[], st s1, ut *u1)
{
  int *ip;
  st   sl;
  ut   *ul;

  ip = &(ia[3]);
  *ip = 7; /* modifies ia[3] */

  ip = &(ia[*ip]);

  *ip = 8; /* modifies ia[?] */

  model(ia); /* modifies ia[6] */
  ip = ia;
  model(ip); /* modifies ia[6] */

  s1.a = 3;    /* okay */
  *(s1.b) = 7; /* modifies *(s1.b) */
  
  sl = s1;     /* okay (sl is NOT an alias for s1) */
  modst(sl);   /* was: does not modify anything --- structure is passed by value */
               /* NOT TRUE: does modify sl since *s1.b is the same reference */
  sl.a = 3;    /* okay */
  ip = s1.b;   /* okay, ip aliases s1.b */
  *(s1.b) = 3; /* modifies s1.b */
  *ip = 6;     /* modifies s1.b */
  *(sl.b) = 3; /* okay */

  ul = u1;     /* ul aliases u1 */
  modu(ul);    /* okay */
  ul->a = 3;   /* modifies u1->a */
  sl = ul->s;  /* okay */
  sl.a = 3;    /* okay */
  *(sl.b) = 6; /* modifies *(u1->s.b) through alias */
  return 5;
}
