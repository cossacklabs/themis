typedef struct {
  char *x;
} T;

static void foo(/*@special@*/ T* x)
     /*@defines x->x@*/
     /*@post:notnull x->x@*/ ;
     
static void bar (/*@out@*/ T* x)
     /*@post:notnull x->x@*/
{
  foo(x);
}

void test()
{
  T a;
  foo(&a);
  bar(&a);
}
