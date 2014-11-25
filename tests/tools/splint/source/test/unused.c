typedef int *dumbtype;

typedef struct _st
{
  int x;
  int y;
  int dumb;
  struct _inner
    {
      int l;
      int m;
      int silly;
    } inner;
} st;

extern int undef (int x);
static int r (void);

static int f (int x, st stx)
{
  return (x + stx.x + stx.y + stx.inner.l + stx.inner.m + r());
}

static int r (void)
{
  return undef (3);
}

