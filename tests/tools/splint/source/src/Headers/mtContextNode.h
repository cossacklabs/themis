/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** mtContextNode.h
*/

# ifndef MTCONTEXTNODE_H
# define MTCONTEXTNODE_H

typedef enum
{
  MTC_ANY = 0, MTC_PARAM, MTC_RESULT, MTC_REFERENCE, MTC_CLAUSE, MTC_LITERAL, MTC_NULL
} mtContextKind ;

/*@constant int MTC_NUMCONTEXTS@*/
/*@+enumint@*/
# define MTC_NUMCONTEXTS (MTC_NULL + 1)
/*@=enumint@*/

struct s_mtContextNode {
  mtContextKind context;
  ctype type; /* This state only applies to types matching. */
} ;

/*@constant null mtContextNode mtContextNode_undefined@*/
# define mtContextNode_undefined ((mtContextNode) 0)

extern /*@falsewhennull@*/ bool mtContextNode_isDefined (mtContextNode p_s) /*@*/ ;
# define mtContextNode_isDefined(s) ((s) != mtContextNode_undefined)

extern cstring mtContextNode_unparse (mtContextNode) /*@*/ ;

extern mtContextNode mtContextNode_createAny (void) /*@*/ ;
extern mtContextNode mtContextNode_createParameter (ctype) /*@*/ ;
extern mtContextNode mtContextNode_createReference (ctype) /*@*/ ;
extern mtContextNode mtContextNode_createResult (ctype) /*@*/ ;
extern mtContextNode mtContextNode_createClause (ctype) /*@*/ ;
extern mtContextNode mtContextNode_createNull (ctype) /*@*/ ;
extern mtContextNode mtContextNode_createLiteral (ctype) /*@*/ ;

extern void mtContextNode_free (/*@only@*/ mtContextNode) ;

extern bool mtContextNode_isReference (mtContextNode) /*@*/;
extern bool mtContextNode_isResult (mtContextNode) /*@*/;
extern bool mtContextNode_isParameter (mtContextNode) /*@*/;
extern bool mtContextNode_isClause (mtContextNode) /*@*/;
extern bool mtContextNode_isLiteral (mtContextNode) /*@*/;
extern bool mtContextNode_isNull (mtContextNode) /*@*/;

extern bool mtContextNode_matchesEntry (mtContextNode, uentry) /*@*/ ;
extern bool mtContextNode_matchesRef (mtContextNode, sRef) /*@*/ ;

extern bool mtContextNode_matchesRefStrict (mtContextNode, sRef) /*@*/ ;
   /* Doesn't allow matches with unknown type. */

extern void mtContextNode_showRefError (mtContextNode, sRef) ;

# else
# error "Multiple include"
# endif
