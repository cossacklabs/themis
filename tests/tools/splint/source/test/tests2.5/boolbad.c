typedef /*@abstract@*/ enum {FALSE = 0, TRUE = 1} BOOLEAN;

void func (BOOLEAN foo)
{
  foo = FALSE;
}

int main (void)
{
    BOOLEAN foo;

    func (foo);

    return 0;
}
