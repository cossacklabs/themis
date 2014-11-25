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
# include "signature.h"
# include "signature2.h"
# include "scan.h"
# include "scanline.h"
# include "tokentable.h"
# include "syntable.h"
# include "lslinit.h"
# include "lclinit.h"
# include "lclscan.h"
# include "lclscanline.h"
# include "lclsyntable.h"
# include "lcltokentable.h"

/* needed to parse init files */
# include "shift.h"
#if TRACING == 1
/*@notfunction@*/
# define LTRACE(rule) printf ("Reducing: %s\n", rule)
#else
/*@notfunction@*/
# define LTRACE(rule)
#endif

static void LocalUserError (ltoken p_t, /*@temp@*/ char *p_msg)
  /*@modifies *g_warningstream@*/;

static /*@only@*/ ltoken nextToken;

static /*@only@*/ /*@null@*/ inputStream s_initFile = inputStream_undefined;

static void InitFile (void) /*@modifies nextToken@*/ ;
static void InitLines (void) /*@modifies nextToken@*/ ;
static void InitLine (void) /*@modifies nextToken@*/ ;
static void Classification (void) /*@modifies nextToken@*/ ;
static void CharClass (void) /*@modifies nextToken@*/ ;

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

static void InitReduce (LSLInitRuleCode p_rule) /*@modifies nextToken@*/ ;
static void UpdateXCharKeywords (charCode) /*@modifies nextToken@*/ ;
static void ProcessExtensionChar (void) /*@modifies nextToken@*/ ;
static void ProcessEndCommentChar (void) /*@modifies nextToken@*/ ;
static void ProcessSingleChar (charCode p_code) /*@modifies nextToken@*/ ;
static void ProcessToken (ltokenCode p_code) /*@modifies nextToken@*/ ;
static void ProcessSynonym (void) /*@modifies nextToken@*/ ;

/* If TRUE character has been redefined as a singleChar. */
static bool defineSingleChar[LASTCHAR + 1];

static charCode currentExtensionChar;

/* LSL init file keyword tokens.  */

static /*@dependent@*/ ltoken endCommentCharToken;
static /*@dependent@*/ ltoken idCharToken;
static /*@dependent@*/ ltoken opCharToken;
static /*@dependent@*/ ltoken extensionCharToken;
static /*@dependent@*/ ltoken singleCharToken;
static /*@dependent@*/ ltoken whiteCharToken;
static /*@dependent@*/ ltoken quantifierSymToken;
static /*@dependent@*/ ltoken logicalOpToken;
static /*@dependent@*/ ltoken eqOpToken;
static /*@dependent@*/ ltoken equationSymToken;
static /*@dependent@*/ ltoken eqSepSymToken;
static /*@dependent@*/ ltoken selectSymToken;
static /*@dependent@*/ ltoken openSymToken;
static /*@dependent@*/ ltoken sepSymToken;
static /*@dependent@*/ ltoken closeSymToken;
static /*@dependent@*/ ltoken simpleIdToken;
static /*@dependent@*/ ltoken mapSymToken;
static /*@dependent@*/ ltoken markerSymToken;
static /*@dependent@*/ ltoken commentSymToken;
static /*@dependent@*/ ltoken synonymToken;

static bool
hasFirstChar (ltoken tok)
{
  return (ltoken_isChar (tok)
	  && lscanCharClass (cstring_firstChar (ltoken_unparse (tok))) == SINGLECHAR);
}

void
lslinit_setInitFile (inputStream s)
{
  llassert (inputStream_isUndefined (s_initFile));
  s_initFile = s;
}

/*
**
**  Parsing functions for init file processing, in the same order as the
**  grammar file lslinit.cfg.  This is top-down order, as much as possible.
**
*/

static void lslinit_processInitFile (void)
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
  if (ltoken_getCode (nextToken) == LST_EOL)
    {
     /* Nothing on line. */
      InitReduce (INITLINE1);
    }
  else
    {
      Classification ();
      InitReduce (INITLINE2);
    }

  if (ltoken_getCode (nextToken) != LST_EOL)
    {
      LocalUserError (nextToken, "Unexpected tokens on line");
    }

  ltoken_free (nextToken);
  nextToken = LSLScanNextToken ();	       
}

