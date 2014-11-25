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
** mtgrammar.y
**
** Grammar for .mts files.
*/

%{

# include "bison.reset"
# include "splintMacros.nf"
# include "basic.h"

# ifndef S_SPLINT_S
extern ctype mtscanner_lookupType (mttok p_tok) /*@modifies p_tok@*/ ;
# endif

  /*@i523@*/ /* can't include these here
    # include "mtgrammar.h"
    # include "mtscanner.h"
  */

static /*@noreturn@*/ void mterror (char *);

/*@-noparams@*/ /* Can't list params since YYSTYPE isn't defined yet. */
extern int mtlex () ;
static void yyprint (/*FILE *p_file, int p_type, YYSTYPE p_value */);
/*@=noparams@*/

# define YYPRINT(file, type, value) yyprint (file, type, value)

# define YYDEBUG 1

# include "bison.head"

%}

%pure_parser 

%union {
  mttok tok; 
  mtDeclarationNode mtdecl;
  mtDeclarationPiece mtpiece;
  mtDeclarationPieces mtpieces;
  mtContextNode mtcontext;
  mtValuesNode mtvalues;
  mtDefaultsNode mtdefaults;
  mtDefaultsDeclList mtdeflist;
  mtAnnotationsNode mtannotations;
  mtAnnotationList mtannotlist;
  mtAnnotationDecl mtannotdecl;
  mtMergeNode mtmerge;
  mtMergeItem mtmergeitem;
  mtMergeClauseList mtmergeclauselist;
  mtMergeClause mtmergeclause;
  mtTransferClauseList mttransferclauselist;
  mtTransferClause mttransferclause;
  mtTransferAction mttransferaction;
  mtLoseReferenceList mtlosereferencelist;
  mtLoseReference mtlosereference;
  pointers pointers;
  /*@only@*/ cstringList cstringlist;
  ctype ctyp;
  /*@only@*/ qtype qtyp;
  qual qual;
  qualList quals;
}

/* Don't forget to enter all tokens in mtscanner.c */
%token <tok> MT_BADTOK

%token <tok> MT_END
%token <tok> MT_STATE MT_GLOBAL

%token <tok> MT_CONTEXT
%token <tok> MT_ONEOF

%token <tok> MT_DEFAULTS MT_DEFAULT
%token <tok> MT_REFERENCE MT_PARAMETER MT_RESULT MT_CLAUSE MT_LITERAL MT_NULL

%token <tok> MT_ANNOTATIONS
%token <tok> MT_ARROW

%token <tok> MT_MERGE
%token <tok> MT_TRANSFERS MT_PRECONDITIONS MT_POSTCONDITIONS
%token <tok> MT_LOSEREFERENCE

%token <tok> MT_AS
%token <tok> MT_ERROR

%token <tok> MT_PLUS MT_STAR MT_BAR
%token <tok> MT_LPAREN MT_RPAREN
%token <tok> MT_LBRACKET MT_RBRACKET
%token <tok> MT_LBRACE MT_RBRACE
%token <tok> MT_COMMA

%token <tok> MT_CHAR MT_INT MT_FLOAT MT_DOUBLE MT_VOID  MT_ANYTYPE MT_INTEGRALTYPE MT_UNSIGNEDINTEGRALTYPE
%token <tok> MT_SIGNEDINTEGRALTYPE 
%token <tok> MT_CONST MT_VOLATILE MT_RESTRICT
%token <tok> MT_STRINGLIT
%token <tok> MT_IDENT

%type <pointers> pointers
%type <ctyp> optType typeSpecifier typeName abstractDecl abstractDeclBase 
%type <qtyp> typeExpression
%type <qtyp> completeType completeTypeAux optCompleteType
%type <mtpiece> declarationPiece
%type <mtcontext> contextDeclaration
%type <mtcontext> contextSelection optContextSelection
%type <mtvalues> valuesDeclaration
%type <tok> defaultNode
%type <mtdefaults> defaultsDeclaration
%type <mtdeflist> defaultDeclarationList
%type <mtannotations> annotationsDeclaration
%type <mtannotlist> annotationsDeclarationList
%type <mtannotdecl> annotationDeclaration
%type <mtmerge> mergeDeclaration
%type <mtmergeitem> mergeItem
%type <mtmergeclauselist> mergeClauses
%type <mtmergeclause> mergeClause
%type <mttransferclauselist> transfersDeclaration transferClauses preconditionsDeclaration postconditionsDeclaration
%type <mttransferclause> transferClause 
%type <mttransferaction> transferAction errorAction
%type <mtlosereferencelist> loseReferenceDeclaration lostClauses
%type <mtlosereference> lostClause
%type <cstringlist> valuesList
%type <mtdecl> declarationNode
%type <mtpieces> declarationPieces
%type <tok> valueChoice
%type <quals> innerModsList
%type <qual> innerMods

