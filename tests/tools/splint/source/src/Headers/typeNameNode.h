/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/

typedef struct {
  bool isTypeName;
  /*@null@*/ typeNamePack typename;
  /*@null@*/ opFormNode opform;  
} *typeNameNode;

extern void typeNameNode_free (/*@only@*/ /*@null@*/ typeNameNode p_n);
extern /*@only@*/ cstring typeNameNode_unparse (/*@null@*/ typeNameNode p_n);
