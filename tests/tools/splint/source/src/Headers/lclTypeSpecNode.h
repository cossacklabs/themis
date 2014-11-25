/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

typedef enum 
{
  LTS_TYPE, LTS_STRUCTUNION,
  LTS_ENUM, LTS_CONJ
} lclTypeSpecKind;

typedef lclTypeSpecNode lcltsp;

typedef struct
{
  lcltsp a;
  lcltsp b;
} *lclconj;

struct s_lclTypeSpecNode {
  lclTypeSpecKind kind;
  qualList quals;
  union {
    /*@null@*/ CTypesNode type;
    /*@null@*/ strOrUnionNode structorunion;
    /*@null@*/ enumSpecNode enumspec;
    lclconj conj;
  } content;
  pointers pointers;
} ;

/*@constant null lclTypeSpecNode lclTypeSpecNode_undefined; @*/
# define lclTypeSpecNode_undefined ((lclTypeSpecNode) 0)

extern /*@falsewhennull@*/ bool lclTypeSpecNode_isDefined (lclTypeSpecNode p_x) /*@*/ ;
# define lclTypeSpecNode_isDefined(x) ((x) != lclTypeSpecNode_undefined)

extern /*@null@*/ /*@only@*/ lclTypeSpecNode
  lclTypeSpecNode_copy (/*@null@*/ lclTypeSpecNode p_n) /*@*/ ;

extern /*@only@*/ cstring 
  lclTypeSpecNode_unparse (/*@null@*/ lclTypeSpecNode p_n) /*@*/ ;

extern /*@only@*/ cstring 
  lclTypeSpecNode_unparseComments (/*@null@*/ lclTypeSpecNode p_typespec) /*@*/ ;






