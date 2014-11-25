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
** uentryList.c (from slist_template.c)
*/

# include "splintMacros.nf"
# include "basic.h"

/*@only@*/ /*@notnull@*/ uentryList
uentryList_new ()
{
  uentryList s = (uentryList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = uentryListBASESIZE;
  s->elements = (uentry *) 
    dmalloc (sizeof (*s->elements) * uentryListBASESIZE);
  s->current = 0;

  return (s);
}

/*@only@*/ uentryList
uentryList_single (/*@keep@*/ uentry el)
{
  uentryList s = (uentryList) dmalloc (sizeof (*s));
  
  s->nelements = 1;
  s->nspace = uentryListBASESIZE - 1;
  s->elements = (uentry *) dmalloc (sizeof (*s->elements) * uentryListBASESIZE);
  
  s->elements[0] = el;
  s->current = 0;

  return (s);
}

static void
uentryList_grow (uentryList s)
{
  int i;
  uentry *newelements;

  llassert (!uentryList_isUndefined (s));

  s->nspace += uentryListBASESIZE; 
  
  newelements = (uentry *) dmalloc (sizeof (*newelements) 
				    * (s->nelements + s->nspace));
    
  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }
  
  sfree (s->elements);
  s->elements = newelements;
}

void uentryList_clear (uentryList s)
{
  if (uentryList_isUndefined (s))
    {
      ;
    }
  else
    {
      s->nspace += s->nelements;
      s->nelements = 0;
    }
}