%start file

%%

file
: {}
| mtsDeclaration {}
;

mtsDeclaration
: MT_STATE declarationNode MT_END 
  { mtreader_processDeclaration ($2); }
| MT_GLOBAL MT_STATE declarationNode MT_END
  { mtreader_processGlobalDeclaration ($3); }
;

declarationNode
: MT_IDENT declarationPieces
  { $$ = mtDeclarationNode_create ($1, $2); }
;

declarationPieces
: { $$ = mtDeclarationPieces_create (); }
| declarationPiece declarationPieces 
  { $$ = mtDeclarationPieces_append ($2, $1); }
;

declarationPiece
: contextDeclaration { $$ = mtDeclarationPiece_createContext ($1); }
| valuesDeclaration { $$ = mtDeclarationPiece_createValues ($1); }
| defaultsDeclaration { $$ = mtDeclarationPiece_createDefaults ($1); }
| defaultNode { $$ = mtDeclarationPiece_createValueDefault ($1); }
| annotationsDeclaration { $$ = mtDeclarationPiece_createAnnotations ($1); }
| mergeDeclaration { $$ = mtDeclarationPiece_createMerge ($1); } 
| transfersDeclaration { $$ = mtDeclarationPiece_createTransfers ($1); }
| preconditionsDeclaration { $$ = mtDeclarationPiece_createPreconditions ($1); }
| postconditionsDeclaration { $$ = mtDeclarationPiece_createPostconditions ($1); }
| loseReferenceDeclaration { $$ = mtDeclarationPiece_createLosers ($1); }
;

contextDeclaration
: MT_CONTEXT contextSelection { $$ = $2; }
  /* ??? should it be a list? */
;

optContextSelection
: /* empty */ { $$ = mtContextNode_createAny (); }
| contextSelection
;

contextSelection
: MT_PARAMETER optType { $$ = mtContextNode_createParameter ($2); }
| MT_REFERENCE optType { $$ = mtContextNode_createReference ($2); }
| MT_RESULT optType    { $$ = mtContextNode_createResult ($2); } 
| MT_CLAUSE optType    { $$ = mtContextNode_createClause ($2); } 
| MT_LITERAL optType   { $$ = mtContextNode_createLiteral ($2); }
| MT_NULL optType      { $$ = mtContextNode_createNull ($2); }
;

/*
** Wish I could share the C grammar here...cut-and-paste instead.
*/

optType
: /* empty */ { $$ = ctype_unknown; }
| typeExpression { DPRINTF (("Type: %s", qtype_unparse ($1))); $$ = qtype_getType ($1); }
;

typeExpression
: completeType
| completeType abstractDecl  { $$ = qtype_newBase ($1, $2); }
;

completeType
: completeTypeAux { $$ = $1; }
| completeType MT_BAR typeExpression
  { $$ = qtype_mergeAlt ($1, $3); }
;

completeTypeAux
: typeSpecifier optCompleteType { $$ = qtype_combine ($2, $1); }
;

optCompleteType
: /* empty */ { $$ = qtype_unknown (); }
| completeType { $$ = $1; }
;

abstractDecl
 : pointers { $$ = ctype_adjustPointers ($1, ctype_unknown); }
 | abstractDeclBase
 | pointers abstractDeclBase { $$ = ctype_adjustPointers ($1, $2); }
;

pointers
 : MT_STAR { $$ = pointers_createMt ($1); }
 | MT_STAR innerModsList { $$ = pointers_createModsMt ($1, $2); }
 | MT_STAR pointers { $$ = pointers_extend (pointers_createMt ($1), $2); }
 | MT_STAR innerModsList pointers { $$ = pointers_extend (pointers_createModsMt ($1, $2), $3); }
;

innerMods
 : MT_CONST    { $$ = qual_createConst (); }
 | MT_VOLATILE { $$ = qual_createVolatile (); }
 | MT_RESTRICT { $$ = qual_createRestrict (); }
;

innerModsList
 : innerMods { $$ = qualList_single ($1); }
 | innerModsList innerMods { $$ = qualList_add ($1, $2); }
;

abstractDeclBase
 : MT_LPAREN abstractDecl MT_RPAREN { $$ = ctype_expectFunction ($2); }
 | MT_LBRACKET MT_RBRACKET { $$ = ctype_makeArray (ctype_unknown); }
 | abstractDeclBase MT_LBRACKET MT_RBRACKET { $$ = ctype_makeArray ($1); }
/*
 | abstractDeclBase MT_LBRACKET constantExpr MT_RBRACKET 
   { $$ = ctype_makeFixedArray ($1, exprNode_getLongValue ($3)); }
*/
;

