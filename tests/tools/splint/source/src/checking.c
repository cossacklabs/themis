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
** checking.c
**
** sort checking.
**
**  AUTHOR:
**	Yang Meng Tan,
**         Massachusetts Institute of Technology
*/

# include "splintMacros.nf"
# include "basic.h"
# include "llgrammar.h"
# include "checking.h"
# include "lclscan.h"

/*@+ignorequals@*/

static /*@only@*/ cstring printBadArgs (sortSetList p_args);
static /*@only@*/ sortSet 
  standardOperators (/*@null@*/ nameNode p_n, sortSetList p_argSorts, sort p_q);
static bool isStandardOperator (/*@null@*/ nameNode p_n);
static void assignSorts (termNode p_t, sort p_s);

/*@null@*/ termNode
computePossibleSorts (/*@returned@*/ /*@null@*/ termNode t)
{
  ltoken errtok;

  if (t != (termNode) 0)
    {
      switch (t->kind)
	{
	case TRM_LITERAL:
	case TRM_CONST:
	case TRM_VAR:
	case TRM_ZEROARY:
	case TRM_SIZEOF:
	case TRM_UNCHANGEDALL:
	case TRM_UNCHANGEDOTHERS:
	case TRM_QUANTIFIER:
	  break;
	case TRM_APPLICATION:
	  {
	    bool fail = FALSE;
	    sortSetList argSorts = sortSetList_new ();
	    lslOpSet ops;
	    sortSet standards;

	    if (termNodeList_size (t->args) != 0)
	      {
		termNodeList_elements (t->args, arg)
		{
		  (void) computePossibleSorts (arg);

		  if (sortSet_size (arg->possibleSorts) == 0)
		    {
		      fail = TRUE;
		    }
		  else
		    {
		      sortSetList_addh (argSorts, arg->possibleSorts);
		    } 
		} end_termNodeList_elements;

		if (fail)
		  {
		    lslOpSet_free (t->possibleOps);
		    sortSetList_free (argSorts);
		    t->possibleOps = lslOpSet_new ();
		    return t;
		  }
	      }
	    
	    ops = symtable_opsWithLegalDomain (g_symtab, t->name, argSorts, t->given);
	    lslOpSet_free (t->possibleOps);
	    t->possibleOps = ops;

	    lslOpSet_elements (t->possibleOps, op)
	      {
		sort sort;
		sort = sigNode_rangeSort (op->signature);
		(void) sortSet_insert (t->possibleSorts, sort);
	      } end_lslOpSet_elements;

	    standards = standardOperators (t->name, argSorts, t->given);

	    sortSet_elements (standards, el)
	      {
		(void) sortSet_insert (t->possibleSorts, el);
	      } end_sortSet_elements;

	    sortSet_free (standards);

	    if (!(t->error_reported) && sortSet_size (t->possibleSorts) == 0)
	      {
		unsigned int arity = termNodeList_size (t->args);
		errtok = nameNode_errorToken (t->name);
		
		/* errorShowPoint (inputStream_thisLine (lclsource), ltoken_getCol (errtok)); */
		
		if (isStandardOperator (t->name))
		  {
		    lclerror (errtok, 
			      message ("Type error: %q not declared for %q",
				      nameNode_unparse (t->name), printBadArgs (argSorts)));
		  }
		else if (t->name != NULL 
			 && symtable_opExistsWithArity (g_symtab, t->name, arity))
		  {
		    sigNodeSet possibleOps = symtable_possibleOps (g_symtab, t->name);
		    cstring opName = nameNode_unparse (t->name);
		    
		    /*
                    ** all these will be standardOperators soon...
		    */

		    if (cstring_equalLit (opName, "__ [__]"))
		      {
			lclerror (errtok, 
				  message ("Type error: %q not declared for %q",
					  opName, printBadArgs (argSorts)));
		      }
		    else
		      {
			lclerror (errtok, 
				  message ("Type error: %q declared: %q\ngiven: %q",
					  opName, 
					  sigNodeSet_unparseSomeSigs (possibleOps),
					  printBadArgs (argSorts)));
		      }
		  }
		else
		  {
		    sigNodeSet possibleOps;
		    int npossibleOps;

		    llassert (t->name != NULL);

		    possibleOps = symtable_possibleOps (g_symtab, t->name);
		    npossibleOps = sigNodeSet_size (possibleOps);

		    /*
		     ** evs --- check is it is wrong arity...
		     */
		    
		    if (npossibleOps == 0)
		      {
			lclerror 
			  (errtok, 
			   message ("Undeclared operator: %q", nameNode_unparse (t->name)));      
		      }
		    else
		      {
			lclerror
			  (errtok, 
			   message ("Operator %q declared for %q arguments, given %d",
				   nameNode_unparse (t->name),
				   sigNodeSet_unparsePossibleAritys (possibleOps),
				   arity));
		      }
		  }
		t->error_reported = TRUE;
	      }
	    sortSetList_free (argSorts);
	    break;
	  }
	}
    }
      
  return t;
}

