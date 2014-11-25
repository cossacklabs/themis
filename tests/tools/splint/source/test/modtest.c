int x, y, ai[], bi[];

typedef struct _ts { int a; int b; } tst;

tst ts;

tst *tstp;

int g (int a[], int *p)
{
  return 7;
}

int f (int i[], int *j)
{
  i[3] = y;
  j[2] = x;
  ai[3] = 5;
  ai[4] = 8;   /* not a modification error ai[x'] */
  i = j;
  x++;
  y++;
  ts.a = i[2];
  ts.b = tstp->a;
  tstp->b = 6;
  g(j, i);

  return 3;
}

int h(int a[])
{
  y++;
  (void) g(a, &x);
  return y;
}

