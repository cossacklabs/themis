typedef int U_type;
static int x;

void f3 (/*@null@*/ int *a1, int a2, int *a3, int a4);

typedef struct {
    char sf3 ;
} some_struct ;

void f2 (int n_bytes) ;

void f1 (void)
{
    f2 ((int) offsetof (some_struct, sf3));
    f3 (0, 0, (U_type *)&x, 0);
}



