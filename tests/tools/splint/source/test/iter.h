# define intFor(i, j, k, ___x) \
  { int ___x = (i);  while ((___x) <= (j)) { (___x) += (k); 

# define end_intFor }}

# define arrayElements(i,j,a,___x)\ /* 9. Macro parameter j used more than once (in post loop test) */
 { int ___c; int *(___b) = a; for (___c = (i); ___c <= (j); ___c++) { int ___x = *(___b); 
   /* 9. j used more than once (in loop test) */

# define end_arrayElements \
  (___b)++; }}


