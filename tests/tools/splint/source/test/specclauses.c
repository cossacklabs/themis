typedef struct
{
  char *name;
  char *id;
  int year;
} record;

extern void setName (/*@special@*/ record *r, /*@only@*/ char *name)
  /*@defines r->name@*/ ;

extern /*@observer@*/ char *getName (/*@special@*/ record r)
  /*@uses r.name@*/ ;

extern void freeName (/*@special@*/ record r)
  /*@releases r.name@*/ ;

extern /*@observer@*/ char *f (/*@only@*/ char *name, char *id)
{
  record r;

  if (0 == 1)
    {
      setName (&r, name);
      return (getName (r)); /* r.name not released */
    }
  else if (1 == 2)
    {
      return (getName (r)); /* r.name not defined */
    }
  else if (2 == 3)
    {
      r.name = malloc (sizeof (char) * 12);
      setName (&r, name); /* r.name allocated, memory leak */
      return id; /* r.name not released */
    }
  else
    {
      setName (&r, name);
      freeName (r);
      printf ("%s\n", r.name); /* dead! */
      freeName (r);
      return id;
    }
}

