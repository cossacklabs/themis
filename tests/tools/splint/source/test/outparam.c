typedef struct _st { int a; int b; } *st;

void h(st s, st t)
{
  int i;
  st u1, u2;

  u1->a = 3; /* 1. Variable u1 used before definition */
  u2 = u1;
  i = u2->a; 

  i = (*s).a; /* 2. Field s->a used before definition */
  t->a = i;  
}

void f(/*@out@*/ int *a, int *b)
{
  int x;
  int *y;

  x = *a;  /* 3. Value *a used before definition */
  x = *a;  /* not reported */
  x = *a;  /* not reported */
  x = *b; 

  y = a;  
  *a = 3; 
}

int g()
{
  int *b;
  int *c, *d;
  st s, t, t2, t3, t4;
  struct _st u;

  f(c, b);      /* 4, 5. Unallocated storage c passed as out parameter: c,
	                 Variable b used before definition */
  f(d, c);      /* 6. Unallocated storage d passed as out parameter: d */
  *c = *d; 
  s = t;        /* 7. Variable t used before definition */
  s = t2->a;    /* 8, 9. Variable t2 used before definition, 
		         Assignment of int to st: s = t2->a */
  t3->a = 3;   /* 10. Variable t3 used before definition */
  u.a = 3;   
  t4 = (st)malloc(sizeof(struct _st));
  t4->a = 3;   /* 11. Possible arrow access from null pointer: t4 */
  return *b;   /* 10. Fresh storage not released before return: t4 */
}



