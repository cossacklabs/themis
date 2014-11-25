/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef struct {
  typeNameNodeList namelist;  /* the positional arguments */
  replaceNodeList replacelist; /* the rest of replace renamings */  
} *nameAndReplaceNode;

typedef struct {
  bool is_replace; /* TRUE means replace, false means name */ 
  union {
    replaceNodeList replace;
    nameAndReplaceNode name;
  } content;
} *renamingNode;

extern /*@only@*/ cstring renamingNode_unparse (/*@null@*/ renamingNode p_x);
