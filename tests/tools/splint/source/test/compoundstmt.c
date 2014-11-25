/* GCC extension: http://gcc.gnu.org/onlinedocs/gcc-3.0.3/gcc_5.html#SEC68 */

extern void fvoid (void) ;

int main (int argc, char **argv) {
  int i, j;
  j = 1;
  i = ({ int __tmp; __tmp=(5); __tmp; });
  j = ({ int __tmp; __tmp=(5); fvoid (); });
  printf("%d\n", i);
  return 1;
}
