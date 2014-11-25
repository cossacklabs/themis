/*;-*-C-*-;
** Splint - annotation-assisted static program checker
** Copyright (C) 1994-2003 University of Virginia,
**         Massachusetts Institute of Technology
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or (at your
** option) any later version.
** 
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** General Public License for more details.
** 
** The GNU General Public License is available from http://www.gnu.org/ or
** the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
** MA 02111-1307, USA.
**
** For information on splint: splint@cs.virginia.edu
** To report a bug: splint-bug@cs.virginia.edu
** For more information: http://www.splint.org
*/
/*
** Original author: Yang Meng Tan, Massachusetts Institute of Technology
*/
%{

# include "splintMacros.nf"
# include "basic.h"
# include "lclscan.h"
# include "checking.h"
# include "lslparse.h" 
# include "lh.h"
# include "usymtab_interface.h"

/*@-noparams@*/
static /*@unused@*/ void yyprint ();
/*@=noparams@*/

/*@-redecl@*/
void ylerror (char *) /*@modifies *g_warningstream@*/ ;
/*@=redecl@*/

bool g_inTypeDef = FALSE;

/*@constant int YYDEBUG;@*/
# define YYDEBUG 1

/*@notfunction@*/
# define YYPRINT(file, type, value) yyprint (file, type, value)

/*
** This is necessary, or else when the bison-generated code #include's malloc.h,
** there will be a parse error.
**
** Unfortunately, it means the error checking on malloc, etc. is lost for allocations
** in bison-generated files under Win32.
*/

# ifdef WIN32
# undef malloc
# undef calloc
# undef realloc
# endif

%}

/*@-readonlytrans@*/

%union 
{
  ltoken ltok;  /* a leaf is also an ltoken */
  qual typequal;
  unsigned int count;
  /*@only@*/ ltokenList ltokenList;
  /*@only@*/ abstDeclaratorNode abstDecl; 
  /*@only@*/ declaratorNode declare;
  /*@only@*/ declaratorNodeList declarelist;
  /*@only@*/ typeExpr typeexpr;
  /*@only@*/ arrayQualNode array;
  /*@only@*/ quantifierNode quantifier;
  /*@only@*/ quantifierNodeList quantifiers;
  /*@only@*/ varNode var;
  /*@only@*/ varNodeList vars;
  /*@only@*/ storeRefNode storeref;
  /*@only@*/ storeRefNodeList storereflist;
  /*@only@*/ termNode term;
  /*@only@*/ termNodeList termlist;
  /*@only@*/ programNode program; 
  /*@only@*/ stmtNode stmt;
  /*@only@*/ claimNode claim;
  /*@only@*/ typeNode type;
  /*@only@*/ iterNode iter;
  /*@only@*/ fcnNode fcn;
  /*@only@*/ fcnNodeList fcns;
  /*@only@*/ letDeclNode letdecl;
  /*@only@*/ letDeclNodeList letdecls;
  /*@only@*/ lclPredicateNode lclpredicate;
  /*@only@*/ modifyNode modify;
  /*@only@*/ paramNode param;
  /*@only@*/ paramNodeList paramlist;
  /*@only@*/ declaratorInvNodeList declaratorinvs;	
  /*@only@*/ declaratorInvNode declaratorinv;	
  /*@only@*/ abstBodyNode abstbody;
  /*@only@*/ abstractNode abstract;
  /*@only@*/ exposedNode exposed;
  /*@only@*/ pointers pointers;
  /*    taggedUnionNode taggedunion; */
  /*@only@*/ globalList globals;
  /*@only@*/ constDeclarationNode constdeclaration;
  /*@only@*/ varDeclarationNode vardeclaration;
  /*@only@*/ varDeclarationNodeList vardeclarationlist;
  /*@only@*/ initDeclNodeList initdecls;
  /*@only@*/ initDeclNode initdecl;
  /*@only@*/ stDeclNodeList structdecls;
  /*@only@*/ stDeclNode structdecl;
  /*@only@*/ strOrUnionNode structorunion;
  /*@only@*/ enumSpecNode enumspec; 
  /*@only@*/ lclTypeSpecNode lcltypespec;
  /*@only@*/ typeNameNode typname;
  /*@only@*/ opFormNode opform;
  /*@only@*/ sigNode signature;
  /*@only@*/ nameNode name;
  /*@only@*/ typeNameNodeList namelist;
  /*@only@*/ replaceNode replace;	
  /*@only@*/ replaceNodeList replacelist;
  /*@only@*/ renamingNode renaming;
  /*@only@*/ traitRefNode traitref;
  /*@only@*/ traitRefNodeList traitreflist;
  /*@only@*/ importNode import;
  /*@only@*/ importNodeList importlist;
  /*@only@*/ interfaceNode iface;
  /*@only@*/ interfaceNodeList interfacelist; 
  /*@only@*/ CTypesNode ctypes;
  /*@-redef@*/
} /*@=redef@*/

/* Order of precedence is increasing going down the list */

%left     simpleOp
%right    PREFIX_OP 
%left     POSTFIX_OP 
%left     LLT_MULOP    
  /* as arithmetic binary operator, or as iteration construct in claims */
%left     LLT_SEMI
%left     LLT_VERTICALBAR
%nonassoc ITERATION_OP /* two * cannot follow each other */
%left     LLT_LPAR LLT_LBRACKET selectSym 
  /* to allow mixing if-then-else with other kinds of terms */
%left     LLT_IF_THEN_ELSE 
%left     logicalOp

/* Note: the grammar parses b = p /\ q as (b = p) /\ q, that is,
    = has higher precedence than logicalOp.  
   Reminder: = > logicalOp >= if_then_else > == (present in LSL) */

/* Precedence of claim operators: ( > * > | >; (| and; left-assoc) */

/* These are not needed in the grammar, 
   but needed in init files and lclscanline.c */

%token <ltok> eqSepSym          /* \eqsep */
%token <ltok> equationSym     /* \equals or == */
%token <ltok> commentSym        /* \comment */
%token <ltok> LLT_WHITESPACE
%token <ltok> LLT_EOL  /*@-varuse@*/  /* yacc declares yytranslate here */
/* used to bypass parsing problems in C types */
%token <ltok> LLT_TYPEDEF_NAME /*@=varuse@*/

/* LSL reserved extension symbols */

%token <ltok> quantifierSym     /* \forall */
%token <ltok> logicalOp         /* \implies, \and, \not, \or */
%token <ltok> selectSym         /* \select */
%token <ltok> openSym           /* \( */
%token <ltok> closeSym          /* \) */
%token <ltok> sepSym            /* \, */

%token <ltok> simpleId          /* \: id-char +, Ordinary Identifier */
%token <ltok> mapSym            /* \arrow, -> */
%token <ltok> markerSym         /* \marker, __ */
%token <ltok> preSym            /* \pre */
%token <ltok> postSym           /* \post */
%token <ltok> anySym            /* \any */

/* Generic LSL operators */

%token <ltok> simpleOp          /* opSym - reserved */

/* Reserved special symbols */

%token <ltok> LLT_COLON              /* : */
%token <ltok> LLT_COMMA              /* , */
%token <ltok> LLT_EQUALS             /* = */
%token <ltok> LLT_LBRACE             /* { */
%token <ltok> LLT_RBRACE             /* } */
%token <ltok> LLT_LBRACKET           /* [ */
%token <ltok> LLT_RBRACKET           /* ] */
%token <ltok> LLT_LPAR               /* ( */
%token <ltok> LLT_RPAR               /* ) */
%token <ltok> LLT_QUOTE              /* ' */
%token <ltok> LLT_SEMI               /*; */
%token <ltok> LLT_VERTICALBAR        /* | */

/* C operator tokens and Combined C/LSL operator tokens */

%token <ltok> eqOp                /* \eq, \neq, ==, != */
%token <ltok> LLT_MULOP               /* * */

/* LCL C literal tokens */

%token <ltok> LLT_CCHAR
%token <ltok> LLT_CFLOAT
%token <ltok> LLT_CINTEGER
%token <ltok> LLT_LCSTRING

