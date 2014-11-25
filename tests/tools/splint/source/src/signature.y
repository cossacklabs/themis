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
** signature.y
**
** Grammar for parsing LSL signatures.
**
** Original author: Yang Meng Tan, Massachusetts Institute of Technology
*/

%{

# include <stdio.h>
# include "splintMacros.nf"
# include "basic.h"
# include "lslparse.h"
# include "signature.h"

static void lslerror (char *);
extern int lsllex ();

/*@dependent@*/ /*@null@*/ lslOp importedlslOp;

/*@-noparams@*/ /* Can't list params since YYSTYPE isn't defined yet. */
static void yyprint (/*FILE *p_file, int p_type, YYSTYPE p_value */);
/*@=noparams@*/

# define YYPRINT(file, type, value) yyprint (file, type, value)

# define YYDEBUG 1

/*@-redef@*/
/*@-readonlytrans@*/
/*@-nullassign@*/

%}

%pure_parser 

/* CONVENTIONS:  Reserved words are in ALL CAPS (plus markerSym)
		Characters appearing in the grammar are reserved:
						% ( ) , : \ */

/* Changes made
   - added LBRACKET stuff in OpForm (missing in LSL)
   - dif from LCL: open vs openSym, close vs closeSym
   - yylval vs yylval.tok
*/

%union {
  ltoken ltok;  /* a leaf is also an ltoken */
  unsigned int count;
  /*@only@*/  ltokenList ltokenList;
  /*@only@*/  opFormNode opform;
  /*@owned@*/ sigNode signature;
  /*@only@*/  nameNode name;
  /*@owned@*/ lslOp operator;
  /*@only@*/  lslOpList operators;
  /*@-redef@*/ /*@-matchfields@*/ 
} 
/*@=redef@*/ /*@=matchfields@*/

%token <ltok> LST_SIMPLEID
%token <ltok> LST_LOGICALOP         /* \implies, \and, \not, \or */
%token <ltok> LST_EQOP              /* \eq, \neq, ==, != */
%token <ltok> LST_SIMPLEOP          /* opSym - reserved */
%token <ltok> LST_MAPSYM            /* LSL's -> */
  /* FIELDMAPSYM = "\\field_arrow", only for LCL symtable import */
%token <ltok> LST_FIELDMAPSYM       /* LCL's struct field operator -> */
%token <ltok> LST_MARKERSYM         /* \marker, __ */
%token <ltok> LST_ifTOKEN LST_thenTOKEN LST_elseTOKEN
  /* different from LSL: separate LBRACKET from openSym and
     RBRACKET from closeSym */
%token <ltok> LST_LBRACKET LST_RBRACKET
%token <ltok> LST_SELECTSYM         /* \select or . */
%token <ltok> LST_SEPSYM LST_OPENSYM LST_CLOSESYM
%token <ltok> LST_COLON             /* : */
%token <ltok> LST_COMMA             /* , */
%token <ltok> LST_EOL LST_COMMENTSYM LST_WHITESPACE 

 /* %token LST_WHITESPACE */ /* Duplicate tokey removed */
%token LST_QUANTIFIERSYM
%token LST_EQUATIONSYM
%token LST_EQSEPSYM
%token LST_COMPOSESYM
%token LST_LPAR
%token LST_RPAR
%token LST_assertsTOKEN
%token LST_assumesTOKEN
%token LST_byTOKEN
%token LST_convertsTOKEN
%token LST_enumerationTOKEN
%token LST_equationsTOKEN
%token LST_exemptingTOKEN
%token LST_forTOKEN
%token LST_generatedTOKEN 
%token LST_impliesTOKEN 
%token LST_includesTOKEN
%token LST_introducesTOKEN
%token LST_ofTOKEN
%token LST_partitionedTOKEN
%token LST_traitTOKEN
%token LST_tupleTOKEN
%token LST_unionTOKEN
%token LST_BADTOKEN

%type <ltok> anyOp separator sortId opId
%type <count> middle placeList
%type <ltokenList> domain sortList
%type <opform> opForm
%type <signature> signature
%type <name> name 
%type <operator> operator
%type <operators> operatorList top

%%

top: operatorList { lslOpList_free ($1); } 
  ;
 
operatorList: operator 
              { lslOpList x = lslOpList_new ();
		g_importedlslOp = $1;
		lslOpList_add (x, $1);
 		$$ = x; }
            | operatorList operator
              { lslOpList_add ($1, $2);
		$$ = $1; } 
