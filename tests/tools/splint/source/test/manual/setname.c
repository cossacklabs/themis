# include "setname.h"

void setName (employee e)
 /*@modifies e->name@*/
{
  strcpy (e->name, "");
}