static void
Classification (void)
{
  if (ltoken_getRawText (nextToken) == ltoken_getText (endCommentCharToken)
      || ltoken_getRawText (nextToken) == ltoken_getText (idCharToken)
      || ltoken_getRawText (nextToken) == ltoken_getText (opCharToken)
      || ltoken_getRawText (nextToken) == ltoken_getText (extensionCharToken)
      || ltoken_getRawText (nextToken) == ltoken_getText (singleCharToken)
      || ltoken_getRawText (nextToken) == ltoken_getText (whiteCharToken))
    {
      CharClass ();
      InitReduce (CLASSIFICATION1);
    }
  else if (ltoken_getRawText (nextToken) == ltoken_getText (quantifierSymToken)
	   || ltoken_getRawText (nextToken) == ltoken_getText (logicalOpToken)
	   || ltoken_getRawText (nextToken) == ltoken_getText (eqOpToken)
	   || ltoken_getRawText (nextToken) == ltoken_getText (equationSymToken)
	   || ltoken_getRawText (nextToken) == ltoken_getText (eqSepSymToken)
	   || ltoken_getRawText (nextToken) == ltoken_getText (selectSymToken)
	   || ltoken_getRawText (nextToken) == ltoken_getText (openSymToken)
	   || ltoken_getRawText (nextToken) == ltoken_getText (sepSymToken)
	   || ltoken_getRawText (nextToken) == ltoken_getText (closeSymToken)
	   || ltoken_getRawText (nextToken) == ltoken_getText (simpleIdToken)
	   || ltoken_getRawText (nextToken) == ltoken_getText (mapSymToken)
	   || ltoken_getRawText (nextToken) == ltoken_getText (markerSymToken)
	   || ltoken_getRawText (nextToken) == ltoken_getText (commentSymToken))
    {
      TokenClass ();
      InitReduce (CLASSIFICATION2);
    }
  else if (ltoken_getRawText (nextToken) == ltoken_getText (synonymToken))
    {
      SynClass ();
      InitReduce (CLASSIFICATION3);
    }
  else
    {
      LocalUserError (nextToken,
		      "expected character, token, or synonym classification");
    }
}

static void
CharClass (void)
{
  ltoken charClassToken;

  charClassToken = nextToken;

  nextToken = LSLScanNextToken ();		/* Discard char class keyword. */

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

  ltoken_free (charClassToken);
}

static void
EndCommentChars (void)
{
  EndCommentChar ();
  InitReduce (LRC_ENDCOMMENT1);

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      EndCommentChar ();
      InitReduce (LRC_ENDCOMMENT2);
    }

}

static void
IdChars (void)
{
  IdChar ();
  InitReduce (IDCHARS1);

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      IdChar ();
      InitReduce (IDCHARS2);
    }
}

static void
OpChars (void)
{
  OpChar ();
  InitReduce (OPCHARS1);

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      OpChar ();
      InitReduce (OPCHARS2);
    }
}

static void
ExtensionChar (void)
{
  if (ltoken_isChar (nextToken)
      && lscanCharClass (cstring_firstChar (ltoken_unparse (nextToken))) == SINGLECHAR)
    {
      LSLGenShiftOnly (nextToken);
      nextToken = LSLScanNextToken ();
      InitReduce (LRC_EXTENSIONCHAR1);
    }
  else
    {
      LocalUserError (nextToken, "expected only one character");
    }
}

static void
SingChars (void)
{
  SingChar ();
  InitReduce (SINGCHARS1);

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      SingChar ();
      InitReduce (SINGCHARS2);
    }
}

static void
WhiteChars (void)
{
  WhiteChar ();
  InitReduce (WHITECHARS1);

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      WhiteChar ();
      InitReduce (WHITECHARS2);
    }
}

static void
EndCommentChar (void)
{
  if (ltoken_isChar (nextToken))
    {
      LSLGenShiftOnly (nextToken);
      nextToken = LSLScanNextToken ();
      InitReduce (LRC_ENDCOMMENTCHAR1);
    }
  else
    {
      LocalUserError (nextToken, "expected only one character");
    }
}

static void
IdChar (void)
{
  if (hasFirstChar (nextToken))
    {
      LSLGenShiftOnly (nextToken);
      nextToken = LSLScanNextToken ();
      InitReduce (IDCHAR1);
    }
  else
    {
      LocalUserError (nextToken, "character is already defined, cannot redefine");
    }
}

static void
OpChar (void)
{
  if (hasFirstChar (nextToken))
    {
      LSLGenShiftOnly (nextToken);
      nextToken = LSLScanNextToken ();
      InitReduce (OPCHAR1);
    }
  else
    {
      LocalUserError (nextToken, "character is already defined, cannot redefine");
    }
}

static void
SingChar (void)
{
  if (hasFirstChar (nextToken))
    {
      LSLGenShiftOnly (nextToken);
      nextToken = LSLScanNextToken ();
      InitReduce (SINGCHAR1);
    }
  else
    {
      LocalUserError (nextToken, "character is already defined, cannot redefine");
    }
}

