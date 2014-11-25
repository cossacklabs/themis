/*
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
** For information on splint: info@splint.org
** To report a bug: splint-bug@splint.org
** For more information: http://www.splint.org
*/
/*
** lslinit.c
**
** Processor for Larch Shared Language Init Files
*/

# include "splintMacros.nf"
# include "basic.h"
# include "gram.h"
# include "lclscan.h"
# include "scanline.h"
# include "lclscanline.h"
# include "lcltokentable.h"
# include "lclsyntable.h"
# include "lslinit.h"
# include "lclinit.h"
# include "shift.h"

# if 0
/*@notfunction@*/
# define TRACE(rule) printf ("Reducing: %s\n", rule)
# else
/*@notfunction@*/
# define TRACE(rule)
# endif

static /*@dependent@*/ ltoken nextToken;
static void InitLines (void) /*@modifies nextToken@*/ ;
static void InitLine (void) /*@modifies nextToken@*/;
static void Classification (void) /*@modifies nextToken@*/;
static void CharClass (void) /*@modifies nextToken@*/;

static void EndCommentChars (void) /*@modifies nextToken@*/ ;
static void IdChars (void) /*@modifies nextToken@*/ ;
static void OpChars (void) /*@modifies nextToken@*/ ;
static void ExtensionChar (void) /*@modifies nextToken@*/ ;
static void SingChars (void) /*@modifies nextToken@*/ ;
static void WhiteChars (void) /*@modifies nextToken@*/ ;
static void EndCommentChar (void) /*@modifies nextToken@*/ ;
static void IdChar (void) /*@modifies nextToken@*/ ;
static void OpChar (void) /*@modifies nextToken@*/ ;
static void SingChar (void) /*@modifies nextToken@*/ ;
static void WhiteChar (void) /*@modifies nextToken@*/ ;

static void TokenClass (void) /*@modifies nextToken@*/ ;
static void QuantifierSymToks (void) /*@modifies nextToken@*/ ;
static void LogicalOpToks (void) /*@modifies nextToken@*/ ;
static void EqOpToks (void) /*@modifies nextToken@*/ ;
static void EquationSymToks (void) /*@modifies nextToken@*/ ;
static void EqSepSymToks (void) /*@modifies nextToken@*/ ;
static void SelectSymToks (void) /*@modifies nextToken@*/ ;
static void OpenSymToks (void) /*@modifies nextToken@*/ ;
static void SepSymToks (void) /*@modifies nextToken@*/ ;
static void CloseSymToks (void) /*@modifies nextToken@*/ ;
static void SimpleIdToks (void) /*@modifies nextToken@*/ ;
static void MapSymToks (void) /*@modifies nextToken@*/ ;
static void MarkerSymToks (void) /*@modifies nextToken@*/ ;
static void CommentSymToks (void) /*@modifies nextToken@*/ ;
static void QuantifierSymTok (void) /*@modifies nextToken@*/ ;
static void LogicalOpTok (void) /*@modifies nextToken@*/ ;
static void EqOpTok (void) /*@modifies nextToken@*/ ;
static void EquationSymTok (void) /*@modifies nextToken@*/ ;
static void EqSepSymTok (void) /*@modifies nextToken@*/ ;
static void SelectSymTok (void) /*@modifies nextToken@*/ ;
static void OpenSymTok (void) /*@modifies nextToken@*/ ;
static void SepSymTok (void) /*@modifies nextToken@*/ ;
static void CloseSymTok (void) /*@modifies nextToken@*/ ;
static void SimpleIdTok (void) /*@modifies nextToken@*/ ;
static void MapSymTok (void) /*@modifies nextToken@*/ ;
static void MarkerSymTok (void) /*@modifies nextToken@*/ ;
static void CommentSymTok (void) /*@modifies nextToken@*/ ;
static void SynClass (void) /*@modifies nextToken@*/ ;
static void OldToken (void) /*@modifies nextToken@*/ ;
static void NewToken (void) /*@modifies nextToken@*/ ;
static void Token (void) /*@modifies nextToken@*/ ;

static void InitReduce (LCLInitRuleCode p_rule) /*@modifies nextToken@*/ ;
static void UpdateXCharKeywords (charCode p_xChar) /*@modifies nextToken@*/ ;
static void ProcessExtensionChar (void) /*@modifies nextToken@*/ ;
static void ProcessEndCommentChar (void) /*@modifies nextToken@*/ ;
static void ProcessSingleChar (charCode p_code) /*@modifies nextToken@*/ ;
static void ProcessToken (ltokenCode p_code) /*@modifies nextToken@*/ ;
static void ProcessSynonym (void) /*@modifies nextToken@*/ ;

static void
  LocalUserError (/*@unused@*/ ltoken p_t, /*@temp@*/ char *p_msg)
  /*@modifies *g_warningstream@*/ ;

/* If TRUE character has been redefined as a singleChar. */
static bool defineSingleChar[LASTCHAR + 1];

static charCode currentExtensionChar;

/* LSL init file keyword tokens.  */

static /*@exposed@*/ ltoken endCommentCharToken;
static /*@exposed@*/ ltoken idCharToken;
static /*@exposed@*/ ltoken opCharToken;
static /*@exposed@*/ ltoken extensionCharToken;
static /*@exposed@*/ ltoken singleCharToken;
static /*@exposed@*/ ltoken whiteCharToken;

