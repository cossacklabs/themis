/*
** Character arrays can be initialized using string literals (without becoming observers).
*/

void f (void)
{
  char s0[] = "abc";
  char s1[3] = "abc"; /* warning about no room for nullterminator */
  char *p = "abc";

  *s0 = 'b'; /* okay */
  s1[1] = 'd'; /* okay */
  *p = 'c'; /* error */
}
