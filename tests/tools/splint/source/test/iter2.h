/*@iter intFor (int i, int j, int k, yield int val); @*/
# define intFor(i, j, k, ___x) \
  { int ___x = (i);  while ((___x) <= (j)) { (___x) += (k); 

# define end_intFor }}

/*@iter arrayElements (int i, int j, int *a, yield int el); @*/
# define arrayElements(i,j,a,___x)\
 { int ___c; int *(___b) = a; for (___c = (i); ___c <= (j); ___c++) { int ___x = *(___b); 
   /* 9. j used more than once (in loop test) */

# define end_arrayElements \
  (___b)++; }}


