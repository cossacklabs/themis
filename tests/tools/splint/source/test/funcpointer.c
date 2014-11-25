# include <stdlib.h>

typedef int ***(**f1)(void);       /* f1 is a pointer to a function taking no args, returning int ***  */
typedef int (*f2)(void);           /* f2 is a function taking no args, returning an int */
typedef int **(*f3)(void);         /* f3 is a function taking no args, returning a pointer to a pointer to an int */
typedef int *(*f4[])(void);        /* f4 is an array of functions taking no args, returning pointers to ints */
typedef int *(f5)(int);            /* f5 is a function taking an int, returning a pointer to an int */ 
typedef int *(*f6(int,int))(int);  /* f6 is a function taking 2 int args, returns a function taking an int and
				     returning a pointer to an int */
             /* gcc complains if the pointer before f6 is omitted.  why? */
/*@-paramuse*/
int func1(void) { return 3;}
int *func2(void) { return (int *)malloc(sizeof(int));} 
     /* 1. Possibly null storage returned as non-null
     ** 2. Returned storage not completely defined (allocated only)
     */
/*@null@*/ int **func3(void) { return (int **)0;}
/*@null@*/ int ***func4(void) { return (int ***)0;}
int *func5(int i) { return &i; } /* 3. Immediate address &i returned as only: &i */
     /* 4. Return value references stack-allocated storage i: &i */
int *(*func6(int x, int y))(int) { return func5; }

int main (void)
{
  f1 t1; f2 t2; f3 t3; f3 *t3p; f4 *t4; f6 t6;
  int x, *xp, ***xppp;

  t1 = func1; /* 5. Assignment of int () * to f1 */        
  t1 = func4; /* 6. Assignment of int * * * () * to f1 */
  *t1 = func4; 

  t2 = func1; 
  t2 = func2; /* 7. Assignment of int * () * to f2 */

  t3 = func3;  
  t3p = func3; /* 8. Assignment of int * * () * to f3 *: t3p = func3 */

  t4 = func2; /* 9. Assignment of int * () * to f4 *: t4 = func2 */

  xppp = (*t1)(); 
  x = (t1)(); /* 10. Call to non-function (type f1): (t1) */
  x = (t2)(); 
  xp = (*t1)(); /* 11. Assignment of int * * * to int *: xp = (*t1)() */
  xp = (t4)(); /* 12. Call to non-function (type f4 *): (t4) */

  t6 = func1; /* 13. Assignment of int () * to f6: t6 = func1 */
  t6 = func6; /* funcpointer.c:43: invalid lvalue in assignment */ 
  (void) (t6)(); /* 14. Function (t6) called with 0 args, expects 2 */
  (void) (t6)(3); /* 15. Function (t6) called with 1 args, expects 2 */
  (void) ((t6)(3))(); /* 16. Function (t6) called with 1 args, expects 2 */
                      /* 17. Function ((t6)(3)) called with 0 args, expects 1 */
  (void) ((t6)(5, 3))(7); 
  return 3;
}
  
/* function pointer bug, provided by Marc Espie */

typedef void (*func)(void);

void (*f[10])(void);
func g[10];

void a(void)
{
}

void b(void)
{
  func *h;
  
  h = f;
  f[0] = a;
  g[0] = a;
  f[1] = g[0];
  h[2] = a;
}




