# include "employee.h"
 
char *
employee_getName (employee e) 
{ 
  return e->name; 
}

/*@observer@*/ char *
employee_obsName (employee e) 
{ return e->name; }
 
/*@exposed@*/ char *
employee_exposeName (employee e)
{ return e->name; }
 
void 
employee_capName (employee e)
{
  char *name;
  
  name = employee_obsName (e);
  *name = toupper (*name);
}


