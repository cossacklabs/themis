/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** symtable.h
*/

# ifndef SYMTABLE_H
# define SYMTABLE_H

/* There are 3 separate namespaces being maintained in this symbol
table: (N1) for C objects, functions, typedef names and enumeration
constants.  (N2) for C tags.  (N3) for LSL operators.  I re-use the
old linked list to represent N1 since it is here and it is convenient
for representing scopes.  Two more new hashtables are added to store
N2 and N3, each keyed on the lsymbol of the identifier. */

/*@constant observer char *BEGINSYMTABLE;@*/
# define BEGINSYMTABLE "%LCLSymbolTable"

/*@constant observer char *SYMTABLEEND;@*/
# define SYMTABLEEND "%LCLSymbolTableEnd"

typedef long unsigned symbolKey;

typedef struct {
  ltoken id;
  bool	export;            /* all HOF maps to one sort kind: SRT_HOF */
  /*@only@*/ signNode signature;      /* in terms of sorts, no HOF */
  /*@only@*/ pairNodeList globals;    /* list of (sort globalnameSym) */
} *fctInfo;

typedef struct {
    ltoken	id;
    sort	basedOn; 
    bool	abstract;   /* TRUE means LCL abstract type */
    bool	modifiable; /* TRUE indicates the abstract type is mutable */
    bool	export;     /* if TRUE, export it from this module */
} *typeInfo;

typedef enum 
{
  VRK_CONST,  /* an LCL constant */
  VRK_ENUM,   /* an enumeration constant, 
		 a special kind of LCL constant */
  VRK_VAR,    /* an LCL variable */
  VRK_PRIVATE,/* a variable private to current function/module */
              /* function private overrides module private, usual scoping
		 rules */
  VRK_GLOBAL, /* in globals of a function */ 
  VRK_LET,    /* local let variable in a function decl */
  VRK_PARAM,  /* formal parameters of a function/claim */
  VRK_QUANT   /* bound by a quantifier */
} varKind;

typedef struct {
    ltoken	id;
    sort	sort;
    varKind	kind;
    bool	export; /* if TRUE, export it from this module */
} *varInfo;

/* The same lsymbol can originate from different files:
   an LCL file or an LSL file.  Each occurrence in an LCL file appears
   as one of {opId, anyOp, logicalOp, simpleOp, eqOp, MULOP} ltoken. 
   Each occurrence in an LSL file appears as an lslOp AST. */ 

typedef struct {
  /*@only@*/ nameNode name;
  /*@only@*/ sigNodeSet signatures;
} *opInfo;

typedef struct {
    ltoken	id;
    tagKind     kind; 
    bool        imported;
    sort sort;
    /*@reldef@*/ union
      {
	/*@null@*/ /*@dependent@*/ ltokenList enums;
        /*@null@*/ stDeclNodeList decls;
      } content;
} *tagInfo;

typedef enum {
  IK_SORT, IK_OP, IK_TAG
  } infoKind;

typedef struct {
  infoKind kind;
  union {
    tagInfo tag;
    sort sort;
    opInfo op;} content;
} htData;

typedef enum { 
  SPE_GLOBAL, SPE_FCN, SPE_QUANT, SPE_CLAIM,
  SPE_ABSTRACT, SPE_INVALID
  } scopeKind;

typedef struct {
    scopeKind	kind;
} *scopeInfo;

/*
** A simple hash table implementation: Given a key K and a 
** pointer P to a piece of data, associate K with P.
** 
** Assumes that K is unsigned int, and data supports
**   3 operations: 
**   data_key: htData * -> unsigned int
*/

typedef struct s_htEntry {
  /*@only@*/ htData *data;
  /*@only@*/ struct s_htEntry *next;
} htEntry;

/* Each bucket is a linked list of entries */

typedef htEntry bucket;
typedef /*@relnull@*/ /*@only@*/ bucket *o_bucket;

typedef struct {
  unsigned int count;
  unsigned int size;
  /*@relnull@*/ /*@only@*/ o_bucket *buckets; 
} symHashTable;

/* Local to implementation */

typedef struct s_symtableStruct *symtable;

extern /*@falsewhennull@*/ bool typeInfo_exists(/*@null@*/ typeInfo p_ti);
# define typeInfo_exists(ti)  ((ti) != NULL)

extern /*@falsewhennull@*/ bool varInfo_exists(/*@null@*/ varInfo p_vi);
# define varInfo_exists(vi)   ((vi) != NULL)

extern /*@falsewhennull@*/ bool tagInfo_exists(/*@null@*/ tagInfo p_oi);
# define tagInfo_exists(ti)   ((ti) != NULL)

extern /*@falsewhennull@*/ bool opInfo_exists(/*@null@*/ opInfo p_oi);
# define opInfo_exists(oi)    ((oi) != NULL)

extern /*@only@*/ symtable symtable_new (void) /*@*/ ;
extern void symtable_enterScope (symtable p_stable, /*@keep@*/ scopeInfo p_si);
extern void symtable_exitScope(symtable p_stable);

extern bool symtable_enterFct (symtable p_stable, /*@only@*/ fctInfo p_fi);
extern void symtable_enterType (symtable p_stable, /*@only@*/ typeInfo p_ti);

/* not only --- it is copied! */
extern bool symtable_enterVar (symtable p_stable, /*@temp@*/ varInfo p_vi);
extern void symtable_enterOp (symtable p_st,
			      /*@only@*/ /*@notnull@*/ nameNode p_n, 
			      /*@owned@*/ sigNode p_oi);
extern bool symtable_enterTag (symtable p_st, /*@only@*/ tagInfo p_ti);
extern bool symtable_enterTagForce(symtable p_st, /*@only@*/ tagInfo p_ti);

extern bool symtable_exists(symtable p_stable, lsymbol p_i);

extern /*@observer@*/ /*@null@*/ typeInfo symtable_typeInfo(symtable p_stable, lsymbol p_i);
extern /*@observer@*/ /*@null@*/ varInfo symtable_varInfo(symtable p_stable, lsymbol p_i);
extern /*@observer@*/ /*@null@*/ varInfo symtable_varInfoInScope(symtable p_stable, lsymbol p_id);
extern /*@observer@*/ /*@null@*/ opInfo symtable_opInfo(symtable p_st, 
							/*@notnull@*/ nameNode p_n);
extern /*@observer@*/ /*@null@*/ tagInfo symtable_tagInfo(symtable p_st, lsymbol  p_i);

extern void symtable_export(symtable p_stable, bool p_yesNo);
extern void symtable_dump(symtable p_stable, FILE *p_f, bool p_lco);

extern void symtable_import(inputStream p_imported, ltoken p_tok, mapping p_map);

extern /*@unused@*/ void symtable_printStats (symtable p_s);

extern lsymbol lsymbol_sortFromType (symtable, lsymbol);
extern cstring tagKind_unparse (tagKind p_k);
extern lsymbol lsymbol_translateSort (mapping p_m, lsymbol p_s);
extern void varInfo_free (/*@only@*/ varInfo p_v);

extern /*@only@*/ lslOpSet 
  symtable_opsWithLegalDomain (symtable p_tab, /*@temp@*/ /*@null@*/ nameNode p_n, 
			       sortSetList p_argSorts, sort p_q);
extern /*@observer@*/ sigNodeSet 
  symtable_possibleOps (symtable p_tab, nameNode p_n);
extern bool symtable_opExistsWithArity(symtable p_tab, nameNode p_n, int p_arity);

extern void symtable_free (/*@only@*/ symtable p_stable);

# else
# error "Multiple include"
# endif

