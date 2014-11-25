typedef struct 
{
  int id;
  char *name;
} *record;

int *badResult (void)  
  /* 1. Function result is used in defines clause of badResult but ...
     2. Special clause accesses field of non-struct or union result */
   /*@defines result->name@*/ 
{
  return NULL; /* 3. Null storage returned as non-null: NULL */
} 

/*@special@*/ record createName (/*@only@*/ char *name)
   /*@defines result->name@*/ 
{
  record res = (record) malloc (sizeof (*res));

  assert (res != NULL);
  res->name = name;
  return res;
}

/*@special@*/ record createName2 (void)
   /*@defines result->name@*/ 
{
  record res = (record) malloc (sizeof (*res));
  return res; /* 4. Undefined storage res->name corresponds to storage listed ...
		 5. Possibly null storage res returned as non-null: res */
}

/*@special@*/ record createName3 (void)
   /*@defines result->id@*/ 
   /*@allocates result->name@*/
{
  record res = (record) malloc (sizeof (*res));

  assert (res != NULL);

  res->name = (char *) malloc (sizeof (char) * 23);
  res->id = 21;
  return res;
}

/*@special@*/ record createName4 (void)
   /*@defines result->id@*/ 
   /*@allocates result->name@*/
{
  record res = (record) malloc (sizeof (*res));

  assert (res != NULL);

  res->id = 21;
  return res; /* 6. Unallocated storage res->name corresponds to storage ... */
}





