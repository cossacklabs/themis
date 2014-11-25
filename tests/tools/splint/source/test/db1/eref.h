# ifndef EREF_H
# define EREF_H

# include "employee.h"

typedef int eref;

/* Private typedefs used in macros  */
typedef enum { used, avail } eref_status;
typedef struct {
  employee *conts;
  eref_status *status;
  int size;
} eref_ERP;

/* Declared here so that macros can use it  */
extern eref_ERP eref_Pool;

# include "eref.lh"

# define erefNIL -1

# define eref_free(er)        (eref_Pool.status[er] = avail)
# define eref_assign(er, e)   (eref_Pool.conts[er] = e) 
# define eref_get(er)         (eref_Pool.conts[er])
# define eref_equal(er1, er2) (er1 == er2) 

# endif
