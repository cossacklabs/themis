int (f)(int x);

int g()
{
  return f(3);
}

int f(int x)
{
  return x;
}
