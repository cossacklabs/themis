typedef /*@abstract@*/ struct
{
  char *name;
} *employee;

extern void setName (employee e) /*@modifies e@*/;
