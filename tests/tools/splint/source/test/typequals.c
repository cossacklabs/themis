# include "tq.h"

void f (tam xtam, tai xtai, tm xtm, ti xti)
   /*@modifies xtam, xtai, xtm, xti@*/
{
  *xtam = 3; /* 4. Operand of * is abstract type (tam): *xtam */
  *xtai = 4; /* 5. Operand of * is abstract type (tai): *xtai */
  *xtm = 5;
  *xti = 8;
}