static void
WhiteChar (void)
{
  if (hasFirstChar (nextToken))
    {
      LSLGenShiftOnly (nextToken);
      nextToken = LSLScanNextToken ();
      InitReduce (WHITECHAR1);
    }
  else
    {
      LocalUserError (nextToken, "character is already defined, cannot redefine");
    }
}

static void
TokenClass (void)
{
  ltoken tokenClassToken;

  tokenClassToken = nextToken;

  nextToken = LSLScanNextToken ();

  if (ltoken_getRawText (tokenClassToken) == ltoken_getText (quantifierSymToken))
    {
      QuantifierSymToks ();
      InitReduce (TOKENCLASS1);
    }
  else if (ltoken_getRawText (tokenClassToken) == ltoken_getText (logicalOpToken))
    {
      LogicalOpToks ();
      InitReduce (TOKENCLASS2);
    }
  else if (ltoken_getRawText (tokenClassToken) == ltoken_getText (eqOpToken))
    {
      EqOpToks ();
      InitReduce (TOKENCLASS3);
    }
  else if (ltoken_getRawText (tokenClassToken)
	   == ltoken_getText (equationSymToken))
    {
      EquationSymToks ();
      InitReduce (TOKENCLASS4);
    }
  else if (ltoken_getRawText (tokenClassToken) == ltoken_getText (eqSepSymToken))
    {
      EqSepSymToks ();
      InitReduce (TOKENCLASS5);
    }
  else if (ltoken_getRawText (tokenClassToken) == ltoken_getText (selectSymToken))
    {
      SelectSymToks ();
      InitReduce (TOKENCLASS6);
    }
  else if (ltoken_getRawText (tokenClassToken) == ltoken_getText (openSymToken))
    {
      OpenSymToks ();
      InitReduce (TOKENCLASS7);
    }
  else if (ltoken_getRawText (tokenClassToken) == ltoken_getText (sepSymToken))
    {
      SepSymToks ();
      InitReduce (TOKENCLASS8);
    }
  else if (ltoken_getRawText (tokenClassToken) == ltoken_getText (closeSymToken))
    {
      CloseSymToks ();
      InitReduce (TOKENCLASS9);
    }
  else if (ltoken_getRawText (tokenClassToken) == ltoken_getText (simpleIdToken))
    {
      SimpleIdToks ();
      InitReduce (TOKENCLASS10);
    }
  else if (ltoken_getRawText (tokenClassToken) == ltoken_getText (mapSymToken))
    {
      MapSymToks ();
      InitReduce (TOKENCLASS11);
    }
  else if (ltoken_getRawText (tokenClassToken) == ltoken_getText (markerSymToken))
    {
      MarkerSymToks ();
      InitReduce (TOKENCLASS12);
    }
  else if (ltoken_getRawText (tokenClassToken)
	   == ltoken_getText (commentSymToken))
    {
      CommentSymToks ();
      InitReduce (TOKENCLASS13);
    }
  else
    {
      LocalUserError (nextToken, "expected token classification");
    }

  ltoken_free (tokenClassToken);
}

static void
QuantifierSymToks (void)
{
  QuantifierSymTok ();
  InitReduce (QUANTIFIERSYMTOKS1);

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      QuantifierSymTok ();
      InitReduce (QUANTIFIERSYMTOKS2);
    }
}

static void
LogicalOpToks (void)
{
  LogicalOpTok ();
  InitReduce (LOGICALOPTOKS1);

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      LogicalOpTok ();
      InitReduce (LOGICALOPTOKS2);
    }
}

static void
EqOpToks (void)
{
  EqOpTok ();
  InitReduce (LRC_EQOPTOKS1);

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      EqOpTok ();
      InitReduce (LRC_EQOPTOKS2);
    }
}

static void
EquationSymToks (void)
{
  EquationSymTok ();
  InitReduce (LRC_EQUATIONSYMTOKS1);

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      EquationSymTok ();
      InitReduce (LRC_EQUATIONSYMTOKS2);
    }
}

static void
EqSepSymToks (void)
{
  EqSepSymTok ();
  InitReduce (LRC_EQSEPSYMTOKS1);

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      EqSepSymTok ();
      InitReduce (LRC_EQSEPSYMTOKS2);
    }
}

static void
SelectSymToks (void)
{
  SelectSymTok ();
  InitReduce (SELECTSYMTOKS1);

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      SelectSymTok ();
      InitReduce (SELECTSYMTOKS2);
    }
}

static void
OpenSymToks (void)
{
  OpenSymTok ();
  InitReduce (OPENSYMTOKS1);

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      OpenSymTok ();
      InitReduce (OPENSYMTOKS2);
    }
}

static void
SepSymToks (void)
{
  SepSymTok ();
  InitReduce (SEPSYMTOKS1);

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      SepSymTok ();
      InitReduce (SEPSYMTOKS2);
    }
}