/* LCL reserved words */

%token <ltok> LLT_ALL
%token <ltok> LLT_ANYTHING
%token <ltok> LLT_BE
%token <ltok> LLT_BODY
%token <ltok> LLT_CLAIMS
%token <ltok> LLT_CHECKS
%token <ltok> LLT_CONSTANT
%token <ltok> LLT_ELSE
%token <ltok> LLT_ENSURES
%token <ltok> LLT_FOR
%token <ltok> LLT_FRESH
%token <ltok> LLT_IF
%token <ltok> LLT_IMMUTABLE
%token <ltok> LLT_IMPORTS
%token <ltok> LLT_CONSTRAINT /* was INVARIANT */
%token <ltok> LLT_ISSUB
%token <ltok> LLT_LET
%token <ltok> LLT_MODIFIES
%token <ltok> LLT_MUTABLE
%token <ltok> LLT_NOTHING
%token <ltok> LLT_INTERNAL
%token <ltok> LLT_FILESYS
%token <ltok> LLT_OBJ
%token <ltok> LLT_OUT
%token <ltok> LLT_SEF
%token <ltok> LLT_ONLY LLT_PARTIAL LLT_OWNED LLT_DEPENDENT LLT_KEEP LLT_KEPT LLT_TEMP 
%token <ltok> LLT_SHARED LLT_UNIQUE LLT_UNUSED
%token <ltok> LLT_EXITS LLT_MAYEXIT LLT_NEVEREXIT LLT_TRUEEXIT LLT_FALSEEXIT
%token <ltok> LLT_UNDEF LLT_KILLED
%token <ltok> LLT_CHECKMOD LLT_CHECKED LLT_UNCHECKED LLT_CHECKEDSTRICT
%token <ltok> LLT_TRUENULL
%token <ltok> LLT_FALSENULL
%token <ltok> LLT_LNULL
%token <ltok> LLT_LNOTNULL
%token <ltok> LLT_RETURNED
%token <ltok> LLT_OBSERVER
%token <ltok> LLT_EXPOSED
%token <ltok> LLT_REFCOUNTED
%token <ltok> LLT_REFS
%token <ltok> LLT_RELNULL
%token <ltok> LLT_RELDEF
%token <ltok> LLT_KILLREF
%token <ltok> LLT_NULLTERMINATED
%token <ltok> LLT_TEMPREF
%token <ltok> LLT_NEWREF
%token <ltok> LLT_PRIVATE
%token <ltok> LLT_REQUIRES
%token <ltok> LLT_RESULT
%token <ltok> LLT_SIZEOF
%token <ltok> LLT_SPEC
%token <ltok> LLT_TAGGEDUNION  /* keep it for other parts of LCL checker */
%token <ltok> LLT_THEN
%token <ltok> LLT_TYPE
%token <ltok> LLT_TYPEDEF
%token <ltok> LLT_UNCHANGED
%token <ltok> LLT_USES

/* LCL C keywords */

%token <ltok> LLT_CHAR
%token <ltok> LLT_CONST
%token <ltok> LLT_DOUBLE
%token <ltok> LLT_ENUM
%token <ltok> LLT_FLOAT
%token <ltok> LLT_INT
%token <ltok> LLT_ITER
%token <ltok> LLT_YIELD
%token <ltok> LLT_LONG
%token <ltok> LLT_SHORT
%token <ltok> LLT_SIGNED
%token <ltok> LLT_UNKNOWN
%token <ltok> LLT_STRUCT
%token <ltok> LLT_TELIPSIS
%token <ltok> LLT_UNION
%token <ltok> LLT_UNSIGNED
%token <ltok> LLT_VOID
%token <ltok> LLT_VOLATILE

%token <ltok> LLT_PRINTFLIKE LLT_SCANFLIKE LLT_MESSAGELIKE

%type <interfacelist> interface externals optDeclarations declarations
%type <iface> external declaration imports uses export private private2
%type <type> type 
%type <fcn> fcn
%type <fcns> fcns
%type <claim> claim 
%type <iter> iter
%type <vardeclaration> varDeclaration globalDecl privateInit 
%type <globals> globals
%type <ltokenList> interfaceNameList traitIdList domain sortList 
%type <import> importName
%type <importlist> importNameList
%type <traitreflist> traitRefNodeList 
%type <traitref> traitRef 
%type <renaming> renaming
%type <namelist> nameList 
%type <name> name 
%type <replacelist> replaceNodeList
%type <replace> replace
%type <opform> opForm
%type <signature> signature
%type <typname> typeName
%type <count> middle placeList 
%type <pointers> pointers 
%type <abstDecl> optAbstDeclarator 
%type <lcltypespec> lclTypeSpec lclType sortSpec
%type <ltokenList> enumeratorList postfixOps
%type <ctypes> CTypes typeSpecifier
%type <structorunion> structOrUnionSpec 
%type <enumspec> enumSpec
%type <declare> declarator 
%type <typeexpr> notype_decl after_type_decl abstDeclarator parameter_decl 
%type <declarelist> declaratorList
%type <structdecls> structDecls 
%type <structdecl> structDecl
%type <constdeclaration> constDeclaration
%type <initdecls> initDecls 
%type <initdecl> initDecl 
%type <vardeclarationlist> privateInits
%type <abstract> abstract
%type <exposed> exposed 
%type <declaratorinvs> declaratorInvs
%type <declaratorinv> declaratorInv
%type <abstbody> abstBody optExposedBody 
%type <lclpredicate> optClaim optEnsure optRequire optChecks lclPredicate 
%type <lclpredicate> optTypeInv typeInv 
%type <modify> optModify
%type <letdecls> optLetDecl beDeclList 
%type <letdecl> beDecl 
%type <term> term constLclExpr initializer value equalityTerm 
%type <term> simpleOpTerm prefixOpTerm secondary primary lclPrimary 
%type <term> bracketed sqBracketed matched term0 cLiteral
%type <termlist> args infixOpPart valueList termList
%type <program> optBody callExpr
%type <stmt> stmt 
%type <storereflist> storeRefList 
%type <storeref> storeRef  
%type <var> quantified 
%type <vars> quantifiedList
%type <quantifier> quantifier 
%type <quantifiers> quantifiers
%type <array> arrayQual 
%type <paramlist> optParamList paramList realParamList iterParamList realIterParamList
%type <param> param iterParam
%type <ltok> open close anyOp separator simpleOp2 stateFcn 
%type <ltok> interfaceName 
%type <ltok> varId fcnId tagId claimId sortId traitId opId CType optTagId
%type <ltok> simpleIdOrTypedefName
%type <typequal> specialQualifier special

/*
** Conventions in calling static semantic routines:
**   The inputs are in strict order (in AST) as well as in the position of
**   the call to the static semantic routine. 
*/ 

/*@=redef@*/
/*@=readonlytrans@*/

%%

interface
 : externals { lhExternals ($1); } optDeclarations 
   { interfaceNodeList_free ($1); interfaceNodeList_free ($3); }
;

externals   
 : /* empty */        { $$ = interfaceNodeList_new (); }
 | externals external { $$ = interfaceNodeList_addh ($1, $2);}  
;

external
 : imports
 | uses
;

optDeclarations    
 : /* empty */              { $$ = interfaceNodeList_new (); }
 | export declarations      { $$ = consInterfaceNode ($1, $2);}  
 | private declarations     { $$ = consInterfaceNode ($1, $2);}  
;

declarations 
 : /* empty */              { $$ = interfaceNodeList_new (); }
 | declarations declaration { $$ = interfaceNodeList_addh ($1, $2);}  
;

declaration
 : export
 | private
 | uses 
;

imports   
 : LLT_IMPORTS importNameList LLT_SEMI 
   { $$ = makeInterfaceNodeImports ($2);
     /* assume subspecs are already processed, symbol table info in external file */
   }
;

uses   
 : LLT_USES traitRefNodeList LLT_SEMI  
   { $$ = makeInterfaceNodeUses ($2); readlsignatures ($$);}
