unsigned int ui;
long int li;
long unsigned short si; /* 1. contradictory qualifiers */

long int f (unsigned int x)
{
  long int loli = 12;
  int loc = 3;

  loc = f (li);    /* 2, 3. pass long as unsigned, assign unsigned to int */
  loc = f (ui);    /* 4. assign unsigned to int */
  loc = f (loc);   /* 5, 6. pass int as unsigned, assign unsigned to int */

  loli = loc;      /* [7. if -relaxquals] */
  return x;        /* 7. return unsigned int as long int */
}

unsigned int x1 = 0;
unsigned int x2 = 23;
unsigned int x3 = -15; /* 8. assign -15 to unsigned */
unsigned int x4 = -15U; 
long int x5 = 0L;
long int x6 = 0;
short int x7 = 14; 

