typedef /*@abstract@*/ struct
{
  int nelements;
  int *elements;
} *intSet ;

extern intSet intSet_new (void);