;

operator: name LST_COLON signature 
          { $$ = makelslOpNode ($1, $3); }
/* The next production is never used in the output of lsl -syms 
          |  name
          { $$ = makelslOpNode ($1, (sigNode)0); } */
;
 
name: opId /* check for the case of if_then_else */
      { $$ = makeNameNodeId ($1); } 
    | opForm
      { $$ = makeNameNodeForm ($1); }
;
 
opForm 
     : LST_ifTOKEN LST_MARKERSYM LST_thenTOKEN LST_MARKERSYM LST_elseTOKEN LST_MARKERSYM
       { $$ = makeOpFormNode ($1, OPF_IF, opFormUnion_createMiddle (0), ltoken_undefined); }
     | anyOp
       { $$ = makeOpFormNode ($1, OPF_ANYOP, opFormUnion_createAnyOp ($1), ltoken_undefined); }
     | LST_MARKERSYM anyOp
       { $$ = makeOpFormNode ($1, OPF_MANYOP, opFormUnion_createAnyOp ($2), ltoken_undefined); }
     | anyOp LST_MARKERSYM
       { $$ = makeOpFormNode ($1, OPF_ANYOPM, opFormUnion_createAnyOp ($1), ltoken_undefined); }
     | LST_MARKERSYM anyOp LST_MARKERSYM
       { $$ = makeOpFormNode ($1, OPF_MANYOPM, opFormUnion_createAnyOp ($2), ltoken_undefined); }
     | LST_OPENSYM middle LST_CLOSESYM
       { $$ = makeOpFormNode ($1, OPF_MIDDLE, opFormUnion_createMiddle ($2), $3); }
     | LST_MARKERSYM LST_OPENSYM middle LST_CLOSESYM
       { $$ = makeOpFormNode ($2, OPF_MMIDDLE, opFormUnion_createMiddle ($3), $4); }
     | LST_OPENSYM middle LST_CLOSESYM LST_MARKERSYM
       { $$ = makeOpFormNode ($1, OPF_MIDDLEM, opFormUnion_createMiddle ($2), $3); }
     | LST_MARKERSYM LST_OPENSYM middle LST_CLOSESYM LST_MARKERSYM
       { $$ = makeOpFormNode ($2, OPF_MMIDDLEM, 
			      opFormUnion_createMiddle ($3), $4); }
     | LST_LBRACKET middle LST_RBRACKET
       { $$ = makeOpFormNode ($1, OPF_BMIDDLE, 
			      opFormUnion_createMiddle ($2), $3); }
     | LST_MARKERSYM LST_LBRACKET middle LST_RBRACKET 
       { $$ = makeOpFormNode ($2, OPF_BMMIDDLE, 
			      opFormUnion_createMiddle ($3), $4); }
     | LST_LBRACKET middle LST_RBRACKET LST_MARKERSYM
     { $$ = makeOpFormNode ($1, OPF_BMIDDLEM, 
			    opFormUnion_createMiddle ($2), $3); }
     | LST_MARKERSYM LST_LBRACKET middle LST_RBRACKET LST_MARKERSYM
     { $$ = makeOpFormNode ($2, OPF_BMMIDDLEM, 
			    opFormUnion_createMiddle ($3), $4); }
     | LST_SELECTSYM LST_SIMPLEID
     { $$ = makeOpFormNode ($1, OPF_SELECT, 
			    opFormUnion_createAnyOp ($2), ltoken_undefined); }
     | LST_MARKERSYM LST_SELECTSYM LST_SIMPLEID
     { $$ = makeOpFormNode ($1, OPF_MSELECT, 
			    opFormUnion_createAnyOp ($3), ltoken_undefined); }
        /* not in LSL, need FILEDMAPSYM to distinguish it from MAPSYM */ 
        /* LST_FIELDMAPSYM = "\\field_arrow", only for LCL symtable import */
     | LST_MARKERSYM LST_FIELDMAPSYM LST_SIMPLEID
     { $$ = makeOpFormNode ($1, OPF_MMAP, 
			    opFormUnion_createAnyOp ($3), ltoken_undefined); }
;			  

anyOp: LST_SIMPLEOP
       { $$ = $1; }
     | LST_LOGICALOP
       { $$ = $1; }
     | LST_EQOP
       { $$ = $1; }
;

middle: /* empty */
        { $$ = 0; }      
      | placeList
        { $$ = $1; }      
;
 
