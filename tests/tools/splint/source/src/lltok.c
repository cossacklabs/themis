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
** lltok.c
*/

# include "splintMacros.nf"
# include "basic.h"

# include "cgrammar.h"
# include "cgrammar_tokens.h"

bool
lltok_isSemi (lltok tok)
{
  return (tok->tok == TSEMI);
}

bool
lltok_isMult (lltok tok)
{
  return (tok->tok == TMULT);
}

bool lltok_isIncOp (lltok tok)
{
  return (tok->tok == INC_OP);
}

bool lltok_isDecOp  (lltok tok)
{
  return (tok->tok == DEC_OP);
}


/* DRL added this function 10/23/2000 for boolean stuff */
bool lltok_isEqOp (lltok tok)
{
  return (tok->tok == EQ_OP);
}

bool lltok_isNotEqOp (lltok tok)
{
  return (tok->tok == NE_OP);
}

/* DRL added this function 10/25/2000 for boolean stuff */
bool lltok_isAndOp (lltok tok)
{
  return (tok->tok == AND_OP);
}

bool lltok_isOrOp (lltok tok)
{
  return (tok->tok == OR_OP);
}

bool lltok_isNotOp (lltok tok)
{
  return (tok->tok ==  TEXCL);
}
/*drl7x added this function 11/20/00 */

bool lltok_isLt_Op (lltok tok)
{
  return (tok->tok ==  TLT);
}

bool lltok_isGt_Op (lltok tok)
{
  return (tok->tok ==  TGT);
}
  
bool lltok_isGe_Op (lltok tok)
{
  return (tok->tok ==  GE_OP);
}

bool lltok_isLe_Op (lltok tok)
{
  return (tok->tok ==  LE_OP);
}

/* end drl7x added */


/*drl7x added 11 30 2000*/
bool  lltok_isPlus_Op (lltok tok)
{
  return (tok->tok ==  TPLUS);
}

bool  lltok_isMinus_Op (lltok tok)
{
  return (tok->tok ==  TMINUS);
}

/*drl7x added 6 6 2001 */


bool  lltok_isAmpersand_Op (lltok tok)
{
  return (tok->tok == TAMPERSAND);
}

/*end drl added */

/*drl7x added 6 15 2001 */

bool  lltok_isExcl_Op (lltok tok)
{
  return (tok->tok == TEXCL);
}

bool  lltok_isTilde_Op (lltok tok)
{
  return (tok->tok == TTILDE);
}
/*end drl added */


bool lltok_isEnsures (lltok tok)
{
  return (tok->tok == QPOSTCLAUSE);
}

bool lltok_isRequires (lltok tok)
{
  return (tok->tok == QPRECLAUSE);
}

