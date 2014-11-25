int f (int i1, int i2, unsigned u1, unsigned u2)
{
  int x = i1 & i2; /* not unsigned */
  unsigned u = u1 & i2;
  short s = i1 & i2;  /* not unsigned */
  short s2 = i1 + i2;

  u = u1 | u2;
  u = i1 << 3; /* left operand not unsigned */
  u = u1 >> 4; /* okay */
  u = u1 >> i1;

  u >>= 3;
  u = i1 ^ u1; /* ! unsigned */
  u = u1 ^ i1; /* ! not unsigned */

  return x;
}

/*

>>, >>=
<<, <<=
    warn if not unsigned

&, &=
|, |=
^ (XOR), ^=
   warn if not unsigned
*/
