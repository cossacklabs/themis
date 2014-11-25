typedef struct _su
{
  int x1;
  union
  {
    int u1;
    int u2;
  } ;
  int x2;
} su;

su wtd = { 1, NULL, 2 };

int f (su *s)
{
  return s->u1 + s->x1;
}


