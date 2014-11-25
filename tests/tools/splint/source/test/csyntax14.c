typedef int atom;

typedef struct atomrep {
        atom atom;
        struct atomrep *link;
} atomrep;

int f()
{
/*@-varuse -noreturn*/
  atomrep * x = (atomrep *) 0;
}
