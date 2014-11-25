typedef int gender;

int f()
{
  static char *gender[] = { "male", "female", "?" };

  if (*gender[1] == *gender[2])
    return 3;
  else
    return 5;
}