static void
CloseSymToks (void)
{
  CloseSymTok ();
  InitReduce (CLOSESYMTOKS1);

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      CloseSymTok ();
      InitReduce (CLOSESYMTOKS2);
    }
}

static void
SimpleIdToks (void)
{
  SimpleIdTok ();
  InitReduce (SIMPLEIDTOKS1);

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      SimpleIdTok ();
      InitReduce (SIMPLEIDTOKS2);
    }
}

static void
MapSymToks (void)
{
  MapSymTok ();
  InitReduce (MAPSYMTOKS1);

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      MapSymTok ();
      InitReduce (MAPSYMTOKS2);
    }
}

static void
MarkerSymToks (void)
{
  MarkerSymTok ();
  InitReduce (MARKERSYMTOKS1);

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      MarkerSymTok ();
      InitReduce (MARKERSYMTOKS2);
    }
}

static void
CommentSymToks (void)
{
  CommentSymTok ();
  InitReduce (COMMENTSYMTOKS1);

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      CommentSymTok ();
      InitReduce (COMMENTSYMTOKS2);
    }
}

static void
QuantifierSymTok (void)
{
  Token ();
  InitReduce (QUANTIFIERSYMTOK1);
}

static void
LogicalOpTok (void)
{
  Token ();
  InitReduce (LOGICALOPTOK1);
}

static void
EqOpTok (void)
{
  Token ();
  InitReduce (LRC_EQOPTOK1);
}

static void
EquationSymTok (void)
{
 /* ### EquationSymTok (); ### */
  Token ();
  InitReduce (LRC_EQUATIONSYMTOK1);
}

static void
EqSepSymTok (void)
{
  Token ();
  InitReduce (LRC_EQSEPSYMTOK1);

}

static void
SelectSymTok (void)
{
  Token ();
  InitReduce (SELECTSYMTOK1);
}

static void
OpenSymTok (void)
{
  Token ();
  InitReduce (OPENSYMTOK1);
}

static void
SepSymTok (void)
{
  Token ();
  InitReduce (SEPSYMTOK1);
}

static void
CloseSymTok (void)
{
  Token ();
  InitReduce (CLOSESYMTOK1);
}

static void
SimpleIdTok (void)
{
  Token ();
  InitReduce (SIMPLEIDTOK1);
}

static void
MapSymTok (void)
{
  Token ();
  InitReduce (MAPSYMTOK1);
}

static void
MarkerSymTok (void)
{
  Token ();
  InitReduce (MARKERSYMTOK1);

}

static void
CommentSymTok (void)
{
  Token ();
  InitReduce (COMMENTSYMTOK1);
}


