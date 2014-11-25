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
** constraintTerm.c
*/

/* #define DEBUGPRINT 1 */

# include <ctype.h> /* for isdigit */
# include "splintMacros.nf"
# include "basic.h"
# include "cgrammar.h"
# include "cgrammar_tokens.h"

# include "exprChecks.h"
# include "exprNodeSList.h"

bool constraintTerm_isDefined (constraintTerm t)
{
  return t != NULL;
}

void constraintTerm_free (/*@only@*/ constraintTerm term)
{
  llassert (constraintTerm_isDefined (term));

  fileloc_free (term->loc);
  
  switch (term->kind) 
    {
    case CTT_EXPR:
      /* we don't free an exprNode*/
      break;
    case CTT_SREF:
      /* sref */
      sRef_free (term->value.sref);
      break;
    case CTT_INTLITERAL:
      /* don't free an int */
      break;
    case CTT_ERRORBADCONSTRAINTTERMTYPE:
    default:
      /* type was set incorrectly */
      llcontbug (message("constraintTerm_free type was set incorrectly"));
    }

  term->kind = CTT_ERRORBADCONSTRAINTTERMTYPE;
  free (term);
}

/*@only@*/ static/*@out@*/ constraintTerm new_constraintTermExpr (void)
{
  constraintTerm ret;
  ret = dmalloc (sizeof (* ret ) );
  ret->value.intlit = 0;
  return ret;
}


bool constraintTerm_isIntLiteral (constraintTerm term)
{
  llassert(term != NULL);
  
  if (term->kind == CTT_INTLITERAL)
    return TRUE;

  return FALSE;
}


bool constraintTerm_isInitBlock (/*@observer@*/ /*@temp@*/ constraintTerm c) /*@*/
{
  llassert (c != NULL);

  if (c->kind == CTT_EXPR)
    {
      if (exprNode_isInitBlock (c->value.expr))
	{
	  return TRUE;
	}
    }
  return FALSE;
}


bool constraintTerm_isExprNode (/*@observer@*/ /*@temp@*/ constraintTerm c) /*@*/
{
  llassert (c != NULL);

  if (c->kind == CTT_EXPR)
    {
      return TRUE;
    }
  return FALSE;
}

/*@access exprNode@*/
int constraintTerm_getInitBlockLength (/*@observer@*/ /*@temp@*/ constraintTerm c) /*@*/
{
  exprNodeList list;
  int ret;
  llassert (c != NULL);
  llassert (constraintTerm_isInitBlock (c) );
  llassert (c->kind == CTT_EXPR);

  llassert(exprNode_isDefined(c->value.expr) );

  if (exprNode_isUndefined(c->value.expr) )
    {
      return 1;
    }

  if (c->value.expr->edata == exprData_undefined)
    {
      return 1;
    }
  list = exprData_getArgs(c->value.expr->edata);

  ret = exprNodeList_size(list);

  return ret;  
}
/*@noaccess exprNode@*/


bool constraintTerm_isStringLiteral (constraintTerm c) /*@*/
{
  llassert (c != NULL);
  if (c->kind == CTT_EXPR)
    {
      if (exprNode_knownStringValue(c->value.expr) )
	{
	  return TRUE;
	}
    }
  return FALSE;
}



cstring constraintTerm_getStringLiteral (constraintTerm c)
{
  llassert (c != NULL);
  llassert (constraintTerm_isStringLiteral (c) );
  llassert (c->kind == CTT_EXPR);
  
  return (cstring_copy ( multiVal_forceString (exprNode_getValue (c->value.expr) ) ) );
}

constraintTerm constraintTerm_simplify (/*@returned@*/ constraintTerm term) /*@modifies term@*/
{
  if (term->kind == CTT_EXPR)
    {
      if ( exprNode_knownIntValue (term->value.expr ) )
	{
	  long int temp;

	  temp  = exprNode_getLongValue (term->value.expr);
	  term->value.intlit = (int)temp;
	  term->kind = CTT_INTLITERAL;
	}
    }
  return term;
}

fileloc constraintTerm_getFileloc (constraintTerm t)
{
  llassert (constraintTerm_isDefined (t));
  return (fileloc_copy (t->loc) );
}

constraintTermType constraintTerm_getKind (constraintTerm t)
{
  llassert (constraintTerm_isDefined(t) );
  
  return (t->kind);
}

/*@exposed@*/ sRef constraintTerm_getSRef (constraintTerm t)
{
  llassert (constraintTerm_isDefined(t) );
  llassert (t->kind == CTT_SREF);

  return (t->value.sref);
}

