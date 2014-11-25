#include <stdlib.h>			/* free, EXIT_SUCCESS */

typedef int Bool;	
#define False	0

static void CheckQualsSub(Bool Found);

void CheckQuals(void)
{
    Bool B = False;
    
    CheckQualsSub (B);
}
