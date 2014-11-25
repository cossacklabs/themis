int f(void)
{
  double (*y)[3];
  double z[3];

  y = &z;         /* line 6 */
  return 0;
}