/*@only@*/ constraintTerm constraintTerm_makeExprNode (/*@dependent@*/ exprNode e)
{
  constraintTerm ret = new_constraintTermExpr ();
  ret->loc =  fileloc_copy (exprNode_loc (e));
  ret->value.expr = e;
  ret->kind = CTT_EXPR;
  ret = constraintTerm_simplify (ret);
  return ret;
}

/*@only@*/ constraintTerm constraintTerm_makesRef (/*@temp@*/ /*@observer@*/ sRef s)
{
  constraintTerm ret = new_constraintTermExpr();
  ret->loc =  fileloc_undefined;
  ret->value.sref = sRef_saveCopy(s);
  ret->kind = CTT_SREF;
  ret = constraintTerm_simplify(ret);
  return ret;
}



constraintTerm constraintTerm_copy (constraintTerm term)
{
  constraintTerm ret;
  ret = new_constraintTermExpr();
  ret->loc = fileloc_copy (term->loc);
  
  switch (term->kind)
    {
    case CTT_EXPR:
      ret->value.expr = term->value.expr;
      break;
    case CTT_INTLITERAL:
      ret->value.intlit = term->value.intlit;
      break;
      
    case CTT_SREF:
      ret->value.sref = sRef_saveCopy(term->value.sref);
      break;
    default:
      BADEXIT;
    }
  ret->kind = term->kind;
  return ret;
}

constraintTerm constraintTerm_setFileloc (/*@returned@*/ constraintTerm term, fileloc loc) 
{
  llassert(term != NULL);

  if ( fileloc_isDefined(  term->loc ) )
    fileloc_free(term->loc);

  term->loc = fileloc_copy(loc);
  return term;
}


static cstring constraintTerm_getName (constraintTerm term)
{
  cstring s;
  s = cstring_undefined;
  
  llassert (term != NULL);

  switch (term->kind)
    {
    case CTT_EXPR:

      s = message ("%s", exprNode_unparse (term->value.expr) );
      break;
    case CTT_INTLITERAL:
      s = message (" %d ", (int) term->value.intlit);
      break;
      
    case CTT_SREF:
      s = message ("%q", sRef_unparse (term->value.sref) );

      break;
    default:
      BADEXIT;
      /*@notreached@*/
      break;
    }
  
  return s;
}

constraintTerm 
constraintTerm_doSRefFixBaseParam (/*@returned@*/constraintTerm term, exprNodeList arglist) /*@modifies term@*/
{
  llassert (term != NULL);
  
  switch (term->kind)
    {
    case CTT_EXPR:

      break;
    case CTT_INTLITERAL:
      break;
      
    case CTT_SREF:
      term->value.sref = sRef_fixBaseParam (term->value.sref, arglist);
      break;
    default:
      BADEXIT;
    }
  return term;
  
}

cstring constraintTerm_unparse (constraintTerm term)  /*@*/
{
  cstring s;
  s = cstring_undefined;
  
  llassert (term != NULL);

  switch (term->kind)
    {
    case CTT_EXPR:

      s = message ("%s @ %q", exprNode_unparse (term->value.expr),
		   fileloc_unparse (term->loc) );
      break;
    case CTT_INTLITERAL:
      s = message ("%d", (int)term->value.intlit);
      break;
      
    case CTT_SREF:
      s = message ("%q", sRef_unparseDebug (term->value.sref) );

      break;
    default:
      BADEXIT;
    }
  
  return s;
}


constraintTerm constraintTerm_makeIntLiteral (long i)
{
  constraintTerm ret = new_constraintTermExpr();
  ret->value.intlit = i;
  ret->kind = CTT_INTLITERAL;
  ret->loc =  fileloc_undefined;
  return ret;
}

bool constraintTerm_canGetValue (constraintTerm term)
{
  if (term->kind == CTT_INTLITERAL)
    {
      return TRUE;
    }
  else if (term->kind == CTT_SREF)
    {
      if (sRef_hasValue (term->value.sref))
	{
	  multiVal mval = sRef_getValue (term->value.sref);

	  return multiVal_isInt (mval); /* for now, only try to deal with int values */
	}
      else
	{
	  return FALSE;
	}
    }
  else if (term->kind == CTT_EXPR)
    {
      return FALSE;
    }
  else
    {
      return FALSE;
    }
}

void constraintTerm_setValue (constraintTerm term, long value) 
{
  if (term->kind == CTT_INTLITERAL)
    {
      term->value.intlit = value;
    }
  else
    {
      BADBRANCH;
    }
}

