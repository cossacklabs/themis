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
** aliasTable.c
*/

# include "splintMacros.nf"
# include "basic.h"

/*@constant int ATINVALID; @*/
# define ATINVALID -1

static sRefSet
  aliasTable_canAliasAux (aliasTable p_s, sRef p_sr, int p_lim) /*@*/ ;
static sRefSet
  aliasTable_aliasedByLimit (aliasTable p_s, sRef p_sr, int p_lim) /*@*/ ;
static sRefSet 
  aliasTable_aliasedByAux (aliasTable p_s, sRef p_sr, int p_lim) /*@*/ ;

aliasTable
aliasTable_new ()
{
  return (aliasTable_undefined);
}

static /*@only@*/ /*@notnull@*/ aliasTable
aliasTable_newEmpty (void)
{
  aliasTable s = (aliasTable) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = aliasTableBASESIZE;
  s->keys = (sRef *) dmalloc (sizeof (*s->keys) * aliasTableBASESIZE);
  s->values = (sRefSet *) dmalloc (sizeof (*s->values) * aliasTableBASESIZE);
  
  return (s);
}

static void
aliasTable_grow (/*@notnull@*/ aliasTable s)
{
  int i;
  o_sRefSet *oldvalues = s->values;
  sRef    *oldkeys = s->keys;
  
  s->nspace += aliasTableBASESIZE; 

  s->values = (sRefSet *) dmalloc (sizeof (*s->values)
				   * (s->nelements + s->nspace));
  s->keys = (sRef *) dmalloc (sizeof (*s->keys) * (s->nelements + aliasTableBASESIZE));

  if (s->keys == (sRef *) 0 || s->values == (sRefSet *)0)
    {
      llfatalerror (cstring_makeLiteral ("aliasTable_grow: out of memory!"));
    }

  for (i = 0; i < s->nelements; i++)
    {
      s->values[i] = oldvalues[i];
      s->keys[i] = oldkeys[i];
    }
  
  sfree (oldvalues);
  sfree (oldkeys);
}

static int aliasTable_lookupRefs (/*@notnull@*/ aliasTable s, sRef sr)
{
  int i;

  
  for (i = 0; i < aliasTable_size (s); i++)
    {
      if (sRef_same (sr, s->keys[i])) 
	{
	  DPRINTF (("sRef match: %s / %s",
		    sRef_unparseFull (sr),
		    sRef_unparseFull (s->keys[i])));
	  return i;
	}
    }

  return ATINVALID;
}

/*
** sr aliases al (and anything al aliases!)
*/

aliasTable
aliasTable_addMustAlias (/*@returned@*/ aliasTable s,
			 /*@exposed@*/ sRef sr,
			 /*@exposed@*/ sRef al)
{
  int ind;
  sRefSet ss;
  
  llassert (NOALIAS (sr, al));
  
  DPRINTF (("Adding alias: %s / %s", sRef_unparseFull (sr),
	    sRef_unparseFull (al)));

  if (aliasTable_isUndefined (s))
    {
      s = aliasTable_newEmpty ();
      ind = ATINVALID;
    }
  else
    {
      ind = aliasTable_lookupRefs (s, sr);
    }
  
  ss = aliasTable_canAlias (s, al); 
  DPRINTF (("Previous aliases: %s", sRefSet_unparse (ss)));
  
  if (ind == ATINVALID)
    {
      if (s->nspace <= 0) {
	aliasTable_grow (s);
      }

      s->nspace--;
      s->keys[s->nelements] = sr;
      s->values[s->nelements] = sRefSet_single (al); 
      ind = s->nelements;
      s->nelements++;      
    }
  else
    {
      s->values[ind] = sRefSet_insert (s->values[ind], al); 
    }
  
  s->values[ind] = sRefSet_unionExcept (s->values[ind], ss, s->keys[ind]); 
  DPRINTF (("New aliases: %s", sRefSet_unparse (s->values[ind])));

  sRefSet_free (ss);
  return s;
}

static aliasTable 
  aliasTable_addSet (/*@returned@*/ aliasTable s,
		     /*@exposed@*/ sRef key, /*@only@*/ sRefSet value)
{
  if (!sRefSet_isEmpty (value))
    {
      if (aliasTable_isUndefined (s))
	{
	  s = aliasTable_newEmpty ();
	}
      else
	{
	  if (s->nspace <= 0)
	    {
	      aliasTable_grow (s);
	    }
	}

      s->nspace--;
      s->keys[s->nelements] = key;
      s->values[s->nelements] = value;
      s->nelements++;      
    }
  else
    {
      sRefSet_free (value);
    }

  return s;
}

