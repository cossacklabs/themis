# include "bool.h"
# include "pivo.h"

int pivo_pyet;
int michelob_light; /* bad */
pivo budlight;

/*@constant int pivo_devenast;@*/
# define pivo_devenast 12

bool pivo_yedno (pivo p)
{
  /* pivo is accessible if +accessczech */

  return (p == 1);
}

/* okay, since no types are accessible */
int samAdams (int x)
{
  return x;
}

/*@access pivo@*/

int budweiser (int x) /* definitely an error! */
{
  return x;
}

pivo pivo_budvar (int x) /* that's better! */
{
  return x;
}
  