uentryList uentryList_add (uentryList s, /*@keep@*/ uentry el)
{
  if (uentryList_isUndefined (s))
    {
      s = uentryList_new ();
    }

  if (s->nspace <= 0)
    uentryList_grow (s);
  
  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

/*@only@*/ cstring
  uentryList_unparse (uentryList s)
{
  cstring st = cstring_undefined;
  int i;
  
  if (uentryList_isDefined (s))
    {
      for (i = 0; i < uentryList_size (s); i++)
	{
	  if (i == 0)
	    {
	      st = message ("%q;", uentry_unparse (s->elements[i]));
	    }
	  else
	    st = message ("%q %q;", st, uentry_unparse (s->elements[i]));
	}
    }
  
  return (st);
}

/*@unused@*/ /*@only@*/ cstring
  uentryList_unparseFull (uentryList s)
{
  cstring st = cstring_undefined;
  int i;
  
  if (uentryList_isDefined (s))
    {
      for (i = 0; i < uentryList_size (s); i++)
	{
	  if (i == 0)
	    {
	      st = message ("%q;", uentry_unparseFull (s->elements[i]));
	    }
	  else
	    {
	      st = message ("%q %q;", st, uentry_unparseFull (s->elements[i]));
	    }
	}
    }
  
  return (st);
}

cstring uentryList_unparseParams (uentryList s)
{
  int i;
  cstring st = cstring_undefined;

  
  if (uentryList_isUndefined (s))
    {
      return st;
    }
  else if (uentryList_isVoid (s))
    {
      return (cstring_makeLiteral ("void"));
    }
  else
    {
      for (i = 0; i < uentryList_size (s); i++)
	{
	  if (i == 0)
	    {
	      st = message ("%s", ctype_unparse (uentry_getType (s->elements[i])));
	    }
	  else
	    {
	      st = message ("%q, %s", st, ctype_unparse (uentry_getType (s->elements[i])));
	    }
	}
      
      return st;
    }
}

bool uentryList_matchParams (uentryList p1, uentryList p2, bool force, bool arg)
{
  int sz1 = uentryList_size (p1);
  int sz2 = uentryList_size (p2);
  int i;
  
  if (p1 == p2) return TRUE;

  if (uentryList_isMissingParams (p1) || uentryList_isMissingParams (p2))
    {
      return TRUE;
    }

  if (sz1 != sz2)
    return FALSE;

  for (i = 0; i < sz1; i++)
    {
      if (!ctype_genMatch (uentry_getType (p1->elements[i]), 
			   uentry_getType (p2->elements[i]), 
			   force, arg, FALSE, FALSE))
	{
	  return FALSE;
	}
    }

  return TRUE;
}

/*@only@*/ cstring
uentryList_unparseAbbrev (uentryList p)
{
  bool first = TRUE;
  cstring s = cstring_undefined;
  int i = 0;
  
  if (uentryList_isUndefined (p))
    return s;

  if (uentryList_size (p) == 0)
    return cstring_makeLiteral ("void");

  for (i = 0; i < p->nelements && i < uentryList_abbrevBreadth; i++)
    {
      if (first)
	{
	  s = message ("%q;", uentry_unparseAbbrev (p->elements[i]));
	  first = FALSE;
	}
      else
	{
	  s = message ("%q %q;", s, uentry_unparseAbbrev (p->elements[i]));
	}
    }
  
  if (i != uentryList_size (p))
    s = message ("%q, ...", s);
  
  return (s);
}

static int
uentryList_lookupDirectName (uentryList s, cstring name)
{
  if (uentryList_isDefined (s))
    {
      int i;
      
      for (i = 0; i < uentryList_size (s); i++)
	{
	  if (cstring_equal (name, uentry_rawName (s->elements[i])))
	    {
	      return i;
	    }
	}
    }

   return -1;
}

int
uentryList_lookupRealName (uentryList s, cstring name)
{
  if (uentryList_isDefined (s))
    {
      int i;
      
      for (i = 0; i < uentryList_size (s); i++)
	{
	  cstring uname = uentry_getName (s->elements[i]);

	  if (cstring_equal (name, uname))
	    {
	      cstring_free (uname);
	      return i;
	    }      

	  cstring_free (uname);
	}
    }

   return -1;
}

uentryList uentryList_copy (uentryList s)
{
  if (uentryList_isDefined (s))
    {
      uentryList t = (uentryList) dmalloc (sizeof (*t));
      int i;
      
      t->nelements = s->nelements;
      t->nspace = 0;
      t->current = s->current;
      
      if (s->nelements > 0)
	{
	  t->elements = (uentry *) dmalloc (sizeof (*t->elements) * t->nelements);
	  
	  for (i = 0; i < s->nelements; i++) 
	    {
	      t->elements[i] = uentry_copy (s->elements[i]); 
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
      return uentryList_undefined;
    }
}

void
uentryList_free (uentryList s)
{
  if (!uentryList_isUndefined (s)) 
    {
      int i;

      for (i = 0; i < s->nelements; i++)
	{
	  uentry_free (s->elements[i]);  
	}

      sfree (s->elements);
      sfree (s);
    }
}

void
uentryList_freeShallow (uentryList s)
{
  if (!uentryList_isUndefined (s)) 
    {
      /*@-mustfree@*/ /* free shallow does not free the element */ 
      sfree (s->elements);
      /*@=mustfree@*/
      sfree (s);
    }
}

bool
uentryList_isVoid (uentryList cl)
{
  if (cl != NULL && cl->nelements == 1)
    {
      return (ctype_isVoid (ctype_realType (uentry_getType (cl->elements[0]))));
    }

  return FALSE;
}

/*@exposed@*/ uentry
uentryList_getN (uentryList p, int n)
{
  llassert (uentryList_isDefined (p));

  if (n < 0 || (n >= uentryList_size (p)))
    {
      llcontbug (message ("uentryList_getN: out of range: %d (size %d)",
			  n, uentryList_size (p)));
      return uentry_undefined;
    }

  return (p->elements[n]);
}

void uentryList_fixMissingNames (uentryList cl)
{
  uentryList_elements (cl, ce)
    {
      if (!uentry_hasRealName (ce))
	{
	  ctype ct = uentry_getType (ce);

	  if (ctype_isUA (ct))
	    {
	      uentry_setName (ce, usymtab_getTypeEntryName (ctype_typeId (ct)));
	    }
	  else
	    {
	      llbug (message ("uentryList_fixMissingNames: not UA: %s", 
			      ctype_unparse (ct)));
	    }

	  uentry_setType (ce, ctype_int);
	}
    } end_uentryList_elements;
}

void uentryList_fixImpParams (uentryList cl)
{
  
  if (context_getFlag (FLG_PARAMIMPTEMP))
    {
      uentryList_elements (cl, ce)
	{
	  sRef s = uentry_getSref (ce);
	  alkind ak = sRef_getAliasKind (s);

	  if (alkind_isUnknown (ak) || alkind_isImplicit (ak))
	    {
	      exkind ek = sRef_getExKind (s);

	      if (exkind_isKnown (ek))
		{
		  sRef_setAliasKind (s, AK_IMPDEPENDENT, fileloc_undefined);
		}
	      else
		{
		  sRef_setAliasKind (s, AK_IMPTEMP, fileloc_undefined);
		}
	      	    }
	  else
	    {
	      	    }
	} end_uentryList_elements;
    }
}

int
uentryList_compareParams (uentryList s, uentryList t)
{
  int i, sz;

  if (s == t) return 0;

  if (uentryList_isUndefined (s)) return 1;
  if (uentryList_isUndefined (t)) return -1;
  
  sz = uentryList_size (s);
  
  INTCOMPARERETURN (uentryList_size (t), sz);
  
  for (i = 0; i < sz; i++)
    {
      COMPARERETURN (uentry_compare (s->elements[i], t->elements[i]));
    }
  
  return 0;
}

int
uentryList_compareStrict (uentryList s, uentryList t)
{
  int i, sz;

  if (s == t) 
    {
      return 0;
    }
  
  if (uentryList_isMissingParams (s))
    {
      if (uentryList_isMissingParams (t))
	{
	  return 0;
	}
      else
	{
	  return 1;
	}
    }
  else 
    {
      if (uentryList_isMissingParams (t))
	{
	  return -1;
	}
      else
	{
	  sz = uentryList_size (s);
	  
	  INTCOMPARERETURN (uentryList_size (t), sz);
	  
	  for (i = 0; i < sz; i++)
	    {
	      COMPARERETURN (uentry_compareStrict (s->elements[i], t->elements[i]));
	    }
	  
	  return 0;
	}
    }
}

int
uentryList_compareFields (uentryList s, uentryList t)
{
  int i, sz;

  if (s == t) return 0;

  if (uentryList_isUndefined (s))
    return 1;
  if (uentryList_isUndefined (t))
    return -1;
  
  sz = uentryList_size (s);
  
  if (uentryList_size (t) != sz)
    {
      return (int_compare (sz, uentryList_size (t)));
    }
  
  for (i = 0; i < sz; i++)
    {
      uentry se = s->elements[i];
      uentry te = t->elements[i];
      int namecmp = cstring_compare (uentry_rawName (se), uentry_rawName (te));

      if (namecmp == 0)
	{
	  int uc = uentry_compare (s->elements[i], t->elements[i]);
	  
	  if (uc != 0) 
	    { 
	      DPRINTF (("Bad compare: %s / %s",
			uentry_unparseFull (s->elements [i]),
			uentry_unparseFull (t->elements [i])));

	      return uc; 
	    }
	}
      else
	{
	  return (namecmp);
	}
    }

  return 0;
}

/*@exposed@*/ uentry 
uentryList_current (uentryList s)
{
  llassert (uentryList_isDefined (s));
  llassert (!(s->current < 0 || (s->current >= s->nelements)));
  return (s->elements[s->current]);
}

cstring
uentryList_dumpParams (uentryList s)
{
  cstring st = cstring_undefined;

  if (uentryList_isUndefined (s)) return st;
  
  uentryList_elements (s, current)
    {
      DPRINTF (("Dump param: %s", uentry_unparse (current)));
      st = message ("%q%q,", st, uentry_dumpParam (current));
  } end_uentryList_elements;

  return st;
}

/*@only@*/ cstring
uentryList_dumpFields (uentryList s)
{
  cstring st = cstring_undefined;

  if (uentryList_isUndefined (s)) return st;

  uentryList_elements (s, current)
  {
    if (!uentry_isVariable (current))
      {
	llassert (uentry_isFunction (current));
	DPRINTF (("Dump field: %s", uentry_unparse (current)));
	st = message ("%q!%q,", st, uentry_dump (current));
      }
    else
      {
	DPRINTF (("Dump field: %s", uentry_unparse (current)));
	st = message ("%q%q,", st, uentry_dump (current));
      }
  } end_uentryList_elements;
  
  return st;
}

/*@only@*/ uentryList 
uentryList_undumpFields (char **s, fileloc loc)
{
  uentryList ul = uentryList_new ();

  while (**s != '\0' && **s != '}') 
    {
      if (**s == '!')
	{
	  reader_checkChar (s, '!');
	  ul = uentryList_add (ul, uentry_undump (ekind_function, loc, s));
	}
      else
	{
	  ul = uentryList_add (ul, uentry_undump (ekind_variable, loc, s));
	}
      reader_checkChar (s, ',');
    }

  reader_checkChar (s, '}');
  return ul;
}

/*@only@*/ uentryList
uentryList_undump (char **s)
{
  char c;
  uentryList pn = uentryList_new ();
  int paramno = 0;

  c = **s;

  while (c != '#' && c != '@' && c != ')')
    {
      uentry ue = uentry_undump (ekind_variable, g_currentloc, s);
      
      
      if (!uentry_isUndefined (ue))
	{
	  pn = uentryList_add (pn, ue);
	}
      else
	{
	  uentry_free (ue);
	}

      reader_checkChar (s, ',');
      c = **s;
      paramno++;
    }

  reader_checkChar (s, ')');
  return pn;
}

void 
uentryList_reset (uentryList s)
{
  if (uentryList_isUndefined (s)) return;
  s->current = 0;
}

bool
uentryList_isFinished (uentryList s)
{
  if (uentryList_isUndefined (s)) return TRUE;
  return (s->current > s->nelements - 1);
}

void 
uentryList_advanceSafe (uentryList s)
{
  if (uentryList_isUndefined (s)) return;

  s->current++;

  if (s->current > s->nelements)
    {
      s->current = s->nelements;
    }
}

int
uentryList_size (uentryList s)
{
  if (uentryList_isUndefined (s)) return 0;

  if (uentryList_isVoid (s))
    return 0;
  
  return s->nelements;
}

bool
uentryList_isMissingParams (uentryList s)
{
  return (uentryList_isUndefined (s) || s->nelements == 0);
}

bool uentryList_hasReturned (uentryList ul)
{
  uentryList_elements (ul, current)
    {
      if (uentry_isReturned (current)) return TRUE;
    } end_uentryList_elements;

  return FALSE;
}

/*@exposed@*/ uentry 
uentryList_lookupField (uentryList f, cstring name)
{
  int i = uentryList_lookupDirectName (f, name);

  if (i >= 0)
    {
      return (uentryList_getN (f, i));
    }
  else
    {
      uentryList_elements (f, el)
	{
	  if (uentry_isUnnamedVariable (el))
	    {
	      ctype ct = uentry_getType (el);

	      if (ctype_isStruct (ct) || ctype_isUnion (ct))
		{
		  uentryList fields = ctype_getFields (ct);
		  uentry ue = uentryList_lookupField (fields, name);

		  if (uentry_isValid (ue))
		    {
		      return ue;
		    }
		}
	    }
	}
      end_uentryList_elements ;

      return uentry_undefined;
    }
}

/*@only@*/ uentryList
  uentryList_mergeFields (/*@only@*/ uentryList f1, /*@only@*/ uentryList f2)
{
  DPRINTF (("Merge: %s + %s", uentryList_unparse (f1), uentryList_unparse (f2)));

  if (uentryList_isUndefined (f1))
    {
      return  (f2);
    }

  if (uentryList_isDefined (f2))
    {
      uentryList_elements (f2, current)
	{
	  uentry old = uentryList_lookupField (f1, uentry_rawName (current));
	  
	  if (uentry_isValid (old))
	    {
	      voptgenerror
		(FLG_SYNTAX,
		 message ("Field name reused: %s", uentry_rawName (current)),
		 uentry_whereDefined (current));
	      llgenmsg (message ("Previous use of %s", uentry_rawName (current)),
			uentry_whereDefined (old));
	    }
	  
	  /* okay to use exposed current since f2 is killed */
	  /*@-exposetrans@*/ /*@-dependenttrans@*/
	  f1 = uentryList_add (f1, current); 
	  /*@=exposetrans@*/ /*@=dependenttrans@*/

	} end_uentryList_elements;
      
      sfree (f2->elements);
      sfree (f2);
    }

  return (f1);
}

void
uentryList_showFieldDifference (uentryList p1, uentryList p2)
{
  uentry cp1, cp2;
  int index;

  llassert (NOALIAS (p1, p2));
  llassert (uentryList_isDefined (p1));
  llassert (uentryList_isDefined (p2));
  
  for (index = 0; index < p1->nelements; index++)
    {
      cp1 = p1->elements[index];

      if (index == p2->nelements)
	{
	  llgenindentmsg
	    (message ("Field present in %s, missing in %rdeclaration: %q", 
		      uentry_specDeclName (cp1),
		      uentry_isDeclared (cp1),
		      uentry_unparse (cp1)),
	     uentry_whereEither (cp1));
	  return;
	}
	  
      cp2 = p2->elements[index];

      if (!(cstring_equal (uentry_rawName (cp1), uentry_rawName (cp2))))
	{
	  llgenindentmsg 
	    (message ("Field %s in %s corresponds to %s in %rdeclaration", 
		      uentry_rawName (cp1),
		      uentry_specOrDefName (cp1),
		      uentry_rawName (cp2),
		      uentry_isCodeDefined (cp1)),
	     uentry_whereDefined (cp2));
	  uentry_showWhereLastPlain (cp1);
	  return;
	}
      else 
	{
	  /* evs 2000-07-25 was ctype_match, should match uentryList_matchFields */

	  if (!ctype_almostEqual (uentry_getType (cp1), uentry_getType (cp2)))
	    {
	      llgenindentmsg 
		(message ("Field %s %rdeclared as %s, %s as %s",
			  uentry_rawName (cp2),
			  uentry_isCodeDefined (cp1),
			  ctype_unparse (uentry_getType (cp1)),
			  uentry_specOrDefName (cp2),
			  ctype_unparse (uentry_getType (cp2))),
		 uentry_whereDefined (cp2));
	      uentry_showWhereLastPlain (cp1);
	      return;
	    }
	}
    }

  if (index != p2->nelements)
    {
      cp2 = p2->elements[index];

      llgenindentmsg 
	(message ("Extra field in new declaration: %q",
		  uentry_unparse (cp2)),
	 uentry_whereEither (cp2));

      return;
    }

  llbug (message ("uentryList_showFieldDifference: match: %q / %q",
		  uentryList_unparse (p1), uentryList_unparse (p2)));
}

bool
uentryList_equivFields (uentryList p1, uentryList p2)
{
  return (uentryList_compareFields (p1, p2) == 0);
}

bool
uentryList_matchFields (uentryList p1, uentryList p2)
{
  int index;
  uentry cp1, cp2;

  if (p1 == p2) 
    {
      return (TRUE);
    }

  if (uentryList_isEmpty (p1) || uentryList_isEmpty (p2))
    {
      return (TRUE);
    }

  if (uentryList_size (p1) != uentryList_size (p2))
    {
      return FALSE;
    }

  for (index = 0; index < p1->nelements; index++)
    {
      cp1 = p1->elements[index];
      cp2 = p2->elements[index];

      /*
      ** Should compare uentry's --- need to fix report errors too.
      */

      if (!(cstring_equal (uentry_rawName (cp1), uentry_rawName (cp2))
	    && (ctype_almostEqual (uentry_getType (cp1), uentry_getType (cp2)))))
	{ 
	  return FALSE;
	}
    }

  return TRUE;
}