long constraintTerm_getValue (constraintTerm term) 
{
  llassert (constraintTerm_canGetValue (term));

  if (term->kind == CTT_INTLITERAL)
    {
      return term->value.intlit; 
    }
  else if (term->kind == CTT_SREF)
    {
      if (sRef_hasValue (term->value.sref))
	{
	  multiVal mval = sRef_getValue (term->value.sref);

	  return multiVal_forceInt (mval); /* for now, only try to deal with int values */
	}
      else
	{
	  BADBRANCH;
	}
    }
  else if (term->kind == CTT_EXPR)
    {
      BADBRANCH;
    }
  else
    {
      BADBRANCH;
    }

  BADEXIT;
}

/*drl added this 10.30.001
 */

/*@exposed@*/ exprNode constraintTerm_getExprNode (constraintTerm t)
{
  llassert (t != NULL);
  
  llassert (t->kind == CTT_EXPR);

  return t->value.expr;

}

 /*@exposed@*/ sRef constraintTerm_getsRef (constraintTerm t)
{
  llassert (t != NULL);
  if (t->kind == CTT_EXPR)
    {
      return exprNode_getSref(t->value.expr);
    }

  if (t->kind == CTT_SREF)
    {
      return t->value.sref;
    }

  return sRef_undefined;
}

bool constraintTerm_probSame (constraintTerm term1, constraintTerm term2)
{
  cstring s1, s2;

  llassert (term1 !=NULL && term2 !=NULL);
     
 DPRINTF ((message
	    ("Comparing srefs for %s and  %s ", constraintTerm_print(term1), constraintTerm_print(term2)
	     )
	    )
	   );
  
  s1 = constraintTerm_getName (term1);
  s2 = constraintTerm_getName (term2);

  if (cstring_equal (s1, s2) )
    {
      DPRINTF ((message (" %q and %q are same", s1, s2 ) ) );
     return TRUE;
   }
  else
     {
     DPRINTF ((message (" %q and %q are not same", s1, s2 ) ) );
     return FALSE;
   }   
}

bool constraintTerm_similar (constraintTerm term1, constraintTerm term2)
{
  sRef s1, s2;
  
  llassert (term1 !=NULL && term2 !=NULL);
  
  if (constraintTerm_canGetValue (term1) && constraintTerm_canGetValue (term2))

    /*3/30/2003 comment updated to reflect name change form INTLITERAL to CTT_INTLITERAL*/
    /* evans 2001-07-24: was (term1->kind == CTT_INTLITERAL) && (term2->kind == CTT_INTLITERAL) ) */
    {
      long t1, t2;

      t1 = constraintTerm_getValue (term1);
      t2 = constraintTerm_getValue (term2);

      return (t1 == t2);
    }

        /*drl this if statement handles the case where constraintTerm_canGetValue only returns
	  true for term1 or term2 but no both
	  if constraintTerm_canGetValue returned tru for both we would have returned in the previous if statement
	  I suppose this could be done with xor but I've never used xor and don't feel like starting now
	  besides this way is more effecient.	  
	*/
  if (constraintTerm_canGetValue (term1) || constraintTerm_canGetValue (term2))
    {

      return FALSE;
    }

  s1 = constraintTerm_getsRef (term1);
  s2 = constraintTerm_getsRef (term2);

  if (!(sRef_isValid(s1) && sRef_isValid(s2)))
    {
      return FALSE;
    }
  
  DPRINTF((message
	    ("Comparing srefs for %s and  %s ", constraintTerm_print(term1), constraintTerm_print(term2)
	     )
	    )
	   );
  
  if (sRef_similarRelaxed(s1, s2)   || sRef_sameName (s1, s2) )
    {
      DPRINTF ((message (" %s and %s are same", constraintTerm_print(term1), constraintTerm_print(term2)  )  ));
      return TRUE;
    }
  else
    {
      DPRINTF ((message (" %s and %s are not same", constraintTerm_print(term1), constraintTerm_print(term2)  )  ));
      return FALSE;
    }       
}

void constraintTerm_dump (/*@observer@*/ constraintTerm t,  FILE *f)
{
  fileloc loc;
  constraintTermValue value;
  constraintTermType kind;
  uentry u;
  
  loc = t->loc;

  value = t->value;

  kind  = t->kind;

  fprintf(f, "%d\n", (int) kind);
  
  switch (kind)
    {
      
    case CTT_EXPR:
      u = exprNode_getUentry(t->value.expr);
      fprintf (f, "%s\n", cstring_toCharsSafe (uentry_rawName (u)));
      break;
      
    case CTT_SREF:
      {
	sRef s;

	s =  t->value.sref;
	
	if (sRef_isResult (s ) )
	  {
	    fprintf(f, "Result\n");
	  }
	else if (sRef_isParam (s))
	  {
	    int param;
	    ctype ct;
	    cstring ctString;

	    
	    ct =  sRef_getType (s); 
	    param = sRef_getParam(s);

	    ctString =  ctype_dump(ct);
	    
	    fprintf(f, "Param %s %d\n", cstring_toCharsSafe(ctString), (int) param );
	    cstring_free(ctString);
	  }
	else if (sRef_isField (s) )
	  {
	    fprintf(f, "sRef_dump %s\n", cstring_toCharsSafe(sRef_dump(s)) );
	  }
	else
	  {
	    u = sRef_getUentry(s);
	    fprintf (f, "%s\n", cstring_toCharsSafe (uentry_rawName (u)));
	  }
	
      }
      break;
      
    case CTT_INTLITERAL:
      fprintf (f, "%ld\n", t->value.intlit);
      break;
      
    default:
      BADEXIT;
    }
  
}


