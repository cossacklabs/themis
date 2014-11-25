# include <stdio.h>
# include <stdlib.h>
# include "eref.h"

eref_ERP eref_Pool;            /* private */
static bool needsInit = TRUE;  /* private */

void eref_initMod (void) 
{
  int i;
  const int size = 16;
  
  if (!needsInit) 
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
      exit (EXIT_FAILURE);
    }
  
  eref_Pool.status = (eref_status *) malloc (size * sizeof (eref_status));

  if (eref_Pool.status == 0) 
    {
      printf ("Malloc returned null in eref_initMod\n");
      exit (EXIT_FAILURE);
    }
  
  eref_Pool.size = size;

  if (size > 3)
    {
      eref_Pool.status[0] = avail;
    }
  else
    {
      ;
    }
}

