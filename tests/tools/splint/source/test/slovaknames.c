# include "bool.h"
# include "pivo.h"

int pivoPyet;
int michelobLight; /* bad */
pivo budlight;

/*@constant int pivoDevenast;@*/
# define pivoDevenast 12

bool pivoYedno (pivo p)
{
  /* pivo is accessible if +accessczech */

  return (p == 1);
}

/* no types are accessible (error for +slovakfcns) */
int samAdams (int x)
{
  return x;
}

/* no types are accessible (okay for +slovakfcns) */
int samuel_adams (int x)
{
  return x;
}

/*@access pivo@*/

int budweiser (int x) /* definitely an error! */
{
  return x;
}

pivo pivoBudvar (int x) /* that's better! */
{
  return x;
}
  



