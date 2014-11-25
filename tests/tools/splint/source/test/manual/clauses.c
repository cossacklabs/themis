typedef struct {
  int id;
  /*@only@*/ char *name;
} *record;

static /*@special@*/ record record_new (void)
     /*@defines result->id@*/
{ 
  record r = (record) malloc (sizeof (*r));
  assert (r != NULL); 
  r->id = 3;
  return r;
}

static void record_setName (/*@special@*/ record r, /*@only@*/ char *name)
     /*@defines r->name@*/
{
  r->name = name;
}

record record_create (/*@only@*/ char *name)
{ 
  record r = record_new ();  
  record_setName (r, name);
  return r;
}

void record_clearName (/*@special@*/ record r)
     /*@releases r->name@*/
     /*@ensures isnull r->name@*/
{ 
  free (r->name); 
  r->name = NULL;
}

void record_free (/*@only@*/ record r)
{
  record_clearName (r);
  free (r);
}