;

export
 : constDeclaration
   { declareConstant ($1); $$ = interfaceNode_makeConst ($1); }
 | varDeclaration
   { declareVar ($1); $$ = interfaceNode_makeVar ($1); }
 | type
   { declareType ($1); $$ = interfaceNode_makeType ($1); }
 | fcn
   { declareFcn ($1, typeId_invalid); $$ = interfaceNode_makeFcn ($1); }
 | claim
   { $$ = interfaceNode_makeClaim ($1); }
 | iter
   { declareIter ($1); $$ = interfaceNode_makeIter ($1); }                              
;

iter 
 : LLT_ITER varId LLT_LPAR iterParamList LLT_RPAR LLT_SEMI
   { $$ = makeIterNode ($2, $4); }
;
 
iterParamList      
 : /* empty */         { $$ = paramNodeList_new (); }
 | realIterParamList   { $$ = $1; }
;
 
realIterParamList  
 : iterParam           
   { $$ = paramNodeList_add (paramNodeList_new (),  $1); }
 | realIterParamList LLT_COMMA iterParam  
   { $$ = paramNodeList_add ($1,$3); }     
;

iterParam          
 : LLT_YIELD param            { $$ = markYieldParamNode ($2); }
 | param                  { $$ = $1; }
;

private   
 : LLT_SPEC { symtable_export (g_symtab, FALSE); } private2 
   { $$ = $3; symtable_export (g_symtab, TRUE); } 
;

private2 
 : constDeclaration
   { declarePrivConstant ($1); $$ =  interfaceNode_makePrivConst ($1); } 
 | varDeclaration
   { declarePrivVar ($1); $$ = interfaceNode_makePrivVar ($1); }
 | type 
   { declarePrivType ($1); $$ = interfaceNode_makePrivType ($1); }
 | fcn
   { declarePrivFcn ($1, typeId_invalid); $$ = interfaceNode_makePrivFcn ($1); }
;

constDeclaration   
 : LLT_CONSTANT lclTypeSpec initDecls LLT_SEMI
   { $$ = makeConstDeclarationNode ($2, $3); } 
;

varDeclaration    
 : lclTypeSpec initDecls LLT_SEMI
   { $$ = makeVarDeclarationNode ($1, $2, FALSE, FALSE); $$->qualifier = QLF_NONE; } 
 | LLT_CONST lclTypeSpec initDecls LLT_SEMI
   { $$ = makeVarDeclarationNode ($2, $3, FALSE, FALSE); $$->qualifier = QLF_CONST; } 
 | LLT_VOLATILE lclTypeSpec initDecls LLT_SEMI
   { $$ = makeVarDeclarationNode ($2, $3, FALSE, FALSE); $$->qualifier = QLF_VOLATILE; }
;

type
 : abstract                     { $$ = makeAbstractTypeNode ($1); } 
 | exposed                      { $$ = makeExposedTypeNode ($1); } 
;

special
 : LLT_PRINTFLIKE  { $$ = qual_createPrintfLike (); }
 | LLT_SCANFLIKE   { $$ = qual_createScanfLike (); }
 | LLT_MESSAGELIKE { $$ = qual_createMessageLike (); }
;

fcn   
 : lclTypeSpec declarator globals { enteringFcnScope ($1, $2, $3); } LLT_LBRACE
   privateInits optLetDecl optChecks optRequire optModify optEnsure optClaim LLT_RBRACE
   { $$ = makeFcnNode (qual_createUnknown (),  $1, $2, $3, $6, $7, 
		       $8, $9, $10, $11, $12); 
     /* type, declarator, glovbls, privateinits,
	lets, checks, requires, modifies, ensures, claims */
     symtable_exitScope (g_symtab);
   }
 | special lclTypeSpec declarator globals { enteringFcnScope ($2, $3, $4); }
   LLT_LBRACE
   privateInits optLetDecl optChecks optRequire optModify optEnsure optClaim 
   LLT_RBRACE
   { $$ = makeFcnNode ($1, $2, $3, $4, $7, 
		       $8, $9, $10, $11, $12, $13); 
     /* type, declarator, glovbls, privateinits,
	lets, checks, requires, modifies, ensures, claims */
     symtable_exitScope (g_symtab);
   }
;

claim 
 : LLT_CLAIMS claimId LLT_LPAR optParamList LLT_RPAR globals
   { enteringClaimScope ($4, $6); } 
   LLT_LBRACE optLetDecl optRequire optBody optEnsure LLT_RBRACE
   {      $$ = makeClaimNode ($2, $4, $6, $9, $10, $11, $12); 
     symtable_exitScope (g_symtab); } 
 | LLT_CLAIMS fcnId claimId LLT_SEMI
   { $$ = (claimNode) 0; }
;

abstract
 : LLT_MUTABLE {g_inTypeDef = TRUE; } LLT_TYPE LLT_TYPEDEF_NAME {g_inTypeDef = FALSE; } abstBody
   { $$ = makeAbstractNode ($1, $4, TRUE, FALSE, $6); } 
 | LLT_MUTABLE {g_inTypeDef = TRUE; } LLT_REFCOUNTED LLT_TYPE 
   LLT_TYPEDEF_NAME {g_inTypeDef = FALSE; } abstBody
   { $$ = makeAbstractNode ($1, $5, TRUE, TRUE, $7); } 
 | LLT_REFCOUNTED LLT_MUTABLE {g_inTypeDef = TRUE; } LLT_TYPE 
   LLT_TYPEDEF_NAME {g_inTypeDef = FALSE; } abstBody
   { $$ = makeAbstractNode ($2, $5, TRUE, TRUE, $7); } 
 | LLT_IMMUTABLE {g_inTypeDef = TRUE; } LLT_TYPE LLT_TYPEDEF_NAME {g_inTypeDef = FALSE; } abstBody
   { $$ = makeAbstractNode ($1, $4, FALSE, FALSE, $6); } 
;

exposed
 : LLT_TYPEDEF lclTypeSpec { g_inTypeDef = TRUE; setExposedType ($2); } declaratorInvs
   { g_inTypeDef = FALSE; } LLT_SEMI
   { $$ = makeExposedNode ($1, $2, $4); /* to support mutually recursive types */ }
 | structOrUnionSpec LLT_SEMI
   { $$ = makeExposedNode ($2, makeLclTypeSpecNodeSU ($1), declaratorInvNodeList_new ()); }
 | enumSpec LLT_SEMI
   { $$ = makeExposedNode ($2, makeLclTypeSpecNodeEnum ($1), declaratorInvNodeList_new ()); }

/* evs - 26 Feb 1995 (changed to be consistent with C grammar)
 | STRUCT tagId LLT_SEMI
   { (void) checkAndEnterTag (TAG_FWDSTRUCT, ltoken_copy ($2));
     lhForwardStruct ($2); $$ = (exposedNode)0;
   }
 | UNION tagId LLT_SEMI
   { (void) checkAndEnterTag (TAG_FWDUNION, ltoken_copy ($2));
     lhForwardUnion ($2);
     $$ = (exposedNode)0; 
   }
*/
;

importNameList  
 : importName        
   { $$ = importNodeList_add (importNodeList_new (),  $1); } 
 | importNameList LLT_COMMA importName  
   { $$ = importNodeList_add ($1, $3); } 
;

importName       
 : interfaceName      { $$ = importNode_makePlain ($1); }
 | simpleOp interfaceName simpleOp
   { checkBrackets ($1, $3); $$ = importNode_makeBracketed ($2); }
 | LLT_LCSTRING           { $$ = importNode_makeQuoted ($1); } 
;

interfaceNameList  
 : interfaceName                         { $$ = ltokenList_singleton ($1); } 
 | interfaceNameList LLT_COMMA interfaceName { $$ = ltokenList_push ($1, $3); }
;

interfaceName   
 : simpleIdOrTypedefName
   /* to allow module names to be the same as LCL type names */
;