static /*@only@*/ cstring
printBadArgs (sortSetList args)
{
  if (sortSetList_size (args) == 1)
    {
      return (sortSet_unparseOr (sortSetList_head (args)));
    }
  else
    {
      cstring s = cstring_undefined;
      int argno = 1;

      sortSetList_elements (args, ss)
	{
	  if (argno == 1)
	    s = message ("arg %d: %q", argno, sortSet_unparseOr (ss));
	  else
	    s = message ("%q; arg %d: %q", s, argno, sortSet_unparseOr (ss));
	  argno++;
	} end_sortSetList_elements;

      return s;
    }
}

termNode 
checkSort (/*@returned@*/ termNode t)
{
  sortSet sorts;
  sort theSort;
  int size;
  ltoken errtok;

  (void) computePossibleSorts (t);
  sorts = t->possibleSorts;

  llassert (sortSet_isDefined (sorts));

  size = sortSet_size (sorts);
  switch (size)
    {
    case 0:			/* complain later */
      break;
    case 1:			/* just right */
      theSort = sortSet_choose (sorts);
      assignSorts (t, theSort);
      break;
    default:
     /* we allow C literals to have multiple sorts */
      if (t->kind != TRM_LITERAL)
	{
	  errtok = termNode_errorToken (t);
	  t->error_reported = TRUE;

	  lclerror (errtok, 
		    message ("Term %q: can have more than one possible type.  Possible types: %q",
			     termNode_unparse (t), sortSet_unparseClean (sorts)));
	}
    }
  return t;
}

static void
  assignSorts (termNode t, sort s)
{
 /* other kinds are already assigned bottom-up */
  ltoken errtok;

  switch (t->kind)
    {
    case TRM_ZEROARY:	/* pick s to be the sort chosen */
    case TRM_LITERAL:
      sortSet_elements (t->possibleSorts, s2)
      {
	if (sort_equal (s2, s))
	  {
	    sortSet_free (t->possibleSorts);
	    t->possibleSorts = sortSet_new ();
	    (void) sortSet_insert (t->possibleSorts, s);
	    t->sort = s;
	  }
	return;
      } end_sortSet_elements;
      break;
    case TRM_APPLICATION:
      {
	lslOpSet sigs = t->possibleOps;
	lslOpSet oldops = lslOpSet_undefined;
	sigNode op = (sigNode) 0;
	nameNode name = t->name;
	termNodeList args = t->args;
	bool found = FALSE;

	errtok = nameNode_errorToken (name);

	/* why compute again? to check for duplicates */
	lslOpSet_elements (sigs, sig)
	  {
	    sort rsort = sigNode_rangeSort (sig->signature);

	    if (sort_equal (s, rsort))
	      {
		lslOp iop;
		
		if (found)
		  {
		    t->error_reported = TRUE;
		    
		    lclerror (errtok,
			      message ("Ambiguous operator %q: %q or %q",
				       nameNode_unparse (name), 
				       sigNode_unparse (op),
				       sigNode_unparse (sig->signature)));
		    return;
		  }

		iop = (lslOp) dmalloc (sizeof (*iop));
		found = TRUE;
		op = sig->signature;

		oldops = t->possibleOps;
		t->possibleOps = lslOpSet_new ();
		iop->name = nameNode_copy (name);
		iop->signature = op;
		(void) lslOpSet_insert (t->possibleOps, iop);
		t->sort = s;
		/*@-branchstate@*/ 
	      } 
	    /*@=branchstate@*/
	  } end_lslOpSet_elements;

	lslOpSet_free (oldops);

	if (!found)
	  {
	    if (sortSet_size (t->possibleSorts) == 1)
	      {
		t->sort = sortSet_choose (t->possibleSorts);
	      }
	    else
	      {
		/* errorShowPoint (inputStream_thisLine (lclsource), ltoken_getCol (errtok)); */
		t->error_reported = TRUE;
		
		lclerror (errtok, message ("Operator not found: %q", 
					   nameNode_unparse (name)));
		/* evs --- ??? */
	      }
	    return;
	  }
	
	if (termNodeList_empty (args))
	  {
	    if (op != (sigNode) 0)
	      {
		/* was --- NB: copy to avoid interference */
		/* shouldn't need to copy --- its a fresh list */
		sortList dom = sigNode_domain (op);

		sortList_reset (dom);
		termNodeList_elements (args, arg)
		  {
		    assignSorts (arg, sortList_current (dom));
		    sortList_advance (dom);
		  } end_termNodeList_elements;

		sortList_free (dom);
	      }
	    else
	      {
		errtok = nameNode_errorToken (name);
		/* errorShowPoint (inputStream_thisLine (lclsource), ltoken_getCol (errtok)); */
		t->error_reported = TRUE;
		
		lclerror (errtok, message ("No matching operator: %q",
					   nameNode_unparse (name)));
	      }
	    return;
	  }
	break;
      }
    default:			/* do nothing */
      break;
    }
}

