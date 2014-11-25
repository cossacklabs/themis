#include <stdlib.h>
# define x 23

void * utl_calloc
(
    const size_t num,
    const size_t size
)
{
    void *newblock = NULL;

    newblock = calloc(num, size);

#
    return newblock;
}

