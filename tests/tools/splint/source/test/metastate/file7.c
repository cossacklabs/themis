void *msg;

int f (void *p)
{
  return 3;
}

int g ()
{
  msg = malloc (sizeof (*msg));

  return f (msg);
}
