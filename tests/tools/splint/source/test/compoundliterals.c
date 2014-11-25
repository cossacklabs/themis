/* Compound literals, added to C99 */
struct simple {
  int a;
  int b;
};

typedef struct simple simple_t;

int main (/*@unused@*/ int argc, /*@unused@*/ char **argv) {
  /* This works */
  struct simple simple1 = {1,2};
  /* These don't */
  struct simple simple2 = (struct simple){1,2};
  simple_t simple3 = (((((((simple_t){1,2}))))));
  simple2 = (struct simple) { 1, 2 };
  printf("%d, %d\n", simple1.a, simple1.b);
  printf("%d, %d\n", simple2.a, simple2.b);
  printf("%d, %d\n", simple3.a, simple3.b);
  return 1;
}