static /*@exposed@*/ ltoken quantifierSymToken;
static /*@exposed@*/ ltoken logicalOpToken;
static /*@exposed@*/ ltoken eqOpToken;
static /*@exposed@*/ ltoken equationSymToken;
static /*@exposed@*/ ltoken eqSepSymToken;
static /*@exposed@*/ ltoken selectSymToken;
static /*@exposed@*/ ltoken openSymToken;
static /*@exposed@*/ ltoken sepSymToken;
static /*@exposed@*/ ltoken closeSymToken;
static /*@exposed@*/ ltoken simpleIdToken;
static /*@exposed@*/ ltoken mapSymToken;
static /*@exposed@*/ ltoken markerSymToken;
static /*@exposed@*/ ltoken commentSymToken;

static /*@exposed@*/ ltoken synonymToken;

static /*@exposed@*/ ltoken
insertSimpleToken (char *s) /*@modifies internalState@*/ 
{
    return (LCLInsertToken (simpleId, lsymbol_fromChars (s), 0, FALSE));
}

static bool
hasFirstChar (ltoken tok) /*@*/ 
{
  return (ltoken_isChar (tok)
	  && ltoken_isSingleChar (cstring_firstChar (ltoken_unparse (tok))));
}

void
lclinit_process (void)
{
  InitLines ();

  InitReduce (INITFILE1);

  if (ltoken_getCode (nextToken) != LEOFTOKEN)
    {
      LocalUserError (nextToken, "unexpected tokens after end-of-file");
    }
}

static void
InitLines (void)
{
  setCodePoint ();
  InitReduce (INITLINES1);

  if (ltoken_getCode (nextToken) != LEOFTOKEN)
    {
      InitLine ();
      InitReduce (INITLINES2);
    }

  while (ltoken_getCode (nextToken) != LEOFTOKEN)
    {
      InitLine ();
      InitReduce (INITLINES3);
    }
}

static void
InitLine (void)
{
  if (ltoken_getCode (nextToken) == LLT_EOL)
    {
      /* Nothing on line. */
      InitReduce (INITLINE1);
    }
  else
    {
      Classification ();
      InitReduce (INITLINE2);
    }

  if (ltoken_getCode (nextToken) != LLT_EOL)
    {
      LocalUserError (nextToken, "Unexpected tokens on line");
    }
  
  nextToken = LCLScanNextToken (); /* Discard EOL	*/
}

static void
Classification (void)
{
  lsymbol ntext = ltoken_getRawText (nextToken);  
    
  if (ntext == ltoken_getText (endCommentCharToken)
      || ntext == ltoken_getText (idCharToken)
      || ntext == ltoken_getText (opCharToken)
      || ntext == ltoken_getText (extensionCharToken)
      || ntext == ltoken_getText (singleCharToken)
      || ntext == ltoken_getText (whiteCharToken))
    {
      CharClass ();
      InitReduce (CLASSIFICATION1);
    }
  else if (ntext == ltoken_getText (quantifierSymToken)
	   || ntext == ltoken_getText (logicalOpToken)
	   || ntext == ltoken_getText (eqOpToken)
	   || ntext == ltoken_getText (equationSymToken)
	   || ntext == ltoken_getText (eqSepSymToken)
	   || ntext == ltoken_getText (selectSymToken)
	   || ntext == ltoken_getText (openSymToken)
	   || ntext == ltoken_getText (sepSymToken)
	   || ntext == ltoken_getText (closeSymToken)
	   || ntext == ltoken_getText (simpleIdToken)
	   || ntext == ltoken_getText (mapSymToken)
	   || ntext == ltoken_getText (markerSymToken)
	   || ntext == ltoken_getText (commentSymToken))
    {
      TokenClass ();
      InitReduce (CLASSIFICATION2);
    }
  else if (ntext == ltoken_getText (synonymToken))
    {
      SynClass ();
      InitReduce (CLASSIFICATION3);
    }
  else
    {
      llbug (message ("Expected character, token, or synonym classification: %s",
		      ltoken_getRawString (nextToken)));
      /* pop off all tokens on this line */
    }
}

static void
CharClass (void)
{
  ltoken charClassToken;
  charClassToken = nextToken;
  nextToken = LCLScanNextToken ();		/* Discard char class keyword. */

  if (ltoken_getRawText (charClassToken) == ltoken_getText (endCommentCharToken))
    {
      EndCommentChars ();
      InitReduce (CHARCLASS1);
    }
  else if (ltoken_getRawText (charClassToken) == ltoken_getText (idCharToken))
    {
      IdChars ();
      InitReduce (CHARCLASS2);
    }
  else if (ltoken_getRawText (charClassToken) == ltoken_getText (opCharToken))
    {
      OpChars ();
      InitReduce (CHARCLASS3);
    }
  else if (ltoken_getRawText (charClassToken)
	   == ltoken_getText (extensionCharToken))
    {
      ExtensionChar ();
      InitReduce (CHARCLASS4);
    }
  else if (ltoken_getRawText (charClassToken) == ltoken_getText (singleCharToken))
    {
      SingChars ();
      InitReduce (CHARCLASS5);
    }
  else if (ltoken_getRawText (charClassToken) == ltoken_getText (whiteCharToken))
    {
      WhiteChars ();
      InitReduce (CHARCLASS6);
    }
  else
    {
      LocalUserError (nextToken, "expected character classification");
    }
}