/*
** When aliases are cleared:
**
**    o remove all entries for sr
**    o replace all aliases for things which alias sr with sr's aliases
**
** Clear aliases for sr; if sr is a direct param reference, clear its aliases too.
*/

static void aliasTable_clearAliasesAux (/*@notnull@*/ aliasTable p_s, sRef p_sr)
   /*@modifies p_s@*/ ;

void aliasTable_clearAliases (aliasTable s, sRef sr)
{
  if (aliasTable_isUndefined (s))
    {
      return;
    }
  else
    {
      sRef rb = sRef_getRootBase (sr);

            
      if (!sRef_isCvar (sr) && sRef_isLocalVar (rb))
	{
	  int ind = aliasTable_lookupRefs (s, rb);
	  
	  if (ind != ATINVALID)
	    {
	      sRefSet al = s->values[ind];
	      
	      	      
	      sRefSet_realElements (al, el)
		{
		  		  
		  if (sRef_isParam (el))
		    {
		      if (sRef_sameName (el, rb))
			{
			  sRef fb = sRef_fixBase (sr, el); 

			  aliasTable_clearAliasesAux (s, fb); 
			}
		    }
		} end_sRefSet_realElements ;
	    }
	}
      
      aliasTable_clearAliasesAux (s, sr); 
    }  
}

static
void aliasTable_clearAliasesAux (/*@notnull@*/ aliasTable s, sRef sr)
{
  int i;
  
  for (i = 0; i < s->nelements; i++)
    {
      sRef key = s->keys[i];
      
      if (sRef_includedBy (key, sr))
	{
	  sRefSet_clear (s->values[i]);
	}
      else
	{
	  (void) sRefSet_deleteBase (s->values[i], sr);   
	}
    }
}

/*
** returns set of all sRefs that must alias sr (but are different from sr)
*/

static /*@only@*/ sRefSet aliasTable_aliasedByAux (aliasTable s, sRef sr, int lim)
{
  static bool hadWarning = FALSE;
  sRefSet res = sRefSet_undefined;
  int i;

  llassert (!sRef_isConj (sr));
  
  
  if (aliasTable_isUndefined (s) || lim >= ALIASSEARCHLIMIT)
    {
      if (lim >= ALIASSEARCHLIMIT && !hadWarning)
	{
	  llquietbug
	    (message ("Alias search limit exceeded, checking %q. "
		      "This either means there is a variable with at least "
		      "%d indirections, or there is a bug in Splint.",
		      sRef_unparse (sr),
		      ALIASSEARCHLIMIT));
	  
	  hadWarning = TRUE;
	}

      return sRefSet_undefined;
    }
  else
    {
      sRefSet abl;

      if (sRef_isPointer (sr))
	{
	  abl = aliasTable_aliasedByLimit (s, sRef_getBase (sr), lim);
	  res = sRefSet_addIndirection (abl);
	}
      else if (sRef_isAddress (sr))
	{
	  abl = aliasTable_aliasedByLimit (s, sRef_getBase (sr), lim);
	  res = sRefSet_removeIndirection (abl);
	}
      else if (sRef_isField (sr))
	{
	  abl = aliasTable_aliasedByLimit (s, sRef_getBase (sr), lim);
	  res = sRefSet_accessField (abl, sRef_getField (sr));
	}
      else if (sRef_isArrayFetch (sr))
	{
	  abl = aliasTable_aliasedByLimit (s, sRef_getBase (sr), lim);

	  if (sRef_isIndexKnown (sr))
	    {
	      int idx = sRef_getIndex (sr);
	      
	      res = sRefSet_fetchKnown (abl, idx);
	    }
	  else
	    {
	      res = sRefSet_fetchUnknown (abl);
	    }
	}
      else
	{
	  abl = sRefSet_undefined;
	}

      sRefSet_free (abl);
    }

  for (i = 0; i < s->nelements; i++)
    {
      sRef elem = s->keys[i];
      
      if (!sRef_same (sr, elem)) /* was sameName */
	{
	  	  
	  sRefSet_realElements (s->values[i], current)
	    {
	      	      
	      if (sRef_similar (sr, current))
		{
		  		  		  res = sRefSet_insert (res, sRef_fixOuterRef (elem));
		  /*@innerbreak@*/ break;
		}
	    } end_sRefSet_realElements;
	} 
    }
  
    return res;
}

