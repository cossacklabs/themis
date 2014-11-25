/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef struct {
  quantifierNodeList quantifiers;
  ltoken open; /* for ease of debugging */
  termNode body;
  ltoken close; /* for ease of debugging */
} *quantifiedTermNode;