static void
EndCommentChars (void)
{
  EndCommentChar ();
  InitReduce (LRC_ENDCOMMENT1);

  while (ltoken_getCode (nextToken) != LLT_EOL)
    {
      EndCommentChar ();
      InitReduce (LRC_ENDCOMMENT2);
    }

}

static void IdChars (void) /*@modifies nextToken@*/
{
  IdChar ();
  InitReduce (IDCHARS1);
  
  while (ltoken_getCode (nextToken) != LLT_EOL)
    {
      IdChar ();
      InitReduce (IDCHARS2);
    }
}

static void OpChars (void) /*@modifies nextToken@*/
{
  OpChar ();
  InitReduce (OPCHARS1);

  while (ltoken_getCode (nextToken) != LLT_EOL)
    {
      OpChar ();
      InitReduce (OPCHARS2);
    }
}

static void ExtensionChar (void) /*@modifies nextToken@*/
{
  if (hasFirstChar (nextToken))
    {
      LSLGenShift (nextToken);
      nextToken = LCLScanNextToken ();
      InitReduce (LRC_EXTENSIONCHAR1);
    }
  else
    {
      LocalUserError (nextToken, "expected only one character");
    }
}

static void SingChars (void) /*@modifies nextToken@*/
{
  SingChar ();
  InitReduce (SINGCHARS1);

  while (ltoken_getCode (nextToken) != LLT_EOL)
    {
      SingChar ();
      InitReduce (SINGCHARS2);
    }
}

static void WhiteChars (void) /*@modifies nextToken@*/
{
  WhiteChar ();
  InitReduce (WHITECHARS1);

  while (ltoken_getCode (nextToken) != LLT_EOL)
    {
      WhiteChar ();
      InitReduce (WHITECHARS2);
    }
}

static void EndCommentChar (void) /*@modifies nextToken@*/
{
  if (ltoken_isChar (nextToken))
    {
      LSLGenShift (nextToken);
      nextToken = LCLScanNextToken ();
      InitReduce (LRC_ENDCOMMENTCHAR1);
    }
  else
    {
      LocalUserError (nextToken, "expected only one character");
    }
}

static void IdChar (void) /*@modifies nextToken@*/
{
  if (hasFirstChar (nextToken))
    {
      LSLGenShift (nextToken);
      nextToken = LCLScanNextToken ();
      InitReduce (IDCHAR1);
    }
  else
    {
      LocalUserError (nextToken,
		      "Character is already defined, cannot redefine");
    }
}

static void OpChar (void) /*@modifies nextToken@*/
{
  if (hasFirstChar (nextToken))
    {
      LSLGenShift (nextToken);
      nextToken = LCLScanNextToken ();
      InitReduce (OPCHAR1);
    }
  else
    {
      LocalUserError (nextToken, 
		      "Character is already defined, cannot redefine");
    }
}

static void SingChar (void) /*@modifies nextToken@*/
{
  if (hasFirstChar (nextToken))
    {
      LSLGenShift (nextToken);
      nextToken = LCLScanNextToken ();
      InitReduce (SINGCHAR1);
    }
  else
    {
      LocalUserError (nextToken, 
		      "Character is already defined, cannot redefine");
    }
}

static void WhiteChar (void) /*@modifies nextToken@*/
{
  if (hasFirstChar (nextToken))
    {
      LSLGenShift (nextToken);
      nextToken = LCLScanNextToken ();
      InitReduce (WHITECHAR1);
    }
  else
    {
      LocalUserError (nextToken, "Character is already defined, cannot redefine");
    }
}

static void
TokenClass (void) /*@modifies nextToken@*/
{
  ltoken tokenClassToken;
  lsymbol ttext = ltoken_getRawText (nextToken);

  tokenClassToken = nextToken;

  /* Discard token class keyword. */
  nextToken = LCLScanNextToken ();

  if (ttext == ltoken_getText (quantifierSymToken))
    {
      QuantifierSymToks ();
      InitReduce (TOKENCLASS1);
    }
  else if (ttext == ltoken_getText (logicalOpToken))
    {
      LogicalOpToks ();
      InitReduce (TOKENCLASS2);
    }
  else if (ttext == ltoken_getText (eqOpToken))
    {
      EqOpToks ();
      InitReduce (TOKENCLASS3);
    }
  else if (ttext == ltoken_getText (equationSymToken))
    {
      EquationSymToks ();
      InitReduce (TOKENCLASS4);
    }
  else if (ttext == ltoken_getText (eqSepSymToken))
    {
      EqSepSymToks ();
      InitReduce (TOKENCLASS5);
    }
  else if (ttext == ltoken_getText (selectSymToken))
    {
      SelectSymToks ();
      InitReduce (TOKENCLASS6);
    }
  else if (ttext == ltoken_getText (openSymToken))
    {
      OpenSymToks ();
      InitReduce (TOKENCLASS7);
    }
  else if (ttext == ltoken_getText (sepSymToken))
    {
      SepSymToks ();
      InitReduce (TOKENCLASS8);
    }
  else if (ttext == ltoken_getText (closeSymToken))
    {
      CloseSymToks ();
      InitReduce (TOKENCLASS9);
    }
  else if (ttext == ltoken_getText (simpleIdToken))
    {
      SimpleIdToks ();
      InitReduce (TOKENCLASS10);
    }
  else if (ttext == ltoken_getText (mapSymToken))
    {
      MapSymToks ();
      InitReduce (TOKENCLASS11);
    }
  else if (ttext == ltoken_getText (markerSymToken))
    {
      MarkerSymToks ();
      InitReduce (TOKENCLASS12);
    }
  else if (ttext == ltoken_getText (commentSymToken))
    {
            CommentSymToks ();
      InitReduce (TOKENCLASS13);
    }
  else
    {
      LocalUserError (nextToken, "expected token classification");
    }
}

