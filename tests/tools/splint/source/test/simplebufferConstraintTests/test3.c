
void t()
{
  char g [100];
  char j [23];
  j[22] = 'd';  /*safe */
  g[0] = 'd';  /*   safe */
  g[67] = 'f;'; /*     safe */
  g[101] = 'g'; /*    unsafe */
  g[100] = 'f'; /*    unsafe */
}
