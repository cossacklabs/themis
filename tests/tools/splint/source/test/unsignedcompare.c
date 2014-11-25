void test (int fd, void *buffer, size_t cnt)
{
  unsigned long res;

  res = read(fd, buffer, cnt);

  if (res <= 0)
    {
      printf ("You lose!");
    }

  if (res < 0 || res >= 0)
    {
      printf ("twice loser");
    }
}
