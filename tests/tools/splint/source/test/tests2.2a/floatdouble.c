int f (float f, double d)
{
  float f1 = d;  /* bad without +floatdouble */
  double d1 = f; /* okay relaxed */

  f1 = d + f;

  return 2;
}