typeSpecifier
: MT_CHAR { $$ = ctype_char; } 
| MT_INT { $$ = ctype_int; }
| MT_FLOAT { $$ = ctype_float; }
| MT_DOUBLE { $$ = ctype_double; }
| MT_VOID { $$ = ctype_void; }
| MT_ANYTYPE { $$ = ctype_unknown; }
| MT_INTEGRALTYPE { $$ = ctype_anyintegral; }
| MT_UNSIGNEDINTEGRALTYPE { $$ = ctype_unsignedintegral; }
| MT_SIGNEDINTEGRALTYPE { $$ = ctype_signedintegral; }
| typeName
 /* | suSpc 
 | enumSpc
 | typeModifier NotType { $$ = ctype_fromQual ($1); } */
;

typeName
 : MT_IDENT { $$ = mtscanner_lookupType ($1); }
;

valuesDeclaration
: MT_ONEOF valuesList { $$ = mtValuesNode_create ($2); }
;

valuesList
: MT_IDENT { $$ = cstringList_single (mttok_getText ($1)); }
| MT_IDENT MT_COMMA valuesList 
  { $$ = cstringList_prepend ($3, mttok_getText ($1)); }
;

defaultNode
: MT_DEFAULT valueChoice { $$ = $2; }
;

defaultsDeclaration
: MT_DEFAULTS defaultDeclarationList { $$ = mtDefaultsNode_create ($1, $2); }
;

defaultDeclarationList
: contextSelection MT_ARROW valueChoice 
{ $$ = mtDefaultsDeclList_single (mtDefaultsDecl_create ($1, $3)); }
| contextSelection MT_ARROW valueChoice defaultDeclarationList 
{ $$ = mtDefaultsDeclList_prepend ($4, mtDefaultsDecl_create ($1, $3)); }
;

annotationsDeclaration
: MT_ANNOTATIONS annotationsDeclarationList { $$ = mtAnnotationsNode_create ($2); }
;

annotationsDeclarationList
: annotationDeclaration { $$ = mtAnnotationList_single ($1); }
| annotationDeclaration annotationsDeclarationList 
  { $$ = mtAnnotationList_prepend ($2, $1); }
;

annotationDeclaration
: MT_IDENT optContextSelection MT_ARROW valueChoice 
  { $$ = mtAnnotationDecl_create ($1, $2, $4); }
;

mergeDeclaration
: MT_MERGE mergeClauses { $$ = mtMergeNode_create ($2); }
;

mergeClauses
: mergeClause { $$ = mtMergeClauseList_single ($1); }
| mergeClause mergeClauses { $$ = mtMergeClauseList_prepend ($2, $1); }
;

mergeClause
: mergeItem MT_PLUS mergeItem MT_ARROW transferAction
  { $$ = mtMergeClause_create ($1, $3, $5); }
;

mergeItem
: valueChoice { $$ = mtMergeItem_createValue ($1); } 
| MT_STAR { $$ = mtMergeItem_createStar ($1); } 
;

preconditionsDeclaration
: MT_PRECONDITIONS transferClauses { $$ = $2; }
;

postconditionsDeclaration
: MT_POSTCONDITIONS transferClauses { $$ = $2; }
;

transfersDeclaration
: MT_TRANSFERS transferClauses { $$ = $2; }
;

loseReferenceDeclaration
: MT_LOSEREFERENCE lostClauses { $$ = $2; }
;

lostClauses
: lostClause { $$ = mtLoseReferenceList_single ($1); }
| lostClause lostClauses { $$ = mtLoseReferenceList_prepend ($2, $1); }
;

lostClause
: valueChoice MT_ARROW errorAction { $$ = mtLoseReference_create ($1, $3); }
;

transferClauses
: transferClause { $$ = mtTransferClauseList_single ($1); }
| transferClause transferClauses { $$ = mtTransferClauseList_prepend ($2, $1); }
;

transferClause
: valueChoice MT_AS valueChoice MT_ARROW transferAction 
  { $$ = mtTransferClause_create ($1, $3, $5); }
;

transferAction
: valueChoice { $$ = mtTransferAction_createValue ($1); }
| errorAction { $$ = $1; }
;

errorAction
: MT_ERROR { $$ = mtTransferAction_createError ($1); } 
| MT_ERROR MT_STRINGLIT { $$ = mtTransferAction_createErrorMessage ($2); }
;

valueChoice
 : MT_IDENT 
;

%%

# include "bison.reset"

extern char *yytext;

static void mterror (char *s) 
{
  
  if (s != NULL)
    {
      llparseerror
	(message ("Parse error in meta-state file: %s", cstring_fromChars (s)));
    }
  else
    {
      llparseerror
	(message ("Parse error in meta-state file"));
    }

}

static void yyprint (FILE *file, int type, YYSTYPE value)
{
  cstring tname = mttok_unparse (value.tok);
  fprintf (file, " (%s)", cstring_toCharsSafe (tname));
  cstring_free (tname);
}