static /*@only@*/ sRefSet aliasTable_aliasedByLimit (aliasTable s, sRef sr, int lim)
{
  sRefSet res;
  
  
  if (sRef_isConj (sr))
    {
      res = sRefSet_unionFree (aliasTable_aliasedByLimit (s, sRef_getConjA (sr), lim),
			       aliasTable_aliasedByLimit (s, sRef_getConjB (sr), lim));
    }
  else
    {
      res = aliasTable_aliasedByAux (s, sr, lim + 1);
    }
  
    return res;
}

/*@only@*/ sRefSet aliasTable_aliasedBy (aliasTable s, sRef sr)
{ 
  if (sRef_isConj (sr))
    {
      return (sRefSet_unionFree (aliasTable_aliasedBy (s, sRef_getConjA (sr)),
				 aliasTable_aliasedBy (s, sRef_getConjB (sr))));
    }

  return (aliasTable_aliasedByAux (s, sr, 0));
}

/*@only@*/ sRefSet aliasTable_canAlias (aliasTable s, sRef sr)
{
  sRefSet res;

    
  if (sRef_isConj (sr))
    {
      res = sRefSet_unionFree (aliasTable_canAlias (s, sRef_getConjA (sr)),
			       aliasTable_canAlias (s, sRef_getConjB (sr)));
    }
  else
    {
      res = aliasTable_canAliasAux (s, sr, 0);
          }

    return res;
}

/*
** need to limit the depth of aliasing searches 
*/

static /*@only@*/ sRefSet aliasTable_canAliasLimit (aliasTable s, sRef sr, int lim)
{
  sRefSet res;
  
  if (sRef_isConj (sr))
    {
      sRefSet a = aliasTable_canAliasLimit (s, sRef_getConjA (sr), lim);
      sRefSet b = aliasTable_canAliasLimit (s, sRef_getConjB (sr), lim);

      res = sRefSet_unionFree (a, b);
    }
  else
    {
      res = aliasTable_canAliasAux (s, sr, lim + 1);
    }
  
  return res;
}

static /*@only@*/ sRefSet 
  aliasTable_canAliasAux (aliasTable s, sRef sr, int lim)
{
  static bool hadWarning = FALSE;
  llassert (!sRef_isConj (sr));
  
  
  if (aliasTable_isUndefined (s) || lim >= ALIASSEARCHLIMIT)
    {
      if (lim >= ALIASSEARCHLIMIT && !hadWarning)
	{
	  llquietbug
	    (message ("Alias search limit exceeded, checking %q. "
		      "This either means there is a variable with at least "
		      "%d indirections, or there is a bug in Splint.",
		      sRef_unparse (sr),
		      ALIASSEARCHLIMIT));
	  
	  hadWarning = TRUE;
	}

      return sRefSet_undefined;
    }
  else
    {
      int ind = aliasTable_lookupRefs (s, sr);

      if (sRef_isPointer (sr) || sRef_isAddress (sr) || sRef_isField (sr)
	  || sRef_isArrayFetch (sr))
	{
	  sRef base = sRef_getBase (sr);
	  sRefSet tmp = aliasTable_canAliasLimit (s, base, lim);
	  sRefSet ret;

	  if (sRef_isPointer (sr))
	    {
	      ret = sRefSet_addIndirection (tmp); 
	    }
	  else if (sRef_isAddress (sr))
	    {
	      ret = sRefSet_removeIndirection (tmp);
	    }
	  else if (sRef_isField (sr))
	    {
	      ret = sRefSet_accessField (tmp, sRef_getField (sr));
	    }
	  else if (sRef_isArrayFetch (sr))
	    {
	      if (sRef_isIndexKnown (sr))
		{
		  ret = sRefSet_fetchKnown (tmp, sRef_getIndex (sr));
		}
	      else
		{
		  ret = sRefSet_fetchUnknown (tmp);
		}
	    }
	  else
	    {
	      BADBRANCH;
	    }

	  if (ind != ATINVALID)
	    {
	      ret = sRefSet_union (ret, s->values[ind]);
	    }
	  
	  sRefSet_free (tmp);
	  return ret;
	}
      
      if (ind == ATINVALID) return sRefSet_undefined;      
      
      return sRefSet_newCopy (s->values[ind]);
    }
}