static void
QuantifierSymToks (void) /*@modifies nextToken@*/
{
  QuantifierSymTok ();
  InitReduce (QUANTIFIERSYMTOKS1);

  while (ltoken_getCode (nextToken) != LLT_EOL)
    {
      QuantifierSymTok ();
      InitReduce (QUANTIFIERSYMTOKS2);
    }
}

static void
LogicalOpToks (void) /*@modifies nextToken@*/
{
  LogicalOpTok ();
  InitReduce (LOGICALOPTOKS1);

  while (ltoken_getCode (nextToken) != LLT_EOL)
    {
      LogicalOpTok ();
      InitReduce (LOGICALOPTOKS2);
    }
}

static void
EqOpToks (void) /*@modifies nextToken@*/
{
  EqOpTok ();
  InitReduce (LRC_EQOPTOKS1);

  while (ltoken_getCode (nextToken) != LLT_EOL)
    {
      EqOpTok ();
      InitReduce (LRC_EQOPTOKS2);
    }
}

static void
EquationSymToks (void) /*@modifies nextToken@*/
{
  EquationSymTok ();
  InitReduce (LRC_EQUATIONSYMTOKS1);

  while (ltoken_getCode (nextToken) != LLT_EOL)
    {
      EquationSymTok ();
      InitReduce (LRC_EQUATIONSYMTOKS2);
    }
}

static void
EqSepSymToks (void) /*@modifies nextToken@*/
{
  EqSepSymTok ();
  InitReduce (LRC_EQSEPSYMTOKS1);

  while (ltoken_getCode (nextToken) != LLT_EOL)
    {
      EqSepSymTok ();
      InitReduce (LRC_EQSEPSYMTOKS2);
    }
}

static void
SelectSymToks (void) /*@modifies nextToken@*/
{
  SelectSymTok ();
  InitReduce (SELECTSYMTOKS1);

  while (ltoken_getCode (nextToken) != LLT_EOL)
    {
      SelectSymTok ();
      InitReduce (SELECTSYMTOKS2);
    }
}

static void
OpenSymToks (void) /*@modifies nextToken@*/
{
  OpenSymTok ();
  InitReduce (OPENSYMTOKS1);

  while (ltoken_getCode (nextToken) != LLT_EOL)
    {
      OpenSymTok ();
      InitReduce (OPENSYMTOKS2);
    }
}

static void
SepSymToks (void) /*@modifies nextToken@*/
{
  SepSymTok ();
  InitReduce (SEPSYMTOKS1);

  while (ltoken_getCode (nextToken) != LLT_EOL)
    {
      SepSymTok ();
      InitReduce (SEPSYMTOKS2);
    }
}

static void
CloseSymToks (void) /*@modifies nextToken@*/
{
  CloseSymTok ();
  InitReduce (CLOSESYMTOKS1);

  while (ltoken_getCode (nextToken) != LLT_EOL)
    {
      CloseSymTok ();
      InitReduce (CLOSESYMTOKS2);
    }
}

static void
SimpleIdToks (void) /*@modifies nextToken@*/
{ 
  SimpleIdTok ();
  InitReduce (SIMPLEIDTOKS1);

  while (ltoken_getCode (nextToken) != LLT_EOL)
    {
      SimpleIdTok ();
      InitReduce (SIMPLEIDTOKS2);
    }
}

static void
MapSymToks (void) /*@modifies nextToken@*/
{
  MapSymTok ();
  InitReduce (MAPSYMTOKS1);

  while (ltoken_getCode (nextToken) != LLT_EOL)
    {
      MapSymTok ();
      InitReduce (MAPSYMTOKS2);
    }
}

static void
MarkerSymToks (void) /*@modifies nextToken@*/
{
  MarkerSymTok ();
  InitReduce (MARKERSYMTOKS1);

  while (ltoken_getCode (nextToken) != LLT_EOL)
    {
      MarkerSymTok ();
      InitReduce (MARKERSYMTOKS2);
    }
}

static void
CommentSymToks (void) /*@modifies nextToken@*/
{
  CommentSymTok ();
    InitReduce (COMMENTSYMTOKS1);
  
  while (ltoken_getCode (nextToken) != LLT_EOL)
    {
            CommentSymTok ();
            InitReduce (COMMENTSYMTOKS2);
    }
}

static void
QuantifierSymTok (void) /*@modifies nextToken@*/
{
  Token ();
  InitReduce (QUANTIFIERSYMTOK1);
}

static void
LogicalOpTok (void) /*@modifies nextToken@*/
{
  Token ();
  InitReduce (LOGICALOPTOK1);
}

static void
EqOpTok (void) /*@modifies nextToken@*/
{
  Token ();
  InitReduce (LRC_EQOPTOK1);
}

