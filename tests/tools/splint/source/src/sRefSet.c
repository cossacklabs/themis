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
** sRefSet.c
**
** based on set_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

sRefSet
sRefSet_new ()
{
  return sRefSet_undefined;
}

static /*@notnull@*/ /*@only@*/ sRefSet
sRefSet_newEmpty (void)
{
  sRefSet s = (sRefSet) dmalloc (sizeof (*s));
  
  s->entries = 0;
  s->nspace = sRefSetBASESIZE;
  s->elements = (sRef *) dmalloc (sizeof (*s->elements) * sRefSetBASESIZE);

  return (s);
}

/*@only@*/ sRefSet
sRefSet_single (/*@exposed@*/ sRef sr)
{
  sRefSet s = (sRefSet) dmalloc (sizeof (*s));
  
  s->entries = 1;
  s->nspace = sRefSetBASESIZE - 1;
  s->elements = (sRef *) dmalloc (sizeof (*s->elements) * sRefSetBASESIZE);
  s->elements[0] = sr;

  return (s);
}

static void
sRefSet_grow (/*@notnull@*/ sRefSet s)
{
  int i;
  sRef *newelements; 

  s->nspace = sRefSetBASESIZE;
  newelements = (sRef *) dmalloc (sizeof (*newelements) * (s->entries + s->nspace));

  for (i = 0; i < s->entries; i++)
    {
      newelements[i] = s->elements[i];
    }

  sfree (s->elements);
  s->elements = newelements;
}

sRefSet
sRefSet_insert (sRefSet s, /*@exposed@*/ sRef el)
{
  if (sRefSet_isUndefined (s))
    {
      s = sRefSet_newEmpty ();
    }

  
  if (!sRefSet_isSameMember (s, el))
    {
      
      if (s->nspace <= 0)
	sRefSet_grow (s);

      s->nspace--;

      llassert (s->elements != NULL);
      s->elements[s->entries] = el;
      s->entries++;
    }
  else
    {
      ;
    }
  
  return s;
}

void
sRefSet_clear (sRefSet s)
{
  if (sRefSet_isDefined (s))
    {
      s->nspace += s->entries;
      s->entries = 0;
    }
}

/*
** slow algorithm...but it doesn't matter
*/

void
sRefSet_clearStatics (sRefSet s)
{
  if (sRefSet_isDefined (s))
    {
      int i;
      
      for (i = 0; i < s->entries; i++)
	{
	  sRef current = s->elements[i];
	  
	  if (sRef_isFileStatic (sRef_getRootBase (current)))
	    {
	      int j;
	      
	      for (j = i; j < s->entries - 1; j++)
		{
		  s->elements[j] = s->elements[j+1];
		}
	      
	      s->entries--;
	      s->nspace++;
	      i--;
	    }
	}
    }
}

bool
sRefSet_delete (sRefSet s, sRef el)
{
  int i;

  if (sRefSet_isUndefined (s)) return FALSE;

  if (s->elements != NULL)
    {
      for (i = 0; i < s->entries; i++)
	{
	  sRef current = s->elements[i];
	  
	  if (sRef_realSame (el, current))
	    {
	      int j;

	      for (j = i; j < s->entries - 1; j++)
		{
		  s->elements[j] = s->elements[j+1];
		}
	      
	      s->entries--;
	      s->nspace++;
	      return TRUE;
	    }
	}
    }

  return FALSE;
}

/*@exposed@*/ sRef
sRefSet_choose (sRefSet s)
{
  llassert (sRefSet_isDefined (s));
  llassert (s->entries > 0);
  llassert (s->elements != NULL);

  return (s->elements[0]);
}

/*@exposed@*/ sRef
sRefSet_mergeIntoOne (sRefSet s)
{
  sRef res;
  int i;

  if (sRefSet_isUndefined (s)) return sRef_undefined;
  if (s->entries == 0) return sRef_undefined;

  llassert (s->elements != NULL);

  res = s->elements[0];

  for (i = 1; i < s->entries; i++)
    {
      sRef tmp;

      tmp = sRef_makeConj (res, s->elements[i]);
      res = tmp;
    }
  
  return res;
}

/*
** this is really yucky...but it works...
*/