placeList: LST_MARKERSYM
           { $$ = 1; }      
         | placeList separator LST_MARKERSYM
           { $$ = $1 + 1; }      
;
 
separator: LST_COMMA
           { $$ = $1; }      
         | LST_SEPSYM
           { $$ = $1; }      
;

signature: domain LST_MAPSYM sortId
           { $$ = makesigNode ($2, $1, $3); }
;

domain: /* empty */
        { $$ = ltokenList_new (); } 
      | sortList
        { $$ = $1; }
;
 
sortList: sortId
          { $$ = ltokenList_singleton ($1); } 
        | sortList LST_COMMA sortId
          { $$ = ltokenList_push ($1, $3); }  
;

sortId: LST_SIMPLEID 
        { 
	  $$ = $1; 
	  ltoken_setText ($$, processTraitSortId (ltoken_getText ($1))); 
	} 
;

opId: LST_SIMPLEID
      { $$ = $1; }
;

%%

# include "bison.reset"

extern char *yytext;

void lslerror (char *s) 
{
  llfatalbug 
    (cstring_makeLiteral 
     ("There has been a problem in the parser with LSL signatures. This is believed to result "
      "from a problem with bison v. 1.25.  Please try rebuidling Splint "
      "using the pre-compiled grammar files by commenting out the "
      "BISON= line in the top-level Makefile."));
}

static void yyprint (FILE *file, int type, YYSTYPE value)
{
  fprintf (file, " (%u:%u type: %d; text: %s) ", 
	   ltoken_getLine (value.ltok), 
	   ltoken_getCol (value.ltok), 
	   type, 
	   ltoken_getRawTextChars (value.ltok));
}

extern void PrintToken (ltoken tok) {
  char *codStr;
  
  switch (ltoken_getCode (tok)) 
    {
    case NOTTOKEN:          codStr = "*** NOTTOKEN ***"; break;
    case LST_QUANTIFIERSYM: codStr = "QUANTIFIERSYM"; break;
    case LST_LOGICALOP:     codStr = "LOGICALOP: "; break; 
    case LST_SELECTSYM:     codStr = "LST_SELECTSYM"; break;
    case LST_OPENSYM:	    codStr = "LST_OPENSYM"; break;
    case LST_SEPSYM:	    codStr = "SEPSYM"; break;
    case LST_CLOSESYM:	    codStr = "LST_CLOSESYM"; break;
    case LST_SIMPLEID:	    codStr = "LST_SIMPLEID"; break;
    case LST_MAPSYM:	    codStr = "MAPSYM"; break;
    case LST_MARKERSYM:	    codStr = "LST_MARKERSYM"; break;
    case LST_COMMENTSYM:    codStr = "COMMENTSYM"; break;
    case LST_SIMPLEOP:	    codStr = "SIMPLEOP"; break;
    case LST_COLON:         codStr = "LST_COLON"; break;
    case LST_COMMA:	    codStr = "COMMA"; break;
    case LST_LBRACKET:	    codStr = "LST_LBRACKET"; break;
    case LST_LPAR:	    codStr = "LST_LPAR"; break;
    case LST_RBRACKET:	    codStr = "LST_RBRACKET"; break;
    case LST_RPAR:          codStr = "LST_RPAR"; break;
    case LST_EQOP:          codStr = "LST_EQOP"; break;
    case LST_WHITESPACE:    codStr = "WHITESPACE,"; break;
    case LST_EOL:           codStr = "LST_EOL"; break;
    case LST_elseTOKEN:     codStr = "elseTOKEN"; break;
    case LST_ifTOKEN:       codStr = "ifTOKEN"; break;
    case LST_thenTOKEN:     codStr = "thenTOKEN"; break;
    case LST_BADTOKEN:      codStr = "*** BADTOKEN ***"; break;
    case LEOFTOKEN: /* can't reach LEOFTOKEN easily */
      codStr = "LEOFTOKEN"; break;
    default:
      codStr = "*** invalid token code ***";
      break;
    } /* end switch */
  
  /* only used for debugging */
  printf ("%u:%u: Token Code (%u): %s",
	  ltoken_getLine (tok), ltoken_getCol (tok), 
	  ltoken_getCode (tok), codStr);
  if (ltoken_getRawText (tok) != 0) 
    {
      printf (", Token String (%lu): %s\n", 
	      ltoken_getRawText (tok), ltoken_getRawTextChars (tok));
    }
  else printf ("\n");
}




