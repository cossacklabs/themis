/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef enum
{
  LPD_PLAIN, LPD_CHECKS, LPD_REQUIRES, LPD_ENSURES,
  LPD_INTRACLAIM, LPD_CONSTRAINT, LPD_INITIALLY
} lclPredicateKind;

struct s_lclPredicateNode {
  ltoken tok; /* for debugging */
  lclPredicateKind kind;
  termNode predicate;
} ;