bool
sRefSet_deleteBase (sRefSet s, sRef base)
{
  int i = 0;
  int offset = 0;

  if (sRefSet_isUndefined (s) || (s->elements == NULL))
    {
      return FALSE;
    } ;

  while (i + offset < s->entries)
    {
      sRef current = s->elements[i + offset];

      while (sRef_includedBy (current, base))
	{
	  	  offset++;
	  if (i + offset >= s->entries) goto doneLoop;
	  current = s->elements [i + offset];
	}
      
      if (offset > 0)
	{
	  	  s->elements [i] = current;
	}
      
      i++;
    }

 doneLoop:
  s->entries -= offset;
  s->nspace += offset;

  return (offset > 0);
}

/*
** modifies *s1
*/

sRefSet
sRefSet_unionFree (/*@returned@*/ sRefSet s1, sRefSet s2)
{
  sRefSet res = sRefSet_union (s1, s2);

  sRefSet_free (s2);
  return res;
}

sRefSet
sRefSet_union (/*@returned@*/ sRefSet s1, sRefSet s2)
{
  if (s1 == s2) 
    {
      return s1;
    }

  if (sRefSet_isEmpty (s1))
    {
      s1 = sRefSet_copyInto (s1, s2);
    }
  else
    {
      sRefSet_allElements (s2, el)
	{
	  s1 = sRefSet_insert (s1, el);
	} end_sRefSet_allElements;
    }

  return s1;
}

/*
** s1 <- s1 U (s2 - ex - params)
*/

sRefSet
sRefSet_unionExcept (/*@returned@*/ sRefSet s1, sRefSet s2, sRef ex)
{
  if (s1 == s2) return s1;

  sRefSet_allElements (s2, el)
    {
      if (sRef_same (el, ex))
	{
	  ;
	}
      else
	{
	  s1 = sRefSet_insert (s1, el);
	}
    } end_sRefSet_allElements;
  
  return s1;
}

/*@only@*/ sRefSet
sRefSet_realNewUnion (sRefSet s1, sRefSet s2)
{
  llassert (NOALIAS (s1, s2));

  if (sRefSet_isUndefined (s1))
    {
      return (sRefSet_newCopy (s2));
    }
  else
    {
      sRefSet ret = sRefSet_newCopy (s1);
      
      sRefSet_allElements (s2, el)
	{
	  ret = sRefSet_insert (ret, el);
	} end_sRefSet_allElements;
      
      return ret;
    }
}

/* slow! */

/*@only@*/ sRefSet
sRefSet_intersect (sRefSet s1, sRefSet s2)
{
  sRefSet s = sRefSet_new ();

  llassert (NOALIAS (s1, s2));

  sRefSet_allElements (s1, el)
    {
      if (sRefSet_member (s2, el))
	{
	  s = sRefSet_insert (s, el);
	}
    } end_sRefSet_allElements;
  
    return s;
}

sRefSet
sRefSet_levelUnion (/*@returned@*/ sRefSet sr, sRefSet s, int lexlevel)
{
  llassert (NOALIAS (sr, s));

  sRefSet_allElements (s, el)
    {
      if (sRef_lexLevel (el) <= lexlevel)
	{
	  sr = sRefSet_insert (sr, el);
	}
    } end_sRefSet_allElements;

  return sr;
}

void
sRefSet_levelPrune (sRefSet s, int lexlevel)
{
  if (sRefSet_isDefined (s))
    {
      int i;
      int backcount = sRefSet_size (s) - 1;
      
      for (i = 0; i <= backcount; i++)
	{
	  sRef el = s->elements[i]; 
	  
	  if (sRef_lexLevel (el) > lexlevel)
	    {
	      int j;
	      
	      
	      for (j = backcount; j > i; j--)
		{
		  backcount--;
		  s->entries--;
		  s->nspace++;
		  
		  if (sRef_lexLevel (s->elements[j]) <= lexlevel)
		    {
		      s->elements[i] = s->elements[j];
		      
		      if (backcount == i) s->entries++;
		      /*@innerbreak@*/ break;
		    }
		}

	      if (backcount == i)
		{
		  s->entries--;
		}
	    }
	}
    }
}

/*
** s1 <- s2
*/

sRefSet sRefSet_copyInto (/*@returned@*/ sRefSet s1, /*@exposed@*/ sRefSet s2)
{
  int origentries;
  
  llassert (NOALIAS (s1, s2));
  
  if (sRefSet_isUndefined (s1))
    {
      if (sRefSet_isEmpty (s2))
	{
	  return s1;
	}
      else
	{
	  s1 = sRefSet_newEmpty ();
	}
    } 

  origentries = s1->entries;

  s1->nspace = s1->entries + s1->nspace;
  s1->entries = 0;

  sRefSet_allElements (s2, el)
    {
      if (s1->nspace == 0)
	{
	  sRefSet_grow (s1);
	}

      s1->elements[s1->entries] = el; 
      s1->nspace--;
      s1->entries++;
    } end_sRefSet_allElements;

  return s1;
}

