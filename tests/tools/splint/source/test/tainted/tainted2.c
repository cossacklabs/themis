# include <assert.h>

/*@untainted@*/ char *test (int fromuser, /*@untainted@*/ char *def)
{
  char *stk = NULL;

  if (fromuser != 0)
    {
      stk = malloc (sizeof (char) * strlen (def));
      assert (stk != NULL);
      strcpy (stk, def);
    }
  else
    {
      stk = malloc (sizeof (char) * 128);
      assert (stk != NULL);
      (void) fgets (stk, 128, stdin);
    }

  return stk;
}
