int i;
int j;

int f(int x, int y, int z)
{
  printf("%d %d", x, y);
  printf("%d %d", y++, z++);  
  printf("%d %d", z++, z);  /* arg 2 modifies z, used by arg 3 */

  return (f((i++, j++), (i = 3), j)); /* arg 1 modifies i, used by 2,
					 arg 1 modifies j, used by 3,
					 arg 2 modifies i, used by 1 */
}
  