/*@only@*/ sRefSet
  sRefSet_newCopy (/*@exposed@*/ sRefSet s)
{
  if (sRefSet_isEmpty (s))
    {
      return sRefSet_undefined;
    }
  else
    {
      sRefSet r = (sRefSet) dmalloc (sizeof (*r));
      int i;
      
      r->entries = s->entries;
      r->nspace = s->nspace;
      r->elements = (sRef *) dmalloc (sizeof (*r->elements) * (s->entries + s->nspace));
      
      for (i = 0; i < s->entries; i++)
	{
	  r->elements[i] = s->elements[i]; 
	}  
      
      return r;
    }
}

/*@only@*/ sRefSet
sRefSet_levelCopy (/*@exposed@*/ sRefSet s, int lexlevel)
{
  if (sRefSet_isEmpty (s))
    {
      return sRefSet_undefined;
    }
  else
    {
      sRefSet r = (sRefSet) dmalloc (sizeof (*r));
      int i;
      
      r->nspace = s->entries;
      r->entries = 0;
      r->elements = (sRef *) dmalloc (sizeof (*r->elements) * (s->entries)); 

      for (i = 0; i < s->entries; i++)
	{
	  if (sRef_lexLevel (s->elements[i]) <= lexlevel)
	    {
	      r->elements[r->entries] = s->elements[i];
	      r->entries++;
	      r->nspace--;
	    }
	}  
      
      return r;
    }
}

/*@only@*/ sRefSet
sRefSet_newDeepCopy (sRefSet s)
{
  if (sRefSet_isUndefined (s))
    {
      return sRefSet_newEmpty ();
    }
  else
    {
      sRefSet r = (sRefSet) dmalloc (sizeof (*r));
      int i;
      
      r->entries = s->entries;
      r->nspace = s->nspace;
      r->elements = (sRef *) dmalloc (sizeof (*r->elements) * (s->entries + s->nspace));
      
      for (i = 0; i < s->entries; i++)
	{
	  r->elements[i] = sRef_copy (s->elements[i]);
	}  
      
      return r;
    }
}

static bool
sRefSet_isElementCompare (bool (*test)(sRef, sRef), sRefSet s, sRef el)
{
  sRefSet_allElements (s, e)
    {
      if ((test)(el, e))
	{
	  return TRUE;
	}
    } end_sRefSet_allElements;

  return FALSE;
}

static bool
sRefSet_isElementTest (bool (*test)(sRef), sRefSet s)
{
  sRefSet_allElements (s, e)
    {
      if ((test)(e))
	{
	  return TRUE;
	}
    } end_sRefSet_allElements;

  return FALSE;
}

bool
sRefSet_hasRealElement (sRefSet s)
{
  sRefSet_allElements (s, e)
    {
      if (sRef_isMeaningful (e) && !sRef_isUnconstrained (e))
	{
	  return TRUE;
	}
    } end_sRefSet_allElements;

  return FALSE;
}

bool
sRefSet_containsSameObject (sRefSet s, sRef el)
{
  return (sRefSet_isElementCompare (sRef_sameObject, s, el));
}

bool
sRefSet_isSameMember (sRefSet s, sRef el)
{
  return (sRefSet_isElementCompare (sRef_realSame, s, el));
}

bool
sRefSet_isSameNameMember (sRefSet s, sRef el)
{
  return (sRefSet_isElementCompare (sRef_sameName, s, el));
}

bool
sRefSet_member (sRefSet s, sRef el)
{
  return (sRefSet_isElementCompare (sRef_similar, s, el));
}

bool
sRefSet_hasStatic (sRefSet s)
{
  return (sRefSet_isElementTest (sRef_isFileStatic, s));
}

bool
sRefSet_hasUnconstrained (sRefSet s)
{
  return (sRefSet_isElementTest (sRef_isUnconstrained, s));
}

cstring
  sRefSet_unparseUnconstrained (sRefSet s)
{
  int num = 0;
  cstring res = cstring_undefined;
  
  sRefSet_allElements (s, el)
    {
      if (sRef_isUnconstrained (el))
	{
	  if (cstring_isUndefined (res))
	    {
	      res = cstring_copy (sRef_unconstrainedName (el));
	    }
	  else
	    {
	      res = message ("%q, %s", res, sRef_unconstrainedName (el));
	    }
	  
	  num++;
	}
    } end_sRefSet_allElements ;
  
  if (num == 0)
    {
      llassert (cstring_isUndefined (res));
      return (cstring_makeLiteral ("<ERROR: no unconstrained calls>"));
    }
  else if (num == 1)
    {
      return (message ("unconstrained function %q", res));
    }
  else
    {
      return (message ("unconstrained functions %q", res));
    }
}

