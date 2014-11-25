int f (int a[], int a2[73] /*legal, but meaningless!*/, int m [] [3] /* int m [][] leads to error for m[1] */ ) {
  int b[10];
  int c[10];
  /* int pa[]; illegal!*/
  int *p = (int *) malloc (sizeof (int) * 10);

  a++;
  a2++;
  /* b++; illegal! */

  printf ("a: %d / a2: %d / b: %d / m: %d / m[0]: %d / p: %d / int: %d\n", 
	  sizeof (a), sizeof (a2), sizeof (b),
	  sizeof (m), sizeof (m[1]), sizeof (p), sizeof (int));
  /* b = c; illegal*/
  /* b = a; illegal */
  a = b; /* okay! */
}

int main (void) {
  int a[10];
  int m[10][3];
  int x = f (a, a, m);

  {
    int k[x];
    k[2] = 12;

  }
    
}
