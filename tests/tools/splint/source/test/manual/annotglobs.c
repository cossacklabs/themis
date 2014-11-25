int globnum;

struct { 
  char *firstname, *lastname;
  int id; 
} globname;

void 
initialize (/*@only@*/ char *name)
  /*@globals undef globnum,
             undef globname @*/
{
  globname.id = globnum;
  globname.lastname = name;
}
 
void finalize (void)
   /*@globals killed globname@*/
{
  free (globname.lastname);
}
