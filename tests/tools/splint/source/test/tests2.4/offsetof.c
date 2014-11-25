# include <stddef.h>

typedef struct _S { union { int i; struct { int m; } s; } u; } S;

int f () {
  size_t t;
  int m;

  t = offsetof (S, u.i);
  m = offsetof (S, u);
  t = offsetof (S, u.s.m);
  t = offsetof (S, u.s.k);
  return m;
}