traitRefNodeList   
 : traitRef
   { $$ = traitRefNodeList_add (traitRefNodeList_new (),  $1); } 
 | traitRefNodeList LLT_COMMA traitRef
   { $$ = traitRefNodeList_add ($1, $3); } 
;

traitRef   
 : traitId
   { $$ = makeTraitRefNode (ltokenList_singleton ($1), (renamingNode)0); } 
 | traitId LLT_LPAR renaming LLT_RPAR
   { $$ = makeTraitRefNode (ltokenList_singleton ($1), $3); } 
 | LLT_LPAR traitIdList LLT_RPAR
   { $$ = makeTraitRefNode ($2, (renamingNode)0); } 
 | LLT_LPAR traitIdList LLT_RPAR LLT_LPAR renaming LLT_RPAR
   { $$ = makeTraitRefNode ($2, $5); } 
;

traitIdList   
 : traitId                     { $$ = ltokenList_singleton ($1); } 
 | traitIdList LLT_COMMA traitId   { $$ = ltokenList_push ($1, $3); } 
;

renaming   
 : replaceNodeList   
   { $$ = makeRenamingNode (typeNameNodeList_new (),  $1); } 
 | nameList
   { $$ = makeRenamingNode ($1, replaceNodeList_new ()); } 
 | nameList LLT_COMMA replaceNodeList { $$ = makeRenamingNode ($1, $3); } 
;
 
nameList
 : typeName
   { $$ = typeNameNodeList_add (typeNameNodeList_new (),  $1); } 
 | nameList LLT_COMMA typeName       { $$ = typeNameNodeList_add ($1, $3); } 
;

replaceNodeList   
 : replace
   { $$ = replaceNodeList_add (replaceNodeList_new (),  $1); } 
 | replaceNodeList LLT_COMMA replace { $$ = replaceNodeList_add ($1, $3); } 
;

replace
 : typeName LLT_FOR CType            { $$ = makeReplaceNode ($2, $1, TRUE, $3, NULL, NULL); } 
 | typeName LLT_FOR name             { $$ = makeReplaceNameNode ($2, $1, $3); }
 | typeName LLT_FOR name signature   { $$ = makeReplaceNode ($2, $1, FALSE, ltoken_undefined,
							 $3, $4); } 
;

name    
 : opId                          { $$ = makeNameNodeId ($1); } 
 | opForm                        { $$ = makeNameNodeForm ($1); } 
;

initializer : constLclExpr
;

constLclExpr : term
;

initDecls
 : initDecl 
   { $$ = initDeclNodeList_add (initDeclNodeList_new (),  $1); } 
 | initDecls LLT_COMMA initDecl      
   { $$ = initDeclNodeList_add ($1, $3); } 
;

initDecl  
 : declarator                    { $$ = makeInitDeclNode ($1, (termNode)0); } 
 | declarator LLT_EQUALS initializer { $$ = makeInitDeclNode ($1, $3); } 
;

globals   
 : /* empty */ /* has the same structure */
   { $$ = varDeclarationNodeList_new (); }
 | globals globalDecl
   { varDeclarationNodeList_addh ($1, $2); $$ = $1; }
;

globalDecl   
 : lclTypeSpec initDecls LLT_SEMI    { $$ = makeVarDeclarationNode ($1, $2, TRUE, FALSE); } 
 | LLT_INTERNAL LLT_SEMI                 { $$ = makeInternalStateNode (); }
 | LLT_FILESYS LLT_SEMI                  { $$ = makeFileSystemNode (); }
;

privateInits   
 : /* empty */                  { $$ = varDeclarationNodeList_new (); }
 | privateInits privateInit     { varDeclarationNodeList_addh ($1, $2); $$ = $1; }
;

privateInit   
 : LLT_PRIVATE lclTypeSpec initDecls LLT_SEMI
   { $$ = makeVarDeclarationNode ($2, $3, FALSE, TRUE); } 
;

optLetDecl   
 : /* empty */                 { $$ = letDeclNodeList_new (); }
 | LLT_LET beDeclList LLT_SEMI         { $$ = $2; } 
;

beDeclList   
 : beDecl                      { $$ = letDeclNodeList_add (letDeclNodeList_new (),  $1); } 
 | beDeclList LLT_COMMA beDecl     { $$ = letDeclNodeList_add ($1, $3); } 
;

beDecl   
 : varId LLT_COLON sortSpec LLT_BE term { $$ = makeLetDeclNode ($1, $3, $5); } 
 | varId                LLT_BE term { $$ = makeLetDeclNode ($1, (lclTypeSpecNode)0, $3); } 
;

sortSpec : lclTypeSpec
;

optChecks   
 : /* empty */                  { $$ = (lclPredicateNode)0; }
 | LLT_CHECKS lclPredicate LLT_SEMI     { checkLclPredicate ($1, $2); $$ = makeChecksNode ($1, $2); }
;

optRequire
 : /* empty */                  { $$ = (lclPredicateNode)0; }
 | LLT_REQUIRES lclPredicate LLT_SEMI   { checkLclPredicate ($1, $2); $$ = makeRequiresNode ($1, $2);} 
;

optModify
 : /* empty */                  { $$ = (modifyNode)0; }
 | LLT_MODIFIES LLT_NOTHING LLT_SEMI        { $$ = makeModifyNodeSpecial ($1, TRUE); } 
 | LLT_MODIFIES LLT_ANYTHING LLT_SEMI       { $$ = makeModifyNodeSpecial ($1, FALSE); } 
 | LLT_MODIFIES storeRefList LLT_SEMI   { $$ = makeModifyNodeRef ($1, $2); } 
;

storeRefList   
 : storeRef                     { $$ = storeRefNodeList_add (storeRefNodeList_new (),  $1); } 
 | storeRefList LLT_COMMA storeRef  { $$ = storeRefNodeList_add ($1, $3); } 
;

storeRef   
 : term                         { $$ = makeStoreRefNodeTerm ($1); } 
 | lclType                      { $$ = makeStoreRefNodeType ($1, FALSE); } 
 | LLT_OBJ lclType                  { $$ = makeStoreRefNodeType ($2, TRUE); } 
 | LLT_INTERNAL                     { $$ = makeStoreRefNodeInternal (); }
 | LLT_FILESYS                      { $$ = makeStoreRefNodeSystem (); }
;

optEnsure
 : /* empty */                  { $$ = (lclPredicateNode)0; }
 | LLT_ENSURES lclPredicate LLT_SEMI    { checkLclPredicate ($1, $2); $$ = makeEnsuresNode ($1, $2);} 
;

optClaim   
 : /* empty */                  { $$ = (lclPredicateNode)0; }
 | LLT_CLAIMS lclPredicate LLT_SEMI     { checkLclPredicate ($1, $2); $$ = makeIntraClaimNode ($1, $2);} 
;

optParamList       
 : /* empty */                  { $$ = paramNodeList_new (); }
 | realParamList                { $$ = $1; }
;

realParamList      
 : paramList
 | LLT_TELIPSIS                  { $$ = paramNodeList_add (paramNodeList_new (),  paramNode_elipsis ()); }
 | paramList LLT_COMMA LLT_TELIPSIS  { $$ = paramNodeList_add ($1, paramNode_elipsis ()); }
;

paramList   
 : param                     { $$ = paramNodeList_single ($1); }
 | paramList LLT_COMMA param     { $$ = paramNodeList_add ($1, $3); } 
;

optBody
 : /* empty */                      { $$ = (programNode)0; }
 | LLT_BODY LLT_LBRACE callExpr LLT_RBRACE      { $$ = $3; } 
 | LLT_BODY LLT_LBRACE callExpr LLT_SEMI LLT_RBRACE { $$ = $3; } 
;

