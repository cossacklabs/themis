# include "bool.h"

typedef struct s_abst *abst;

typedef struct 
{
  char *name;
  int val;
} *immut;

struct s_abst
{
  char *name;
  int   val;
  /*@dependent@*/ abst  parent;
  immut im;
} ;


# include "repexpose.lh"