static void
SynClass (void)
{
  if (ltoken_getRawText (nextToken) == ltoken_getText (synonymToken))
    {
      ltoken_free (nextToken);
      nextToken = LSLScanNextToken ();

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
OldToken (void)
{
  Token ();
  InitReduce (OLDTOKEN1);

}

static void
NewToken (void)
{
  Token ();
  InitReduce (NEWTOKEN1);

}

static void
Token (void)
{
  if (ltoken_getCode (nextToken) == LST_EOL
      || ltoken_getCode (nextToken) == LEOFTOKEN)
    {
      LocalUserError (nextToken, "unexpected end-of-line or end-of-file");
    }
  else
    {
      LSLGenShiftOnly (nextToken);
      nextToken = LSLScanNextToken ();
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
**
*/

static void
InitReduce (LSLInitRuleCode rule)
{
  switch (rule)
    {
      case INITFILE1:
      LTRACE ("INITFILE1");
      break;

    case INITLINES1:
      LTRACE ("INITLINES1");
      break;

    case INITLINES2:
      LTRACE ("INITLINES2");
      break;

    case INITLINES3:
      LTRACE ("INITLINES3");
      break;

    case INITLINE1:
      LTRACE ("INITLINE1");
      break;

    case INITLINE2:
      LTRACE ("INITLINE2");
      break;

    case CLASSIFICATION1:
      LTRACE ("CLASSIFICATION1");
      break;

    case CLASSIFICATION2:
      LTRACE ("CLASSIFICATION2");
      break;

    case CLASSIFICATION3:
      LTRACE ("CLASSIFICATION3");
      break;

    case CHARCLASS1:
      LTRACE ("CHARCLASS1");
      break;

    case CHARCLASS2:
      LTRACE ("CHARCLASS2");
      break;

    case CHARCLASS3:
      LTRACE ("CHARCLASS3");
      break;

    case CHARCLASS4:
      LTRACE ("CHARCLASS4");
      break;

    case CHARCLASS5:
      LTRACE ("CHARCLASS5");
      break;

    case CHARCLASS6:
      LTRACE ("CHARCLASS6");
      break;

    case LRC_ENDCOMMENT1:
      LTRACE ("LRC_ENDCOMMENT1");
      break;

    case LRC_ENDCOMMENT2:
      LTRACE ("LRC_ENDCOMMENT2");
      break;

    case IDCHARS1:
      LTRACE ("IDCHARS1");
      break;

    case IDCHARS2:
      LTRACE ("IDCHARS2");
      break;

    case OPCHARS1:
      LTRACE ("OPCHARS1");
      break;

    case OPCHARS2:
      LTRACE ("OPCHARS2");
      break;

    case LRC_EXTENSIONCHAR1:
      LTRACE ("LRC_EXTENSIONCHAR1");
      ProcessExtensionChar ();
      break;

    case SINGCHARS1:
      LTRACE ("SINGCHARS1");
      break;

    case SINGCHARS2:
      LTRACE ("SINGCHARS2");
      break;

    case WHITECHARS1:
      LTRACE ("WHITECHARS1");
      break;

    case WHITECHARS2:
      LTRACE ("WHITECHARS2");
      break;

    case LRC_ENDCOMMENTCHAR1:
      LTRACE ("LRC_ENDCOMMENTCHAR1");
      ProcessEndCommentChar ();
      break;

    case IDCHAR1:
      LTRACE ("IDCHAR1");
      ProcessSingleChar (IDCHAR);
      break;

    case OPCHAR1:
      LTRACE ("OPCHAR1");
      ProcessSingleChar (OPCHAR);
      break;

    case SINGCHAR1:
      LTRACE ("SINGCHAR1");
      ProcessSingleChar (SINGLECHAR);
      break;

    case WHITECHAR1:
      LTRACE ("CHAR1");
      ProcessSingleChar (WHITECHAR);
      break;

    case TOKENCLASS1:
      LTRACE ("TOKENCLASS1");
      break;

    case TOKENCLASS2:
      LTRACE ("TOKENCLASS2");
      break;

    case TOKENCLASS3:
      LTRACE ("TOKENCLASS3");
      break;

    case TOKENCLASS4:
      LTRACE ("TOKENCLASS4");
      break;

    case TOKENCLASS5:
      LTRACE ("TOKENCLASS5");
      break;

    case TOKENCLASS6:
      LTRACE ("TOKENCLASS6");
      break;

    case TOKENCLASS7:
      LTRACE ("TOKENCLASS7");
      break;

    case TOKENCLASS8:
      LTRACE ("TOKENCLASS8");
      break;

    case TOKENCLASS9:
      LTRACE ("TOKENCLASS9");
      break;

    case TOKENCLASS10:
      LTRACE ("TOKENCLASS10");
      break;

    case TOKENCLASS11:
      LTRACE ("TOKENCLASS11");
      break;

    case TOKENCLASS12:
      LTRACE ("TOKENCLASS12");
      break;

    case TOKENCLASS13:
      LTRACE ("TOKENCLASS13");
      break;

    case QUANTIFIERSYMTOKS1:
      LTRACE ("QUALIFERSYMTOKS1");
      break;

    case QUANTIFIERSYMTOKS2:
      LTRACE ("QUANTIFIERSYMTOKS2");
      break;

    case LOGICALOPTOKS1:
      LTRACE ("LOGICALOPTOKS1");
      break;

    case LOGICALOPTOKS2:
      LTRACE ("LOGICALOPTOKS2");
      break;

    case LRC_EQOPTOKS1:
      LTRACE ("LRC_EQOPTOKS1");
      break;

    case LRC_EQOPTOKS2:
      LTRACE ("LRC_EQOPTOKS2");
      break;

    case LRC_EQUATIONSYMTOKS1:
      LTRACE ("LRC_EQUATIONSYMTOKS1");
      break;

    case LRC_EQUATIONSYMTOKS2:
      LTRACE ("LRC_EQUATIONSYMTOKS2");
      break;

    case LRC_EQSEPSYMTOKS1:
      LTRACE ("LRC_EQSEPSYMTOKS1");
      break;

    case LRC_EQSEPSYMTOKS2:
      LTRACE ("LRC_EQSEPSYMTOKS2");
      break;

    case SELECTSYMTOKS1:
      LTRACE ("SELECTSYMTOKS1");
      break;

    case SELECTSYMTOKS2:
      LTRACE ("SELECTSYMTOKS2");
      break;

    case OPENSYMTOKS1:
      LTRACE ("OPENSYMTOKS1");
      break;

    case OPENSYMTOKS2:
      LTRACE ("OPENSYMTOKS2");
      break;

    case SEPSYMTOKS1:
      LTRACE ("SEPSYMTOKS1");
      break;

    case SEPSYMTOKS2:
      LTRACE ("SEPSYMTOKS2");
      break;

    case CLOSESYMTOKS1:
      LTRACE ("CLOSESYMTOKS1");
      break;

    case CLOSESYMTOKS2:
      LTRACE ("CLOSESYMTOKS2");
      break;

    case SIMPLEIDTOKS1:
      LTRACE ("SIMPLEIDTOKS1");
      break;

    case SIMPLEIDTOKS2:
      LTRACE ("SIMPLEIDTOKS2");
      break;

    case MAPSYMTOKS1:
      LTRACE ("MAPSYMTOKS1");
      break;

    case MAPSYMTOKS2:
      LTRACE ("MAPSYMTOKS2");
      break;

    case MARKERSYMTOKS1:
      LTRACE ("MARKERSYMTOKS1");
      break;

    case MARKERSYMTOKS2:
      LTRACE ("MARKERSYMTOKS2");
      break;

    case COMMENTSYMTOKS1:
      LTRACE ("COMMENTSYMTOKS1");
      break;

    case COMMENTSYMTOKS2:
      LTRACE ("COMMENTSYMTOKS2");
      break;

    case QUANTIFIERSYMTOK1:
      LTRACE ("QUANTIFERSYMTOK1");
      ProcessToken (LST_QUANTIFIERSYM);
      break;

    case LOGICALOPTOK1:
      LTRACE ("LOGICALOPTOK1");
      ProcessToken (LST_LOGICALOP);
      break;

    case LRC_EQOPTOK1:
      LTRACE ("LRC_EQOPTOK1");
      ProcessToken (LST_EQOP);
      break;

    case LRC_EQUATIONSYMTOK1:
      LTRACE ("LRC_EQUATIONSYMTOK1");
      ProcessToken (LST_EQUATIONSYM);
      break;

    case LRC_EQSEPSYMTOK1:
      LTRACE ("LRC_EQSEPSYMTOK1");
      ProcessToken (LST_EQSEPSYM);
      break;

    case SELECTSYMTOK1:
      LTRACE ("SELECTSYMTOK1");
      ProcessToken (LST_SELECTSYM);
      break;

    case OPENSYMTOK1:
      LTRACE ("OPENSYMTOK1");
      ProcessToken (LST_OPENSYM);
      break;

    case SEPSYMTOK1:
      LTRACE ("SEPSYMTOK1");
      ProcessToken (LST_SEPSYM);
      break;

    case CLOSESYMTOK1:
      LTRACE ("CLOSESYMTOK1");
      ProcessToken (LST_CLOSESYM);
      break;

    case SIMPLEIDTOK1:
      LTRACE ("SIMPLEIDTOK1");
      ProcessToken (LST_SIMPLEID);
      break;

    case MAPSYMTOK1:
      LTRACE ("MAPSYMTOK1");
      ProcessToken (LST_MAPSYM);
      break;

    case MARKERSYMTOK1:
      LTRACE ("MARKERSYMTOK1");
      ProcessToken (LST_MARKERSYM);
      break;

    case COMMENTSYMTOK1:
      LTRACE ("COMMENTSYMTOK1");
      ProcessToken (LST_COMMENTSYM);
      break;

    case SYNCLASS1:
      LTRACE ("SYNCLASS1");
      ProcessSynonym ();
      break;

    case OLDTOKEN1:
      LTRACE ("OLDTOKEN1");
      break;

    case NEWTOKEN1:
      LTRACE ("NEWTOKEN1");
      break;

    default:
      llcontbuglit ("InitReduce: bad switch");
      break;

    }				/* end switch	    */
}				/* end InitReduce () */



/* Reset the first character of the predefined extensionChar keywords when  */
/* the extensionChar changes.  e.g. "extensionChar @" changes "forall" to   */
/* "@forall".								    */

static void
UpdateXCharKeywords (charCode xCharCode)
{
  char xChar = (char) xCharCode;
  char *str;

  str = ltoken_getTextChars (ltoken_forall);
  *str = xChar;

  str = ltoken_getTextChars (ltoken_and);
  *str = xChar;

  str = ltoken_getTextChars (ltoken_or);
  *str = xChar;

  str = ltoken_getTextChars (ltoken_implies);
  *str = xChar;

  str = ltoken_getTextChars (ltoken_eq);
  *str = xChar;

  str = ltoken_getTextChars (ltoken_neq);
  *str = xChar;

  str = ltoken_getTextChars (ltoken_equals);
  *str = xChar;

  str = ltoken_getTextChars (ltoken_eqsep);
  *str = xChar;

  str = ltoken_getTextChars (ltoken_select);
  *str = xChar;

  str = ltoken_getTextChars (ltoken_open);
  *str = xChar;

  str = ltoken_getTextChars (ltoken_sep);
  *str = xChar;

  str = ltoken_getTextChars (ltoken_close);
  *str = xChar;

  str = ltoken_getTextChars (ltoken_id);
  *str = xChar;

  str = ltoken_getTextChars (ltoken_arrow);
  *str = xChar;

  str = ltoken_getTextChars (ltoken_marker);
  *str = xChar;

  str = ltoken_getTextChars (ltoken_comment);
  *str = xChar;

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
      && lscanCharClass (firstChar) == SINGLECHAR)
    {
     /* Is a single character that has not been defined before.	    */
     /* Can only have one extension char.  Release old one. */
      lsetCharClass (firstChar, CHC_EXTENSION);
      
      /* this is a (bogus) type bug! caught by splint */
      /* lsetCharClass (currentExtensionChar, SINGLECHAR); */

      lsetCharClass ((char) currentExtensionChar, SINGLECHAR);

      currentExtensionChar = (charCode) firstChar;
      UpdateXCharKeywords (currentExtensionChar);
    }
  else
    {
     /* Already redefined.  Don't allow to be redefined. */
      LocalUserError (stackToken, "character is already defined, cannot redefine");
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

  if (LSLIsEndComment (firstChar))
    {
      LocalUserError (stackToken,
		    "already defined as a endCommentChar, cannot redefine");
    }
  else
    {
      lsetEndCommentChar (firstChar, TRUE);
    }
  ltoken_free (stackToken);
}

static void
ProcessSingleChar (charCode code)
{
  ltoken stackToken = LSLGenTopPopShiftStack ();
  char firstChar = cstring_firstChar (ltoken_unparse (stackToken));

  if (!defineSingleChar[(int)firstChar]
      && lscanCharClass (firstChar) == SINGLECHAR)
    {
      /* Is a single character that has not been defined before.	    */
      /* It's OK to redefine once. */
      lsetCharClass (firstChar, code);
      /* OK to mark as a defined singleChar even if not.  Only check	    */
      /* defineSingleChar[] if defining a singleChar.			    */
      defineSingleChar[(int)firstChar] = TRUE;
    }
  else
    {
      LocalUserError (stackToken, "character is already defined, cannot redefine");
    }
  ltoken_free (stackToken);
}

static void
ProcessToken (ltokenCode code)
{
  ltoken stackToken, temp;
  lsymbol sym;

  stackToken = LSLGenTopPopShiftStack ();
  sym = ltoken_getText (stackToken);

  if (LSLIsSyn (sym))
    {
      LocalUserError (stackToken,
		      "already defined as a synonym, cannot redefine");
    }

  /* Get the token from the token table, so can check if the token    */
  /* was updated by a previous token.				    */
  temp = LSLGetToken (sym);
  
  if (ltoken_isStateDefined (temp))
    {
      if ((code == LST_OPENSYM && sym == lsymbol_fromChars ("[")) ||
	  (code == LST_CLOSESYM && sym == lsymbol_fromChars ("]")))
	{
	  /* ignore "openSym [" and "closeSym ]" TokenClass */
	  ltoken_free (stackToken);
	  return;
	}
      else
	{
	  LocalUserError (stackToken, "already defined, cannot redefine");
	  PrintToken (temp);
	}
    }
  
  LSLUpdateToken (code, ltoken_getText (stackToken), TRUE);
  ltoken_free (stackToken);
}


static void
ProcessSynonym (void)
{
  ltoken newtok;
  ltoken oldtok;

  newtok = LSLGenTopPopShiftStack ();
  oldtok = LSLGenTopPopShiftStack ();

  if (ltoken_wasSyn (newtok))
    {
     /* The token has a synonym.  This means that the synonym was in the */
     /* init file, so complain about redefining as a synonym again	    */
      LocalUserError (newtok, "newtok already is a synonym, cannot redefine");
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

  LSLAddSyn (ltoken_getText (newtok), ltoken_getText (oldtok));
  ltoken_free (oldtok);
  ltoken_free (newtok);
}


/*
 * Utilities, in alphabetical order
 */

static void
LocalUserError (ltoken t, /*@temp@*/ char *msg)
{
  lldiagmsg (message ("%s %s in the LSL init file:", 
		      ltoken_unparse (t), cstring_fromChars (msg)));

  ltoken_free (nextToken);
  nextToken = LSLScanNextToken ();	       

  while (ltoken_getCode (nextToken) != LST_EOL)
    {
      ltoken_free (nextToken);
      nextToken = LSLScanNextToken ();
    }
}

/*
**  Required initialization and cleanup routines
*/

static /*@exposed@*/ ltoken insertSimpleToken (char *text) 
  /*@modifies internalState@*/
{
  return (LSLInsertToken (LST_SIMPLEID, lsymbol_fromChars (text), 0, FALSE));
}

static void
lslinit_initProcessInitFile (void)
{
  int i;

  LSLGenInit (TRUE);		/* parsing LSLinit not LCLinit */

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

  for (i = 0; i <= LASTCHAR; i++)
    {
      defineSingleChar[i] = FALSE;
    }
  
  /*
  ** Record the current extension character so can redefine back to	    
  ** singleChar if a new extension character is redefined.		    
  */

  currentExtensionChar = (charCode) CHAREXTENDER;

  LSLReportEolTokens (TRUE);
  ltoken_free (nextToken);
  nextToken = LSLScanNextToken ();	       
}

void lslinit_process (void)
  /*@globals undef g_symtab; @*/
  /*@modifies g_symtab, internalState, fileSystem; @*/
{
  /*
  ** Open init file provided by user, or use the default LCL init file 
  */
  
  cstring larchpath = context_getLarchPath ();
  inputStream initstream = inputStream_undefined;

  setCodePoint ();

  if (inputStream_isUndefined (s_initFile))
    {
      s_initFile = inputStream_create (cstring_makeLiteral (INITFILENAME), 
				       cstring_makeLiteralTemp (LCLINIT_SUFFIX),
				       FALSE);
      
      if (!inputStream_getPath (larchpath, s_initFile))
	{
	  lldiagmsg (message ("Continuing without LCL init file: %s",
			      inputStream_fileName (s_initFile)));
	}
      else 
	{
	  if (!inputStream_open (s_initFile))
	    {
	      lldiagmsg (message ("Continuing without LCL init file: %s",
				  inputStream_fileName (s_initFile)));
	    }
	}
    }
  else 
    {
      if (!inputStream_open (s_initFile))
	{
	  lldiagmsg (message ("Continuing without LCL init file: %s",
			      inputStream_fileName (s_initFile)));
	}
    }

  /* Initialize checker */

  lsymbol_initMod ();
  LCLSynTableInit ();

  setCodePoint ();

  LCLSynTableReset ();
  LCLTokenTableInit ();

  setCodePoint ();

  LCLScanLineInit ();
  setCodePoint ();
  LCLScanLineReset ();
  setCodePoint ();
  LCLScanInit ();

  setCodePoint ();

  /* need this to initialize LCL checker */

  llassert (inputStream_isDefined (s_initFile));      
  if (inputStream_isOpen (s_initFile))
    {
      setCodePoint ();

      LCLScanReset (s_initFile);
      lclinit_initMod ();
      lclinit_reset ();

      setCodePoint ();
      lclinit_process ();
      lclinit_cleanup ();

      setCodePoint ();
      check (inputStream_close (s_initFile));
    }
  
  /* Initialize LSL init files, for parsing LSL signatures from LSL */
  
  initstream = inputStream_create (cstring_makeLiteral ("lslinit.lsi"), 
				    cstring_makeLiteralTemp (".lsi"),
				    FALSE);
  
  if (!inputStream_getPath (larchpath, initstream))
    {
      lldiagmsg (message ("Continuing without LSL init file: %s",
			  inputStream_fileName (initstream)));
    }
  else 
    {
      if (!inputStream_open (initstream))
	{
	  lldiagmsg (message ("Continuing without LSL init file: %s",
			      inputStream_fileName (initstream)));
	}
    }
      
  setCodePoint ();
  lsynTableInit ();
  lsynTableReset ();

  setCodePoint ();
  ltokenTableInit ();

  setCodePoint ();
  lscanLineInit ();
  lscanLineReset ();
  LSLScanInit ();

  if (inputStream_isOpen (initstream))
    {
      setCodePoint ();
      LSLScanReset (initstream);
      lslinit_initProcessInitFile ();
      lslinit_processInitFile ();
      check (inputStream_close (initstream));
    }
      
  inputStream_free (initstream);
  
  if (lclHadError ())
    {
      lclplainerror 
	(cstring_makeLiteral ("LSL init file error.  Attempting to continue."));
    }
  
  setCodePoint ();
  g_symtab = symtable_new ();
  
  /* 
  ** sort_init must come after symtab has been initialized 
  */
  sort_init ();
  abstract_init ();
  setCodePoint ();
  
  /* 
  ** Equivalent to importing old spec_csupport.lcl
  ** define immutable LCL type "bool" and bool constants TRUE and FALSE
  ** and initialized them to be equal to LSL's "true" and "false".
  **
  ** Reads in CTrait.syms (derived from CTrait.lsl) on LARCH_PATH.
  */
      
  LCLBuiltins (); 
  LCLReportEolTokens (FALSE);
}
