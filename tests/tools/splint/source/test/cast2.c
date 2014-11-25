typedef /*@abstract@*/ int *abst1;
typedef /*@abstract@*/ int abst2;

/*@access abst1@*/
/*@access abst2@*/

abst1 f1 (int *x)
{
  return x; /* 1. Function returns reference to parameter x,
	       2. Implicitly temp storage x returned as implicitly only: x */
}

abst1 f2 (/*@exposed@*/ int *x)
{
  return (abst1) x; /* 3. Implicitly dependent storage x returned as implicitly only */
}

abst2 g1 (int x)
{
  return x;
}

abst2 g2 (int x)
{
  return (abst2) x;
}
