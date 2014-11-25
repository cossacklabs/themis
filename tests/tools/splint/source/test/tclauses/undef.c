extern int glob;
extern /*@only@*/ int *ogp;

int f (void) /*@globals undef glob, killed ogp;@*/
{
  free (ogp);
  return glob;
}

# if 0
static int sglob;
static /*@only@*/ int *sogp;

int f1 (void) /*@globals undef sglob, killed sogp;@*/
{
  free (sogp);
  return sglob;
}

# endif