cstring
lltok_unparse (lltok tok)
{
  char *lit;

  switch (tok->tok)
    {
    case BREAK:      lit = "break"; break;
    case CASE:       lit = "case"; break;
    case CONTINUE:   lit = "continue"; break;
    case DEFAULT:    lit = "default"; break;
    case DO:         lit = "do"; break;
    case CDOUBLE:    lit = "double"; break;
    case CELSE:      lit = "else"; break;
    case CFOR:       lit = "for"; break;
    case GOTO:       lit = "goto"; break;
    case CIF:        lit = "if"; break;
    case RETURN:     lit = "return"; break;
    case CSIZEOF:    lit = "sizeof"; break;
    case SWITCH:     lit = "switch"; break;
    case WHILE:      lit = "while"; break;
    case CSTRUCT:    lit = "struct"; break;
    case CTYPEDEF:   lit = "typedef"; break;
    case CUNION:     lit = "union"; break;
    case CENUM:      lit = "enum"; break;
    case QEXTERN:     lit = "extern"; break;
    case QAUTO:       lit = "auto"; break;
    case QSTATIC:     lit = "static"; break;
    case RIGHT_ASSIGN: lit = ">>="; break;
    case LEFT_ASSIGN:  lit = "<<="; break;
    case ADD_ASSIGN: lit = "+="; break;
    case SUB_ASSIGN: lit = "-="; break;
    case MUL_ASSIGN: lit = "*="; break;
    case DIV_ASSIGN: lit = "/="; break;
    case MOD_ASSIGN: lit = "%="; break;
    case AND_ASSIGN: lit = "&="; break;
    case XOR_ASSIGN: lit = "^="; break;
    case OR_ASSIGN:  lit = "|="; break;
    case RIGHT_OP:   lit = ">>"; break;
    case LEFT_OP:    lit = "<<"; break;
    case INC_OP:     lit = "++"; break; 
    case DEC_OP:     lit = "--"; break;
    case ARROW_OP:   lit = "->"; break;
    case AND_OP:     lit = "&&"; break;
    case OR_OP:      lit = "||"; break;
    case LE_OP:      lit = "<="; break;
    case GE_OP:      lit = ">="; break;
    case EQ_OP:      lit = "=="; break;
    case NE_OP:      lit = "!="; break;
    case TSEMI:      lit = ";"; break;
    case TLBRACE:    lit = "{"; break;
    case TRBRACE:    lit = "}"; break;
    case TCOMMA:     lit = ","; break;
    case TCOLON:     lit = ":"; break;
    case TASSIGN:    lit = "="; break;
    case TLPAREN:    lit = "("; break;
    case TRPAREN:    lit = ")"; break;
    case TLSQBR:     lit = "["; break;
    case TRSQBR:     lit = "]"; break;
    case TDOT:       lit = "."; break; /* evans 2003-05-31: Fix reported by psanzani. */
    case TAMPERSAND: lit = "&"; break;
    case TEXCL:      lit = "!"; break;
    case TTILDE:     lit = "~"; break;
    case TMINUS:     lit = "-"; break;
    case TPLUS:      lit = "+"; break;
    case TMULT:      lit = "*"; break;
    case TDIV:       lit = "/"; break;
    case TPERCENT:   lit = "%"; break;
    case TLT:        lit = "<"; break;
    case TGT:        lit = ">"; break;
    case TCIRC:      lit = "^"; break;
    case TBAR:       lit = "|"; break;
    case TQUEST:     lit = "?"; break;  
    case QOUT:       lit = "out"; break;
    case QONLY:      lit = "only"; break;
    case QKEEP:      lit = "keep"; break;
    case QKEPT:      lit = "kept"; break;
    case QTEMP:      lit = "temp"; break;
    case QSHARED:    lit = "shared"; break;
    case QUNIQUE:    lit = "unique"; break;
    case QREF:       lit = "ref"; break;
    case QCHECKED:   lit = "checked"; break;
    case QCHECKEDSTRICT: lit = "checkedstrict"; break;
    case QCHECKMOD:  lit = "checkmod"; break;
    case QUNCHECKED: lit = "unchecked"; break;
    case QPRECLAUSE: lit = "requires"; break;
    case QPOSTCLAUSE: lit = "ensures"; break;
    case QTRUENULL:  lit = "truenull"; break;
    case QFALSENULL: lit = "falsenull"; break;
    case QRETURNED:  lit = "returned"; break;
    case QEXPOSED:   lit = "exposed"; break;
    case QNULL:      lit = "null"; break;
    case QRELNULL:   lit = "refnull"; break;
    case QOBSERVER:  lit = "observer"; break;
    case QPARTIAL:   lit = "partial"; break;
    case QSPECIAL:   lit = "special"; break;
    case QUSES:      lit = "uses"; break;
    case QDEFINES:   lit = "defines"; break;
    case QALLOCATES: lit = "allocates"; break;
    case QSETS:      lit = "sets"; break;
    case QRELEASES:  lit = "releases"; break;
    case QSETBUFFERSIZE: lit = "setBufferSize"; break;
    case QSETSTRINGLENGTH: lit = "setStringLength"; break;
    BADDEFAULT;
    }
 
  return cstring_makeLiteralTemp (lit);
}

lltok
lltok_create (int tok, fileloc loc)
{
  lltok l = (lltok) dmalloc (sizeof (*l));

  l->tok = tok;
  l->loc = loc;
  DPRINTF (("Create: %s [%p]", lltok_unparse (l), loc));
  return (l);
}

void lltok_free (lltok t) 
{
  fileloc_free (t->loc);
  sfree (t);
}

void lltok_free2 (lltok t1, lltok t2) 
{
  lltok_free (t1);
  lltok_free (t2);
}

void lltok_free3 (lltok t1, lltok t2, lltok t3) 
{
  lltok_free (t1);
  lltok_free (t2);
  lltok_free (t3);
}