/*@only@*/ constraintTerm constraintTerm_undump (FILE *f)
{
  constraintTermType kind;
  constraintTerm ret;
  
  uentry ue;
  
  char *str;
  char *os;

  os = mstring_create (MAX_DUMP_LINE_LENGTH);

  str = fgets (os, MAX_DUMP_LINE_LENGTH, f);

  llassert (str != NULL);

  kind = (constraintTermType) reader_getInt(&str);
  str = fgets(os, MAX_DUMP_LINE_LENGTH, f);

  llassert (str != NULL);

  switch (kind)
    {
      
    case CTT_SREF:
      {
	sRef s;
	char * term;
	term = reader_getWord(&str);

	if (term == NULL)
	  {
	    llfatalbug (message ("Library file appears to be corrupted.") );
	  }
	if (strcmp (term, "Result") == 0 )
	  {
	    s = sRef_makeResult (ctype_unknown);
	  }
	else if (strcmp (term, "Param" ) == 0 )
	  {
	    int param;
	    char *str2, *ostr2;
	    
	    ctype t;

	    reader_checkChar(&str, ' ');
	    str2  = reader_getWord(&str);
	    param = reader_getInt(&str);

	if (str2 == NULL)
	  {
	    llfatalbug (message ("Library file appears to be corrupted.") );
	  }
	    
	    ostr2 = str2;
	    t = ctype_undump(&str2) ;
	    s = sRef_makeParam (param, t, stateInfo_makeLoc (g_currentloc, SA_CREATED));
	    free (ostr2);
	  }
	else if (strcmp (term, "sRef_dump" ) == 0 )
	  {
	    reader_checkChar(&str, ' ');
	    s = sRef_undump (&str);
	  }
	else  /* This must be an identified that we can search for in usymTab */
	  {
	    cstring termStr = cstring_makeLiteralTemp(term);

	    ue = usymtab_lookup (termStr);
	    s = uentry_getSref(ue);
	  }
	
	ret = constraintTerm_makesRef(s);

	free(term);
      }
      break;

    case CTT_EXPR:
      {
	sRef s;
	char * term;
	cstring termStr;
		
	term = reader_getWord(&str);

	if (term == NULL)
	  {
	    llfatalbug (message ("Library file appears to be corrupted.") );
	  }

	/* This must be an identifier that we can search for in usymTab */
	termStr = cstring_makeLiteralTemp(term);
	
	ue = usymtab_lookup (termStr);
	s = uentry_getSref(ue);
	ret = constraintTerm_makesRef(s);

	free (term);
      }
      break;
      
      
    case CTT_INTLITERAL:
      {
	int i;

	i = reader_getInt(&str);
	ret = constraintTerm_makeIntLiteral (i);
      }
      break;
      
    default:
      BADEXIT;
    }
  free (os);

  return ret;
}



/* drl added sometime before 10/17/001*/
ctype constraintTerm_getCType (constraintTerm term)
{
  ctype ct;
  
  switch (term->kind)
    {
    case CTT_EXPR:
      ct = exprNode_getType (term->value.expr);
      break;

    case CTT_INTLITERAL:
      ct = ctype_signedintegral;
      break;
      
    case CTT_SREF:
      ct = sRef_getType (term->value.sref) ;
      break;
    default:
      BADEXIT;
    }
  return ct;
}

bool constraintTerm_isConstantOnly (constraintTerm term)
{  
  switch (term->kind)
    {
    case CTT_EXPR:
      if (exprNode_isNumLiteral (term->value.expr) ||
	  exprNode_isStringLiteral (term->value.expr) ||
	  exprNode_isCharLiteral  (term->value.expr) )
	{
	  return TRUE;
	}
      else
	{
	  return FALSE;
	}

    case CTT_INTLITERAL:
      return TRUE;
            
    case CTT_SREF:
      if ( sRef_isConst (term->value.sref) )
	{
	  return TRUE;
	}
      else
	{
	  return FALSE;
	}
    default:
      BADEXIT;
    }
  
  BADEXIT;
}
