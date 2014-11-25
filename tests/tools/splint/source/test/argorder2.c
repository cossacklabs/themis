int i;

int h()
{
  return i;
}

int f(int x, int y)
{
  i = i + x + y;
  return i;
}
  
int g()
{
  (void) f(f(1, 2), f(1, 2)); /* 1, 2. arg1 modifies i, used by arg2, vice versa */
  (void) f(f(1, 2), h());     /* 3. arg1 modifies i, used by arg2 */
  (void) f(f(1, 2), i);       /* 4. arg1 modifies i, used by arg2 */
  return (f(i++, i));         /* 5. arg1 modifies i, used by arg2 */
}
