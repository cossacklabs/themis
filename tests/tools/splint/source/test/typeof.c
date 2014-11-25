/* gcc extension: http://gcc.gnu.org/onlinedocs/gcc-3.0.3/gcc_5.html#SEC74 */

int main (int /*@unused{*/ argc, char /*@unused@*/ **argv) {
  int i = 0;
  typeof (i) j;
  __typeof__ (int *) ip;
  typeof (typeof (char *) [4]) y; /* char *y[4] */

  j = 10;
  *ip = j;
  *y[3] = 'a';
  printf("%d\n", *ip);
  return 1;
}
