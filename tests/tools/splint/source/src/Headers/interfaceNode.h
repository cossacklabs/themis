/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef enum {
  INF_IMPORTS, INF_USES, 
  INF_EXPORT, INF_PRIVATE
} interfaceNodeKind;

typedef struct {
  interfaceNodeKind kind; /* which kind of interface node */
  union { 
    /* evs 8 Sept 1993 */
    importNodeList imports;     /* a list of simpleId or TYPEDEF_NAME */
    traitRefNodeList uses;      /* a list of traitRef */
    exportNode export;     
    privateNode private;
  } content; /* can be empty */
  /* the list that made up <interface> is chained thru next */
} *interfaceNode;

extern /*@unused@*/ cstring interfaceNode_unparse (interfaceNode p_x);
extern void interfaceNode_free (/*@null@*/ /*@only@*/ interfaceNode p_x);


