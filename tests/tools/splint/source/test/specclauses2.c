/*@-paramuse@*/

typedef struct
{
  char *name;
  char *id;
  int year;
} record;

extern void allocYear (/*@special@*/ record *r)
  /*@allocates r->year@*/  /* 1. Allocates clause includes *<parameter 1>.year of ... */
{
  r->year = 23;
} /* 2. Unallocated storage r->year corresponds to storage listed ... */

extern void setName (/*@special@*/ record *r, /*@only@*/ char *name)
  /*@defines r->name@*/ 
{
  r->name = name;
}

extern void setName1 (/*@special@*/ record *r, /*@only@*/ char *name)
  /*@defines r->name@*/ 
{
  free (name);
} /* 3. Storage r->name listed in defines clause not defined at ... */

extern void setName2 (/*@special@*/ record *r, char *name)
  /*@sets r->name@*/ 
{
  strcpy (r->name, name);
}

extern void setName3 (/*@special@*/ record *r, char *name)
  /*@sets r->name@*/ 
{
} /* 4. Storage *(r->name) listed in sets clause not defined at ... */

extern void setName4 (/*@special@*/ record *r, /*@only@*/ char *name)
  /*@sets r->name@*/ 
{
  r->name = name; /* 5. Implicitly only storage r->name not released before ... */
}

extern void allocName (/*@special@*/ record *r)
  /*@allocates r->name@*/ 
{
  r->name = (char *) malloc (sizeof (char) * 20);
}

extern void allocName2 (/*@special@*/ record *r)
  /*@allocates r->name@*/ 
{
} /* 6. Storage r->name listed in allocates clauses is not ... */

extern void freeName (/*@special@*/ record r)
  /*@releases r.name@*/
{
  free (r.name); 
}

extern void freeName2 (/*@special@*/ record r)
  /*@releases r.name@*/
{
} /* 7. Storage r.name listed in releases clause not released */

extern void freeName3 (/*@special@*/ record *r)
  /*@releases r->name@*/
{
  free (r->name);
}

extern void freeName4 (/*@special@*/ record *r)
  /*@releases r->name@*/
{
} /* 8. Storage r->name listed in releases clause not released ... */
