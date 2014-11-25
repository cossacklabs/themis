/*
** Some tests for obvious loop exec
*/

void g (/*@out@*/ int x[])
{
  int i;

  for (i = 0; i < 2; i++)
    {
      x[i] = i;
    }
}

void g1 (/*@out@*/ int x[])
{
  int i;

  for (i = 0; i <= 2; i++)
    {
      x[i] = i;
    }

}

void g2 (/*@out@*/ int x[])
{
  int i;

  for (i = 0; i <= 0; i++) /* won't execute */
    {
      x[i] = i;
    }

}