cstring
sRefSet_unparseUnconstrainedPlain (sRefSet s)
{
  cstring res = cstring_undefined;

  sRefSet_allElements (s, el)
    {
      if (sRef_isUnconstrained (el))
	{
	  if (cstring_isUndefined (res))
	    {
	      res = cstring_copy (sRef_unconstrainedName (el));
	    }
	  else
	    {
	      res = message ("%q, %s", res, sRef_unconstrainedName (el));
	    }
	}
    } end_sRefSet_allElements ;

  return res;
}
      
bool
sRefSet_modifyMember (sRefSet s, sRef m)
{
  bool ret = FALSE;

  sRefSet_allElements (s, e)
    {
      if (sRef_similar (m, e))
	{
	  sRef_setModified (e);
	  ret = TRUE;
	}
    } end_sRefSet_allElements;

  
  return ret;
}

/*@exposed@*/ sRef
sRefSet_lookupMember (sRefSet s, sRef el)
{
  sRefSet_allElements (s, e)
    {
      if (sRef_similar (el, e))
	{
	  return e;
	}
    } end_sRefSet_allElements;
  
  return sRef_undefined;
}

int sRefSet_size (sRefSet s)
{
  if (sRefSet_isUndefined (s)) return 0;
  return s->entries;
}

/*@only@*/ cstring
sRefSet_unparse (sRefSet s)
{
  int i;
  cstring st = cstring_makeLiteral ("{");

  if (sRefSet_isDefined (s))
    {
      for (i = 0; i < sRefSet_size (s); i++)
	{
	  if (i == 0)
	    st = message ("%q %q", st, sRef_unparse (s->elements[i]));
	  else
	    st = message ("%q, %q", st, sRef_unparse (s->elements[i]));
	}
    }
  
  st = message ("%q }", st);
  return st;
}

cstring sRefSet_unparsePlain (sRefSet s)
{
  int i;
  cstring st = cstring_undefined;

  if (sRefSet_isDefined (s))
    {
      for (i = 0; i < sRefSet_size (s); i++)
	{
	  if (i == 0)
	    st = sRef_unparse (s->elements[i]);
	  else
	    st = message ("%q, %q", st, sRef_unparse (s->elements[i]));
	}
    }
  
  return st;
}

cstring
sRefSet_unparseDebug (sRefSet s)
{
  int i;
  cstring st = cstring_makeLiteral ("{");

  if (sRefSet_isDefined (s))
    {
      for (i = 0; i < sRefSet_size (s); i++)
	{
	  if (i == 0)
	    {
	      st = message ("%q %q", st, sRef_unparseDebug (s->elements[i]));
	    }
	  else
	    {
	      st = message ("%q, %q", st, sRef_unparseDebug (s->elements[i]));
	    }
	}
    }
  
  st = message ("%q }", st);
  return st;
}

cstring
sRefSet_unparseFull (sRefSet s)
{
  int i;
  cstring st = cstring_makeLiteral ("{");

  if (sRefSet_isDefined (s))
    {
      for (i = 0; i < sRefSet_size (s); i++)
	{
	  if (i == 0)
	    {
	      st = message ("%q %q", st, sRef_unparseFull (s->elements[i]));
	    }
	  else
	    {
	      st = message ("%q, %q", st, sRef_unparseFull (s->elements[i]));
	    }
	}
    }
  
  st = message ("%q }", st);
  return st;
}

void
sRefSet_fixSrefs (sRefSet s)
{
  if (sRefSet_isDefined (s))
    {
      int i;
      
      for (i = 0; i < sRefSet_size (s); i++)
	{
	  sRef current = s->elements[i];

	  if (sRef_isLocalVar (current))
	    {
	      s->elements[i] = uentry_getSref (sRef_getUentry (current));
	    }
	}
    }
}

void
sRefSet_free (/*@only@*/ sRefSet s)
{
  if (!sRefSet_isUndefined (s))
    {
      /* evans 2003-10-20: increase size sanity limit from 1000 */
      llassertprint (s->entries < 99999, ("sRefSet free size: %d", s->entries));

      sfree (s->elements); 
      sfree (s);
    }
}

