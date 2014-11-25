int f (int x[32])
{
  int a[23];

  if (sizeof(x) == 3)
    {
      return 1;
    }

  return sizeof(a);
}
