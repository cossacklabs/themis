# ifndef EMPSET_H
# define EMPSET_H

# include "eref.h"
# include "erc.h"
# include "ereftab.h"

typedef erc empset;

ereftab known;

/*
  Abstraction function, toEmpSet:
    e \in toEmpSet(s) == 
      exists er (count(er, s.val) = 1
        /\ getERef(known, e) = er)

  Rep invariant:
    forall s: empset
      (forall er: eref (count(er, s.val) <= 1)
      /\ s.activeIters = 0
      /\ forall er: eref (count(er, s.val) = 1
        => in(known, er)))
*/

# include "empset.lh"

# define empset_create()  (erc_create())
# define empset_final(s) (erc_final(s))
# define empset_member(e, s) \
         (!(eref_equal(_empset_get(e, s), erefNIL)))
# define empset_size(es) (erc_size(es))
# define empset_choose(es) (eref_get(erc_choose(es)))
# define empset_sprint(es) (erc_sprint(es))

# endif
