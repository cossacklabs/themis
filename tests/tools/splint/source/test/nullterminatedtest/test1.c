#include <stdio.h>

void fcn1 (/*@nullterminated@*/ char *e);

int main (){
/*@unused@*/ char c[];

char *d;

 c[0] = '\0';
fcn1(c);
fcn1("hello");
fcn1(d);

return 0; 
}
