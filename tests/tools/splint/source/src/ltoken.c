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
** ltoken.c
*/

# include "splintMacros.nf"
# include "basic.h"
# include "llgrammar.h"
# include "scanline.h"
# include "lclscanline.h"

/* 
** Place to store the \keyword (\forall ...) tokens.  These tokens will	    
** have to be modified when the extensionChar ("\") changes. 		    
** set in LCLScanLineInit of lclscanline.c or in scanline.c 
*/

/*@-namechecks@*/ /* These should all start with g_ */
ltoken ltoken_forall;
ltoken ltoken_exists;
ltoken ltoken_true;
ltoken ltoken_false;
ltoken ltoken_not;
ltoken ltoken_and;
ltoken ltoken_or;
ltoken ltoken_implies;
ltoken ltoken_eq;
ltoken ltoken_neq;
ltoken ltoken_equals;
ltoken ltoken_eqsep;
ltoken ltoken_select;
ltoken ltoken_open;
ltoken ltoken_sep;
ltoken ltoken_close;
ltoken ltoken_id;
ltoken ltoken_arrow;
ltoken ltoken_marker;
ltoken ltoken_pre;
ltoken ltoken_post;
ltoken ltoken_comment;
ltoken ltoken_any;
ltoken ltoken_compose;
ltoken ltoken_if;
ltoken ltoken_result;
ltoken ltoken_typename;
ltoken ltoken_bool;
ltoken ltoken_farrow;
ltoken ltoken_lbracked;
ltoken ltoken_rbracket;
/*@=namechecks@*/

static /*@notnull@*/ ltoken ltoken_new (void)
{
  ltoken tok = (ltoken) dmalloc (sizeof (*tok));

  tok->code = NOTTOKEN;
  tok->col = 0;
  tok->line = 0;
  tok->text = lsymbol_undefined;
  tok->fname = lsymbol_undefined;
  tok->rawText = lsymbol_undefined;
  tok->defined = FALSE;
  tok->hasSyn = FALSE;

  return tok;
}

ltoken ltoken_create (ltokenCode code, lsymbol text)
{
  ltoken tok = ltoken_new ();

  tok->code = code;
  tok->text = text;
  
  return tok;
}

ltoken ltoken_createRaw (ltokenCode code, lsymbol text)
{
  ltoken tok = ltoken_new ();

  tok->code = code;
  tok->rawText = text;

  return tok;
}

ltoken ltoken_createType (ltokenCode code, SimpleIdCode idtype, lsymbol text)
{
  ltoken tok = ltoken_new ();

  /* no...what's the real llassert (code == simpleId); */

  tok->code = code;
  tok->idtype = idtype;
  tok->text = text;
  
  return tok;
}

ltoken ltoken_createFull (ltokenCode code, lsymbol text, 
			  cstring file, int line, 
			  int col)
{
  ltoken tok = (ltoken) dmalloc (sizeof (*tok));
  
  tok->code = code;
  tok->text = text;
  tok->fname = lsymbol_fromString (file);
  tok->line = line;
  tok->col = col;
  tok->rawText = lsymbol_undefined;
  tok->defined = FALSE;
  tok->hasSyn = FALSE;
  
  return tok;
}

