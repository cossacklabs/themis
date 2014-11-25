#include <stdio.h>

//void free (char * bob);

 /*@nullterminated@*/ char * strcpy ( char * ch1, /*@nullterminated@*/ 
char  * ch2);

char * global;

void test2 ()
{
char * y;
char * x;
strcpy (y, x);

strcpy (y, "bob");
x = "bob";
strcpy (y, x);
strcpy (y, global);
}



 /*@nullterminated@*/ char * tr ( char  * e) {

/*@nullterminated@*/ char * c;

return (e);
}

int main ()
{
/*@nullterminated@*/ char * d;
d = tr(d);
tr(d);
free (d);
return 0;
 
}

