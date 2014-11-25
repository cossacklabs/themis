# include "intSet.h"

extern intSet intSet_insert  
   (/*@returned@*/ intSet s, int x);

/*
** with returned, no error is reported, since the same
** storage is returned from insert, and returned 
** from intSet_singleton as an implicit only result.
*/
 
intSet intSet_singleton (int x)
{
   return (intSet_insert (intSet_new (), x));
 }
 
/*
** without returned:
*/

extern intSet intSet_insert2 (intSet s, int x);
 
intSet intSet_singleton2 (int x)
{
  return (intSet_insert2 (intSet_new (), x));
}