aliasTable aliasTable_copy (aliasTable s)
{
  if (aliasTable_isEmpty (s))
    {
      return aliasTable_undefined;
    }
  else
    {
      aliasTable t = (aliasTable) dmalloc (sizeof (*s));
      int i;

      t->nelements = s->nelements;
      t->nspace = 0;
      t->keys = (sRef *) dmalloc (sizeof (*s->keys) * s->nelements);
      t->values = (sRefSet *) dmalloc (sizeof (*s->values) * t->nelements);
        
      for (i = 0; i < s->nelements; i++)
	{
	  t->keys[i] = s->keys[i];
	  t->values[i] = sRefSet_newCopy (s->values[i]);
	}

      return t;
    }
}

static void
aliasTable_levelPrune (aliasTable s, int lexlevel)
{
  
  
  if (aliasTable_isEmpty (s))
    {
      return;
    }
  else
    {
      int i;
      int backcount = s->nelements - 1;
      
      for (i = 0; i <= backcount; i++)
	{
	  sRef key = s->keys[i];
	  
	  if (sRef_lexLevel (key) > lexlevel)
	    {
	      int j;
	      for (j = backcount; j > i; j--)
		{
		  backcount--;
		  s->nelements--;
		  s->nspace++;
		  
		  if (sRef_lexLevel (s->keys[j]) <= lexlevel)
		    {
		      s->keys[i] = s->keys[j];
		      s->values[i] = s->values[j];
		      sRefSet_levelPrune (s->values[i], lexlevel);
		      /*@innerbreak@*/ break;
		    }
		}
	      if (backcount == i)
		s->nelements--;
	    }
	  else
	    {
	      sRefSet_levelPrune (s->values[i], lexlevel);
	    }
	}
    }
}

/*
** levelUnionSeq
**
**    like level union, but know that t2 was executed after t1.  So if
**    t1 has x -> { a, b } and t2 has x -> { a }, then result has x -> { a }.
**
** NOTE: t2 is "only".
*/

aliasTable aliasTable_levelUnionSeq (/*@returned@*/ aliasTable t1, 
				     /*@only@*/ aliasTable t2, int level)
{
  if (aliasTable_isUndefined (t2))
    {
      return t1;
    }

  if (aliasTable_isUndefined (t1))
    {
      t1 = aliasTable_newEmpty ();
    }
  else
    {
      aliasTable_levelPrune (t1, level);
    }

  aliasTable_elements (t2, key, value)
    {
      if (sRef_lexLevel (key) <= level)
	{
	  int ind = aliasTable_lookupRefs (t1, key);

	  sRefSet_levelPrune (value, level);
	      
	  if (ind == ATINVALID)
	    {
	      /* okay, t2 is killed */
	      /*@-exposetrans@*/ /*@-dependenttrans@*/ 
	      t1 = aliasTable_addSet (t1, key, value);
	      /*@=exposetrans@*/ /*@=dependenttrans@*/ 
	    }
	  else
	    {
	      sRefSet_free (t1->values[ind]);

	      /*@-dependenttrans@*/ /* okay, t2 is killed */
	      t1->values[ind] = value;
	      /*@=dependenttrans@*/
	    } 
	}
      else
	{
	  /*@-exposetrans@*/ /*@-dependenttrans@*/ 
	  sRefSet_free (value);
	  /*@=exposetrans@*/ /*@=dependenttrans@*/ 
	}

    } end_aliasTable_elements;
  
  sfree (t2->keys);
  sfree (t2->values);
  sfree (t2);

    return t1;
}

aliasTable 
aliasTable_levelUnion (/*@returned@*/ aliasTable t1, aliasTable t2, int level)
{
  if (aliasTable_isUndefined (t1))
    {
      if (aliasTable_isUndefined (t2)) 
	{
	  return t1;
	}
      else
	{
	  t1 = aliasTable_newEmpty ();
	}
    }
  else
    {
      aliasTable_levelPrune (t1, level);
    }

  aliasTable_elements (t2, key, cvalue)
    {
      sRefSet value = sRefSet_newCopy (cvalue);

      if (sRef_lexLevel (key) <= level)
	{
	  sRefSet_levelPrune (value, level);

	  if (sRefSet_size (value) > 0)
	    {
	      int ind = aliasTable_lookupRefs (t1, key);
	      
	      if (ind == ATINVALID)
		{
		  t1 = aliasTable_addSet (t1, key, value);
		}
	      else
		{
		  t1->values[ind] = sRefSet_union (t1->values[ind], value);
		  sRefSet_free (value);
		}
	    }
	  else
	    {
	      sRefSet_free (value); 
	    }
	}
      else
	{
	  sRefSet_free (value); 
	}
    } end_aliasTable_elements;

    return t1;
}