static void EquationSymTok (void) /*@modifies nextToken@*/
{
  Token ();
  InitReduce (LRC_EQUATIONSYMTOK1);
}

static void EqSepSymTok (void) /*@modifies nextToken@*/
{
  Token ();
  InitReduce (LRC_EQSEPSYMTOK1);

}

static void SelectSymTok (void) /*@modifies nextToken@*/
{
  Token ();
  InitReduce (SELECTSYMTOK1);
}

static void OpenSymTok (void) /*@modifies nextToken@*/
{
  Token ();
  InitReduce (OPENSYMTOK1);
}

static void SepSymTok (void) /*@modifies nextToken@*/
{
  Token ();
  InitReduce (SEPSYMTOK1);
}

static void CloseSymTok (void) /*@modifies nextToken@*/
{
  Token ();
  InitReduce (CLOSESYMTOK1);
}

static void SimpleIdTok (void) /*@modifies nextToken@*/
{
  Token ();
  InitReduce (SIMPLEIDTOK1);
}

static void
MapSymTok (void) /*@modifies nextToken@*/
{
  Token ();
  InitReduce (MAPSYMTOK1);
}

static void
MarkerSymTok (void) /*@modifies nextToken@*/
{
  Token ();
  InitReduce (MARKERSYMTOK1);

}

static void
CommentSymTok (void) /*@modifies nextToken@*/
{
  Token ();
  InitReduce (COMMENTSYMTOK1);
}


static void
SynClass (void) /*@modifies nextToken@*/
{
  if (ltoken_getRawText (nextToken) == ltoken_getText (synonymToken))
    {
      nextToken = LCLScanNextToken ();

      OldToken ();
      NewToken ();

      InitReduce (SYNCLASS1);
    }
  else
    {
      LocalUserError (nextToken, "expected synonym classification");
    }

}

static void
OldToken (void) /*@modifies nextToken@*/
{
  Token ();
  InitReduce (OLDTOKEN1);

}

static void
NewToken (void) /*@modifies nextToken@*/
{
  Token ();
  InitReduce (NEWTOKEN1);

}

static void
Token (void) /*@modifies nextToken@*/
{
  if (ltoken_getCode (nextToken) == LLT_EOL
      || ltoken_getCode (nextToken) == LEOFTOKEN)
    {
      LocalUserError (nextToken, "unexpected end-of-line or end-of-file");
    }
  else
    {
      LSLGenShift (nextToken);
      nextToken = LCLScanNextToken ();
    }
}

/*
** Init File Processing Routines, these routines use the shift-reduce sequence
** produced by the init file parser and update the necessary tables for the
** scanner.
**
** The same shift stack is used that LSL parser uses.  A different reduce
** procedure is used because the init file grammar is different from the LSL
** grammar.
*/