sRefSet sRefSet_removeIndirection (sRefSet s)
{
  /*
  ** returns a NEW sRefSet containing references to all sRef's in s
  */

  sRefSet t = sRefSet_new ();
  
  
  sRefSet_allElements (s, el)
    {
      if (!sRef_isAddress (el))
	{
	  t = sRefSet_insert (t, sRef_makeAddress (el));
	}
    } end_sRefSet_allElements;

  return t;
}

sRefSet sRefSet_addIndirection (sRefSet s)
{
  /*
  ** returns a NEW sRefSet containing references to all sRef's in s
  */

  sRefSet t = sRefSet_new ();
  
  
  sRefSet_allElements (s, el)
    {
      ctype ct = ctype_realType (sRef_getType (el));
      
      
      if ((ctype_isArrayPtr (ct)))
	{
	  
	  sRef a = sRef_constructPointer (el);
	  t = sRefSet_insert (t, a);   
	}
    } end_sRefSet_allElements;

    return t;
}

sRefSet sRefSet_accessField (sRefSet s, /*@observer@*/ cstring f)
{
  /*
  ** returns a NEW sRefSet containing references to all sRef's in s
  */
  
  sRefSet t = sRefSet_new ();
  
  sRefSet_allElements (s, el)
    {
      ctype ct = ctype_realType (sRef_getType (el));
      
      if ((ctype_isStruct (ct) || ctype_isUnion (ct))
	  && (!uentry_isUndefined (uentryList_lookupField (ctype_getFields (ct), f))))
	{
	  t = sRefSet_insert (t, sRef_makeNCField (el, f));
	}
    } end_sRefSet_allElements;
  
    return t;
}

sRefSet sRefSet_fetchUnknown (sRefSet s)
{
  sRefSet t = sRefSet_new ();
  
  sRefSet_allElements (s, el)
    {
      ctype ct = ctype_realType (sRef_getType (el));
      
      if (ctype_isArrayPtr (ct))
	{
	  t = sRefSet_insert (t, sRef_makeArrayFetch (el));
	}
    } end_sRefSet_allElements;

  return t;
}

sRefSet sRefSet_fetchKnown (sRefSet s, int i)
{
  sRefSet t = sRefSet_new ();
  
  sRefSet_allElements (s, el)
    {
      ctype ct = ctype_realType (sRef_getType (el));
      
      if (ctype_isArrayPtr (ct))
	{
	  t = sRefSet_insert (t, sRef_makeArrayFetchKnown (el, i));
	}
    } end_sRefSet_allElements;

  return t;
}

int sRefSet_compare (sRefSet s1, sRefSet s2)
{
  sRefSet_allElements (s1, el)
    {
      if (!sRefSet_isSameMember (s2, el))
	{
	  return -1;
	}
    } end_sRefSet_allElements;

  sRefSet_allElements (s2, el)
    {
      if (!sRefSet_isSameMember (s1, el))
	{
	  return 1;
	}
    } end_sRefSet_allElements;

  return 0;
}

bool sRefSet_equal (sRefSet s1, sRefSet s2)
{
  sRefSet_allElements (s1, el)
    {
      if (!sRefSet_isSameMember (s2, el))
	{
	  return FALSE;
	}
    } end_sRefSet_allElements;

  sRefSet_allElements (s2, el)
    {
      if (!sRefSet_isSameMember (s1, el))
	{
	  return FALSE;
	}
    } end_sRefSet_allElements;

  return TRUE;
}

/*@only@*/ sRefSet
sRefSet_undump (char **s)
{
  char c;
  sRefSet sl = sRefSet_new ();

  while ((c = **s) != '#' && c != '@' && c != '$' && c != '&')
    {
      sl = sRefSet_insert (sl, sRef_undump (s));

      
      if (**s == ',')
	{
	  (*s)++;
	}
    }

    return sl;
}

/*@only@*/ cstring
sRefSet_dump (sRefSet sl)
{
  cstring st = cstring_undefined;
  bool first = TRUE;

  
  sRefSet_allElements (sl, el)
    {
      if (!first)
	{
	  st = cstring_appendChar (st, ',');
	}
      else
	{
	  first = FALSE;
	}

      st = cstring_concatFree (st, sRef_dump (el));
    } end_sRefSet_allElements;

  return st;
}

void
sRefSet_markImmutable (sRefSet s)
{
  sRefSet_allElements (s, el)
    {
      sRef_markImmutable (el);
    } end_sRefSet_allElements;
}

