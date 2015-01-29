typedef /*@abstract@*/ /*@null@*/ char *mstring;

static mstring mstring_createNew (int x) ;

mstring mstring_space1 (void)
{
  mstring m = mstring_createNew (1);

  /* error, since m could be NULL */
  *m = ' '; *(m + 1) = '\0';
  return m;
}

static /*@notnull@*/ mstring mstring_createNewNN (int x) ;

mstring mstring_space2 (void)
{
  mstring m = mstring_createNewNN (1);

  /* no error, because of notnull annotation */
  *m = ' '; *(m + 1) = '\0';
  return m;
}