callExpr   
 : stmt                             { $$ = makeProgramNode ($1); } 
 | LLT_LPAR callExpr LLT_RPAR 
   /* may need to make a copy of $2 */
   { $$ = $2; $$->wrapped = $$->wrapped + 1; } 
 | callExpr LLT_MULOP        %prec ITERATION_OP
   { programNodeList x = programNodeList_new ();
     programNodeList_addh (x, $1);
     $$ = makeProgramNodeAction (x, ACT_ITER); 
   } 
 | callExpr LLT_VERTICALBAR callExpr  
   { programNodeList x = programNodeList_new ();
     programNodeList_addh (x, $1);
     programNodeList_addh (x, $3);
     $$ = makeProgramNodeAction (x, ACT_ALTERNATE); 
   } 
 | callExpr LLT_SEMI callExpr   
   { programNodeList x = programNodeList_new ();
     programNodeList_addh (x, $1);
     programNodeList_addh (x, $3);
     $$ = makeProgramNodeAction (x, ACT_SEQUENCE); 
   } 
;

stmt               
 : fcnId LLT_LPAR valueList LLT_RPAR 
   { $$ = makeStmtNode (ltoken_undefined, $1, $3); } 
 | fcnId LLT_LPAR LLT_RPAR
   { $$ = makeStmtNode (ltoken_undefined, $1, termNodeList_new ()); }
 | varId LLT_EQUALS fcnId LLT_LPAR LLT_RPAR
   { $$ = makeStmtNode ($1, $3, termNodeList_new ()); } 
 | varId LLT_EQUALS fcnId LLT_LPAR valueList LLT_RPAR
   { $$ = makeStmtNode ($1, $3, $5); }
;

valueList   
 : value                 { $$ = termNodeList_push (termNodeList_new (),  $1); } 
 | valueList LLT_COMMA value { $$ = termNodeList_push ($1, $3); } 
;

value   
 : cLiteral
 | varId                            { $$ = makeSimpleTermNode ($1); } 
 | simpleOp value %prec PREFIX_OP   { $$ = makePrefixTermNode ($1, $2); } 
 | value simpleOp  %prec POSTFIX_OP { $$ = makePostfixTermNode2 ($1, $2); } 
 | value simpleOp value /* infix */ { $$ = makeInfixTermNode ($1, $2, $3); }
 | LLT_LPAR value LLT_RPAR                  { $$ = $2; $$->wrapped = $$->wrapped + 1; }
 | fcnId LLT_LPAR LLT_RPAR
   { $$ = makeOpCallTermNode ($1, $2, termNodeList_new (),  $3); }
 | fcnId LLT_LPAR valueList LLT_RPAR
   { $$ = makeOpCallTermNode ($1, $2, $3, $4); } 
;

abstBody   
 : LLT_SEMI                                  { $$ = (abstBodyNode)0; } 
 | LLT_LBRACE fcns LLT_RBRACE                    { $$ = makeAbstBodyNode ($1, $2); }
 | LLT_LBRACE interfaceNameList LLT_RBRACE LLT_SEMI  { $$ = makeAbstBodyNode2 ($1, $2); }
 | LLT_LBRACE LLT_RBRACE LLT_SEMI                    { $$ = (abstBodyNode)0; }
;

fcns   
 : /* empty */                           { $$ = fcnNodeList_new (); }
 | fcns fcn                              { $$ = fcnNodeList_add ($1, $2); } 
;

optTypeInv   
 : /* empty */                           { $$ = (lclPredicateNode)0; }
 | typeInv
 ;

typeInv   
 : LLT_CONSTRAINT { g_inTypeDef = FALSE; } quantifier LLT_LPAR lclPredicate LLT_RPAR
   { $5->tok = $1; $5->kind = LPD_CONSTRAINT;
     checkLclPredicate ($1, $5);
     $$ = $5;
     symtable_exitScope (g_symtab); 
     g_inTypeDef = TRUE;
   }
;

declaratorInvs     
 : declaratorInv        { $$ = declaratorInvNodeList_add (declaratorInvNodeList_new (),  $1); } 
 | declaratorInvs LLT_COMMA declaratorInv
   { $$ = declaratorInvNodeList_add ($1, $3); } 
;

declaratorInv      
 : declarator { declareForwardType ($1); } optExposedBody
   { $$ = makeDeclaratorInvNode ($1, $3); } 
;

optExposedBody   
 : /* empty */                  { $$ = (abstBodyNode)0; }
 | LLT_LBRACE optTypeInv LLT_RBRACE     { $$ = makeExposedBodyNode ($1, $2); }
;

CType   
 : LLT_VOID           { $$ = $1; ltoken_setIntField ($$, fixBits (TS_VOID, 0)); }
 | LLT_CHAR           { $$ = $1; ltoken_setIntField ($$, fixBits (TS_CHAR, 0)); }
 | LLT_DOUBLE         { $$ = $1; ltoken_setIntField ($$, fixBits (TS_DOUBLE, 0)); }
 | LLT_FLOAT          { $$ = $1; ltoken_setIntField ($$, fixBits (TS_FLOAT, 0)); }
 | LLT_INT            { $$ = $1; ltoken_setIntField ($$, fixBits (TS_INT, 0)); }
 | LLT_LONG           { $$ = $1; ltoken_setIntField ($$, fixBits (TS_LONG, 0)); }
 | LLT_SHORT          { $$ = $1; ltoken_setIntField ($$, fixBits (TS_SHORT, 0)); }
 | LLT_SIGNED         { $$ = $1; ltoken_setIntField ($$, fixBits (TS_SIGNED, 0)); }
 | LLT_UNSIGNED       { $$ = $1; ltoken_setIntField ($$, fixBits (TS_UNSIGNED, 0)); }
 | LLT_UNKNOWN        { $$ = $1; ltoken_setIntField ($$, fixBits (TS_UNKNOWN, 0)); }
;

/*
** CTypes allow "unsigned short int" but we drop all C type qualifiers 
** and storage class except TYPEDEF 
*/

CTypes   
 : CType                       { $$ = makeCTypesNode ((CTypesNode)0, $1); }
 | CTypes CType                { $$ = makeCTypesNode ($1, $2); } 
;

/* Add abstract type names and typedef names */

typeSpecifier   
 : LLT_TYPEDEF_NAME                
   { $$ = makeTypeSpecifier ($1); }
 | CTypes                      
   { $$ = $1; $$->sort = sort_lookupName (lclctype_toSort ($1->intfield)); } 
;

/* Add struct and enum types */

specialQualifier
 : LLT_OUT       { $$ = qual_createOut (); }
 | LLT_UNUSED { $$ = qual_createUnused (); } 
 | LLT_SEF       { $$ = qual_createSef (); }
 | LLT_ONLY      { $$ = qual_createOnly (); }
 | LLT_OWNED     { $$ = qual_createOwned (); }
 | LLT_DEPENDENT { $$ = qual_createDependent (); }
 | LLT_KEEP      { $$ = qual_createKeep (); }
 | LLT_KEPT      { $$ = qual_createKept (); }
 | LLT_OBSERVER  { $$ = qual_createObserver (); }
 | LLT_EXITS     { $$ = qual_createExits (); }
 | LLT_MAYEXIT   { $$ = qual_createMayExit (); }
 | LLT_TRUEEXIT  { $$ = qual_createTrueExit (); }
 | LLT_FALSEEXIT { $$ = qual_createFalseExit (); }
 | LLT_NEVEREXIT { $$ = qual_createNeverExit (); }
 | LLT_TEMP      { $$ = qual_createOnly (); }
 | LLT_SHARED    { $$ = qual_createShared (); }
 | LLT_UNIQUE    { $$ = qual_createUnique (); }
 | LLT_CHECKED   { $$ = qual_createChecked (); }
 | LLT_UNCHECKED { $$ = qual_createUnchecked (); }
 | LLT_CHECKEDSTRICT { $$ = qual_createCheckedStrict (); }
 | LLT_TRUENULL  { $$ = qual_createTrueNull (); }
 | LLT_FALSENULL { $$ = qual_createFalseNull (); }
 | LLT_RELNULL   { $$ = qual_createRelNull (); }
 | LLT_RELDEF    { $$ = qual_createRelDef (); }
 | LLT_REFCOUNTED{ $$ = qual_createRefCounted (); }
 | LLT_REFS      { $$ = qual_createRefs (); }
 | LLT_NEWREF    { $$ = qual_createNewRef (); }
 | LLT_KILLREF   { $$ = qual_createKillRef (); }
 | LLT_LNULL     { $$ = qual_createNull (); }
 | LLT_LNOTNULL  { $$ = qual_createNotNull (); }
 | LLT_RETURNED  { $$ = qual_createReturned (); }
 | LLT_EXPOSED   { $$ = qual_createExposed (); }
 | LLT_PARTIAL   { $$ = qual_createPartial (); }
 | LLT_NULLTERMINATED { $$ = qual_createNullTerminated () ; }
 | LLT_UNDEF { $$ = qual_createUndef (); }
 | LLT_KILLED { $$ = qual_createKilled (); }
