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
** stateClauseList.c
*/

# include "splintMacros.nf"
# include "basic.h"

static /*@notnull@*/ stateClauseList stateClauseList_new (void)
{
  stateClauseList s = (stateClauseList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = stateClauseListBASESIZE;
  s->elements = (stateClause *) 
    dmalloc (sizeof (*s->elements) * stateClauseListBASESIZE);

  return (s);
}

static void
stateClauseList_grow (stateClauseList s)
{
  int i;
  stateClause *newelements;

  llassert (stateClauseList_isDefined (s));

  s->nspace += stateClauseListBASESIZE; 
  
  newelements = (stateClause *) 
    dmalloc (sizeof (*newelements) * (s->nelements + s->nspace));
  
  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }
  
  sfree (s->elements);
  s->elements = newelements;
}

stateClauseList stateClauseList_add (stateClauseList s, stateClause el)
{
  DPRINTF (("Adding: %s", stateClause_unparse (el)));

  if (stateClauseList_isUndefined (s))
    {
      s = stateClauseList_new ();
    }
  else
    {
      stateClauseList_elements (s, cl)
	{
	  if (stateClause_sameKind (cl, el))
	    {
	      voptgenerror
		(FLG_SYNTAX,
		 message ("Multiple %q clauses for one function (ignoring second)",
			  stateClause_unparseKind (cl)),
		 g_currentloc);

	      stateClause_free (el);
	      return s;
	    }
	} end_stateClauseList_elements ;
    }

  if (s->nspace <= 0)
    {
      stateClauseList_grow (s);
    }
  
  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

cstring stateClauseList_unparse (stateClauseList s)
{
  cstring st = cstring_undefined;
  int i;
  
  if (stateClauseList_isDefined (s))
    {
      for (i = 0; i < stateClauseList_size (s); i++)
	{
	  if (i == 0)
	    {
	      st = message ("%q;", stateClause_unparse (s->elements[i]));
	    }
	  else
	    st = message ("%q %q;", st, stateClause_unparse (s->elements[i]));
	}
    }
  
  return (st);
}

stateClauseList stateClauseList_copy (stateClauseList s)
{
  if (stateClauseList_isDefined (s))
    {
      stateClauseList t = (stateClauseList) dmalloc (sizeof (*t));
      int i;
      
      t->nelements = s->nelements;
      t->nspace = 0;
      
      if (s->nelements > 0)
	{
	  t->elements = (stateClause *) dmalloc (sizeof (*t->elements) * t->nelements);
	  for (i = 0; i < s->nelements; i++) 
	    {
	      t->elements[i] = stateClause_copy (s->elements[i]); 
	    }
	}
      else
	{
	  t->elements = NULL;
	}

      return t;
    }
  else
    {
      return stateClauseList_undefined;
    }
}

void
stateClauseList_free (stateClauseList s)
{
  if (!stateClauseList_isUndefined (s)) 
    {
      int i;

      for (i = 0; i < s->nelements; i++)
	{
	  stateClause_free (s->elements[i]);  
	}

      sfree (s->elements);
      sfree (s);
    }
}

cstring stateClauseList_dump (stateClauseList s)
{
  cstring st = cstring_undefined;

  if (stateClauseList_isUndefined (s)) return st;
  
  stateClauseList_elements (s, current)
    {
      st = message ("%q%q$", st, stateClause_dump (current));
    } end_stateClauseList_elements;

  return st;
}

stateClauseList stateClauseList_undump (char **s)
{
  char c;
  stateClauseList pn = stateClauseList_new ();
  int paramno = 0;

  c = **s;

  while (c != '#' && c != '@')
    {
      stateClause sc = stateClause_undump (s);
      
      pn = stateClauseList_add (pn, sc);
      reader_checkChar (s, '$');
      c = **s;
      paramno++;
    }

  return pn;
}

int stateClauseList_compare (stateClauseList s1, stateClauseList s2)
{
  if (stateClauseList_isUndefined (s1)
      && stateClauseList_isUndefined (s2))
    {
      return 0;
    }
  else
    {
      if (s1 - s2 > 0) /* evans 2001-08-21: was (int) s1 > (int) s2) */
	{
	  return 1;
	}
      else
	{
	  return -1;
	}
    }
}
  
static /*@exposed@*/ sRefSet
stateClauseList_getClause (stateClauseList s, stateClause k)
{
  stateClauseList_elements (s, el)
    {
      if (stateClause_matchKind (el, k))
	{
	  return stateClause_getRefs (el);
	}
    } end_stateClauseList_elements ;

  return sRefSet_undefined;
}

void stateClauseList_checkAll (uentry ue)
{
  stateClauseList clauses = uentry_getStateClauseList (ue);
  sRef res = uentry_getSref (ue);		  
  bool specialResult = FALSE;

  DPRINTF (("Check state clauses: %s", uentry_unparseFull (ue)));

  stateClauseList_elements (clauses, cl)
    {
      bool isPre = stateClause_isBeforeOnly (cl);

      if (stateClause_isGlobal (cl))
	{
	  ; 
	}
      else
	{
	  sRefSet refs = stateClause_getRefs (cl);
	  
	  sRefSet_allElements (refs, el)
	    {
	      sRef rb = sRef_getRootBase (el);

	      DPRINTF (("Check: %s", sRef_unparse (el)));

	      if (sRef_isResult (rb)) 
		{
		  /*
		  ** The result type is now know, need to set it:
		  */
		  
		  if (ctype_isUnknown (sRef_getType (rb)))
		    {
		      ctype utype = uentry_getType (ue);
		      llassert (ctype_isFunction (utype));

		      sRef_setTypeFull (rb, ctype_getReturnType (utype));
		      DPRINTF (("el: %s", sRef_unparseFull (el)));
		    }
		}
	      
	      if (stateClause_setsMetaState (cl))
		{
		  qual q = stateClause_getMetaQual (cl);
		  annotationInfo qa = qual_getAnnotationInfo (q);

		  if (!annotationInfo_matchesContextRef (qa, el))
		    {
		      if (optgenerror
			  (FLG_ANNOTATIONERROR,
			   message ("Attribute annotation %s used on inappropriate reference %q in %q clause of %q: %q",
				    qual_unparse (q),
				    sRef_unparse (el),
				    stateClause_unparseKind (cl),
				    uentry_getName (ue),
				    stateClause_unparse (cl)),
			   uentry_whereLast (ue)))
			{
			  /* annotationInfo_showContextError (ainfo, ue); */
			}
		    }
		}

	      if (sRef_isResult (rb))
		{
		  if (isPre)
		    {
		      voptgenerror
			(FLG_INCONDEFS,
			 message ("Function result is used in %q clause of %q "
				  "(%q applies to the state before function is "
				  "called, so should not use result): %q",
				  stateClause_unparseKind (cl),
				  uentry_getName (ue),
				  stateClause_unparseKind (cl),
				  sRef_unparse (el)),
			 uentry_whereLast (ue));
		    }
		  else
		    {
		      if (!sRef_isStateSpecial (res))
			{
			  DPRINTF (("Here we are: %s", sRef_unparseFull (res)));

			  if (!specialResult)
			    {
			      sstate pstate = sRef_getDefState (res);
			      
			      if (!sRef_makeStateSpecial (res))
				{
				  if (optgenerror
				      (FLG_INCONDEFS,
				       message ("Function result is used in %q clause of %q "
						"but was previously annotated with %s: %q",
						stateClause_unparseKind (cl),
						uentry_getName (ue),
						sstate_unparse (pstate),
						sRef_unparse (el)),
				       uentry_whereLast (ue)))
				    {
				      specialResult = TRUE;
				    }
				}			      
			    }
			}
		      
		      DPRINTF (("Fixing result type! %s", sRef_unparseFull (el)));
		      (void) sRef_fixResultType (el, sRef_getType (res), ue);
		    }
		}
	      else if (sRef_isParam (rb))
		{
		  DPRINTF (("Make special: %s", sRef_unparseFull (rb)));
		  
		  if (!sRef_makeStateSpecial (rb))
		    {
		      if (fileloc_isXHFile (uentry_whereLast (ue)))
			{
			  ; /* Okay to override in .xh files */
			}
		      else if (stateClause_isQual (cl))
			{
			  ; /* qual clauses don't interfere with definition state */
			}
		      else
			{
			  voptgenerror 
			    (FLG_INCONDEFS,
			     message ("Reference %q used in %q clause of %q, "
				      "but was previously annotated with %s: %q",
				      sRef_unparse (rb),
				      stateClause_unparseKind (cl),
				      uentry_getName (ue),
				      sstate_unparse (sRef_getDefState (res)),
				      sRef_unparse (el)),
			     uentry_whereLast (ue));
			}
		    }
		  
		  DPRINTF (("Made special: %s", sRef_unparseFull (rb)));
		}
	      else if (sRef_isInvalid (rb))
		{
		  /*@innercontinue@*/ continue;
		}
	      else 
		{
		  BADBRANCHCONT;
		  /*@innercontinue@*/ continue;
		}
	      
	      if (stateClause_isMemoryAllocation (cl))
		{
		  if (!ctype_isVisiblySharable (sRef_getType (el)))
		    {
		      voptgenerror
			(FLG_ANNOTATIONERROR,
			 /*@-sefparams@*/ /* This is okay because its fresh storage. */
			 message 
			 ("%q clauses includes %q of "
			  "non-dynamically allocated type %s",
			  cstring_capitalizeFree (stateClause_unparseKind (cl)),
			  sRef_unparse (el), 
			  ctype_unparse (sRef_getType (el))),
			 uentry_whereLast (ue));
		      /*@=sefparams@*/
		    }
		}
	      
	    } end_sRefSet_allElements ;
	}
    } end_stateClauseList_elements ;
}
  
void stateClauseList_checkEqual (uentry old, uentry unew)
{
  stateClauseList oldClauses = uentry_getStateClauseList (old);
  stateClauseList newClauses = uentry_getStateClauseList (unew);

  if (stateClauseList_isDefined (newClauses))
    {
      stateClauseList_elements (newClauses, cl)
	{
	  if (stateClause_isGlobal (cl))
	    {
	      ;
	    }
	  else
	    {
	      sRefSet sc = stateClauseList_getClause (oldClauses, cl);
	      
	      if (!sRefSet_equal (sc, stateClause_getRefs (cl)))
		{
		  if (optgenerror
		      (FLG_INCONDEFS,
		       message ("Function %q %rdeclared with inconsistent %q clause: %q",
				uentry_getName (old),
				uentry_isDeclared (old),
				stateClause_unparseKind (cl),
				sRefSet_unparsePlain (stateClause_getRefs (cl))),
		       g_currentloc))
		    {
		      uentry_showWhereLastExtra (old, sRefSet_unparsePlain (sc));
		    }
		}
	    }
	} end_stateClauseList_elements ;
	
      stateClauseList_elements (oldClauses, cl)
	{
	  if (stateClause_isGlobal (cl))
	    {
	      ; /* Don't handle globals for now */
	    }
	  else
	    {
	      sRefSet sc = stateClauseList_getClause (newClauses, cl);
	      
	      if (sRefSet_isUndefined (sc) && !sRefSet_isEmpty (stateClause_getRefs (cl)))
		{
		  if (optgenerror
		      (FLG_INCONDEFS,
		       message ("Function %q %rdeclared without %q clause (either "
				"use no special clauses in redeclaration, or "
				"they must match exactly: %q",
				uentry_getName (old),
				uentry_isDeclared (old),
				stateClause_unparseKind (cl),
				sRefSet_unparsePlain (stateClause_getRefs (cl))),
		       g_currentloc))
		    {
		      uentry_showWhereLastExtra (old, sRefSet_unparsePlain (sc));
		    }
		}
	    }
	} end_stateClauseList_elements ;
    }
}



