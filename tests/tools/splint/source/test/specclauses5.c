/*@-paramuse@*/ 

typedef struct 
{
  int id;
  char *name;
} *record;

/*@special@*/ record newrecord (void)
  /*@defines result@*/
  /*@post:isnull result->name@*/
{
  record r = (record) malloc (sizeof (*r));

  assert (r != NULL);
  r->id = 3;
  r->name = NULL;
  return r;
}

record createrecord (/*@only@*/ char *name)
{
  record r = newrecord ();
  r->name = name;
  return r;
}

record createrecord2 (void)
{
  record r = newrecord ();
  return r; /* 1. Null storage r->name derivable from return value: r */
}

/*@special@*/ record newrecord2 (void)
  /*@defines *result@*/
  /*@post:observer result->name@*/
{
  record r = (record) malloc (sizeof (*r));

  assert (r != NULL);
  r->id = 3;
  r->name = NULL;
  return r; /* 2. Non-observer storage r->name corresponds to storage ... */
}

record createrecordx (void)
{
  record r = newrecord2 ();
  return r; /* 3. Observer storage r->name reachable from observer return */
}