static void
InitReduce (LCLInitRuleCode rule) /*@modifies nextToken@*/
{
  switch (rule)
    {
    case INITFILE1:
      TRACE ("INITFILE1");
      break;
      
    case INITLINES1:
      TRACE ("INITLINES1");
      break;
      
    case INITLINES2:
      TRACE ("INITLINES2");
      break;

    case INITLINES3:
      TRACE ("INITLINES3");
      break;

    case INITLINE1:
      TRACE ("INITLINE1");
      break;

    case INITLINE2:
      TRACE ("INITLINE2");
      break;

    case CLASSIFICATION1:
      TRACE ("CLASSIFICATION1");
      break;

    case CLASSIFICATION2:
      TRACE ("CLASSIFICATION2");
      break;

    case CLASSIFICATION3:
      TRACE ("CLASSIFICATION3");
      break;

    case CHARCLASS1:
      TRACE ("CHARCLASS1");
      break;

    case CHARCLASS2:
      TRACE ("CHARCLASS2");
      break;

    case CHARCLASS3:
      TRACE ("CHARCLASS3");
      break;

    case CHARCLASS4:
      TRACE ("CHARCLASS4");
      break;

    case CHARCLASS5:
      TRACE ("CHARCLASS5");
      break;

    case CHARCLASS6:
      TRACE ("CHARCLASS6");
      break;

    case LRC_ENDCOMMENT1:
      TRACE ("LRC_ENDCOMMENT1");
      break;

    case LRC_ENDCOMMENT2:
      TRACE ("LRC_ENDCOMMENT2");
      break;

    case IDCHARS1:
      TRACE ("IDCHARS1");
      break;

    case IDCHARS2:
      TRACE ("IDCHARS2");
      break;

    case OPCHARS1:
      TRACE ("OPCHARS1");
      break;

    case OPCHARS2:
      TRACE ("OPCHARS2");
      break;

    case LRC_EXTENSIONCHAR1:
      TRACE ("LRC_EXTENSIONCHAR1");
      ProcessExtensionChar ();
      break;

    case SINGCHARS1:
      TRACE ("SINGCHARS1");
      break;

    case SINGCHARS2:
      TRACE ("SINGCHARS2");
      break;

    case WHITECHARS1:
      TRACE ("WHITECHARS1");
      break;

    case WHITECHARS2:
      TRACE ("WHITECHARS2");
      break;

    case LRC_ENDCOMMENTCHAR1:
      TRACE ("LRC_ENDCOMMENTCHAR1");
      ProcessEndCommentChar ();
      break;

    case IDCHAR1:
      TRACE ("IDCHAR1");
      ProcessSingleChar (IDCHAR);
      break;

    case OPCHAR1:
      TRACE ("OPCHAR1");
      ProcessSingleChar (OPCHAR);
      break;

    case SINGCHAR1:
      TRACE ("SINGCHAR1");
      ProcessSingleChar (SINGLECHAR);
      break;

    case WHITECHAR1:
      TRACE ("CHAR1");
      ProcessSingleChar (WHITECHAR);
      break;

    case TOKENCLASS1:
      TRACE ("TOKENCLASS1");
      break;

    case TOKENCLASS2:
      TRACE ("TOKENCLASS2");
      break;

    case TOKENCLASS3:
      TRACE ("TOKENCLASS3");
      break;

    case TOKENCLASS4:
      TRACE ("TOKENCLASS4");
      break;

    case TOKENCLASS5:
      TRACE ("TOKENCLASS5");
      break;

    case TOKENCLASS6:
      TRACE ("TOKENCLASS6");
      break;

    case TOKENCLASS7:
      TRACE ("TOKENCLASS7");
      break;

    case TOKENCLASS8:
      TRACE ("TOKENCLASS8");
      break;

    case TOKENCLASS9:
      TRACE ("TOKENCLASS9");
      break;

    case TOKENCLASS10:
      TRACE ("TOKENCLASS10");
      break;

    case TOKENCLASS11:
      TRACE ("TOKENCLASS11");
      break;

    case TOKENCLASS12:
      TRACE ("TOKENCLASS12");
      break;

    case TOKENCLASS13:
      TRACE ("TOKENCLASS13");
      break;

    case QUANTIFIERSYMTOKS1:
      TRACE ("QUALIFERSYMTOKS1");
      break;

    case QUANTIFIERSYMTOKS2:
      TRACE ("QUANTIFIERSYMTOKS2");
      break;

    case LOGICALOPTOKS1:
      TRACE ("LOGICALOPTOKS1");
      break;

    case LOGICALOPTOKS2:
      TRACE ("LOGICALOPTOKS2");
      break;

    case LRC_EQOPTOKS1:
      TRACE ("LRC_EQOPTOKS1");
      break;

    case LRC_EQOPTOKS2:
      TRACE ("LRC_EQOPTOKS2");
      break;

    case LRC_EQUATIONSYMTOKS1:
      TRACE ("LRC_EQUATIONSYMTOKS1");
      break;

    case LRC_EQUATIONSYMTOKS2:
      TRACE ("LRC_EQUATIONSYMTOKS2");
      break;

    case LRC_EQSEPSYMTOKS1:
      TRACE ("LRC_EQSEPSYMTOKS1");
      break;

    case LRC_EQSEPSYMTOKS2:
      TRACE ("LRC_EQSEPSYMTOKS2");
      break;

    case SELECTSYMTOKS1:
      TRACE ("SELECTSYMTOKS1");
      break;

    case SELECTSYMTOKS2:
      TRACE ("SELECTSYMTOKS2");
      break;

    case OPENSYMTOKS1:
      TRACE ("OPENSYMTOKS1");
      break;

    case OPENSYMTOKS2:
      TRACE ("OPENSYMTOKS2");
      break;

    case SEPSYMTOKS1:
      TRACE ("SEPSYMTOKS1");
      break;

    case SEPSYMTOKS2:
      TRACE ("SEPSYMTOKS2");
      break;

    case CLOSESYMTOKS1:
      TRACE ("CLOSESYMTOKS1");
      break;

    case CLOSESYMTOKS2:
      TRACE ("CLOSESYMTOKS2");
      break;

    case SIMPLEIDTOKS1:
      TRACE ("SIMPLEIDTOKS1");
      break;

    case SIMPLEIDTOKS2:
      TRACE ("SIMPLEIDTOKS2");
      break;

    case MAPSYMTOKS1:
      TRACE ("MAPSYMTOKS1");
      break;

    case MAPSYMTOKS2:
      TRACE ("MAPSYMTOKS2");
      break;

    case MARKERSYMTOKS1:
      TRACE ("MARKERSYMTOKS1");
      break;

    case MARKERSYMTOKS2:
      TRACE ("MARKERSYMTOKS2");
      break;

    case COMMENTSYMTOKS1:
      TRACE ("COMMENTSYMTOKS1");
      break;

    case COMMENTSYMTOKS2:
      TRACE ("COMMENTSYMTOKS2");
      break;

    case QUANTIFIERSYMTOK1:
      TRACE ("QUANTIFERSYMTOK1");
      ProcessToken (quantifierSym);
      break;

    case LOGICALOPTOK1:
      TRACE ("LOGICALOPTOK1");
      ProcessToken (logicalOp);
      break;

    case LRC_EQOPTOK1:
      TRACE ("LRC_EQOPTOK1");
      ProcessToken (eqOp);
      break;

    case LRC_EQUATIONSYMTOK1:
      TRACE ("LRC_EQUATIONSYMTOK1");
      ProcessToken (equationSym);
      break;

    case LRC_EQSEPSYMTOK1:
      TRACE ("LRC_EQSEPSYMTOK1");
      ProcessToken (eqSepSym);
      break;

    case SELECTSYMTOK1:
      TRACE ("SELECTSYMTOK1");
      ProcessToken (selectSym);
      break;

    case OPENSYMTOK1:
      TRACE ("OPENSYMTOK1");
      ProcessToken (openSym);
      break;

    case SEPSYMTOK1:
      TRACE ("SEPSYMTOK1");
      ProcessToken (sepSym);
      break;

    case CLOSESYMTOK1:
      TRACE ("CLOSESYMTOK1");
      ProcessToken (closeSym);
      break;

    case SIMPLEIDTOK1:
      TRACE ("SIMPLEIDTOK1");
      ProcessToken (simpleId);
      break;

    case MAPSYMTOK1:
      TRACE ("MAPSYMTOK1");
      ProcessToken (mapSym);
      break;

    case MARKERSYMTOK1:
      TRACE ("MARKERSYMTOK1");
      ProcessToken (markerSym);
      break;

    case COMMENTSYMTOK1:
      TRACE ("COMMENTSYMTOK1");
      ProcessToken (commentSym);
      break;

    case SYNCLASS1:
      TRACE ("SYNCLASS1");
      ProcessSynonym ();
      break;

    case OLDTOKEN1:
      TRACE ("OLDTOKEN1");
      break;

    case NEWTOKEN1:
      TRACE ("NEWTOKEN1");
      break;

    default:
      llcontbuglit ("InitReduce: bad case");
      break;
    }
}	

