int x[3] = { 1 , 2, 3, 4 } ; /* Initializer block for x has 4 elements, but declared as int[3] */
int a[3] = { 1 , 2 } ; /* Initializer block for a has 2 elements, but declared as int[3] */
int aa[3][3] = { { 1 , 2, 3 }, /* Initializer block for aa has 2 elements, but declared as int[3][3] */
                 { 4, 5 } } ; /* Initializer block for aa[1] has 2 elements, but declared as int [3] */

int ab[] = { 1, 2, 3 } ;

char cc[3] = { 'a', 'b', 'c' } ;
char cs[3] = "ab";
char cs1[3] = "abc"; /* gcc doesn't warn for this */  /* stringliteralnoroom */
char cs2[3] = "abcd"; /* gcc warns for this with -Wall */ /* stringliteraltoolong */
char cs3[3] = "a"; /* stringliteral smaller (not on default) */ 

char csx[3][3] = { { 'a', 'b', 'c' } , "def", "gasdf" } ; /* 2 errors */

int a2[][2] = {{1,2},{3,4},{5,6}};
int a3[][2] = {{1,2},{3,4,5},{5,6}};
