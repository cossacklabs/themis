/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** importNode.h
**
** needs "ltoken_type.h"
*/

typedef enum { IMPPLAIN, IMPBRACKET, IMPQUOTE } impkind ;

typedef struct {
  impkind   kind;
  ltoken  val;
} *importNode ;

extern void importNode_free (/*@only@*/ /*@null@*/ importNode p_x);

extern /*@only@*/ importNode importNode_makePlain (/*@only@*/ ltoken p_t);
extern /*@only@*/ importNode importNode_makeBracketed (/*@only@*/ ltoken p_t);
extern /*@only@*/ importNode importNode_makeQuoted (/*@only@*/ ltoken p_t);