/*
** Reset the first character of the predefined extensionChar keywords when  
** the extensionChar changes.  e.g. "extensionChar @" changes "\forall" to  
** "@forall".								    
*/

static void
UpdateXCharKeywords (charCode xChar)
{
  char *str;
  char xchar = (char)xChar;

  str = ltoken_getTextChars (ltoken_forall);
  *str = xchar;

  str = ltoken_getTextChars (ltoken_and);
  *str = xchar;

  str = ltoken_getTextChars (ltoken_or);
  *str = xchar;

  str = ltoken_getTextChars (ltoken_implies);
  *str = xchar;

  str = ltoken_getTextChars (ltoken_eq);
  *str = xchar;

  str = ltoken_getTextChars (ltoken_neq);
  *str = xchar;

  str = ltoken_getTextChars (ltoken_equals);
  *str = xchar;

  str = ltoken_getTextChars (ltoken_eqsep);
  *str = xchar;

  str = ltoken_getTextChars (ltoken_select);
  *str = xchar;

  str = ltoken_getTextChars (ltoken_open);
  *str = xchar;

  str = ltoken_getTextChars (ltoken_sep);
  *str = xchar;

  str = ltoken_getTextChars (ltoken_close);
  *str = xchar;

  str = ltoken_getTextChars (ltoken_id);
  *str = xchar;

  str = ltoken_getTextChars (ltoken_arrow);
  *str = xchar;

  str = ltoken_getTextChars (ltoken_marker);
  *str = xchar;

  str = ltoken_getTextChars (ltoken_comment);
  *str = xchar;
}

/* Different from ProcessCharClass because only allow one extension	    */
/* character. Therefore, the present extension character must be set to a   */
/* singleChar.								    */

static void
ProcessExtensionChar (void)
{
  ltoken stackToken = LSLGenTopPopShiftStack ();
  char firstChar = cstring_firstChar (ltoken_unparse (stackToken));

  if (!defineSingleChar[(int)firstChar]
      && ltoken_isSingleChar (firstChar))
    {
      /*
      ** Is a single character that has not been defined before.	   
      ** Can only have one extension char.  Release old one. 
      */

      LCLSetCharClass (firstChar, CHC_EXTENSION);
      LCLSetCharClass ((char) currentExtensionChar, SINGLECHAR);
      currentExtensionChar = (charCode) firstChar;
      UpdateXCharKeywords (currentExtensionChar);
    }
  else
    {
      /* Already redefined.  Don't allow to be redefined. */
      LocalUserError (stackToken, "Character is already defined, cannot redefine");
    }

  ltoken_free (stackToken);
}

/* Different from ProcessSingleChar because allow any characters to be	    */
/* endCommentChar and also set a different part of the scanner structure.   */

static void
ProcessEndCommentChar (void)
{
  ltoken stackToken = LSLGenTopPopShiftStack ();
  char firstChar = cstring_firstChar (ltoken_unparse (stackToken));

  if (LCLIsEndComment (firstChar))
    {
      LocalUserError (stackToken,
		    "already defined as a endCommentChar, cannot redefine");
    }
  else
    {
      LCLSetEndCommentChar (firstChar, TRUE);
    }
  ltoken_free (stackToken);
}

static void
ProcessSingleChar (charCode code)
{
  ltoken stackToken = LSLGenTopPopShiftStack ();
  char firstChar = cstring_firstChar (ltoken_unparse (stackToken));

  if (!defineSingleChar[(int)firstChar]
      && ltoken_isSingleChar (firstChar))
    {
      /* Is a single character that has not been defined before.	    */
      /* It's OK to redefine once. */
      LCLSetCharClass (firstChar, code);
      /* OK to mark as a defined singleChar even if not.  Only check	    */
      /* defineSingleChar[] if defining a singleChar.			    */
      defineSingleChar[(int)(firstChar)] = TRUE;
    }
  else
    {
      LocalUserError (stackToken, "Character is already defined, cannot redefine");
    }
  
  ltoken_free (stackToken);
}