;

lclTypeSpec   
 : typeSpecifier                        
   { $$ = makeLclTypeSpecNodeType ($1); } /* does not process CONST at all */
 | structOrUnionSpec                    
   { $$ = makeLclTypeSpecNodeSU ($1); }
 | enumSpec                             
   { $$ = makeLclTypeSpecNodeEnum ($1); }
 | specialQualifier lclTypeSpec         
   { $$ = lclTypeSpecNode_addQual ($2, $1); }
 | LLT_VERTICALBAR lclType LLT_COLON lclType LLT_VERTICALBAR
   { $$ = makeLclTypeSpecNodeConj ($2, $4); } 
;

/* 
** Add pointers to the right, only used in StoreRef, maybe should throw
** out and replace its use in StoreRef by CTypeName 
*/

lclType     
 : lclTypeSpec 
 | lclTypeSpec pointers 
   { llassert (lclTypeSpecNode_isDefined ($1));
     $1->pointers = $2; $$ = $1; }
;

pointers   
 : LLT_MULOP          { $$ = pointers_createLt ($1); }
 | pointers LLT_MULOP { $$ = pointers_extend ($1, pointers_createLt ($2)); }
;

structOrUnionSpec  
 : LLT_STRUCT optTagId 
   { (void) checkAndEnterTag (TAG_FWDSTRUCT, ltoken_copy ($2)); } 
   LLT_LBRACE structDecls LLT_RBRACE
   { $$ = makestrOrUnionNode ($1, SU_STRUCT, $2, $5); }
 | LLT_UNION optTagId  
   { (void) checkAndEnterTag (TAG_FWDUNION, ltoken_copy ($2)); } 
   LLT_LBRACE structDecls LLT_RBRACE
   { $$ = makestrOrUnionNode ($1, SU_UNION, $2, $5); }
 | LLT_STRUCT tagId 
   { $$ = makeForwardstrOrUnionNode ($1, SU_STRUCT, $2); }
 | LLT_UNION tagId  
   { $$ = makeForwardstrOrUnionNode ($1, SU_UNION, $2); }
;

optTagId    
 : /* empty */ { $$ = ltoken_undefined; }
 | tagId
 ;

structDecls   
 : structDecl               { $$ = stDeclNodeList_add (stDeclNodeList_new (),  $1); } 
 | structDecls structDecl   { $$ = stDeclNodeList_add ($1, $2); } 
;

/* We don't allow specification of field size */

structDecl   
 : lclTypeSpec declaratorList LLT_SEMI { $$ = makestDeclNode ($1, $2); } 
;

declaratorList   
 : declarator                       
   { $$ = declaratorNodeList_add (declaratorNodeList_new (),  $1); } 
 | declaratorList LLT_COMMA declarator  
   { $$ = declaratorNodeList_add ($1, $3); } 
;

optCOMMA           
 : /* empty */ { ; }
 | LLT_COMMA       { ; }
;

enumSpec   
 : LLT_ENUM optTagId LLT_LBRACE enumeratorList optCOMMA LLT_RBRACE
   { $$ = makeEnumSpecNode ($1, $2, $4); } 
 | LLT_ENUM tagId
   { $$ = makeEnumSpecNode2 ($1, $2); } 
;

enumeratorList   
 : simpleId                             { $$ = ltokenList_singleton ($1); } 
 | enumeratorList LLT_COMMA simpleId        { $$ = ltokenList_push ($1, $3); } 
;

/* This part of declarator rules is adapted from GCC (2.2.1)'s C grammar */

/* An after_type_decl is a declarator that is allowed only after an explicit 
   typeSpecifier, an id can be typedef'd here.  
   A notype_decl is a declarator that may not have seen a typeSpecifier
   preceding it; it cannot typedef'd an id.  */

declarator   
 : after_type_decl                      { $$ = makeDeclaratorNode ($1); }
 | notype_decl                          { $$ = makeDeclaratorNode ($1); }
;

notype_decl    
 : varId                                { $$ = makeTypeExpr ($1); }
 | LLT_LPAR notype_decl LLT_RPAR        { $$ = $2; $$->wrapped = $$->wrapped + 1; } 
 | LLT_MULOP notype_decl                { $$ = makePointerNode ($1, $2); } 
 | notype_decl arrayQual                { $$ = makeArrayNode ($1, $2); } 
 | notype_decl LLT_LPAR LLT_RPAR                { $$ = makeFunctionNode ($1, paramNodeList_new ()); } 
 | notype_decl LLT_LPAR realParamList LLT_RPAR  { $$ = makeFunctionNode ($1, $3); }
;

after_type_decl    
 : LLT_TYPEDEF_NAME                         { $$ = makeTypeExpr ($1); }
 | LLT_LPAR after_type_decl LLT_RPAR        { $$ = $2; $$->wrapped = $$->wrapped + 1; }
 | LLT_MULOP after_type_decl                { $$ = makePointerNode ($1, $2); } 
 | after_type_decl arrayQual                { $$ = makeArrayNode ($1, $2); } 
 | after_type_decl LLT_LPAR LLT_RPAR                { $$ = makeFunctionNode ($1, paramNodeList_new ()); } 
 | after_type_decl LLT_LPAR realParamList LLT_RPAR  { $$ = makeFunctionNode ($1, $3); }
;

/* A parameter_decl is a decl that can appear in a parameter list.  
   We disallow the old C way of giving only the id names without types.
   A parameter_decl is like an after_type_decl except that it does not
   allow a TYPEDEF_NAME to appear in parens.  It will conflict with a
   a function with that typedef as an argument */

parameter_decl     
 : LLT_TYPEDEF_NAME                             { $$ = makeTypeExpr ($1); } 
 | LLT_MULOP parameter_decl /* %prec UNARY */   { $$ = makePointerNode ($1, $2); } 
 | parameter_decl arrayQual                 { $$ = makeArrayNode ($1, $2); } 
 | parameter_decl LLT_LPAR LLT_RPAR                 { $$ = makeFunctionNode ($1, paramNodeList_new ()); } 
 | parameter_decl LLT_LPAR realParamList LLT_RPAR   { $$ = makeFunctionNode ($1, $3); } 
;

/* param : the last production allows C types to be generated without the
   parameter name */

param   
 : lclTypeSpec parameter_decl               { $$ = makeParamNode ($1, $2); } 
 | lclTypeSpec notype_decl                  { $$ = makeParamNode ($1, $2); } 
 | lclTypeSpec optAbstDeclarator            { $$ = makeParamNode ($1, $2); } 
/*
** | OUT lclTypeSpec parameter_decl           { $$ = makeOutParamNode ($1, $2, $3); } 
** | OUT lclTypeSpec notype_decl              { $$ = makeOutParamNode ($1, $2, $3); } 
** | OUT lclTypeSpec optAbstDeclarator        { $$ = makeOutParamNode ($1, $2, $3); } 
*/
;

/* typeName is only used in trait parameter renaming */

typeName   
 : lclTypeSpec optAbstDeclarator           { $$ = makeTypeNameNode (FALSE, $1, $2); } 
 | LLT_OBJ lclTypeSpec optAbstDeclarator       { $$ = makeTypeNameNode (TRUE, $2, $3); } 
 | opForm                                  { $$ = makeTypeNameNodeOp ($1); } 