void
checkLclPredicate (ltoken t, lclPredicateNode n)
{
  sort theSort;

  if ((n == NULL) || (n->predicate == NULL))
    {
      llcontbuglit ("checkLclPredicate expects valid lclPredicate.  "
		    "Skipping current check");
      return;
    }

 /* check only if there are no previous errors */

  if (!n->predicate->error_reported)
    {
     /* check that the sort of n is boolean */
      theSort = n->predicate->sort;
      if (!sort_compatible (theSort, g_sortCapBool))
	{
	  if (sort_isNoSort (theSort))
	    {
	     ; /* "Expects a boolean term.  Given term has unknown sort" */
	    }
	  else
	    {
	      cstring clauset = ltoken_getRawString (t);

	      if (cstring_firstChar (clauset) == '(')
		{
		  clauset = cstring_makeLiteral ("Equality");
		}
	      else
		{
		  /* uppercase first letter */
		  clauset = cstring_copy (clauset);
		  cstring_setChar (clauset, 1, 
				   (char) toupper (cstring_firstChar (clauset))); 
		}

	      lclerror (t, message ("%q expects a boolean term, given %q.", 
				    clauset, sort_unparse (theSort)));
	      
	    }
	}
    }
}

/*
** these should not be doing string comparisons!
*/

static bool isDeRefOperator (cstring s)
{
  return (cstring_equalLit (s, "*"));
}

static bool isStateOperator (cstring s)
{
  return (cstring_equalLit (s, "^") ||
	  cstring_equalLit (s, "'") ||
	  cstring_equalLit (s, "\\any") ||
	  cstring_equalLit (s, "\\pre") ||
	  cstring_equalLit (s, "\\post"));
}

static bool isCompareOperator (cstring s) /* YUCK!!! */
{
  return (cstring_equalLit (s, "\\eq") ||
	  cstring_equalLit (s, "\\neq") ||
	  cstring_equalLit (s, "=") ||
	  cstring_equalLit (s, "!=") ||
	  cstring_equalLit (s, "~=") ||
	  cstring_equalLit (s, "=="));
}

