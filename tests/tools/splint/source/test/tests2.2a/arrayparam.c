int f1(int x[2][]) /* bad */
{
  return x[2][3];
}

int f2(int x[][2]) /* okay */
{
  return x[2][3];
}

int f3(int x[][]) /* bad */
{
  return x[2][3];
}

int f4(int x[][][2]) /* bad */
{
  return x[2][3][1];
}

int f5(int x[2][][2]) /* bad */
{
  return x[2][3][1];
}

int f6(int x[][2][2]) /* okay */
{
  return x[2][3][1];
}

