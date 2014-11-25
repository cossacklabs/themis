typedef struct 
{
  /*@only@*/ char *name;
  int   val;
} *stx;

/*@only@*/ char *stx_name (stx x)
{
  return (x->name); /* 1. Function returns reference to parameter x: (x->name)
		    ** [2]. Return value exposes rep of stx: (x->name)
		    ** 3. Released storage x->name reachable from parameter 
		    */
}

/*@observer@*/ char *stx_observeName (stx x)
{
  return (x->name); 
}

/*@exposed@*/ char *stx_exposeName (stx x)
{
  return (x->name); /* okay */
}

char *f (stx x)
{
  char *s;

  s = stx_name (x);
  free (s);  /* okay */

  s = stx_observeName (x);
  *s = 'x'; /* 4. Modification of observer */
  free (s); /* 5. Pass observer as only */

  s = stx_exposeName (x);
  *s = 'x'; /* okay */
  free (s); /* 6. Pass exposed as only */

  s = stx_observeName (x);
  return s; /* 7. Observer storage s returned without qualification: s
	    ** 8. Dependent storage s returned as unqualified: s
	    */
}