;

/* Abstract declarator is like a declarator, but with no identifier */
 
optAbstDeclarator  
 : /* empty */                            { $$ = (abstDeclaratorNode)0; }
 | abstDeclarator                         { $$ = (abstDeclaratorNode)$1; } 
;

abstDeclarator   
 : LLT_LPAR abstDeclarator LLT_RPAR               { $$ = $2; $$->wrapped = $$->wrapped + 1; }
 | LLT_MULOP abstDeclarator                   { $$ = makePointerNode ($1, $2); } 
 | LLT_MULOP                                  { $$ = makePointerNode ($1, (typeExpr)0); } 
 | arrayQual                              { $$ = makeArrayNode ((typeExpr)0, $1); } 
 | abstDeclarator arrayQual               { $$ = makeArrayNode ($1, $2); } 
 | abstDeclarator LLT_LPAR LLT_RPAR               { $$ = makeFunctionNode ($1, paramNodeList_new ()); } 
 | LLT_LPAR realParamList LLT_RPAR                { $$ = makeFunctionNode ((typeExpr)0, $2); } 
 | abstDeclarator LLT_LPAR realParamList LLT_RPAR { $$ = makeFunctionNode ($1, $3); } 
;

arrayQual   
 : LLT_LBRACKET LLT_RBRACKET                     { $$ = makeArrayQualNode ($1, (termNode)0); } 
 | LLT_LBRACKET constLclExpr LLT_RBRACKET        { $$ = makeArrayQualNode ($1, $2); } 
;

opForm   
 : LLT_IF markerSym LLT_THEN markerSym LLT_ELSE markerSym
   { $$ = makeOpFormNode ($1, OPF_IF, opFormUnion_createMiddle (0), ltoken_undefined); }
 | anyOp
   { $$ = makeOpFormNode ($1, OPF_ANYOP, opFormUnion_createAnyOp ($1), ltoken_undefined); }
 | markerSym anyOp
   { $$ = makeOpFormNode ($1, OPF_MANYOP, opFormUnion_createAnyOp ($2), ltoken_undefined); }
 | anyOp markerSym
   { $$ = makeOpFormNode ($1, OPF_ANYOPM, opFormUnion_createAnyOp ($1), ltoken_undefined); }
 | markerSym anyOp markerSym
   { $$ = makeOpFormNode ($1, OPF_MANYOPM, 
			  opFormUnion_createAnyOp ($2), ltoken_undefined); }
 | open middle close
   { $$ = makeOpFormNode ($1, OPF_MIDDLE, opFormUnion_createMiddle ($2), $3); }
 | markerSym open middle close
   { $$ = makeOpFormNode ($1, OPF_MMIDDLE, opFormUnion_createMiddle ($3), $4); }
 | open middle close markerSym
   { $$ = makeOpFormNode ($1, OPF_MIDDLEM, opFormUnion_createMiddle ($2), $3); }
 | markerSym open middle close markerSym
   { $$ = makeOpFormNode ($2, OPF_MMIDDLEM, opFormUnion_createMiddle ($3), $4); }
 | LLT_LBRACKET middle LLT_RBRACKET
   { $$ = makeOpFormNode ($1, OPF_BMIDDLE, opFormUnion_createMiddle ($2), $3); }
 | LLT_LBRACKET middle LLT_RBRACKET markerSym
   { $$ = makeOpFormNode ($1, OPF_BMIDDLEM, opFormUnion_createMiddle ($2), $3); }

   /* added the next 6 productions (wrt old checker) to complete LSL grammar
   ** LSL treats '[' and '{' as openSym but LCL treats them as LLT_LBRACKET and
   ** LLT_LBRACE, and hence the need for these separate productions 
   */

 | markerSym LLT_LBRACKET middle LLT_RBRACKET 
   { $$ = makeOpFormNode ($2, OPF_BMMIDDLE, opFormUnion_createMiddle ($3), $4); }
 | markerSym LLT_LBRACKET middle LLT_RBRACKET markerSym
   { $$ = makeOpFormNode ($2, OPF_BMMIDDLEM, opFormUnion_createMiddle ($3), $4); }
 | selectSym simpleIdOrTypedefName
   { $$ = makeOpFormNode ($1, OPF_SELECT, 
			  opFormUnion_createAnyOp ($2), ltoken_undefined); }
 | mapSym simpleIdOrTypedefName
   { $$ = makeOpFormNode ($1, OPF_MAP, 
			  opFormUnion_createAnyOp ($2), ltoken_undefined); }
 | markerSym selectSym simpleIdOrTypedefName
   { $$ = makeOpFormNode ($1, OPF_MSELECT, 
			  opFormUnion_createAnyOp ($3), ltoken_undefined); }
 | markerSym mapSym simpleIdOrTypedefName
   { $$ = makeOpFormNode ($1, OPF_MMAP, 
			  opFormUnion_createAnyOp ($3), ltoken_undefined); }
;

open   
 : openSym
 | LLT_LBRACE
;

close   
 : closeSym
 | LLT_RBRACE 
;

anyOp   
 : simpleOp2
 | logicalOp
 | eqOp
;

middle   
 : /* empty */               { $$ = 0; }
 | placeList
 ;

placeList   
 : markerSym                      { $$ = 1; }
 | placeList separator markerSym  { $$ = $1 + 1; }
;

separator   
 : LLT_COMMA
 | sepSym
;

signature   
 : LLT_COLON domain mapSym sortId   { $$ = makesigNode ($1, $2, $4); } 
;

domain   
 : /* empty */               { $$ = ltokenList_new (); }
 | sortList
 ;

sortList   
 : sortId                    { $$ = ltokenList_singleton ($1); } 
 | sortList LLT_COMMA sortId     { $$ = ltokenList_push ($1, $3); } 
;

lclPredicate   
 : term                      { $$ = makeLclPredicateNode (ltoken_undefined, $1, LPD_PLAIN);} 
;

term 
 : term0                     { $$ = checkSort ($1); }
;

/* When "if <term> then <term> else <term> . <logicalOp> <term>"
   shift instead of reduce */

term0   
 : LLT_IF term0 LLT_THEN term0 LLT_ELSE term0 %prec LLT_IF_THEN_ELSE
   { $$ = makeIfTermNode ($1,$2,$3,$4,$5,$6); } 
 | equalityTerm
 | term0 logicalOp term0          { $$ = makeInfixTermNode ($1, $2, $3); } 
;

equalityTerm   
 : simpleOpTerm   /* was   | quantifiers LLT_LPAR term LLT_RPAR */
 | quantifiers LLT_LPAR lclPredicate LLT_RPAR
   /* temp fix, should change interface in future, add lclPredicateKind too */
   { checkLclPredicate ($2, $3); $$ = makeQuantifiedTermNode ($1, $2, $3->predicate, $4);
     symtable_exitScope (g_symtab); 
   } 
 | simpleOpTerm eqOp simpleOpTerm
   { $$ = makeInfixTermNode ($1, $2, $3);} 
 | simpleOpTerm LLT_EQUALS simpleOpTerm 
   { $$ = makeInfixTermNode ($1, $2, $3);} 
;

simpleOpTerm   
 : prefixOpTerm
 | secondary postfixOps          { $$ = makePostfixTermNode ($1, $2); } 
 | secondary infixOpPart         { $$ = CollapseInfixTermNode ($1, $2); } 
;

simpleOp2          
 : simpleOp
 | LLT_MULOP
;

prefixOpTerm   
 : secondary
 | simpleOp2 prefixOpTerm          { $$ = makePrefixTermNode ($1, $2); } 
;

postfixOps   
 : simpleOp2                       { $$ = ltokenList_singleton ($1); } 
 | postfixOps simpleOp2            { $$ = ltokenList_push ($1, $2); } 
;

infixOpPart   
 : simpleOp2 secondary             { $$ = pushInfixOpPartNode (termNodeList_new (),  $1, $2); } 
 | infixOpPart simpleOp2 secondary { $$ = pushInfixOpPartNode ($1, $2, $3); } 
