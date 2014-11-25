# include <stdio.h>

int main (void) {
  int i;

  printf ("a%n\n", &i); 
  printf ("%d\n", i);
  return 0;
}
