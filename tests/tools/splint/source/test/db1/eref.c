# include <stdio.h>
# include <stdlib.h>
# include "eref.h"

eref_ERP eref_Pool;            /* private */
static bool needsInit = TRUE;  /* private */

eref eref_alloc (void) 
{
  int i, res;
  
  for (i=0; (eref_Pool.status[i] == used) && (i < eref_Pool.size); i++);
  
  res = i;

  if (res == eref_Pool.size) 
    {
      eref_Pool.conts =
	(employee *) realloc (eref_Pool.conts,
			      2 * eref_Pool.size * sizeof (employee));
      
      if (eref_Pool.conts == 0) 
	{
	  printf ("Malloc returned null in eref_alloc\n");
	  exit (1);
	}
      
      eref_Pool.status =
	(eref_status *) realloc (eref_Pool.status,
				 2 * eref_Pool.size * sizeof (eref_status));

      if (eref_Pool.status == 0) 
	{
	  printf ("Malloc returned null in eref_alloc\n");
	  exit (1);
	}
      
      eref_Pool.size = 2*eref_Pool.size;

      for (i = res+1; i < eref_Pool.size; i++)
        eref_Pool.status[i] = avail;
    }
  
  eref_Pool.status[res] = used;
  return (eref) res;
}

void eref_initMod (void) 
{
  int i;
  const int size = 16;
  
  if (needsInit == false) /* will produce a warning if FALSE is used instead */
    {
      return;
    }

  needsInit = FALSE;
  bool_initMod ();
  employee_initMod ();

  eref_Pool.conts = (employee *) malloc (size * sizeof (employee));

  if (eref_Pool.conts == 0) 
    {
      printf ("Malloc returned null in eref_initMod\n");
      exit (1);
    }
  
  eref_Pool.status = (eref_status *) malloc (size * sizeof (eref_status));

  if (eref_Pool.status == 0) 
    {
      printf ("Malloc returned null in eref_initMod\n");
      exit (1);
    }
  
  eref_Pool.size = size;

  for (i = 0; i < size; i++)
    {
      eref_Pool.status[i] = avail;
    }
}



