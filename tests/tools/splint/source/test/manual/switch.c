typedef enum {
  YES, NO, DEFINITELY,
  PROBABLY, MAYBE } ynm;

void decide (ynm y)
{
  switch (y)
    {
    case PROBABLY:
    case NO: printf ("No!");
    case MAYBE: printf ("Maybe");    
         /*@fallthrough@*/
    case YES: printf ("Yes!");
    }
}