static /*@only@*/ cstring LCLTokenCode_unparseCodeName (ltokenCode t)
{
  switch (t)
    {
    case NOTTOKEN:         return cstring_makeLiteral ("*** NOTTOKEN ***");
    case quantifierSym:    return cstring_makeLiteral ("QUANTIFIERSYM");      
    case logicalOp:        return cstring_makeLiteral ("LOGICALOP");
    case selectSym:        return cstring_makeLiteral ("SELECTSYM");    
    case openSym:          return cstring_makeLiteral ("OPENSYM");
    case preSym:           return cstring_makeLiteral ("\\pre");
    case postSym:          return cstring_makeLiteral ("\\post");
    case anySym:           return cstring_makeLiteral ("\\any");
    case sepSym:           return cstring_makeLiteral ("SEPSYM");
    case closeSym:         return cstring_makeLiteral ("CLOSESYM");
    case simpleId:         return cstring_makeLiteral ("simpleId");
    case LLT_TYPEDEF_NAME:     return cstring_makeLiteral ("TYPEDEF_NAME");
    case mapSym:           return cstring_makeLiteral ("MAPSYM");
    case markerSym:        return cstring_makeLiteral ("MARKERSYM");
    case commentSym:       return cstring_makeLiteral ("COMMENTSYM");
    case simpleOp:         return cstring_makeLiteral ("SIMPLEOP");
    case LLT_COLON:            return cstring_makeLiteral ("COLON");
    case LLT_COMMA:            return cstring_makeLiteral ("COMMA");
    case LLT_EQUALS:           return cstring_makeLiteral ("LLT_EQUALS");
    case LLT_LBRACE:           return cstring_makeLiteral ("LBRACE");
    case LLT_LBRACKET:         return cstring_makeLiteral ("LBRACKET");
    case LLT_LPAR:             return cstring_makeLiteral ("LPAR");
    case LLT_QUOTE:            return cstring_makeLiteral ("QUOTE");
    case LLT_RBRACE:           return cstring_makeLiteral ("RBRACE");
    case LLT_RBRACKET:         return cstring_makeLiteral ("RBRACKET");
    case LLT_RPAR:             return cstring_makeLiteral ("RPAR");
    case LLT_SEMI:             return cstring_makeLiteral ("SEMI");
    case LLT_VERTICALBAR:      return cstring_makeLiteral ("VERTICALBAR");
    case eqOp:             return cstring_makeLiteral ("EQOP");
    case LLT_MULOP:            return cstring_makeLiteral ("MULOP");
    case LLT_WHITESPACE:       return cstring_makeLiteral ("WHITESPACE,");
    case LEOFTOKEN:        return cstring_makeLiteral ("EOFTOKEN");
    case LLT_EOL:         return cstring_makeLiteral ("LLT_EOL");
    case LLT_CCHAR:            return cstring_makeLiteral ("CCHAR");
    case LLT_CFLOAT:           return cstring_makeLiteral ("CFLOAT");
    case LLT_CINTEGER:         return cstring_makeLiteral ("CINTEGER");
    case LLT_LCSTRING:         return cstring_makeLiteral ("CSTRING");
    case LLT_ALL:              return cstring_makeLiteral ("allTOKEN");
    case LLT_ANYTHING:         return cstring_makeLiteral ("anythingTOKEN");
    case LLT_BE:               return cstring_makeLiteral ("beTOKEN");
    case LLT_CONSTANT:         return cstring_makeLiteral ("constantTOKEN");
    case LLT_ELSE:         return cstring_makeLiteral ("elseTOKEN");
    case LLT_ENSURES:      return cstring_makeLiteral ("ensuresTOKEN");
    case LLT_IF:               return cstring_makeLiteral ("ifTOKEN");
    case LLT_IMMUTABLE:        return cstring_makeLiteral ("immutableTOKEN");
    case LLT_OBJ:              return cstring_makeLiteral ("objTOKEN");
    case LLT_IMPORTS:          return cstring_makeLiteral ("importsTOKEN");
    case LLT_CONSTRAINT:       return cstring_makeLiteral ("constraintTOKEN");
    case LLT_LET:              return cstring_makeLiteral ("letTOKEN");
    case LLT_MODIFIES:         return cstring_makeLiteral ("modifiesTOKEN");
    case LLT_CLAIMS:           return cstring_makeLiteral ("claimsTOKEN");
    case LLT_MUTABLE:          return cstring_makeLiteral ("mutableTOKEN");
    case LLT_FRESH:            return cstring_makeLiteral ("freshTOKEN");
    case LLT_NOTHING:          return cstring_makeLiteral ("nothingTOKEN");
    case LLT_PRIVATE:          return cstring_makeLiteral ("privateTOKEN");
    case LLT_SPEC:             return cstring_makeLiteral ("specTOKEN");
    case LLT_REQUIRES:         return cstring_makeLiteral ("requiresTOKEN");
    case LLT_BODY:             return cstring_makeLiteral ("bodyTOKEN");
    case LLT_RESULT:           return cstring_makeLiteral ("resultTOKEN");
    case LLT_SIZEOF:           return cstring_makeLiteral ("sizeofTOKEN");
    case LLT_THEN:             return cstring_makeLiteral ("thenTOKEN");
    case LLT_TYPE:             return cstring_makeLiteral ("typeTOKEN");
    case LLT_TYPEDEF:          return cstring_makeLiteral ("typedefTOKEN");
    case LLT_UNCHANGED:        return cstring_makeLiteral ("unchangedTOKEN");
    case LLT_USES:             return cstring_makeLiteral ("usesTOKEN");
    case LLT_CHAR:             return cstring_makeLiteral ("charTOKEN");
    case LLT_CONST:            return cstring_makeLiteral ("constTOKEN");
    case LLT_DOUBLE:           return cstring_makeLiteral ("doubleTOKEN");
    case LLT_ENUM:         return cstring_makeLiteral ("enumTOKEN");
    case LLT_FLOAT:            return cstring_makeLiteral ("floatTOKEN");
    case LLT_INT:              return cstring_makeLiteral ("intTOKEN");
    case LLT_LONG:             return cstring_makeLiteral ("longTOKEN");
    case LLT_SHORT:            return cstring_makeLiteral ("shortTOKEN");
    case LLT_STRUCT:           return cstring_makeLiteral ("structTOKEN");
    case LLT_SIGNED:       return cstring_makeLiteral ("signedTOKEN");
    case LLT_UNION:            return cstring_makeLiteral ("unionTOKEN");
    case LLT_UNKNOWN:          return cstring_makeLiteral ("unknownTOKEN");
    case LLT_UNSIGNED:         return cstring_makeLiteral ("unsignedTOKEN");
    case LLT_VOID:             return cstring_makeLiteral ("voidTOKEN");
    case LLT_VOLATILE:         return cstring_makeLiteral ("volatileTOKEN");
    case LLT_TELIPSIS:         return cstring_makeLiteral ("elipsisTOKEN");
    case LLT_ITER:             return cstring_makeLiteral ("iterTOKEN");
    case LLT_YIELD:            return cstring_makeLiteral ("yieldTOKEN");
    default:               return cstring_makeLiteral ("*** invalid token code ***"); 
    } /* end switch */
}