;

secondary   
 : primary
 | bracketed                 { $$ = computePossibleSorts ($1); } 
 | bracketed primary         { $$ = updateMatchedNode ((termNode)0, $1, $2); }
 | primary bracketed         { $$ = updateMatchedNode ($1, $2, (termNode)0); }
 | primary bracketed primary { $$ = updateMatchedNode ($1, $2, $3); }
 | sqBracketed               { $$ = computePossibleSorts ($1); } 
 | sqBracketed primary       { $$ = updateSqBracketedNode ((termNode)0, $1, $2); }
;

bracketed   
 : matched LLT_COLON sortId    { $$ = $1; $$->sort = sort_lookupName (ltoken_getText ($3)); }
 | matched
 ;

sqBracketed   
 : LLT_LBRACKET args LLT_RBRACKET LLT_COLON sortId
   { $$ = makeSqBracketedNode ($1, $2, $3); 
     $$->given = sort_lookupName (ltoken_getText ($5)); }
 | LLT_LBRACKET args LLT_RBRACKET
   { $$ = makeSqBracketedNode ($1, $2, $3); } 
 | LLT_LBRACKET  LLT_RBRACKET LLT_COLON sortId
   { $$ = makeSqBracketedNode ($1, termNodeList_new (),  $2); 
     $$->given = sort_lookupName (ltoken_getText ($4)); 
   }
 | LLT_LBRACKET  LLT_RBRACKET
   { $$ = makeSqBracketedNode ($1, termNodeList_new (),  $2); } 
;

matched      
 : open args  close          { $$ = makeMatchedNode ($1, $2, $3); } 
 | open close                { $$ = makeMatchedNode ($1, termNodeList_new (),  $2); } 
;

args   
 : term                      { $$ = termNodeList_push (termNodeList_new (),  $1); } 
 | args separator term       { $$ = termNodeList_push ($1, $3); } 
;

primary   
 : LLT_LPAR term0 LLT_RPAR           /* may need to make a copy of $2 */
   { $$ = $2; $$->wrapped = $$->wrapped + 1; }
 | varId
   { $$ = makeSimpleTermNode ($1); } 
 | opId LLT_LPAR termList LLT_RPAR
   { $$ = makeOpCallTermNode ($1, $2, $3, $4); } 
 | lclPrimary
 | primary stateFcn
   { $$ = makePostfixTermNode2 ($1, $2); }  
 | primary selectSym simpleIdOrTypedefName
   { ltoken_markOwned ($3); $$ = makeSelectTermNode ($1, $2, $3); } 
 | primary mapSym simpleIdOrTypedefName
   { ltoken_markOwned ($3); $$ = makeMapTermNode ($1, $2, $3); } 
 | primary LLT_LBRACKET LLT_RBRACKET   
   { $$ = updateSqBracketedNode ($1, makeSqBracketedNode ($2, termNodeList_new (),  $3), 
				(termNode)0); }
 | primary LLT_LBRACKET termList LLT_RBRACKET
   { $$ = updateSqBracketedNode ($1, makeSqBracketedNode ($2, $3, $4), (termNode)0); }
 | primary LLT_COLON sortId
   { $$ = $1; $$->given = sort_lookupName (ltoken_getText ($3)); }
;

termList   
 : term0                  { $$ = termNodeList_push (termNodeList_new (),  $1); } 
 | termList LLT_COMMA term0   { $$ = termNodeList_push ($1, $3); } 
;

stateFcn   
 : preSym
 | postSym
 | anySym
 | LLT_QUOTE
;

lclPrimary   
 : cLiteral                         
 | LLT_RESULT                           { $$ = makeSimpleTermNode ($1); } 
 | LLT_FRESH LLT_LPAR termList LLT_RPAR         { $$ = makeOpCallTermNode ($1, $2, $3, $4); } 
 | LLT_UNCHANGED LLT_LPAR LLT_ALL LLT_RPAR          { $$ = makeUnchangedTermNode1 ($1, $3); } 
 | LLT_UNCHANGED LLT_LPAR storeRefList LLT_RPAR { $$ = makeUnchangedTermNode2 ($1, $3); } 
 | LLT_SIZEOF LLT_LPAR term LLT_RPAR
   { termNodeList x = termNodeList_new (); 
     termNodeList_addh (x, $3);
     $$ = makeOpCallTermNode ($1, $2, x, $4); 
   } 
 | LLT_ISSUB LLT_LPAR term LLT_COMMA term LLT_RPAR
   { termNodeList x = termNodeList_new ();
     termNodeList_addh (x, $3);
     termNodeList_addh (x, $5);
     $$ = makeOpCallTermNode ($1, $2, x, $6); 
   } 
 | LLT_SIZEOF LLT_LPAR lclTypeSpec LLT_RPAR       { $$ = makeSizeofTermNode ($1, $3); } 
;

/* removed 94-Mar-25:
**   | MODIFIES LLT_LPAR term LLT_RPAR
**                             {termNodeList x = termNodeList_new ();
**          termNodeList_addh (x, $3);
**        $$ = makeOpCallTermNode ($1, $2, x, $4); } 
*/

cLiteral   
 : LLT_CINTEGER  { $$ = makeLiteralTermNode ($1, g_sortInt); }
 | LLT_LCSTRING  { $$ = makeLiteralTermNode ($1, g_sortCstring); }
 | LLT_CCHAR     { $$ = makeLiteralTermNode ($1, g_sortChar); }
 | LLT_CFLOAT    { $$ = makeLiteralTermNode ($1, g_sortDouble); }
;

quantifiers   
 : quantifier
   { $$ = quantifierNodeList_add (quantifierNodeList_new (),  $1); } 
 | quantifiers quantifier
   { $$ = quantifierNodeList_add ($1, $2); } 
;

quantifier   
 : quantifierSym { scopeInfo si = (scopeInfo) dmalloc (sizeof (*si));
		   si->kind = SPE_QUANT;
		   symtable_enterScope (g_symtab, si); }
   quantifiedList 
   { $$ = makeQuantifierNode ($3, $1); } 
;

quantifiedList   
 : quantified                         { $$ = varNodeList_add (varNodeList_new (),  $1); } 
 | quantifiedList LLT_COMMA quantified    { $$ = varNodeList_add ($1, $3); } 
;

quantified   
 : varId LLT_COLON sortSpec     { $$ = makeVarNode ($1, FALSE, $3); } 
 | varId LLT_COLON LLT_OBJ sortSpec { $$ = makeVarNode ($1, TRUE, $4); } 
;

simpleIdOrTypedefName  
 : simpleId
 | LLT_TYPEDEF_NAME
;

/* a different name space from varId/fcnId/typeId */
fcnId : simpleId ;
varId : simpleId ;
tagId : simpleIdOrTypedefName ;
claimId : simpleIdOrTypedefName ;
sortId : simpleIdOrTypedefName ;
traitId : simpleIdOrTypedefName ;
opId : simpleIdOrTypedefName ;

%%

# include "bison.reset"

/*
** yytext is set in lex scanner 
** extern YYSTYPE yylval;  
** yylval is defined by code generated by bison 
*/

void ylerror (char *s) 
{
  /* 
  ** This resetting of the left context is very important when multiple
  ** files are to be parsed.  We do not want failures to propagate.
  ** Lex/Yacc does not reset the flags when the parsing goes bad.  
  ** BEGIN 0;        
  **/

  /*@-mustfree@*/
  lclfatalerror (yllval.ltok,
		 message ("%s: Token code: %s, Token String: %s", 
			  cstring_fromChars (s), 
			  ltoken_unparseCodeName (yllval.ltok), 
			  ltoken_getRawString (yllval.ltok)));
  /*@=mustfree@*/
}

static void yyprint (FILE *f, int t, YYSTYPE value) 
{
  fprintf (f, " type: %d (%s)", t, 
	   cstring_toCharsSafe (ltoken_getRawString (value.ltok)));
}







