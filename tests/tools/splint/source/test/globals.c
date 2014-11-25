int imp;
/*@checked@*/ int check;
/*@unchecked@*/ int uncheck;
/*@checkedstrict@*/ int checkstrict;
/*@checked@*/ /*@checked@*/ int doubled; /* 1. Redundant checked qualifier on doubled */
/*@checked@*/ /*@unchecked@*/ int contradict; /* 2. Contradictory checked and unchecked qualifiers on contradict */

void noglobs (void)
{
  imp++;         /* if +globunspec +allglobals */
  check++;       /* if +globunspec */
  uncheck++;
  checkstrict++; /* 3. Unauthorized use of global checkstrict */
}

void specglobs (void) /*@*/
{
  imp++;   /* +  if +allglobals, +impcheckedglobals */
  check++; /* 4. Unauthorized use of global check */
  uncheck++;
  checkstrict++; /* 5. Unauthorized use of global checkstrict */
}
