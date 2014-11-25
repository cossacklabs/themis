/* ereftab.h  */

# ifndef EREFTAB_H
# define EREFTAB_H

# include "erc.h"
# include "eref.h"

typedef erc ereftab;

# include "ereftab.lh"
# define ereftab_elements(s, m_x) erc_elements(s, m_x)
# define end_ereftab_elements     end_erc_elements

# endif