static void
ProcessToken (ltokenCode code)
{
  ltoken stackToken, ptok;

  stackToken = LSLGenTopPopShiftStack ();

  
  if (LCLIsSyn (ltoken_getText (stackToken)))
    {
      LocalUserError (stackToken, "already defined as a synonym, cannot redefine");
    }

  /*
  ** Get the token from the token table, so can check if the token    
  ** was updated by a previous token.				    
  */

  ptok = LCLGetToken (ltoken_getText (stackToken));

  if (ltoken_isStateDefined (ptok))
    {
            LocalUserError (stackToken, "already defined, cannot redefine");
    }

    LCLUpdateToken (code, ltoken_getText (stackToken), TRUE);
  ltoken_free (stackToken);
}


static void
ProcessSynonym (void)
{
  ltoken newtok;
  ltoken oldtok;

  
  newtok = LSLGenTopPopShiftStack ();
  oldtok = LSLGenTopPopShiftStack ();

  
 /* ignore synonyms: Bool -> bool
    and:             bool -> Bool */
 /*
     if ((ltoken_getText (newtok) == lsymbol_Bool ||
          ltoken_getText (newtok) == lsymbol_bool) &&
         (ltoken_getText (oldtok) == lsymbol_Bool ||
          ltoken_getText (oldtok) == lsymbol_bool)) return;
 */

  if (ltoken_wasSyn (newtok))
    {
     /*
     ** The token has a synonym.  This means that the synonym was in the 
     ** init file, so complain about redefining as a synonym again	 
     */

      LocalUserError (newtok, "already is a synonym, cannot redefine");
    }

  if (ltoken_hasSyn (newtok))
    {
      /*
      ** newtok already has a synonym defined for it.  Do not allow	   
      ** synonyms to be chained.					    
      */

      LocalUserError (newtok,
		      "newtok already has a synonym, cannot chain synonyms");
    }

  if (ltoken_isStateDefined (newtok))
    {
      LocalUserError (newtok, "newtok already defined, cannot redefine");
    }

    LCLAddSyn (ltoken_getText (newtok), ltoken_getText (oldtok));

  ltoken_free (newtok);
  ltoken_free (oldtok); 
}


/*
** Utilities, in alphabetical order
*/

static void
  LocalUserError (ltoken t, char *msg)
{
  lclplainerror (message ("%s %s in the LCL init file.  Ignoring line.", 
			  ltoken_unparse (t), cstring_fromChars (msg)));

  /* discard the whole current line */

  nextToken = LCLScanNextToken ();		/* Discard bad token	*/

  while (ltoken_getCode (nextToken) != LLT_EOL 
	 && ltoken_getCode (nextToken) != LEOFTOKEN)
    {
      nextToken = LCLScanNextToken ();
    }
}


/*
 *  Required initialization and cleanup routines
 */

void
lclinit_initMod (void)
{
  int i;

  LSLGenInit (FALSE);		/* parsing LCLinit, not LSLinit */
  
  /*
  ** Insert the init file keywords into the token table as undefined	    
  ** SIMPLEIDs.  They are defined as simpleIds since they must be treated 
  ** that way if they do not appear as the first token on a line, and	    
  ** they must be treated that way for the actual LSL parsing. Save the   
  ** tokens so can recognize as init file keywords when necessary.	    
  */

  endCommentCharToken = insertSimpleToken ("endCommentChar");
  idCharToken = insertSimpleToken ("idChar");
  opCharToken = insertSimpleToken ("opChar");
  extensionCharToken = insertSimpleToken ("extensionChar");
  singleCharToken = insertSimpleToken ("singleChar");
  whiteCharToken = insertSimpleToken ("whiteChar");

  quantifierSymToken = insertSimpleToken ("quantifierSym");
  logicalOpToken = insertSimpleToken ("logicalOp");
  eqOpToken = insertSimpleToken ("eqOp");
  equationSymToken = insertSimpleToken ("equationSym");
  eqSepSymToken = insertSimpleToken ("eqSepSym");
  selectSymToken = insertSimpleToken ("selectSym");
  openSymToken = insertSimpleToken ("openSym");
  sepSymToken = insertSimpleToken ("sepSym");
  closeSymToken = insertSimpleToken ("closeSym");
  simpleIdToken = insertSimpleToken ("simpleId");
  mapSymToken = insertSimpleToken ("mapSym");
  markerSymToken = insertSimpleToken ("markerSym");
  commentSymToken = insertSimpleToken ("commentSym");

  synonymToken = insertSimpleToken ("synonym");

  /*
  ** Initialize defineSingleChar array to all FALSE to signal that no	    
  ** characters have been redefined as singleChar. 
  */

  for (i = 0; i <= LASTCHAR; i++)
    {
      defineSingleChar[i] = FALSE;
    }

  /* 
  ** Record the current extension character so can redefine back to	    
  ** singleChar if a new extension character is redefined.		    
  */

  currentExtensionChar = (charCode) CHAREXTENDER;
  
  /* Init file processing needs to have EOL reported. */

  LCLReportEolTokens (TRUE);

  /* Not: context_getBoolName () */

  LCLAddSyn (lsymbol_fromChars ("Bool"),
	     lsymbol_fromChars ("bool"));
}

void
lclinit_reset (void)
{
  nextToken = LCLScanNextToken ();	  
}

void
lclinit_cleanup (void)
{
  ;
}