aliasTable aliasTable_levelUnionNew (aliasTable t1, aliasTable t2, int level)
{
  aliasTable ret = aliasTable_levelUnion (aliasTable_copy (t1), t2, level);

  return ret;
}

/*@only@*/ cstring
aliasTable_unparse (aliasTable s)
{
   cstring st = cstring_undefined;

   if (aliasTable_isUndefined (s)) return (cstring_makeLiteral ("<NULL>"));

   aliasTable_elements (s, key, value)
     {
       st = message ("%q\t%q -> %q\n", st, sRef_unparseFull (key), 
		     sRefSet_unparseFull (value));
     } end_aliasTable_elements;

   return st;
}

/*
** bogus!
*/

void
aliasTable_fixSrefs (aliasTable s)
{
  int i;

  if (aliasTable_isUndefined (s)) return;

  for (i = 0; i < s->nelements; i++)
    {
      sRef old = s->keys[i];

      if (sRef_isLocalVar (old))
	{
	  s->keys[i] = uentry_getSref (sRef_getUentry (old));
	}

      sRefSet_fixSrefs (s->values[i]);
    }
}

void
aliasTable_free (/*@only@*/ aliasTable s)
{
  int i;
  
  if (aliasTable_isUndefined (s)) return;
  
  for (i = 0; i < s->nelements; i++)
    {
      sRefSet_free (s->values[i]); 
    }
  
  sfree (s->values);
  sfree (s->keys);
  sfree (s);
}

void 
aliasTable_checkGlobs (aliasTable t)
{
  aliasTable_elements (t, key, value)
    {
      sRef root = sRef_getRootBase (key);

      if (sRef_isAliasCheckedGlobal (root))
	{
	  sRefSet_realElements (value, sr)
	    {
	      root = sRef_getRootBase (sr);

	      if (((sRef_isAliasCheckedGlobal (root) 
		    && !(sRef_similar (root, key)))
		   || sRef_isAnyParam (root))
		  && !sRef_isExposed (root))
		{
		  if (sRef_isAliasCheckedGlobal (key))
		    {
		      if (!(sRef_isShared (key) 
			    && sRef_isShared (root)))
			{
			  voptgenerror 
			    (FLG_GLOBALIAS,
			     message 
			     ("Function returns with %q variable %q aliasing %q %q",
			      cstring_makeLiteral (sRef_isRealGlobal (key) 
						   ? "global" : "file static"),
			      sRef_unparse (key),
			      cstring_makeLiteral (sRef_isAnyParam (root) 
						   ? "parameter" : "global"),
			      sRef_unparse (sr)),
			     g_currentloc);
			}
		    }

		}
	    } end_sRefSet_realElements;
	}
      else if (sRef_isAnyParam (key) || sRef_isAnyParam (root))
	{
	  sRefSet_realElements (value, sr)
	    {
	      root = sRef_getRootBase (sr);
	      
	      if (sRef_isAliasCheckedGlobal (root) 
		  && !sRef_isExposed (root)
		  && !sRef_isDead (key)
		  && !sRef_isShared (root))
		{
		  voptgenerror 
		    (FLG_GLOBALIAS,
		     message ("Function returns with parameter %q aliasing %q %q",
			      sRef_unparse (key),
			      cstring_makeLiteral (sRef_isRealGlobal (root) 
						   ? "global" : "file static"),
			      sRef_unparse (sr)),
		     g_currentloc);
		}
	    } end_sRefSet_realElements;
	}
      else
	{
	  ;
	}
    } end_aliasTable_elements;
}

# ifdef DEBUGSPLINT

/*
** For debugging only
*/

void aliasTable_checkValid (aliasTable t)
{
  aliasTable_elements (t, key, value)
    {
      sRef_checkCompletelyReasonable (key);

      sRefSet_elements (value, sr) 
	{
	  sRef_checkCompletelyReasonable (sr);
	} end_sRefSet_elements ;
    } end_aliasTable_elements ;
}
# endif
