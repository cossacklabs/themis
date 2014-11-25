# include <stdio.h>
# include <stdlib.h>
# include "eref.h"

typedef enum { ST_USED, ST_AVAIL } erefStatus;
typedef struct {
  /*@reldef@*/ /*@only@*/ employee *conts;
  /*@only@*/ erefStatus *status;
  int size;
} erefTable;

static erefTable eref_Pool;     /* private */
static bool needsInit = TRUE;  /* private */

eref eref_alloc (void) 
   /*@globals eref_Pool@*/
   /*@modifies eref_Pool@*/
{
  int i, res;
  
  for (i=0; (eref_Pool.status[i] == ST_USED) && (i < eref_Pool.size); i++)
    {
      ;
    }
  
  res = i;

  if (res == eref_Pool.size) 
    {
      eref_Pool.conts =
	(employee *) realloc (eref_Pool.conts,
			      2 * eref_Pool.size * sizeof (*eref_Pool.conts));
      
      if (eref_Pool.conts == 0) 
	{
	  printf ("Malloc returned null in eref_alloc\n");
	  exit (EXIT_FAILURE);
	}
      
      eref_Pool.status =
	(erefStatus *) realloc (eref_Pool.status,
				 2 * eref_Pool.size * sizeof (*eref_Pool.status));

      if (eref_Pool.status == 0) 
	{
	  printf ("Malloc returned null in eref_alloc\n");
	  exit (EXIT_FAILURE);
	}
      
      eref_Pool.size = 2*eref_Pool.size;

      for (i = res+1; i < eref_Pool.size; i++)
	{
	  eref_Pool.status[i] = ST_AVAIL;
	}
    }
  
  eref_Pool.status[res] = ST_USED;
  return (eref) res;
}

void eref_initMod (void) 
   /*@globals undef eref_Pool, needsInit, internalState@*/
   /*@modifies eref_Pool, needsInit, internalState@*/
{
  int i;
  const int size = 16;
  
  if (!needsInit) 
    {
      /*@-compdef@*/ return; /*@=compdef@*/
    }

  needsInit = FALSE;
  bool_initMod ();
  employee_initMod ();

  eref_Pool.conts = (employee *) malloc (size * sizeof (*eref_Pool.conts));

  if (eref_Pool.conts == 0) 
    {
      printf ("Malloc returned null in eref_initMod\n");
      exit (EXIT_FAILURE);
    }
  
  eref_Pool.status = (erefStatus *) malloc (size * sizeof (*eref_Pool.status));

  if (eref_Pool.status == 0) 
    {
      printf ("Malloc returned null in eref_initMod\n");
      exit (EXIT_FAILURE);
    }
  
  eref_Pool.size = size;

  /*@+loopexec@*/
  for (i = 0; i < size; i++)
    {
      eref_Pool.status[i] = ST_AVAIL;
    }
  /*@=loopexec@*/
}

void eref_free (eref er)
  /*@globals eref_Pool@*/
  /*@modifies eref_Pool@*/
{
  eref_Pool.status[er] = ST_AVAIL;
}

void eref_assign (eref er, employee e) 
  /*@globals eref_Pool@*/
  /*@modifies eref_Pool@*/
{
  eref_Pool.conts[er] = e;
}

employee eref_get (eref er) 
   /*@globals eref_Pool@*/
{
  return eref_Pool.conts[er];
}
