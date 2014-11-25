int test(int);

int main()
{
  int (*y)(int) = &test;
  int (**x)(int) = &y;
  x(10);  /* called object is not a function */
}

static /*@unused@*/ void foo1(void)
{
  int buf[10];
  buf[10] = 3;
}


static /*@unused@*/ void foo(void) 
{ 
  float *array = NULL; 
  size_t /*@unused@*/ size = sizeof(array[0]); 
} 