static bool isStandardOperator (/*@null@*/ nameNode n)
{
  if (n != (nameNode) 0)
    {
      if (!n->isOpId)
	{
	  opFormNode opf = n->content.opform;

	  llassert (opf != NULL);

	  switch (opf->kind)
	    {
	    case OPF_IF: return TRUE;
	    case OPF_ANYOP:
	      break;
	    case OPF_MANYOP:
	      {
		cstring s = ltoken_getRawString (opf->content.anyop);
		
		if (isStateOperator (s)) return TRUE;
		return FALSE;
	      }
	    case OPF_ANYOPM:
	      /* operator: *__ */
	      {
		cstring s = ltoken_getRawString (opf->content.anyop);
		
		return (isDeRefOperator (s));
	      }
	    case OPF_MANYOPM:
	      {
		cstring s = ltoken_getRawString (opf->content.anyop);
		
		return (isCompareOperator (s));
	      }
	    case OPF_MIDDLE:
	      break;
	    case OPF_MMIDDLE:
	      break;
	    case OPF_MIDDLEM:
	      break;
	    case OPF_MMIDDLEM:
	      break;
	    case OPF_BMIDDLE:
	      break;
	    case OPF_BMMIDDLE:
	      break;
	    case OPF_BMIDDLEM:
	      break;
	    case OPF_BMMIDDLEM:
	      break;
	    case OPF_SELECT:
	      break;
	    case OPF_MAP:
	      break;
	    case OPF_MSELECT:
	      break;
	    case OPF_MMAP:
	      break;
	    default:
	      break;
	    }
	}
      else
	{
	  int code = ltoken_getCode (n->content.opid);
	  
	  if (code == simpleId)
	    {
	      cstring text = nameNode_unparse (n);
	      bool ret = (cstring_equalLit (text, "trashed") 
			  || cstring_equalLit (text, "maxIndex")  
			  || cstring_equalLit (text, "minIndex")
			  || cstring_equalLit (text, "isSub"));
	  
	      cstring_free (text);
	      return ret;
	    }

	  return (code == LLT_MODIFIES || code == LLT_FRESH 
		  || code == LLT_UNCHANGED || code == LLT_SIZEOF);
	}
    }
  return FALSE;
}

