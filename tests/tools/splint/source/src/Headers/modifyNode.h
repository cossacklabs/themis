/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef struct {
  ltoken tok;
  bool modifiesNothing;
  bool hasStoreRefList; 
  /* if hasStoreRefList is FALSE, then it's either NOTHING or ANYTHING */
  /* evs - this is a really kludgy way of doing this... */
  /*@reldef@*/ storeRefNodeList list;
} *modifyNode;

extern /*@unused@*/ /*@only@*/ cstring modifyNode_unparse (/*@null@*/ modifyNode p_m);











