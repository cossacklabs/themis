# include "immutable.h"

void testimmutable (void)
{
  immutable i1 = immutable_create (3);
  immutable_print (i1);
}
