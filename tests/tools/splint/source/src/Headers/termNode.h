/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/

struct s_termNode 
{
  int wrapped; /* the number of matched parentheses around this term */
  termKIND kind; 
  sort sort;
  /*@reldef@*/ sort given; /* if term has a given sort */
  sortSet possibleSorts;
  bool error_reported; /* to prevent multiple sort checking error messages */
  lslOpSet possibleOps; /* only for TRM_ZEROARY and TRM_APPLICATION */
  /* should make a union but skip this for now */
  /*@null@*/ nameNode name;
  termNodeList args; /* in order */
  /*@reldef@*/ ltoken literal; /* for TRM_LITERAL, TRM_UNCHANGEDALL, 
		       TRM_SIZEOF, LCLvariableKIND, 
		       TRM_CONST, TRM_ZEROARY */
  /*@reldef@*/ storeRefNodeList unchanged; /* only for TRM_UNCHANGEDOTHERS */
  /*@reldef@*/ quantifiedTermNode quantified; /* only for TRM_QUANTIFIER */
  /*@reldef@*/ lclTypeSpecNode sizeofField; /* only for TRM_SIZEOF */
} ;

extern /*@falsewhennull@*/ bool termNode_isDefined (/*@null@*/ termNode p_t) /*@*/ ;
# define termNode_isDefined(t) ((t) != NULL)

extern termNode termNode_copySafe (termNode p_t) /*@*/ ;
extern cstring termNode_unparse (/*@null@*/ termNode p_n) /*@*/ ;
extern void termNode_free (/*@only@*/ /*@null@*/ termNode p_t);
