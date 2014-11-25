/* Contributed by Matthew Yeates. */

int foo (char *b);

struct s {
  char *i;
};

static int redir(struct s *s)
{
  s->i = malloc(1 * sizeof(int));

  if (!(s->i))
    return 1;

  if (0) {
    free(s->i);
    return 1;
  }
  
  free (s->i);
  return 0;
}

