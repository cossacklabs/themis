# include "modifies.h"

static int mstat;
static /*@unused@*/ int internalState;

int f3 (int p[]) 
   /*@modifies internalState, fileSystem;@*/ ; /* 1. Modifies list uses ... */

int f4 (int p[]) 
   /*@modifies p[3];@*/;

int f5 (int fileSystem) 
   /*@modifies fileSystem;@*/ ; /* 2. Modifies list uses fileSystem ... */

int f6 (void);

int f6 (void) /*@modifies mstat;@*/ /* 3. Implementation modifies list for ... */
{ 
  return (mstat++);
}

int f1 (/*@unused@*/ int p[])
{
  mstat++; /* 4. Suspect modification of mstat: mstat++ */
  return mstat;
}

int f2 (/*@unused@*/ int p[]) /*@modifies mstat;@*/
{
  mstat++;
  return 3;
}

int g2 (/*@unused@*/ int p[]) 
{
  return 3;
} /* 5. Function g2 specified to modify internal state but no internal */
