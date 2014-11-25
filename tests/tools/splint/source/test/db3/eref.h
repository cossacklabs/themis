# ifndef F_EREF_H
# define F_EREF_H

# include "employee.h"

typedef int eref;

# include "eref.lh"

# define eref_undefined -1

# define eref_isDefined(e)    ((e) != eref_undefined)
# define eref_equal(er1, er2) ((er1) == (er2))

# endif