static /*@only@*/ sortSet
standardOperators (/*@null@*/ nameNode n, sortSetList argSorts, /*@unused@*/ sort q)
{
  sortSet argSet;
  sortSet ret = sortSet_new ();

  if (n == (nameNode) 0) return ret;

  if (n->isOpId)
    {
      int code = ltoken_getCode (n->content.opid);

      if (sortSetList_size (argSorts) == 1)
	{
	  sortSetList_reset (argSorts);
	  
	  argSet = sortSetList_current (argSorts);
	  
	  sortSet_elements (argSet, current)
	    {
	      sortNode sn;

	      sn = sort_quietLookup (current);

	      while (sn->kind == SRT_SYN)
		{
		  sn = sort_quietLookup (sn->baseSort);
		}
	      
	      /*@-loopswitchbreak@*/
	      switch (code)
		{
		case simpleId:
		  {
		    cstring text = ltoken_getRawString (n->content.opid); 
		    
		    if (cstring_equalLit (text, "trashed")) /* GACK! */
		      {
			if (sn->kind == SRT_OBJ ||
			    sn->kind == SRT_ARRAY)
			(void) sortSet_insert (ret, g_sortBool);	  
		      }
		    
		    if (cstring_equalLit (text, "maxIndex") || 
			cstring_equalLit (text, "minIndex"))
		      {
			if (sn->kind == SRT_ARRAY || sn->kind == SRT_PTR)
			  (void) sortSet_insert (ret, g_sortInt);	  
			
			/*		  if (lsymbol_fromChars ("maxIndex") */
		      }
		  }
		  break;
		case LLT_MODIFIES:
		case LLT_FRESH:
		case LLT_UNCHANGED:
		  if (sn->kind == SRT_OBJ ||
		      sn->kind == SRT_ARRAY)
		    {
		      (void) sortSet_insert (ret, g_sortBool);	  
		    }
		  break;
		case LLT_SIZEOF:
		  if (sn->kind == SRT_OBJ ||
		      sn->kind == SRT_ARRAY ||
		      sn->kind == SRT_VECTOR)
		  (void) sortSet_insert (ret, g_sortInt);
		  break;
		default:
		  break;
		}
	    } end_sortSet_elements;
	}
    }
  else
    {
      opFormNode opf = n->content.opform;

      llassert (opf != NULL);

      switch (opf->kind)
	{
	case OPF_IF:
	  /*
          ** if __ then __ else __ : bool, S, S -> S
          **     is defined for all sorts
          */
	  
	  if (sortSetList_size (argSorts) == 3)
	    {
	      argSet = sortSetList_head (argSorts);
	      
	      if (sortSet_member (argSet, g_sortBool))
		{
		  sortSetList_reset (argSorts);
		  sortSetList_advance (argSorts);
		  
		  argSet = sortSetList_current (argSorts);
		  
		  if (sortSet_size (argSet) == 1)
		    {
		      sort clause = sortSet_choose (argSet);
		      sort clause2;

		      sortSetList_advance (argSorts);
		      argSet = sortSetList_current (argSorts);
		      
		      clause2 = sortSet_choose (argSet);
		      
		      if (sortSet_size (argSet) == 1 &&
			  sort_equal (clause, clause2))
			{
			  (void) sortSet_insert (ret, clause);
			}
		    }
		}
	    }
	  break;
	case OPF_MANYOP:
	  {
	    cstring s = ltoken_getRawString (opf->content.anyop);

	    if (isStateOperator (s))
	      {
		if (sortSetList_size (argSorts) == 1)
		  {
		    sortSetList_reset (argSorts);
		    
		    argSet = sortSetList_current (argSorts);
		    
		    sortSet_elements (argSet, current)
		      {
			sortNode sn;

			sn = sort_quietLookup (current);

			while (sn->kind == SRT_SYN)
			  {
			    sn = sort_quietLookup (sn->baseSort);
			  }
			
			switch (sn->kind)
			  {
			  case SRT_OBJ:
			    (void) sortSet_insert (ret, sn->baseSort);
			    break;
			  case SRT_ARRAY:
			    (void) sortSet_insert (ret, 
						   sort_makeVec (ltoken_undefined, current));
			    break; 
			  case SRT_STRUCT:
			    (void) sortSet_insert (ret, 
						   sort_makeTuple (ltoken_undefined, current));
			    break;
			  case SRT_UNION:
			    (void) sortSet_insert (ret, 
						   sort_makeUnionVal (ltoken_undefined, current));
			    break;
			  case SRT_TUPLE:
			  case SRT_UNIONVAL:
			  case SRT_ENUM:
			  case SRT_LAST:
			  case SRT_FIRST:
			  case SRT_NONE:       
			  case SRT_HOF:
			  case SRT_PRIM:
			  case SRT_PTR:
			  case SRT_VECTOR:
			    break;
			  case SRT_SYN:
			    llbuglit ("standardOperators: Synonym in switch");
			  }
		      } end_sortSet_elements ;
		  }
	      }
	  }
	  break;
	case OPF_ANYOPM:
	  /* operator: *__ */
	  {
	    cstring s = ltoken_getRawString (opf->content.anyop);
	    
	    if (isDeRefOperator (s))
	      {
		if (sortSetList_size (argSorts) == 1)
		  {
		    sortSetList_reset (argSorts);
		    
		    argSet = sortSetList_current (argSorts);
		    
		    sortSet_elements (argSet, current)
		      {
			sortNode sn;

			sn = sort_quietLookup (current);
			
			while (sn->kind == SRT_SYN)
			  {
			    sn = sort_quietLookup (sn->baseSort);
			  }
			
			if (sn->kind == SRT_PTR)
			  {
			    (void) sortSet_insert (ret, sn->baseSort);		
			  }
		      } end_sortSet_elements; 
		  }
	      }
	  }
	  break;
	case OPF_ANYOP:
	  break;
	case OPF_MANYOPM:
	  {
	    cstring s = ltoken_getRawString (opf->content.anyop);
	    
	    if (isCompareOperator (s))
	      {
		if (sortSetList_size (argSorts) == 2)
		  {
		    sortSet argSet2;

		    sortSetList_reset (argSorts);
		    
		    argSet = sortSetList_current (argSorts);
		    sortSetList_advance (argSorts);
		    argSet2 = sortSetList_current (argSorts);
		    
		    if (sortSet_size (argSet) == 1)
		      {
			sortSet_elements (argSet, cl)
			  {
			    sortSet_elements (argSet2, cl2)
			      {
				if (sort_equal (cl, cl2))
				  {
				    (void) sortSet_insert (ret, g_sortBool);
				  }
			      } end_sortSet_elements;
			  } end_sortSet_elements; 
		      }
		  }
	      }
	  }
	  break;
	case OPF_MIDDLE:
	  break;
	case OPF_MMIDDLE:
	  break;
	case OPF_MIDDLEM:
	  break;
	case OPF_MMIDDLEM:
	  break;
	case OPF_BMIDDLE:
	  break;
	case OPF_BMMIDDLE:
	  break;
	case OPF_BMIDDLEM:
	  break;
	case OPF_BMMIDDLEM:
	  break;
	case OPF_SELECT:
	  break;
	case OPF_MAP:
	  break;
	case OPF_MSELECT:
	  break;
	case OPF_MMAP:
	  break;
	default:
	  break;
	}
      /*@=loopswitchbreak@*/
    }
  return ret;
}