cstring ltoken_unparseCodeName (ltoken tok)
{
  return LCLTokenCode_unparseCodeName (ltoken_getCode (tok));
}

/*@observer@*/ cstring ltoken_unparse (ltoken s)
{
  if (ltoken_isValid (s))
    {
      return (lsymbol_toString (s->text));
    }
  else
    {
      return cstring_undefined;
    }
}

ltoken ltoken_copy (ltoken tok)
{
  if (ltoken_isValid (tok))
    {
      ltoken ret = (ltoken) dmalloc (sizeof (*ret));
      
      ret->code = tok->code;
      ret->text = tok->text;
      ret->fname = tok->fname;
      ret->line = tok->line;
      ret->col = tok->col;
      ret->rawText = tok->rawText;
      ret->defined = tok->defined;
      ret->hasSyn = tok->hasSyn;
      ret->idtype = tok->idtype;
      ret->intfield = tok->intfield;
      
      return ret;
    }
  else
    {
      return ltoken_undefined;
    }
}

lsymbol ltoken_getRawText (ltoken tok)
{
  if (ltoken_isValid (tok))
    {
      lsymbol ret = tok->rawText;

      if (lsymbol_isUndefined (ret))
	{
	  ret = tok->text;
	}
      
      return ret;
    }
  else
    {
      return lsymbol_undefined;
    }
}

/*@only@*/ cstring ltoken_unparseLoc (ltoken t)
{
  if (ltoken_getCode (t) != NOTTOKEN)
    {
      cstring res = fileloc_unparseRawCol (ltoken_fileName (t), 
					   ltoken_getLine (t),
					   ltoken_getCol (t));
      return res;
    }
  else
    {
      return cstring_makeLiteral ("*** Not Token ***"); 
    }
}

void ltoken_markOwned (/*@owned@*/ ltoken tok)
{
    sfreeEventually (tok);
}

void ltoken_free (/*@only@*/ ltoken tok)
{
  sfree (tok); 
}

bool ltoken_isSingleChar (char c)
{
  return (LCLScanCharClass (c) == SINGLECHAR);
}
