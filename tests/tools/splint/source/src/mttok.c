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
** mttok.c - based on lltok.c
*/

# include "splintMacros.nf"
# include "basic.h"
# include "mtgrammar.h"

cstring
mttok_unparse (mttok tok)
{
  char *lit;

  switch (tok->tok)
    {
    case EOF:               lit = "<EOF>"; break;
    case MT_STATE:          lit = "attribute"; break;
    case MT_GLOBAL:         lit = "global"; break;
    case MT_CONTEXT:        lit = "context"; break;
    case MT_ONEOF:          lit = "oneof"; break;
    case MT_AS:             lit = "as"; break;
    case MT_END:            lit = "end"; break;
    case MT_DEFAULTS:       lit = "defaults"; break;
    case MT_DEFAULT:        lit = "default"; break;
    case MT_REFERENCE:      lit = "reference"; break;
    case MT_PARAMETER:      lit = "parameter"; break;
    case MT_RESULT:         lit = "result"; break;
    case MT_CLAUSE:         lit = "clause"; break;
    case MT_LITERAL:        lit = "literal"; break;
    case MT_NULL:           lit = "null"; break;
    case MT_ANNOTATIONS:    lit = "annotations"; break;
    case MT_ARROW:          lit = "==>"; break;
    case MT_MERGE:          lit = "merge"; break;
    case MT_TRANSFERS:      lit = "transfers"; break;
    case MT_PRECONDITIONS:  lit = "preconditions"; break;
    case MT_POSTCONDITIONS: lit = "postconditions"; break;
    case MT_ERROR:          lit = "error"; break;
    case MT_PLUS:           lit = "+"; break;
    case MT_STAR:           lit = "*"; break;
    case MT_LPAREN:         lit = "("; break;
    case MT_RPAREN:         lit = ")"; break;
    case MT_LBRACE:         lit = "{"; break;
    case MT_RBRACE:         lit = "}"; break;
    case MT_LBRACKET:       lit = "["; break;
    case MT_RBRACKET:       lit = "]"; break;
    case MT_COMMA:          lit =","; break;
    case MT_BAR:            lit = "|"; break;
    case MT_CHAR: lit = "char"; break;
    case MT_INT: lit = "int"; break; 
    case MT_FLOAT: lit = "float"; break;
    case MT_DOUBLE: lit = "double"; break;
    case MT_VOID: lit = "void"; break;
    case MT_ANYTYPE: lit = "anytype"; break;
    case MT_INTEGRALTYPE: lit = "integraltype"; break;
    case MT_UNSIGNEDINTEGRALTYPE: lit = "unsignedintegraltype"; break;
    case MT_SIGNEDINTEGRALTYPE: lit = "signedintegraltype"; break; 
    case MT_CONST: lit = "const"; break;
    case MT_VOLATILE: lit = "volatile"; break;

    case MT_IDENT:          return (message ("identifier: <%s>", tok->text)); 
    case MT_STRINGLIT:      return (message ("literal: <%s>", tok->text)); 
    case MT_BADTOK:         lit = "<error token>"; break;
    default:
      DPRINTF (("Bad token: [%d]", tok->tok));
      BADBRANCH;
      /* BADDEFAULT; */
    }
  
  return cstring_makeLiteral (lit);
}

mttok
mttok_create (int tok, cstring text, fileloc loc)
{
  mttok l = (mttok) dmalloc (sizeof (*l));

  l->tok = tok;
  l->text = text;
  l->loc = loc;

  return (l);
}

fileloc mttok_stealLoc (mttok t)
{
  fileloc res = t->loc;
  t->loc = fileloc_undefined;
  return res;
}

void mttok_free (mttok t) 
{
  fileloc_free (t->loc);
  cstring_free (t->text);
  sfree (t);
}

bool mttok_isError (mttok t)
{
  return ((t)->tok == MT_ERROR);
}

bool mttok_isIdentifier (mttok t)
{
  return ((t)->tok == MT_IDENT);
}
