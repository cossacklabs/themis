/*
** This is not a good implementation.  I should probably replace
** the erc with a hash table.  
*/

# include "ereftab.h"

ereftab ereftab_create (void) 
{
  return erc_create ();
}

void ereftab_insert (ereftab t, employee e, eref er) 
{
  eref_assign (er, e);
  erc_insert (t, er);
}

bool ereftab_delete (ereftab t, eref er) 
{
  bool result;
  
  result = erc_member (er, t);  
  erc_delete (t, er);
  return result;
}

eref ereftab_lookup (employee e, ereftab t) 
{
  employee e1;

  ereftab_elements(t, er)
    {
      e1 = eref_get(er);
      if (employee_equal(&e, &e1)) return er;
    } end_ereftab_elements ;
  return erefNIL;
}

void ereftab_initMod (void) 
{
  bool_initMod ();
  eref_initMod ();
  erc_initMod ();
}

