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
** usymtab
**
** Lexically scoped symbol table.
**
** Unlike a normal symbol table, this table splits to reflect branches.
**
** This is necessary since symbol table entries also reflect
** some state information, like initialization, that may be
** different on different branches.
**
** For each control structure we split:
**
** { }                - a normal scope (no control) US_NORMAL
**
** if <pred> { }      - US_TBRANCH true branch
** while <pred> { }     US_FBRANCH else branch
**
** switch <pred> { }  - US_SWITCH
**
** case x:            - case scope US_CBRANCH
**                      Should be kept in a separate table, because of
**                      weird C syntax.
*/

# include "splintMacros.nf"
# include "basic.h"
# include "structNames.h"
# include "exprChecks.h"
# include "transferChecks.h"

/* Needed to install macros when loading libraries */

# include "cpplib.h"
# include "cpperror.h"
# include "cpphash.h"

/*
** Keep track of type definitions inside a function.
*/

static uentryList functypes = uentryList_undefined;

static bool dbgfree = FALSE;
static bool dbgload = TRUE;

/*@access ekind@*/

/*
** Hack to prevent shadow errors from appearing when function parameters
** are entered into a new scope.
*/

static bool noshadowerror = FALSE;

/*
** Constraint: static variables can live in 1 or > 2. 
**
** except for tags.  All tags must be at the global level.
** Static tags are handled in a kludgey way.
*/

/*
** utab    is the universal symbol table
** globtab is the global environment of utab
** oldtab  is needed for conversions in dumping (after sorting the table)
**
** there can be only one usymtab!
*/

static /*@checkedstrict@*/ /*@owned@*/ /*@notnull@*/ usymtab utab;

/* Reachable environments from from utab */
static /*@checkedstrict@*/ /*@notnull@*/ /*@dependent@*/ usymtab globtab;
static /*@checkedstrict@*/ /*@dependent@*/ usymtab filetab;

/* Used for sorting for dumpage. */
static /*@checkedstrict@*/ /*@owned@*/ usymtab oldtab;

static int usymtab_lexicalLevel (void) /*@globals utab@*/ ;
static bool usymtab_isAltDefinitelyNull (sRef p_s) /*@globals utab@*/ ;
static void refTable_free (/*@only@*/ /*@null@*/ refTable p_x, int p_nentries);
static ctype usymtab_suFieldsType (uentryList p_f, bool p_isStruct) /*@globals globtab@*/ ;

static void usymtab_freeAux (/*@only@*/ usymtab p_u)
     /*@globals globtab, utab, filetab@*/
     /*@modifies p_u@*/ ;

extern int usymtab_getCurrentDepth (void) /*@globals utab@*/ 
{
  return utab->lexlevel;
}

static void
usymtab_freeLevel (/*@notnull@*/ /*@only@*/ usymtab p_u)
  /*@globals globtab, utab, filetab@*/ /*@modifies p_u@*/ ;

static bool usymtab_isDefinitelyNullAux (sRef p_s) /*@globals utab@*/ ;
static /*@only@*/ cstring usymtab_unparseStackTab (usymtab p_t);
static /*@exposed@*/ /*@dependent@*/ uentry 
  usymtab_getRefTab (/*@notnull@*/ usymtab p_u, int p_level, usymId p_index);

# ifdef S_SPLINT_S
/* These are not used anymore... */
static /*@unused@*/ /*@only@*/ cstring 
  usymtab_unparseLocalAux (/*@notnull@*/ usymtab p_s);
static /*@unused@*/ /*@only@*/ cstring 
  usymtab_unparseLocalList (/*@notnull@*/ usymtab p_s);
# endif

static /*@only@*/ cstring usymtab_typeName (/*@notnull@*/ usymtab p_t);
static void usymtab_handleParams (void)
   /*@globals utab, globtab, filetab@*/ 
   /*@modifies utab@*/ ;

static /*@exposed@*/ /*@dependent@*/ uentry 
  usymtab_addRefEntry (/*@notnull@*/ usymtab p_s, cstring p_k);
static /*@exposed@*/ /*@dependent@*/ usymtab
  usymtab_dropEnv (/*@notnull@*/ usymtab p_s);
static /*@exposed@*/ /*@dependent@*/ uentry 
  usymtab_getRefNoisy (/*@notnull@*/ usymtab p_s, int p_level, usymId p_index);

static /*@exposed@*/ /*@dependent@*/ uentry 
  usymtab_lookupQuietAux (usymtab p_s, cstring p_k, bool p_noalt);

static /*@exposed@*/ /*@dependent@*/ uentry 
  usymtab_lookupQuiet (usymtab p_s, cstring p_k);

static /*@exposed@*/ /*@dependent@*/ uentry 
  usymtab_lookupQuietNoAlt (usymtab p_s, cstring p_k);

static void usymtab_printAllAux (usymtab p_s) /*@modifies g_warningstream@*/ ;

/*@function bool usymtab_indexFound (usymId) @*/
# define usymtab_indexFound(u) ((u) != usymId_notfound)

static usymId usymtab_getIndex (/*@notnull@*/ usymtab p_s, cstring p_k);
static /*@exposed@*/ uentry usymtab_fetchIndex (/*@notnull@*/ usymtab p_s, usymId p_ui);
static /*@exposed@*/ uentry usymtab_lookupAux (usymtab p_s, cstring p_k);

static /*@exposed@*/ /*@dependent@*/ /*@notnull@*/ 
   usymtab usymtab_getFileTab (void) /*@globals filetab@*/ ;

static int refTable_lookup (/*@notnull@*/ usymtab p_ut, int p_level, usymId p_index);
static bool usymtab_mustBreak (usymtab p_s);
static bool usymtab_mustEscape (usymtab p_s);

static void recordFunctionType (uentry ue)
{
  llassert (uentry_isDatatype (ue) || uentry_isAnyTag (ue)
	    || uentry_isEnumConstant (ue));
  DPRINTF (("Function type: %s", uentry_unparseFull (ue)));
  /*@-temptrans@*/
  functypes = uentryList_add (functypes, ue);
  /*@=temptrans@*/
}

static void clearFunctionTypes (void)
  /*@modifies globtab@*/
{
  uentryList_elements (functypes, el)
    {
      if (cstring_isDefined (uentry_rawName (el)))
	{
	  if (cstringTable_isDefined (globtab->htable))
	    {
	      cstringTable_remove (globtab->htable, uentry_rawName (el));
	    }

	  uentry_setName (el, cstring_undefined);
	}

      /*@access uentry@*/ 
      llassert (uentry_isValid (el));
      el->sref = sRef_undefined;
      /*@noaccess uentry@*/
    } end_uentryList_elements ;

  uentryList_clear (functypes);
}

static /*@falsewhennull@*/ bool usymtab_isBranch (usymtab u)
{
  return (usymtab_isDefined (u) && 
	  (u->kind == US_TBRANCH || u->kind == US_FBRANCH
	   || u->kind == US_CBRANCH || u->kind == US_SWITCH));
}

static bool usymtab_mustBreak (usymtab s)
{
  llassert (s != GLOBAL_ENV);
  return (s->mustBreak);
}

static bool usymtab_mustEscape (usymtab s)
{
  llassert (s != GLOBAL_ENV);
  return (exitkind_mustEscape (s->exitCode));
}

void usymtab_setMustBreak () 
  /*@modifies utab@*/
{
  llassert (utab != GLOBAL_ENV);
  utab->mustBreak = TRUE;
}

void usymtab_setExitCode (exitkind ex) 
   /*@modifies utab@*/
{
  llassert (utab != GLOBAL_ENV);

  utab->exitCode = ex;
  
  if (exitkind_mustEscape (ex))
    {
      utab->mustBreak = TRUE;
    }
}

bool usymtab_isAltDefinitelyNullDeep (sRef s)
{
  return (sRef_deepPred (usymtab_isAltDefinitelyNull, s));
}

static bool usymtab_isAltDefinitelyNull (sRef s) 
   /*@globals utab@*/
{
  guardSet t;
  bool res;

  t = utab->guards;

  llassert (utab->env != NULL);

  /*@-mods@*/ /* These modifications are cancelled. */
  utab->guards = utab->env->guards;
  utab->env->guards = t;
  /*@=mods@*/

  llassert (usymtab_isDefined (utab));
  res = usymtab_isDefinitelyNull (s);

  /*
  ** This reports a spurious error.  It is okay, because of
  ** the nesting relationship of usymtab environments.
  */

  /*@-mods@*/ /* Cancelling modifications. */
  /*@i1@*/ utab->env->guards = utab->guards;
  /*@i1@*/ utab->guards = t;
  /*@=mods@*/ 

  return res;
}

static /*@notnull@*/ /*@special@*/ usymtab
  usymtab_create (uskind kind, /*@keep@*/ usymtab env, bool nextlevel)
  /*@defines result@*/
  /*@post:isnull result->htable, result->guards, result->aliases@*/
{
  usymtab t = (usymtab) dmalloc (sizeof (*t));
  
  t->nentries = 0;
  t->nspace = CBASESIZE;
  t->entries = (uentry *) dmalloc (sizeof (*t->entries) * CBASESIZE);

  /* We only use a reftable for branch-level symbol tables. 
  */

  t->reftable = (nextlevel 
		 ? NULL
		 : (refentry *) dmalloc (sizeof (*t->reftable) * CBASESIZE));
  
  t->kind = kind;
  t->lexlevel = (env == GLOBAL_ENV ? 0 : env->lexlevel) + (nextlevel ? 1 : 0); 
  
  t->env = env;
  t->htable = NULL;

  t->guards = guardSet_undefined;
  t->aliases = aliasTable_undefined;

  t->mustBreak = FALSE;
  t->exitCode = XK_NEVERESCAPE;

  DPRINTF (("Create usymtab [%p]", t));
  return t;
}

/*
** constructors
*/

static /*@only@*/ /*@notnull@*/ usymtab
  usymtab_createRoot (void) /*@modifies nothing@*/
{
  usymtab u = (usymtab) dmalloc (sizeof (*u));
  
  u->nentries = 0;
  u->nspace = CGLOBBASESIZE;
  u->entries = (uentry *) dmalloc (sizeof (*u->entries) * CGLOBBASESIZE);
  u->env = GLOBAL_ENV;
  u->lexlevel = 0;
  u->htable = cstringTable_create (CGLOBHASHSIZE);
  u->reftable = NULL;
  
  u->guards = guardSet_new ();
  u->aliases = aliasTable_new ();

  u->mustBreak = FALSE;
  u->exitCode = XK_NEVERESCAPE;
  u->kind = US_NORMAL;

  return (u);
}

void
usymtab_initMod (void)
   /*@globals undef utab, undef oldtab, undef globtab, undef filetab @*/
   /*@modifies utab, globtab, filetab, oldtab @*/
{
  utab = usymtab_createRoot ();
  globtab = utab;
  filetab = usymtab_undefined;
  oldtab = usymtab_undefined;
}

void 
usymtab_destroyMod (void) /*@modifies utab, globtab, filetab@*/ /*@globals killed utab@*/ 
{
  DPRINTF (("Destroy usymtab [%p]: %d", utab, utab->nentries));
  usymtab_freeAux (utab);
  utab = usymtab_undefined;
  /*@-globstate@*/ 
} /*@=globstate@*/

void
usymtab_initGlobalMarker () /*@globals globtab@*/
{
  if (uentry_isValid (usymtab_lookupAux (globtab, GLOBAL_MARKER_NAME)))
    {
      ; /* Already entered from load table. */
    }
  else
    {
      usymtab_addGlobalEntry (uentry_makeGlobalMarker ());
    }
}

/*
** utab should be empty?  (requires?)
**
** Adds bool types to symbol table (these are built in for now)
** Only do this is there is no library!
*/

void
usymtab_initBool ()
{
  DPRINTF (("Init bool!"));

  if (context_getFlag (FLG_NOLIB))
    {
      ctype boolt = ctype_bool;
      /* evs 2000-07-24: bool is now treated as abstract (always) */

      uentry boolentry = uentry_makeBoolDatatype (qual_createAbstract ());
      usymtab_supGlobalEntry (boolentry);
      context_addBoolAccess ();

      /*
      ** We supply values 0 and 1 for the constants, so things like
      ** while (TRUE) can be interpreted correctly.
      */

      usymtab_supGlobalEntry 
	(uentry_makeConstantValue (context_getFalseName (), boolt, 
				   fileloc_getBuiltin (), FALSE, 
				   multiVal_makeInt (0)));

      usymtab_supGlobalEntry 
	(uentry_makeConstantValue (context_getTrueName (), boolt, 
				   fileloc_getBuiltin (), FALSE, 
				   multiVal_makeInt (1)));
    }
}

/*
** mutators 
*/

static void
usymtab_grow (/*@notnull@*/ usymtab s)
{
  int i;
  o_uentry *oldsyms = s->entries;

  s->nspace = CBASESIZE;
  s->entries = (uentry *) dmalloc (sizeof (*s->entries) 
				   * (s->nentries + s->nspace));

  for (i = 0; i < s->nentries; i++)
    {
      s->entries[i] = oldsyms[i];
    }

  sfree (oldsyms);

  if (s->reftable != NULL)
    {
      refTable oldRefs = s->reftable;

      s->reftable = (refentry *) dmalloc (sizeof (*s->reftable)
					  * (s->nentries + CBASESIZE + 1));
      
      for (i = 0; i < s->nentries; i++)
	{
	  s->reftable[i] = oldRefs[i];
	}

      /*@-compdestroy@*/ 
      sfree (oldRefs);
      /*@=compdestroy@*/ 
    }
      
}

static void
usymtab_addEntryQuiet (/*@notnull@*/ usymtab s, /*@keep@*/ uentry e)
{
  if (!(s->lexlevel > fileScope || !sRef_modInFunction ()))
    {
      if (uentry_isDatatype (e) 
	  || uentry_isFunction (e)
	  || uentry_isAnyTag (e)
	  || uentry_isEnumConstant (e)
	  || context_inMacro ())
	{
	  /* 
	  ** Not a bug.  Code like,
	  **    int f (void) { typedef int tint; ... }
	  ** is legal.
	  */
	  
	  /* was nothing here! */
	  /*@i@*/ e->sref = sRef_saveCopy (e->sref);
	}
      else
	{
	  llparseerror 
	    (cstring_makeLiteral ("Declaration outside function scope"));

	  llcontbug (message ("usymtab_addEntryQuiet: inconsistent state "
			      "(lexlevel = %d, modFunction = %s) adding: %q", 
			      s->lexlevel, bool_unparse (sRef_modInFunction ()), 
			      uentry_unparse (e)));
	  sRef_setGlobalScope ();
	  /* make sure the sRef is not bashed! */
	  /*@i@*/ e->sref = sRef_saveCopy (e->sref);
	}
    }

  if (s->nspace <= 0)
    {
      usymtab_grow (s);
    }
  
  s->nspace--;
  s->entries[s->nentries] = e;

# ifdef DOANNOTS
  if (s == globtab || s == filetab)
    {
      if (!fileloc_isLib (g_currentloc))
	{
	  uentry_tallyAnnots (e, AN_UNKNOWN);
	}
    }
# endif

  if (cstringTable_isDefined (s->htable))
    {
      cstringTable_insert (s->htable, cstring_copy (uentry_rawName (e)), s->nentries);
    }

  s->nentries++;
}

static /*@observer@*/ uentry /*@alt void@*/
usymtab_addEntryBase (/*@notnull@*/ usymtab s, /*@only@*/ uentry e)
{
  /* 
  ** In theory, we shouldn't need this test because it this is
  ** only called when a library is being read, and it shouldn't
  ** ever have a duplicate entry.  In practice, its safer to
  ** leave it in, though.
  */

  uentry old;

  if (uentry_isValid (old = usymtab_lookupQuiet (s, uentry_rawName (e))))
    {
      DPRINTF (("Duplicate entry: %s", uentry_unparse (e)));
      uentry_free (e); /* duplicate */
      return old;
    }
  else
    {
      usymId thisentry = usymId_fromInt (s->nentries); 
      
      if (uentry_isVar (e))
	{
	  uentry_setSref 
	    (e, sRef_makeCvar (globScope, thisentry, 
			       uentry_getType (e),
			       stateInfo_makeLoc (uentry_whereLast (e), SA_DECLARED)));
	}
      
      usymtab_addEntryQuiet (s, e);
      return e;
    }
}


static /*@observer@*/ uentry /*@alt void@*/
usymtab_addEntryAlways (/*@notnull@*/ usymtab s, /*@only@*/ uentry e)
{
  /* 
  ** In theory, we shouldn't need this test because it this is
  ** only called when a library is being read, and it shouldn't
  ** ever have a duplicate entry.  In practice, its safer to
  ** leave it in, though.
  */

  uentry old;
  usymId thisentry = usymId_fromInt (s->nentries);

  if (uentry_isValid (old = usymtab_lookupQuiet (s, uentry_rawName (e))))
    {
      llcontbug 
	(message ("Duplicate entry in load library: %s. "
		  "Old entry: %q.  New entry: %q", 
		  uentry_rawName (e),
		  uentry_unparseFull (old),
		  uentry_unparseFull (e)));

      uentry_setName (e, message ("__x_%s", uentry_rawName (e)));
      /* This shouldn't happen...unless the library is bad! */
    }


  if (uentry_isVar (e) && !uentry_isGlobalMarker (e))
    {
      uentry_setSref 
	(e, sRef_makeCvar (globScope, thisentry, 
			   uentry_getType (e),
			   stateInfo_makeLoc (uentry_whereLast (e), SA_DECLARED)));
    }
  
  usymtab_addEntryQuiet (s, e);
  return e;
}

static usymId
usymtab_addEntryAux (/*@notnull@*/ usymtab st, /*@keep@*/ uentry e, bool isSref)
     /*@globals globtab@*/
     /*@modifies st, e@*/
{
  usymId thisentry = usymId_fromInt (st->nentries);

  llassert (!uentry_isElipsisMarker (e));

  /*
  ** not true for tags
  **  llassert (usymtab_lookupSafe (uentry_rawName (e)) == uentry_undefined);
  **/

  llassertprint (uentry_isAnyTag (e) ? st == globtab : TRUE, 
		 ("non-global tag: %s / %d", uentry_unparseFull (e), st->lexlevel));

  if (st->lexlevel == fileScope 
      && (!(uentry_isStatic (e)) || uentry_isAnyTag (e))) 
    {
      st = globtab;
    }

  if (isSref)
    {
      ctype ct = uentry_getType (e);

      if (uentry_isFunction (e) && ctype_isFunction (ct))
	{
	  ct = ctype_getReturnType (ct);
	}

      if (uentry_isStatic (e))
	{
	  sRef sr = sRef_makeCvar (st->lexlevel, thisentry, ct,
				   stateInfo_makeLoc (uentry_whereLast (e), SA_DECLARED));

	  if (sRef_isStack (sr) || sRef_isLocalState (sr))
	    {
	      sRef_setAliasKind (sr, AK_STATIC, uentry_whereLast (e));
	      sRef_setDefined (sr, uentry_whereLast (e));
	    }
	  
	  uentry_setSref (e, sr);
	}
      else
	{
	  uentry_setSref 
	    (e, sRef_makeCvar (st->lexlevel, thisentry, ct,
			       stateInfo_makeLoc (uentry_whereLast (e), SA_DECLARED)));
	}
    }

  if (uentry_isDatatype (e))
    {
      uentry_setDatatype (e, typeId_fromUsymId (thisentry));
    }

  if (uentry_isFunction (e))
    {
      ctype ct = uentry_getType (e);

      if (ctype_isFunction (ct)
	  && uentryList_isMissingParams (ctype_argsFunction (ct)))
	{
	  if (uentry_isDeclared (e))
	    {
	      voptgenerror 
		(FLG_NOPARAMS,
		 message ("Function %q declared without parameter list",
			  uentry_getName (e)),
		 uentry_whereDeclared (e));
	    }
	  else
	    {
	      voptgenerror
		(FLG_NOPARAMS,
		 message ("Function %q specified without parameter list",
			  uentry_getName (e)),
		 uentry_whereSpecified (e));
	    }
	}
    }

  if (st == globtab && !uentry_isSpecified (e))
    {
      exprChecks_checkExport (e);
    }
  
  uentry_checkName (e);
  
  usymtab_addEntryQuiet (st, e);
  DPRINTF (("Adding entry: [%p] %s", e, uentry_unparseFull (e)));
  return (thisentry);
}

usymId
usymtab_addEntry (uentry e) 
   /*@globals utab, globtab@*/
   /*@modifies utab, e@*/
{  
  llassertprint (!usymtab_exists (uentry_rawName (e)),
		 ("Entry already exists: %s", uentry_unparse (e)));

  return usymtab_addEntryAux (utab, e, FALSE);
}

void
usymtab_addGlobalEntry (uentry e)
   /*@modifies globtab, e@*/ 
{
  DPRINTF (("Add global: %s / %s", uentry_unparse (e), bool_unparse (uentry_isForward (e))));

  (void) usymtab_addEntryAux (globtab, e, FALSE);
}

/*
** supercede and replace entry use elements of entries array, and
** assign an element to a new value, and then free the old value.
** Need -deparrays to avoid errors for this. 
*/

/*@-deparrays@*/ 

static usymId
usymtab_supEntryAux (/*@notnull@*/ usymtab st, 
		     /*@only@*/ uentry e, bool isSref)
   /*@globals globtab, filetab@*/
   /*@modifies st, globtab, e@*/
{
  cstring ename = uentry_rawName (e);
  bool staticEntry = FALSE;
  usymId eindex;

  DPRINTF (("Sup entry aux: %s", uentry_unparseFull (e)));
  
  /* static tags in global scope */
  if (st->lexlevel == fileScope 
      && (!(uentry_isStatic (e)) || uentry_isAnyTag (e))) 
    {
      eindex = usymtab_getIndex (st, ename);

      if (usymtab_indexFound (eindex))
	{
	  uentry ce = st->entries[eindex];      
	  
	  if (optgenerror
	      (FLG_SHADOW,
	       message ("%s %q shadows static declaration",
			ekind_capName (uentry_getKind (e)),
			uentry_getName (e)),
	       uentry_whereDeclared (e)))
	    {
	      uentry_showWhereLast (ce);
	    }

	  if (eindex == usymId_fromInt (st->nentries - 1))
	    {
	     ;
	    }
	  else
	    {
	      st->entries[eindex] = st->entries[st->nentries - 1];
	    }

	  if (cstringTable_isDefined (st->htable))
	    {
	      cstringTable_replaceKey (st->htable, uentry_rawName (ce), 
				       cstring_copy (uentry_rawName (e)));
	    }

	  uentry_free (ce);
	  st->nentries--;
	}
      
      st = globtab;
    }

  if (uentry_isStatic (e)) {
    if (uentry_isFunction (e)) {
      /* 
      ** Static function declarations are at the file level,
      ** even if they are in a deeper scope. 
      */

      st = usymtab_getFileTab ();
      staticEntry = TRUE;
    } else {
      if (!uentry_isAnyTag (e) && st == globtab) 
	{
	  st = usymtab_getFileTab ();
	  staticEntry = TRUE;
	  DPRINTF (("Static entry!"));
	}
    }
  }

  eindex = usymtab_getIndex (st, ename);
      
  if (usymtab_indexFound (eindex))
    {
      uentry ce = st->entries[eindex];
      
      DPRINTF (("Found entry: %s", uentry_unparse (ce)));

      if (uentry_isPriv (ce)
	  /* || (fileloc_isImport (uentry_whereSpecified (ce))) */
	  || (uentry_isStatic (ce) 
	      && uentry_isAnyTag (ce)
	      && (uentry_isDeclared (ce) 
		  && !fileloc_sameFile (uentry_whereDefined (ce),
					uentry_whereDefined (e)))))
	{
	  /*
          ** overload entry 
	  **    if overloading import should do some checks!
	  */

	  llassert ((st->lexlevel > fileScope || !sRef_modInFunction ()));
	  
	  DPRINTF (("Overloading!"));

	  st->entries[eindex] = e;

	  if (uentry_isDatatype (e))
	    {
	      uentry_setDatatype (e, typeId_fromUsymId (eindex));
	    }
	  
	  if (st == globtab && !uentry_isSpecified (e))
	    {
	      exprChecks_checkExport (e);
	    }

	  if (cstringTable_isDefined (st->htable))
	    {
	      cstringTable_replaceKey (st->htable, uentry_rawName (ce), 
				       cstring_copy (uentry_rawName (e)));
	    }

	  uentry_free (ce);
	  ce = e; 
	}
      else if (uentry_isSpecified (ce))
	{
	  if (uentry_isSpecified (e))
	    {
	      DPRINTF (("Here we are: %s", uentry_unparseFull (e)));

	      if (fileloc_isImport (uentry_whereSpecified (ce)))
		{		  
		  if (cstringTable_isDefined (st->htable))
		    {
		      cstringTable_replaceKey (st->htable, 
					       uentry_rawName (ce), 
					       cstring_copy (uentry_rawName (e)));
		    }
		  
		  uentry_free (ce); 
		  st->entries[eindex] = e;
		  ce = e;

		  if (uentry_isDatatype (e)) uentry_setDatatype (e, typeId_fromUsymId (eindex));
		}
	      else 
		{
		  if (fileloc_isImport (uentry_whereSpecified (e)))
		    {
		      uentry_free (e);
		    }
		  else
		    {
		      /* Respecification errors already reported */
		      DPRINTF (("Respecification: %s / %s", 
				fileloc_unparse (uentry_whereSpecified (e)),
				bool_unparse (fileloc_isSpec (uentry_whereSpecified (e)))));

		      if (uentry_isDatatype (e)) 
			{
			  uentry_setDatatype (e, typeId_fromUsymId (eindex));
			}
		      
		      if (cstringTable_isDefined (st->htable))
			{
			  cstringTable_replaceKey (st->htable, 
						   uentry_rawName (ce), 
						   cstring_copy (uentry_rawName (e)));
			}
		      
		      llassert ((st->lexlevel > fileScope || !sRef_modInFunction ()));
		      uentry_free (ce);
		      st->entries[eindex] = e;
		      ce = e;
		    } 
		}
	    }
	  else /* e not specified */
	    {
	      DPRINTF (("Merging..."));

	      if (uentry_isDeclared (ce))
		{
		  /* evans 2001-08-26
		    No - this can happen for internal typedefs
		    llassert ((st->lexlevel > fileScope || !sRef_modInFunction ()));
		  */

		  DPRINTF (("Merge defn"));
		  uentry_mergeDefinition (ce, e);
		}
	      else 
		{
		  /* evans 2001-08-26
		    No - this can happen for internal typedefs
		    llassert ((st->lexlevel > fileScope || !sRef_modInFunction ()));
		  */

		  DPRINTF (("Merge entries..."));
		  uentry_mergeEntries (ce, e);
		  DPRINTF (("After: %s", uentry_unparseFull (ce)));
		}
	    }
	}
      else /* ce not specified */
	{
	  if (!(st->lexlevel > fileScope || !sRef_modInFunction ()))
	    {
	      if (uentry_isDatatype (e) || uentry_isAnyTag (e)
		  || uentry_isEnumConstant (e)
		  || uentry_isStatic (e)) /* bug fix from Brian St. Pierre */
		{
		  ; /* 
		     ** Not a bug.  Code like,
		     **    int f (void) { typedef int tint; ... }
		     ** is legal.
		     */
		}
	      else
		{
		  llcontbug (message ("usymtab_supEntryAux: inconsistent state "
				      "(lexlevel = %d, modFunction = %s) adding: %q", 
				      st->lexlevel, bool_unparse (sRef_modInFunction ()), 
				      uentry_unparse (e)));

		  if (sRef_modInFunction ())
		    {
		      /* make sure the sRef is not bashed! */
		      /*@i@*/ e->sref = sRef_saveCopy (e->sref);
		    }
		}
	    }
	  
	  DPRINTF (("Merge.."));
	  uentry_mergeDefinition (ce, e);
	}
      
      if (isSref)
	{
	  ctype ct = uentry_getType (ce);
	  
	  if (uentry_isFunction (ce) && ctype_isFunction (ct))
	    {
	      ct = ctype_getReturnType (ct);
	    }
	  
	  uentry_setSref
	    (ce, sRef_makeCvar (st->lexlevel, eindex, ct,
				stateInfo_makeLoc (uentry_whereLast (ce), SA_DECLARED)));
	}
    }
  else /* no previous entry */
    {
      uentry outer;

      if (st->lexlevel == globScope 
	  && !uentry_isStatic (e)
	  && !uentry_isExtern (e)
	  && usymtab_isDefined (filetab))
	{
	  /*
	  ** check if there is a static entry:  (i.e.,
	  **   
	  **    static int f ();
	  **    ...
	  **    int f (); 
	  */
	  
	  eindex = usymtab_getIndex (filetab, ename);
	  
	  if (usymtab_indexFound (eindex))
	    {
	      uentry ce = filetab->entries[eindex];

	      uentry_setStatic (e);
	      uentry_mergeDefinition (ce, e);
	      staticEntry = TRUE;
	      goto exitPoint;
	    }
	}
      
      outer = usymtab_lookupQuiet (st->env, ename);

      DPRINTF (("New  : [%p] %s", e, uentry_unparseFull (e)));
      DPRINTF (("Outer: [%p] %s", outer, uentry_unparseFull (outer)));

      /*
      ** no previous definition, add the new one
      */
      
      if (!noshadowerror 
	  && uentry_isValid (outer)
	  && !(uentry_isYield (e) || uentry_isYield (outer))
	  && fileloc_isDefined (uentry_whereLast (e))
	  && !fileloc_isXHFile (uentry_whereLast (e))
	  && fileloc_isDefined (uentry_whereLast (outer))
	  && !fileloc_isXHFile (uentry_whereLast (outer)))
	{
	  if (!uentry_sameKind (outer, e))
	    {
	      ; /* no error */
	    }
	  else
	    {
	      if (ctype_isUnknown (uentry_getType (outer))
		  || uentry_isForward (outer))
		{
		  ;
		}
	      else
		{
		  if (optgenerror 
		      (FLG_SHADOW,
		       message ("%s %q shadows outer declaration",
				ekind_capName (uentry_getKind (e)),
				uentry_getName (e)),
		       uentry_whereLast (e)))
		    {
		      uentry_showWhereLast (outer);
		    }
		}
	    }
	}
      
      if (st == globtab && context_getFlag (FLG_NEWDECL))
	{
	  voptgenerror 
	    (FLG_NEWDECL,
	     message ("New declaration: %q", uentry_getName (e)),
	     uentry_whereLast (e));
	}

      eindex = usymtab_addEntryAux (st, e, isSref);
    }

 exitPoint:
    return (staticEntry ? usymId_invalid : eindex);
}

static void
usymtab_replaceEntryAux (/*@notnull@*/ usymtab st, /*@only@*/ uentry e)
   /*@globals globtab@*/ /*@modifies st, e@*/
{
  cstring ename = uentry_rawName (e);
  usymId eindex;

  /* static tags in global scope */
  eindex = usymtab_getIndex (st, ename);
  
  if (usymtab_indexFound (eindex))
    {
      uentry ce = st->entries[eindex];      
      
      if (cstringTable_isDefined (st->htable))
	{
	  cstringTable_replaceKey (st->htable, uentry_rawName (ce), 
				   cstring_copy (uentry_rawName (e)));
	}

      uentry_free (ce);
      st->entries[eindex] = e;
    }
  else
    {
      eindex = usymtab_addEntryAux (st, e, FALSE);
    }
}

/*@=deparrays@*/ 

void usymtab_supEntry (uentry e)
   /*@globals utab, filetab, globtab@*/
   /*@modifies utab, globtab, e@*/
{
    (void) usymtab_supEntryAux (utab, e, FALSE);
}

/*
** this should be lots more efficient!
*/

static /*@exposed@*/ uentry 
  usymtab_supEntryReturnAux (/*@notnull@*/ usymtab tab, 
			     /*@only@*/ uentry e, bool isref)
  /*@globals globtab, filetab@*/
  /*@modifies tab, globtab, e@*/
{
  cstring rawName = cstring_copy (uentry_rawName (e));
  bool stat = (tab == globtab) && uentry_isStatic (e);
  uentry ret;

  (void) usymtab_supEntryAux (tab, e, isref);

  if (stat)
    {
      ret = usymtab_lookupAux (filetab, rawName);
    }
  else
    {
      ret = usymtab_lookupAux (tab, rawName);

      if (uentry_isInvalid (ret) && usymtab_isDefined (filetab))
	{
	  ret = usymtab_lookupAux (filetab, rawName);
	}
    }
  
  cstring_free (rawName);
  return ret;
}

/*@dependent@*/ /*@exposed@*/ uentry 
  usymtab_supEntryReturn (/*@only@*/ uentry e)
  /*@globals utab, filetab, globtab@*/
  /*@modifies utab, globtab, e@*/
{
  return (usymtab_supEntryReturnAux (utab, e, FALSE));
}

/*@dependent@*/ /*@exposed@*/ uentry 
  usymtab_supEntrySrefReturn (/*@only@*/ uentry e)
  /*@globals utab, globtab, filetab@*/
  /*@modifies utab, globtab, e@*/
{
  return (usymtab_supEntryReturnAux (utab, e, TRUE));
}

/*@dependent@*/ /*@exposed@*/ uentry 
  usymtab_supGlobalEntryReturn (uentry e)
  /*@globals globtab, filetab@*/
  /*@modifies globtab, e@*/
{
  uentry ret;

  ret = usymtab_supEntryReturnAux (globtab, e, FALSE);

  /*
  ** We need to keep track of internal function declarations, so
  ** we can remove them from the symbol table after exiting this
  ** function.  This is a bit bogus, of course.
  */

  if (sRef_modInFunction ())
    {
      recordFunctionType (ret);
    }

  return (ret);
}

ctype
usymtab_supTypeEntry (/*@only@*/ uentry e)
  /*@globals globtab, filetab@*/
  /*@modifies globtab, e@*/
{
  usymId uid;
  ctype ret;

  if (uentry_isAbstractDatatype (e))
    {
      typeId tid = usymtab_supAbstractTypeEntry (e, FALSE);
      ret = ctype_createAbstract (tid);
      uid = typeId_toUsymId (tid);
    }
  else
    {
      uid = usymtab_supEntryAux (globtab, e, FALSE);
      ret = ctype_createUser (typeId_fromUsymId (uid));
    }

  if (sRef_modInFunction ())
    {
      recordFunctionType (globtab->entries[uid]);
    }

  return ret;
}

uentry
usymtab_supReturnTypeEntry (/*@only@*/ uentry e)
  /*@globals globtab, filetab@*/
  /*@modifies globtab@*/
{
  usymId uid;

  DPRINTF (("Abstract? %s", uentry_unparseFull (e)));

  if (uentry_isAbstractDatatype (e))
    {
      uid = typeId_toUsymId (usymtab_supAbstractTypeEntry (e, FALSE));
    }
  else if (uentry_isMaybeAbstract (e) && context_getFlag (FLG_IMPABSTRACT))
    {
      bool maybeabs = TRUE;
      cstring sname = uentry_getName (e);
      uentry ue = usymtab_lookupGlobSafe (sname);
      cstring_free (sname);

      if (uentry_isValid (ue)) 
	{
	  DPRINTF (("Lookup: %s", uentry_unparseFull (ue)));

	  if (uentry_isDatatype (ue)) 
	    {
	      if (uentry_isMaybeAbstract (ue))
		{
		  ;
		}
	      else
		{
		  maybeabs = FALSE;
		}
	    }
	  else
	    {
	      DPRINTF (("Not datatype!"));
	    }
	}
      
      if (maybeabs)
	{
	  uentry ux;
	  typeId tid = usymtab_supAbstractTypeEntry (e, FALSE);
	  ux = usymtab_getTypeEntry (tid);
	  uentry_setAbstract (ux);
	  uid = typeId_toUsymId (tid);
	}
      else
	{
	  uid = usymtab_supEntryAux (globtab, e, FALSE);
	  e = usymtab_getTypeEntry (typeId_fromUsymId (uid));
	  
	  if (uentry_isMaybeAbstract (e))
	    {
	      uentry_setConcrete (e);
	    }
	}
    }
  else
    {
      uid = usymtab_supEntryAux (globtab, e, FALSE);
      e = usymtab_getTypeEntry (typeId_fromUsymId (uid));

      /*? evans 2002-12-16 removed this? it doesn't make sense
      if (uentry_isMaybeAbstract (e))
	{
	  uentry_setConcrete (e);
	}
      */
    }
  
  if (sRef_modInFunction ())
    {
      recordFunctionType (globtab->entries[uid]);
    }

    return (globtab->entries[uid]);
}

typeId
usymtab_supAbstractTypeEntry (/*@only@*/ uentry e, bool dodef)
  /*@globals globtab, filetab@*/
  /*@modifies globtab, e@*/
{
  typeId uid;
  uentry ue;

  uid = typeId_fromUsymId (usymtab_supEntryAux (globtab, e, FALSE));
  ue = usymtab_getTypeEntry (uid);

  if (dodef)
    {
      uentry_setDatatype (ue, uid);
    }

  if (context_getFlag (FLG_ACCESSMODULE)) /* was accessfile */
    {
      context_addFileAccessType (uid);
    }

  if (sRef_modInFunction ())
    {
      recordFunctionType (globtab->entries[uid]);
    }

  return (uid);
}

typeId
usymtab_supExposedTypeEntry (/*@only@*/ uentry e, bool dodef)
  /*@globals globtab, filetab@*/
  /*@modifies globtab, e@*/
{
  typeId uid;

  uid = typeId_fromUsymId (usymtab_supEntryAux (globtab, e, FALSE));

  if (dodef)
    {
      uentry ue = usymtab_getTypeEntry (uid);

      uentry_setDatatype (ue, uid);
    }

  if (sRef_modInFunction ())
    {
      recordFunctionType (globtab->entries[uid]);
    }

  return uid;
}

ctype
usymtab_supForwardTypeEntry (/*@only@*/ uentry e)
  /*@globals globtab, filetab@*/
  /*@modifies globtab, e@*/
{
  typeId uid = typeId_fromUsymId (usymtab_supEntryAux (globtab, e, FALSE));
  uentry ue = usymtab_getTypeEntry (uid);

    uentry_setDatatype (ue, uid);

  if (sRef_modInFunction ())
    {
      recordFunctionType (globtab->entries[uid]);
    }

  return (uentry_getAbstractType (ue));
}

void
  usymtab_supEntrySref (uentry e)
  /*@globals utab, globtab, filetab@*/
  /*@modifies utab, globtab, e@*/
{
  sRef old = uentry_getSref (e);
  
  if (sRef_isType (old))
    {
      uentry ue = usymtab_supEntryReturnAux (utab, e, TRUE);

      /*@access uentry@*/
      if (uentry_isValid (ue)) 
	{
	  sRef uref = uentry_getSref (ue);

	  sRef_mergeStateQuiet (uref, old); 
	  sRef_clearDerived (uref);
	}
      /*@noaccess uentry@*/
    }
  else if (sRef_isKnown (old))
    {
      usymtab_supEntry (e);
    }
  else
    {
      (void) usymtab_supEntryAux (utab, e, TRUE);
    }
}

void usymtab_supGlobalEntry (/*@only@*/ uentry e)
  /*@globals globtab, filetab@*/
  /*@modifies globtab, filetab, e@*/
{
  usymId uid;

  DPRINTF (("Sup global entry: %s", uentry_unparse (e)));

  uid = usymtab_supEntryAux (globtab, e, FALSE);

  if (sRef_modInFunction ())
    {
      recordFunctionType (globtab->entries[uid]);
    }
}

uentry
  usymtab_supReturnFileEntry (/*@only@*/ uentry e)
  /*@globals filetab, globtab@*/
  /*@modifies filetab, globtab, e@*/
{
  llassert (filetab != usymtab_undefined);
  DPRINTF (("File entry: %s", uentry_unparse (e)));
  return (usymtab_supEntryReturnAux (filetab, e, FALSE));
}

/*
** observers
*/

bool
usymtab_inDeepScope () /*@globals utab@*/
{
  return (utab->lexlevel > paramsScope);
}

static usymId
usymtab_getIndex (/*@notnull@*/ usymtab s, cstring k)
{
  int i;

  DPRINTF (("Lookup %s", k));

  if (cstringTable_isDefined (s->htable))
    {
      i = cstringTable_lookup (s->htable, k);
      return usymId_fromInt (i);
    }
  else
    {
      for (i = 0; i < s->nentries; i++)
	{
	  uentry current = s->entries[i];

	  DPRINTF (("Check %d: %s", i, uentry_rawName (current)));

	  if (!uentry_isUndefined (current) 
	      && cstring_equal (uentry_rawName (current), k))
	    {
	      return usymId_fromInt (i);
	    }
	}

      return usymId_notfound;
    }
}

static uentry
usymtab_fetchIndex (/*@notnull@*/ usymtab s, usymId ui)
{
  int i = usymId_toInt (ui);
  llassert (i >= 0 && i < s->nentries);
  return (s->entries[i]);
}

typeId
usymtab_getTypeId (cstring k) /*@globals globtab@*/
{
  usymId uid = usymtab_getIndex (globtab, k);

  if (!usymtab_indexFound (uid)
      || !(uentry_isDatatype (usymtab_getTypeEntry (typeId_fromUsymId (uid)))))
    
    {
      return typeId_invalid;
    }
  else
    {
      return typeId_fromUsymId (uid);
    }
}

/*@dependent@*/ uentry
usymtab_lookupStructTag (cstring k)
{
  cstring sname = makeStruct (k);
  uentry ue = usymtab_lookupGlob (sname);
  
  cstring_free (sname);
  return (ue);
}

/*@dependent@*/ uentry
usymtab_lookupUnionTag (cstring k)
{
  cstring uname = makeUnion (k);
  uentry res = usymtab_lookupGlob (uname);

  cstring_free (uname);
  return res;
}

/*@dependent@*/ uentry
usymtab_lookupEnumTag (cstring k)
{
  cstring ename = makeEnum (k);
  uentry res = usymtab_lookupGlob (ename);

  cstring_free (ename);
  return res;
}

usymId
usymtab_getId (cstring k) /*@globals globtab@*/
{
  usymId uid = usymtab_getIndex (globtab, k);
  uentry ue;

  if (!usymtab_indexFound (uid))
    {
      return usymId_invalid;
    }

  ue = usymtab_getGlobalEntry (uid);

  if (uentry_isPriv (ue))
    {
      return usymId_invalid;
    }

  return uid;
}

static /*@exposed@*/ uentry 
usymtab_getEntryAux (/*@notnull@*/ usymtab s, usymId uid)
{
  llassert (uid != usymId_invalid);
 
  if (uid < 0 || uid >= usymId_fromInt (s->nentries))
    {
      llcontbug (message ("usymtab_getEntry: out of range: level = %d [%d]",
			  s->lexlevel, uid));
      return uentry_undefined;
    }

  llassertprint (uentry_isValid (s->entries[uid]),
		 ("entry undefined: %d", uid));

  return s->entries[uid];
}

/*@dependent@*/ /*@observer@*/ uentry 
  usymtab_getGlobalEntry (usymId uid)
  /*@globals utab, globtab@*/
{
  if (dbgfree) return (uentry_undefined);
  
  if (utab->lexlevel > paramsScope)
    {
      /* need to do this the awkward way, since it could be in conditional scope */
     return (usymtab_lookupSafe (uentry_rawName (globtab->entries[uid])));
    }
  else
    {
      return (globtab->entries[uid]);
    }
}

/*@dependent@*/ /*@exposed@*/ uentry usymtab_getTypeEntry (typeId uid)
  /*@globals globtab@*/
{
  if (dbgload)
    {
      if (uid >= 0 && uid < typeId_fromInt (globtab->nentries))
	{
	  return (globtab->entries[uid]);
	}
      else
	{
	  return (uentry_undefined);
	}
    }
  else
    {
      llassert (uid >= 0 && uid < typeId_fromInt (globtab->nentries));
      return (globtab->entries[uid]);
    }
}

/*
** in load files
*/

/*@dependent@*/ /*@exposed@*/ uentry usymtab_getTypeEntrySafe (typeId uid)
  /*@globals globtab@*/
{
  if (uid < 0 || uid >= typeId_fromInt (globtab->nentries))
    {
      return uentry_undefined;
    }
  
  return (globtab->entries[uid]);
}

bool usymtab_isBoolType (typeId uid)
  /*@globals globtab@*/
{
  llassert (uid >= 0 && uid < typeId_fromInt (globtab->nentries));

  return (cstring_equal (uentry_rawName (globtab->entries[uid]),
			 context_getBoolName ()));
}
 
cstring usymtab_getTypeEntryName (typeId uid)
   /*@globals globtab@*/
{
  uentry ue;

  if (dbgfree)
    {
      return (cstring_makeLiteral ("<freetype>"));
    }

  ue = usymtab_getTypeEntry (uid);

  if (dbgload && !uentry_isValid (ue))
    {
      return (message ("<missing type: %d>", uid));
    }

  llassertprint (uentry_isValid (ue), ("type undefined: %d", uid));

  return (uentry_getName (ue));
}

# if 0
/*@unused@*/ static void
usymtab_rehash (/*@notnull@*/ usymtab s)
{
  int i;
  
  if (cstringTable_isDefined (s->htable))
    {
      cstringTable_free (s->htable);
    }
  
  s->htable = cstringTable_create (LLAHSHSIZE);

  for (i = 0; i < s->nentries; i++)
    {
      cstringTable_insert (s->htable, cstring_copy (uentry_rawName (s->entries[i])), i);
    }
}
# endif

/*
** superficial copy of usymtab
**
** DO copy spec entries 
*/

static /*@only@*/ /*@notnull@*/ usymtab
usymtab_shallowCopy (/*@notnull@*/ usymtab s) /*@*/
{
  usymtab copytab = usymtab_createRoot ();
  int i;

  for (i = 0; i < s->nentries; i++)
    {
      usymtab_addEntryBase (copytab, s->entries[i]);
    }

  return copytab;
}

static void
usymtab_shallowFree (/*@only@*/ /*@notnull@*/ usymtab s)
{
  aliasTable_free (s->aliases);
  refTable_free (s->reftable, s->nentries);
  sfree (s->entries);
  /*@-compdestroy@*/ sfree (s); /*@=compdestroy@*/
}

usymId usymtab_convertTypeId (typeId uid)
{
  return usymtab_convertId (typeId_toUsymId (uid));
}

/*
** usymtab_convertId: converts usymId from old table to sorted one
*/

usymId usymtab_convertId (usymId uid)
  /*@globals oldtab, utab@*/
{
  uentry ue;
  usymId ret;
  cstring name;

  llassert (usymtab_isDefined (oldtab));

  ue = usymtab_getEntryAux (oldtab, uid);

  llassertprint (uentry_isValid (ue), ("convertId: undefined: %d", uid));

  name = uentry_rawName (ue);

  ret = usymtab_getIndex (utab, name);
  llassert (ret == uid); /*! for now, no rehash! */
  DPRINTF (("Convert: %s [%d] -> %s [%d]",
	    uentry_unparse (ue), uid,
	    uentry_unparse (utab->entries[ret]), ret));

  llassertprint (ret != usymId_invalid, ("convertId: return is invalid"));

  return (ret);
}

void
usymtab_prepareDump (void)
   /*@globals oldtab, utab@*/
   /*@modifies oldtab, utab@*/
{
  llassert (usymtab_inGlobalScope ());
  llassert (oldtab == usymtab_undefined);

  /*
  DPRINTF (("Preparing to dump:"));
  usymtab_printAll ();
  */

  oldtab = usymtab_shallowCopy (utab);

  /* 
  ** alpha compare - make sure order is same on different platforms
  ** error messages appear in same order 
  */

  /*
  qsort (utab->entries, (size_t)utab->nentries, 
	 sizeof (*utab->entries), 
	 (int (*)(const void *, const void *)) uentry_xcomparealpha);
  
  usymtab_rehash (utab);
  */

  /*
  DPRINTF (("After rehash:"));
  usymtab_printAll ();
  */
}

void usymtab_dump (FILE *fout)
     /*@globals utab, oldtab@*/
{
  int i;
  bool neednl = FALSE;
  uentry lastentry = uentry_undefined;
  ekind lastekind = KINVALID;
  int linelen = 0;

  /*
  ** must call prepareDump first
  */

  llassert (oldtab != usymtab_undefined);

  for (i = 0; i < utab->nentries; i++)
    {
      uentry thisentry = utab->entries[i];
      ekind  thisekind = uentry_getKind (thisentry);

      if (!uentry_hasRealName (thisentry))
	{
	  llassert (uentry_isGlobalMarker (thisentry));

	  if (neednl)
	    {
	      check (fputc ('\n', fout) == (int) '\n');
	    }
	  
	  fprintf (fout, "*%d (GlobalMarker)\n", KGLOBALMARKER);
	  lastekind = KINVALID;
	  linelen = 0;
	  neednl = FALSE;
	}
      else
	{
	  if (thisekind != lastekind)
	    {
	      if (neednl)
		{
		  check (fputc ('\n', fout) == (int) '\n');
		}
	      
	      neednl = FALSE;
	      lastentry = uentry_undefined;
	      fprintf (fout, "*%d (%s)\n", ekind_toInt (thisekind),  
		       cstring_toCharsSafe (ekind_capName (thisekind)));
	      lastekind = thisekind;
	      linelen = 0;
	    }
	  
	  /*
	  ** evans - 2001-02-18 - added the - 48 fudge factor...
	  ** extra characters dumped, but I haven't counded them carefully... 
	  */
	  
	  if (uentry_isInvalid (lastentry) || !uentry_equiv (lastentry, thisentry)
	      || (linelen > (MAX_DUMP_LINE_LENGTH - (2 * MAX_NAME_LENGTH) - 48))) 
	    {
	      cstring cdump;
	      
	      DPRINTF (("Dumping entry: %d", i));
	      cdump = message ("^%d %q", i, uentry_dump (thisentry));
	      /* was: cdump = uentry_dump (thisentry)); */
	      
	      lastentry = thisentry;
	      if (neednl)
		{
		  check (fputc ('\n', fout) == (int) '\n');
		  linelen = 0;
		}
	      
	      linelen += cstring_length (cdump);
	      
	      /* no new line here! */
	      if (cstring_length (cdump) > 0) 
		{
		  check (fputs (cstring_toCharsSafe (cdump), fout) != EOF);
		}
	      
	      cstring_free (cdump);
	      neednl = TRUE;
	    }
	  else
	    {
	      cstring cdump = uentry_rawName (thisentry);
	      DPRINTF (("Raw name: %s", cdump));
	      linelen += (cstring_length (cdump) + 1);
	      fprintf (fout, "#%s", cstring_toCharsSafe (cdump));
	    }
	}
    }

  if (neednl)
    {
      check (fputc ('\n', fout) == (int) '\n');
    }

  lastekind = KINVALID;

  fprintf(fout, ";; Library constraints\n");

 /*drl July 27 added this so that libraries without
    buffer constraints would be handled correctly.
    I'm trying to do this without breaking older libraries.

    Splint should still be able to handle libraries without this message.
 */

  
  fprintf(fout, "start_Buffer_Constraints\n");

  for (i = 0; i < utab->nentries; i++)
    {
      uentry thisentry = utab->entries[i];

      if (uentry_isFunction (thisentry) )
	{
	  constraintList preconditions;
	  constraintList postconditions;

	  preconditions = uentry_getFcnPreconditions (thisentry);
	  postconditions = uentry_getFcnPostconditions (thisentry);

	  if ( constraintList_isDefined(preconditions) ||
	       constraintList_isDefined(postconditions) )
	    {
	      fprintf(fout,"%s\n", cstring_toCharsSafe (uentry_rawName(thisentry) ) );
	      if (constraintList_isDefined(preconditions) )
		{
		  fprintf(fout,"pre:\n");
		  constraintList_dump(preconditions, fout);
		  fprintf (fout, ";; end precondition constraints\n" );
		  constraintList_free(preconditions);
		}
	      else
		{
		  fprintf(fout,"pre:EMPTY\n");
		}
	      if (constraintList_isDefined(postconditions) )
		{
		  fprintf(fout,"post:\n");
		  constraintList_dump(postconditions, fout);
		  fprintf (fout, ";; end precondition constraints\n" );
		  constraintList_free(postconditions);
		}
	      else
		{
		  fprintf(fout,"post:EMPTY\n");
		}
	    }
		  
	}
    }
}


void usymtab_load (FILE *f)
  /*@globals utab, globtab@*/
  /*@modifies utab, *f@*/
{
  char *s = mstring_create (MAX_DUMP_LINE_LENGTH);
  char *os = s;
  ekind kind = KINVALID;
  fileloc loc = g_currentloc;
  char c;
  uentry ue;

  dbgload = TRUE;

  llassert (utab == globtab);
  llassert (utab->nentries == 0);

  while (((s = reader_readLine (f, s, MAX_DUMP_LINE_LENGTH)) != NULL)
	 && *s == ';')
    {
      /* ignore ; comments */      ;
    }
  
  while (s != NULL && *s != ';')
    {
      int index;

      if (*s == '*')
	{
	  int ek;
	  s++;
	  ek = reader_getInt (&s);

	  if (ek == KGLOBALMARKER) 
	    {
	      uentry lue = uentry_makeGlobalMarker ();
	      DPRINTF (("Adding global marker: %s", uentry_unparseFull (lue)));
	      usymtab_addEntryAlways (utab, lue);
	      kind = KINVALID;
	      goto nextiter;
	    }
	  else
	    {
	      kind = ekind_fromInt (ek);
	      goto nextiter;
	    }
	}

      if (*s == '$')
	{
	  llfatalerror
	    (cstring_makeLiteral 
	     ("Library is in obsolete format.  Use splint +whichlib "
	      "to see which library is being loaded."));
	}

      if (reader_optCheckChar (&s, '^'))
	{
	  index = reader_getInt (&s);
	}
      else
	{
	  index = -1;
	}

      llassert (kind != KINVALID);
      ue = uentry_undump (kind, loc, &s);

      llassert (utab->nentries == index || index == -1);

      if (uentry_isValid (ue))
	{	
	  int lastindex = utab->nentries;
	  ue = usymtab_addEntryAlways (utab, ue);


# if 0
	  if (uentry_isConstant (ue)) /*@i23! isPreProcessorMacro */
	    {
	      cstring uname = uentry_getName (ue);
	      
	      /* Also check its a macro... */
	      DPRINTF (("Installing: %s", uname));

	      cpphash_installMacro 
		(mstring_copy (cstring_toCharsSafe (uname)),
		 cstring_length (uname),
		 cpplib_createDefinition (message ("%s 255", uname),
					  loc,
					  FALSE, FALSE).defn,
		 cpphash_hashCode (cstring_toCharsSafe (uname),
				   cstring_length (uname),
				   CPP_HASHSIZE));
	      
	      DPRINTF (("After install: %s", uname));
	    }
# endif

	  if (utab->nentries != lastindex + 1)
	    {
	      DPRINTF (("No add: %s", uentry_unparseFull (ue)));
	      BADBRANCH;
	    }
	  /*@-branchstate@*/ 
	} 
      /*@=branchstate@*/

      /*
      ** now, any other names are the same uentry
      */

      while (*(s++) == '#')
	{
	  cstring name = cstring_fromCharsO (reader_getWord (&s));
	  uentry nue = uentry_nameCopy (name, ue);
	  /*
	    DPRINTF (("Name copy: %s", uentry_unparseFull (nue)));
	    BADBRANCH;
	  */

	  usymtab_addEntryAlways (utab, nue);
	}

      while ((c = *s) != '\0' && (c !='\n'))
	{
	  if (c != ' ' || c != '\t') 
	    {
	      llbuglit ("Junk in load file");
	    }

	  s++;
	}

    nextiter:
      {
	s = reader_readLine (f, os, MAX_DUMP_LINE_LENGTH);
      }
    }

  /*DRL added 6/21/01
    to handle reading of buffer overflow related constraints
   */
  while (fgets (s, MAX_DUMP_LINE_LENGTH, f) != NULL 
	 && *s == ';')
    {
      ; /* ignore ;-comments */
    }

  /*drl July 27 added this so that libraries without
    buffer constraints would be handled correctly.
    I'm trying to do this without breaking older libraries*/
  
  /*check for "optional" start buffer constraints message*/

  if (mstring_equalPrefix (s, "start_Buffer_Constraints")) 
    {
      (void) fgets (s, MAX_DUMP_LINE_LENGTH, f);
    }
  
  while (s != NULL && *s != ';')
    {
      constraintList preconditions;
      constraintList postconditions;
      cstring name = cstring_fromChars (reader_getWord (&s));
      cstring temp;

      ue = usymtab_lookup (name);
      cstring_free (name);
      
      preconditions = constraintList_undefined;
      postconditions = constraintList_undefined;
      
      if (!uentry_isValid(ue) )
	{
	  llfatalbug ((message("Invalid uentry for %s library file may be corrupted", cstring_fromChars(s) ) ));
	}
      s = fgets (os, MAX_DUMP_LINE_LENGTH, f);

      temp = cstring_fromChars (reader_getWord(&s) );
      
      if (cstring_compareLit (temp,"pre:") == 0 )
	{
	  preconditions = constraintList_undump (f);
	}
      else
	{
	  if (cstring_compareLit (temp,"pre:EMPTY") != 0 )
	    llfatalbug ((message("Error reading library file pre:EMPTY expected but got %s", temp ) ));
	}
      
      cstring_free(temp);
      
      s = fgets (os, MAX_DUMP_LINE_LENGTH, f);

      temp = cstring_fromChars(reader_getWord(&s) );
      if (cstring_compareLit (temp, "post:") == 0 )
	{
	  postconditions = constraintList_undump (f);
	}
      else
	{
	  if (cstring_compareLit (temp, "post:EMPTY") != 0 )
	    llfatalbug ((message("Error reading library file post:EMPTY expected but got %s", temp ) ));
	}
      
      cstring_free (temp);

      uentry_setPreconditions (ue, functionConstraint_createBufferConstraint (preconditions));
      uentry_setPostconditions (ue, functionConstraint_createBufferConstraint (postconditions));
      
      s = fgets (os, MAX_DUMP_LINE_LENGTH, f);
    }
    
  dbgload = FALSE;
  sfree (os);
}

/*
** file scope for static variables
*/

void
usymtab_enterFile ()
  /*@globals utab, globtab, filetab@*/
  /*@modifies filetab@*/
{
  llassert (utab == globtab);

  # if 0
  /* check globals */
  
  usymtab_entries (globtab, ue)
    {
      if (sRef_hasDerived (uentry_getSref (ue)))
	{
	  fprintf (g_warningstream, "Derived Global: %s\n", uentry_unparse (ue));
	  fprintf (g_warningstream, "sRef: %s\n", sRef_unparseFull (ue->sref));
	}
    } end_usymtab_entries ;

  # endif

  usymtab_enterScope ();
  filetab = utab;
}

void
usymtab_exitFile ()
   /*@globals utab, filetab@*/
   /*@modifies filetab, utab@*/
{
  
  llassert (utab->lexlevel == 1);

  usymtab_exitScope (exprNode_undefined);
  filetab = NULL;
}

void
usymtab_enterScope ()
  /*@globals utab, globtab, filetab@*/
  /*@modifies utab@*/
{
  usymtab t = usymtab_create (US_NORMAL, utab, TRUE);
    	  
  /* unconditional scope: optimize to avoid copy */
  t->aliases = aliasTable_copy (utab->aliases); 
  utab = t;
  
  llassert (usymtab_isDefined (t->env));
  
  if (t->env->lexlevel == paramsScope && context_inFunctionLike ())
    {
      noshadowerror = TRUE;
      usymtab_handleParams ();
      noshadowerror = FALSE;
    }
}

/*
** setup external references:
**    o only, unique params alias external args
**    o other params may alias anything of their type
*/
 
static void
usymtab_handleParams (void)
  /*@globals utab, globtab, filetab@*/
  /*@modifies utab, globtab@*/
{
  usymtab ptab = utab->env;
  uentry fcn = context_getHeader ();

  usymtab_entries (ptab, param)
    {
      uentry ue;

      if (!uentry_isYield (param))
	{
	  sRef uref;
	  sRef pref = uentry_getSref (param);

	  /* Could be a global. */
	  
	  if (uentry_isAnyParam (param))
	    {	  
	      ue = uentry_makeVariable (fixParamName (uentry_rawName (param)),
					uentry_getType (param),
					fileloc_copy (uentry_whereDeclared (param)),
					FALSE);
	      
	      uentry_copyState (ue, param);
	      uentry_setRefParam (ue);
	      
	      ue = usymtab_supEntrySrefReturn (ue);
	      
	      /* must be after supercede! */
	      
	      if (!sRef_stateKnown (pref))
		{
		  uentry_setDefState (ue, SS_DEFINED);
		  uentry_setDefState (param, SS_DEFINED);
		}
	      else
		{
		  if (sRef_isStateSpecial (pref))
		    {
		      uentry_setDefState (ue, SS_SPECIAL); /* ALLOCATED); */
		      /* evans 2002-01-01: should be unnecessary, the pre clauses
		      **    set the state if necessary.
		      */
		    }
		  else
		    {
		      uentry_setDefState (ue, sRef_getDefState (pref));
		    }
		}
	      
	      uref = uentry_getSref (ue);
	      
	      if (sRef_isStack (uref))
		{
		  alkind pkind = sRef_getAliasKind (pref);
		  
		  if (alkind_isKnown (pkind) && !alkind_isLocal (pkind)
		      && !alkind_isStack (pkind))
		    {
		      sRef_setAliasKind (uref, pkind, fileloc_undefined);
		      sRef_setOrigAliasKind (uref, pkind);
		    }
		  else
		    {
		      sRef_setAliasKind (uref, AK_IMPTEMP, fileloc_undefined);
		      sRef_setOrigAliasKind (uref, AK_IMPTEMP);
		      
		      if (uentry_isOut (param))
			{
			  ;
			}
		      else
			{
			  sRef_setDefined (uref, fileloc_undefined);
			}
		    }
		  
		}
	      
	      usymtab_addMustAlias (uref, pref);   
	      
	      if (!(uentry_isOnly (param) || uentry_isUnique (param)))
		{
		  /*
		  ** This is needed for detecting possibly aliased parameters.
		  */

		  sRef s = sRef_makeExternal (uref);
		  usymtab_addMustAlias (uref, s);   
		}
	      
	      if (sRef_isKillRef (pref))
		{
		  sRef_setAliasKind (uref, AK_NEWREF, fileloc_undefined);
		  sRef_setOrigAliasKind (uref, AK_KILLREF);
		}
	      else if (sRef_isRefCounted (uref))
		{
		  sRef_setOrigAliasKind (uref, AK_REFCOUNTED);
		}
	      else
		{
		  /* was AK_UNIQUE */
		  sRef_setOrigAliasKind (uref, AK_LOCAL);
		}
	    }
	  else
	    {
	    }
	}
    } end_usymtab_entries;
  
  
  if (uentry_hasStateClauseList (fcn))
    {
      stateClauseList clauses = uentry_getStateClauseList (fcn);
      
      stateClauseList_preElements (clauses, cl)
	{
	  fileloc loc = stateClause_loc (cl);
	  sRefSet osrs = sRefSet_undefined;
	  sRefSet srs;
	  
	  if (stateClause_isGlobal (cl))
	    {
	      DPRINTF (("Global Marker: %s",
			sRef_unparseFull (usymtab_lookupGlobalMarker ())));
	      llassert (sRef_isGlobalMarker (usymtab_lookupGlobalMarker ()));
	      srs = sRefSet_single (usymtab_lookupGlobalMarker ());
	      osrs = srs;
	    }
	  else
	    {
	      srs = stateClause_getRefs (cl);
	    }
	  
	  sRefSet_elements (srs, el)
	    {
	      sRef base = sRef_getRootBase (el);
	      sRef sb = sRef_updateSref (el);

	      if (sRef_isResult (base))
		{
		  ; /* nothing to do before */
		}
	      else if (sRef_isParam (base) || sRef_isGlobalMarker (base))
		{
		  if (stateClause_setsMetaState (cl))
		    {
		      /* copied from exprNode.c:3040 */
		      qual ql = stateClause_getMetaQual (cl);
		      annotationInfo ainfo = qual_getAnnotationInfo (ql);
		      metaStateInfo minfo = annotationInfo_getState (ainfo);
		      cstring key = metaStateInfo_getName (minfo);
		      int mvalue = annotationInfo_getValue (ainfo);
		      
		      DPRINTF (("Sets meta state! %s", stateClause_unparse (cl)));
			  
		      if (sRef_isResult (base))
			{
			  BADBRANCH;
			}
		      else 
			{
			  sRef_setMetaStateValueComplete (sb, key, mvalue, loc);
			}
		    }
		  else
		    {
		      sRefMod modf = stateClause_getEntryFunction (cl);
		      
		      if (modf != NULL)
			{
			  sRefSet aliases = usymtab_allAliases (sb);
			  
			  modf (sb, loc);
			  
			  sRefSet_elements (aliases, sr)
			    {
			      modf (sr, loc);
			    } end_sRefSet_elements ;
			    
			  sRefSet_free (aliases);
			}
		    }
		}
	      else
		{
		  if (sRef_isValid (base))
		    {
		      DPRINTF (("Base: %s", sRef_unparseFull (base)));
		      BADBRANCH;
		    }
		}
	    } end_sRefSet_elements ; 	  
	} end_stateClauseList_preElements ;
    }
}

void
usymtab_enterFunctionScope (uentry fcn)
  /*@globals utab, filetab, globtab@*/
  /*@modifies utab@*/
{
  usymtab t = usymtab_create (US_NORMAL, utab, TRUE);

  DPRINTF (("Enter function: %s", uentry_unparse (fcn)));

  if (utab->lexlevel != fileScope)
    {
      if (utab->lexlevel > fileScope)
	{
	  llparseerror (cstring_makeLiteral ("New function scope inside function"));
	  
	  while (utab->lexlevel > fileScope)
	    {
	      /*@i@*/ utab = usymtab_dropEnv (utab);
              /*@-branchstate@*/
            }
	  /*@=branchstate@*/
	}
      else
	{
	  llfatalbug (cstring_makeLiteral ("New function not inside file."));
	}
    /*@-branchstate@*/ } /*@=branchstate@*/

  utab = t;
  
  DPRINTF (("Globs: %s", globSet_unparse (uentry_getGlobs (fcn))));

  globSet_allElements (uentry_getGlobs (fcn), el)
    {
      DPRINTF (("Here we go: %s", sRef_unparseFull (el)));
      
      if (sRef_isUndefGlob (el))
	{
	  usymId index = sRef_getScopeIndex (el);
	  sRef sr = sRef_updateSref (el);
	  fileloc loc = uentry_whereEarliest (fcn);
	
	  DPRINTF (("update: %s", sRef_unparseFull (sr)));
	  DPRINTF (("Undef!"));
	  if (sRef_isFileStatic (el))
	    {
	      ctype ct = sRef_getType (el);
	      uentry ue;
	      
	      llassert (usymtab_isDefined (filetab));

	      ue = usymtab_fetchIndex (filetab, index);
	      
	      if (ctype_isRealArray (ct) || ctype_isRealSU (ct))
		{
		  sRef_setAllocated (sr, loc);
		}
	      else
		{
		  sRef_setUndefined (sr, loc);
		}
	    }
	  else
	    {
	      uentry ue = globtab->entries[index];
	      ctype ct = uentry_getType (ue);
	      
	      if (ctype_isArray (ct) || ctype_isSU (ct))
		{
		  sRef_setAllocated (sr, loc);
		}
	      else
		{
		  sRef_setUndefined (sr, loc);
		}
	    }
	}
      else if (sRef_isAllocated (el))
	{
	  sRef sr = sRef_updateSref (el);
	  fileloc loc = uentry_whereEarliest (fcn);
	  
	  sRef_setAllocated (sr, loc);
	}
      else if (sRef_isPartial (el))
	{
	  sRef sr = sRef_updateSref (el);
	  fileloc loc = uentry_whereEarliest (fcn);

	  sRef_setPartial (sr, loc);
	}
      else
	{
	  /*
	  sRef sr = sRef_updateSref (el);
	  fileloc loc = uentry_whereEarliest (fcn);

	  sRef_setDefined (sr, loc);
	  */

	  /* defined */
	  /* shouldn't need to do anything! */ 
	}
    } end_globSet_allElements;

  DPRINTF (("Globs after: %s", globSet_unparse (uentry_getGlobs (fcn))));
# ifdef DEBUGSPLINT
  usymtab_checkAllValid ();
# endif
}

static void
usymtab_caseBranch (void)
  /*@modifies utab@*/
{
  usymtab t = usymtab_create (US_CBRANCH, utab, FALSE);
  utab = t;
}

void
usymtab_switchBranch (/*@unused@*/ exprNode s)
  /*@modifies utab@*/
{
  usymtab t = usymtab_create (US_SWITCH, utab, FALSE);

  t->aliases = aliasTable_copy (utab->aliases);
  utab = t;
}

void
usymtab_trueBranch (/*@only@*/ guardSet guards)
  /*@modifies utab@*/
{
  usymtab t = usymtab_create (US_TBRANCH, utab, FALSE);

  /*
  ** not true! (could be in a macro)
  **
  ** llassertprint (utab->lexlevel > paramsScope,
  ** ("not in scope: %s", usymtab_unparseLocal ()));
  **
  */

  guardSet_free (t->guards);
  t->guards = guards;
  
  aliasTable_free (t->aliases);
  t->aliases = aliasTable_copy (utab->aliases);
  
  utab = t;
}

/*
** consider,
** 
**   { int a; if (...) a = 3; < a may be undefined here!
**
*/

void
usymtab_popTrueBranch (exprNode pred, exprNode expr, clause cl) /*@modifies utab@*/
{
  /*
  ** add a false branch
  ** (could be done more efficiently as a special case, but
  ** it is better to only maintain one version of the code)
  */

  if (utab->kind != US_TBRANCH 
      && context_inIterDef ())
    {
      usymtab_exitScope (expr);
    }
  else
    {
      DPRINTF (("pop true branch.."));
      usymtab_altBranch (guardSet_invert (exprNode_getGuards (pred)));
      usymtab_popBranches (pred, expr, exprNode_undefined, TRUE, cl);
    }
}

void
usymtab_popCaseBranch () /*@modifies utab@*/
{
  llassert (utab->kind == US_CBRANCH);
  usymtab_quietPlainExitScope ();
}

void
usymtab_popTrueExecBranch (exprNode pred, exprNode expr, clause cl)
{
  /*
  ** add a false branch that must return --- that is,
  ** the true branch is always executed!
  */
  
  usymtab_altBranch (guardSet_invert (exprNode_getGuards (pred)));
  usymtab_popBranches (pred, expr, exprNode_makeMustExit (), FALSE, cl);
}

void
usymtab_popOrBranch (exprNode pred, exprNode expr)
  /*@modifies utab@*/
{
  bool mustReturn;
  usymtab env = utab->env;
  usymtab otab = utab;
  int i = 0;

  llassert (env != NULL);

  if (exprNode_isError (expr))
    {
      mustReturn = FALSE;
    }
  else
    {
      mustReturn = exprNode_mustEscape (expr);
    }

  
  llassert (utab->kind == US_TBRANCH);

  /*
  ** merge each entry in table with its original
  ** unless execution cannot continue after this branch
  */

  for (i = 0; i < utab->nentries; i++)
    {
      uentry current = utab->entries[i];
      uentry old = usymtab_lookupAux (env, uentry_rawName (current));

      uentry_mergeState (old, current, exprNode_loc (expr), 
			 mustReturn, FALSE, TRUE, ORCLAUSE);
    }
  
  
  if (mustReturn)
    {
      env->guards = guardSet_levelUnionFree (env->guards, 
					     guardSet_invert (exprNode_getGuards (pred)), 
					     env->lexlevel);
    }
  else
    {
      env->aliases = aliasTable_levelUnion (env->aliases, otab->aliases, env->lexlevel);
    }
 
  /* env is now utab */
  usymtab_quietPlainExitScope ();
}

/*
** case syntax in C is very unrestricted.  This is unfortunate.
**
** A switch case is ended either by a new case or default, or
** a close } that may close the switch or some other control
** structure.  
*/

bool
usymtab_newCase (/*@unused@*/ exprNode pred, exprNode last)
  /*@modifies utab@*/
{
  bool mustBreak = usymtab_mustBreak (utab);
  bool mustReturn = usymtab_mustEscape (utab);
  usymtab stab = utab;

  DPRINTF (("New case!"));

  /*
  ** Find last case (or outer switch)
  */

  while (stab->kind != US_CBRANCH && stab->kind != US_SWITCH)
    {
      stab = stab->env;
      llassert (stab != GLOBAL_ENV);
    }
  
  while (stab->kind == US_CBRANCH)
    {
      stab = stab->env;
      llassert (stab != GLOBAL_ENV);
    }
  
  /*
  ** if its a fall through case, merge in outside entries and last case.
  **
  ** e.g.,
  **        ...
  **        switch
  **          case 1: x = 3; <fall through>
  **          case 2: << x may not be defined
  **
  */

  if (!mustBreak && !mustReturn && utab->kind == US_CBRANCH)
    {
      llassert (stab->kind == US_SWITCH || stab->kind == US_NORMAL);

      usymtab_entries (utab, ue)  /* but, keep track of used variables */
	{
	  uentry old = usymtab_lookupAux (stab, uentry_rawName (ue));

	  llassert (uentry_isValid (old));

	  /* modifies ue, not old */
	  
	  uentry_mergeState (ue, old, exprNode_loc (last),
			     FALSE, FALSE, TRUE, CASECLAUSE); 
	} end_usymtab_entries;

      utab->aliases = aliasTable_levelUnion (utab->aliases, 
					     stab->aliases, utab->lexlevel);
      
      /* 
      ** No need for a new branch.
      */
      
      return FALSE;
    }
  else
    {
      usymtab_caseBranch ();
      /*@-mustfree@*/ /*< utab->aliases >*/
      utab->aliases = aliasTable_copy (stab->aliases);
      /*@=mustfree@*/
      
      return TRUE;
    }
}

/*
** for && (both pred and expr are executed)
*/

void
usymtab_popAndBranch (exprNode pred, /*@unused@*/ exprNode expr)
  /*@modifies utab@*/
{
  usymtab env = utab->env;
  usymtab otab= utab;
  int i = 0;

  llassert (utab->kind == US_TBRANCH);

  /*
  ** merge each entry in table with its original
  ** unless execution cannot continue after this branch
  */

  for (i = 0; i < utab->nentries; i++)
    {
      uentry current = utab->entries[i];
      sRef   tref = uentry_getSref (current);
      uentry old = usymtab_lookupAux (env, uentry_rawName (current));
      sRef   oref = uentry_getSref (old);

      /* note that is current is in a nested branch,
	 it may create a "new" old entry. */

      llassert (uentry_isValid (old));
      uentry_mergeState (old, current, exprNode_loc (expr), 
			 FALSE, FALSE, TRUE, ANDCLAUSE);

      /*
      ** if is it defined by the second clause, then it should be defined.
      */

      if (sRef_isAnyDefined (tref)
	  && (sRef_isAllocated (oref) || sRef_isStateUndefined (oref)))
	{
	  sRef_setDefined (oref, g_currentloc);
	}
    }

  DPRINTF (("Popping and: %s / %s",
	    guardSet_unparse (utab->guards),
	    guardSet_unparse (exprNode_getGuards (pred))));

  utab->guards = guardSet_levelUnionFree (utab->guards, 
					  guardSet_invert (exprNode_getGuards (pred)), 
					  utab->lexlevel);
  utab->aliases = aliasTable_levelUnion (utab->aliases, otab->aliases, utab->lexlevel);

  usymtab_quietPlainExitScope ();

  }

/*
** Stack should be [ US_CBRANCH+ US_SWITCH ]
** Only branches which do not return (except possibly the last branch) are included.
**
** Conditionally merge state from all CBRANCHes.
**
** If allpaths is TRUE, then all possible executions go through some switch 
** case, and the original scope is not merged.
*/

void
usymtab_exitSwitch (/*@unused@*/ exprNode sw, bool allpaths)
  /*@modifies utab@*/
{
  usymtab ttab = utab;
  usymtab stab = ttab;
  usymtab ltab = ttab;
  bool lastMustReturn = usymtab_mustEscape (utab);
  int i;
  
    
  while (stab->kind == US_CBRANCH)
    {
      stab = stab->env;
      llassert (stab != GLOBAL_ENV);
    }

  while (stab->kind == US_NORMAL)
    {
      stab = stab->env;
      llassert (stab != GLOBAL_ENV);
    }

  llassert (stab->kind == US_SWITCH);

  /* go to the scope outside the switch (US_SWITCH is just a marker! */
  stab = stab->env; 
  llassert (stab != GLOBAL_ENV);

  
  ttab = ttab->env;
  llassert (usymtab_isDefined (ttab));
  
  if (ttab->kind == US_CBRANCH)
    {
      /* was quietPlainExitScope --- but, can't free it yet! */
      utab = utab->env;
      llassert (utab != GLOBAL_ENV);

      while (ttab->kind == US_CBRANCH)
	{
	  /*
	  ** (from popTrueBranch)
	  */	  
	  
	  bool mustReturn = usymtab_mustEscape (ttab);
	  bool mustBreak = usymtab_mustBreak (ttab);
	  
	  usymtab_entries (ttab, current)
	    {
	      uentry old = /*@-compmempass@*/ usymtab_lookupAux (ltab, uentry_rawName (current));
       	                   /*@=compmempass@*/ 
	      
	      /*
	      ** note that is this is in a nested branch,
	      ** it may create a "new" old entry. 
	      */
	   
	      if (uentry_isValid (old))
		{
		  if (lastMustReturn)
		    {
		      uentry_mergeUses (current, old);
		      uentry_setState (old, current);
		    }
		  else
		    {
		      uentry_mergeState (old, current, exprNode_loc (sw),
					 mustReturn, FALSE, TRUE, SWITCHCLAUSE);
		    }
		}
	      else
		{
		  ;
		}
	    } end_usymtab_entries;
	  
	  /*
	  ** if entry is not in symbol table for this case, merge with pre-switch
	  ** table
	  */
	  
	  if (!mustReturn && !mustBreak)
	    {
	      usymtab_entries (stab, current)
		{
		  if (!usymtab_indexFound (usymtab_getIndex (ttab, uentry_rawName (current))))
		    {
		      uentry old = /*@-compmempass@*/
			usymtab_lookupAux (ltab, uentry_rawName (current));
		      /*@=compmempass@*/

		      llassert (uentry_isValid (old));
		      uentry_mergeState (old, current, exprNode_loc (sw),
					 FALSE, FALSE, TRUE, SWITCHCLAUSE);
		    }
		} end_usymtab_entries;
	    }
	  
	  ltab->env = ttab->env; 
	  ttab = ltab->env;

	  /*
	  ** Suprious error, becuase of environments.
	  */

	  /*@i1@*/ utab = ltab;

	  lastMustReturn = FALSE;
	  /*@-branchstate@*/ 
	}
    }
  /*@=branchstate@*/

  /*
  ** now, there is one US_CBRANCH.  Merge this with the stab.
  */
  
    
  for (i = 0; i < ltab->nentries; i++)
    {
      uentry current = ltab->entries[i];
      uentry old = usymtab_lookupAux (stab, uentry_rawName (current));
      
      /* note that is this is in a nested branch,
	 it may create a "new" old entry. */
      
         
      if (uentry_isValid (old))
	{
	  if (allpaths)
	    {
	      uentry_mergeUses (current, old);
	      uentry_setState (old, current);
	    }
	  else
	    {
	      uentry_mergeState (old, current, exprNode_loc (sw), 
				 FALSE, FALSE, TRUE, SWITCHCLAUSE);
	    }
	}
      else
	{
	  	}
    }
  
  /*
  ** exit the switch
  */
  
  
  /*
  ** switch may or may not be followed by a new scope
  */

  if (utab->kind == US_SWITCH)
    {
      usymtab_quietPlainExitScope ();  
    }
  else
    {
      usymtab_quietPlainExitScope ();
      llassert (utab->kind == US_SWITCH);
      usymtab_quietPlainExitScope ();   
    }

  }

static void 
updateNullState (sRef el, /*@notnull@*/ usymtab ttab, 
		 /*@notnull@*/ usymtab ftab, bool trueGuard)
{
  sRef base = sRef_getRootBase (el);
  int level = sRef_lexLevel (base);        
    
  if (sRef_isCvar (base))
    {
      usymId index = sRef_getScopeIndex (base);
      uentry ue = usymtab_getRefTab (ttab, level, index);

      if (!uentry_isLset (ue)) 
	{
	  sRef sr = uentry_getSref (ue);

	  if (trueGuard)
	    {
	      sRef_setDerivNullState (sr, el, NS_NOTNULL);
	    }
	  else 
	    {
	      if (!guardSet_isGuarded (ttab->guards, el) 
		  && !sRef_isNotNull (sr))
		{
		  DPRINTF (("Here! %s / %s",
			    sRef_unparseFull (sr),
			    sRef_unparseFull (el)));
		  sRef_setDerivNullState (sr, el, NS_DEFNULL);
		}
	    }
	}
      else
	{
	  ;
	}
      
      ue = usymtab_getRefTab (ftab, level, index);
      
      if (!uentry_isLset (ue)) 
	{
	  sRef sr = uentry_getSref (ue);
	  
	  if (!trueGuard) /* yikes!  forgot the ! */
	    {
	      sRef_setDerivNullState (sr, el, NS_NOTNULL);
	    }
	  else 
	    {
	      
	      if (!guardSet_isGuarded (ftab->guards, el)
		  && !sRef_isNotNull (sr))
		{
		  sRef_setDerivNullState (sr, el, NS_DEFNULL);
		}
	    }
	}
      else
	{
	  ;
	}
    }  
}

void
usymtab_popBranches (exprNode pred, exprNode tbranch, exprNode fbranch, 
		     bool isOpt, clause cl)
     /*@modifies utab@*/
{
  int i = 0;
  usymtab ftab = utab;
  usymtab ttab = utab->env;

  fileloc loc;
  usymtab env;
  guardSet guards = exprNode_getGuards (pred);
  sRefSet tguards = guardSet_getTrueGuards (guards);
  sRefSet fguards = guardSet_getFalseGuards (guards);
  bool mustReturnT = exprNode_mustEscape (tbranch);
  bool mustReturnF = exprNode_mustEscape (fbranch);

  DPRINTF (("Pop branches: %s [mustreturn: %s/%s]", exprNode_unparse (pred),
	    bool_unparse (mustReturnT),
	    bool_unparse (mustReturnF)));

  if (exprNode_isDefined (fbranch))
    {
      loc = exprNode_loc (fbranch);
    }
  else
    {
      loc = exprNode_loc (tbranch);
    }

  llassert (usymtab_isDefined (ttab));
  
  env = ttab->env;
  
  llassert (usymtab_isDefined (env));
  llassert (ftab->kind == US_FBRANCH);
  llassert (ttab->kind == US_TBRANCH);

  /*
  ** For each element that is true guarded (i.e., if (x != NULL)) 
  **          make x = null in false branch,
  **          and x = notnull in true branch.
  **          unless x was set locally in that branch.
  ** For each element that is false guarded (x == NULL) 
  **          make x = null in true, notnull in false.
  **                      
  ** For each element that is either guarded (pred(x)) 
  **     
  */
  
  sRefSet_allElements (tguards, el)
    {
      updateNullState (el, ttab, ftab, TRUE);
    } end_sRefSet_allElements;
    
  sRefSet_allElements (fguards, el)
    {
      updateNullState (el, ttab, ftab, FALSE);
    } end_sRefSet_allElements;

  /*
  ** 
  ** if an entry is in both true and false, merge the entries,
  ** then replace original with new state.
  **
  ** if an entry is in one table, merge it with the original.
  */
  
  DPRINTF (("ftab: %d", ftab->nentries));

  for (i = 0; i < ftab->nentries; i++)
    {
      uentry fthis = ftab->entries[i];
      uentry old = usymtab_lookupAux (env, uentry_rawName (fthis));
      usymId tindex = usymtab_getIndex (ttab, uentry_rawName (fthis));
      
      DPRINTF (("Entry: %s / %s", uentry_unparseFull (fthis), uentry_unparseFull (old)));
	  
      if (uentry_isUndefined (old))
	{
	  /* possible entry was added as an undefined id */
	  DPRINTF (("Undefined! %s", uentry_rawName (fthis)));
	  continue;
	}
      
      if (usymtab_indexFound (tindex))
	{
	  uentry tthis = ttab->entries[tindex];

	  /* note that is this is in a nested branch,
	     it may create a "new" old entry. */
	  
	  if (!mustReturnF)
	    {
	      if (!mustReturnT)
		{
		  uentry_mergeState (fthis, tthis, loc,
				     mustReturnT, FALSE, FALSE, cl);
		}
	      else
		{
		  uentry_mergeUses (fthis, tthis);
		}
	      
	      uentry_setState (old, fthis);
	      
	      /*@-mustfree@*/ 
	    } 
	  /*@=mustfree@*/
	  else
	    {
	      uentry_setState (old, tthis);
	      uentry_mergeState (old, fthis, loc, mustReturnF, 
				 TRUE, FALSE, cl);
	    }
	  
	  ttab->entries[tindex] = uentry_undefined;
	  uentry_free (tthis);
	}
      else
	{
	  uentry_mergeState (old, fthis, loc, mustReturnF, TRUE, FALSE, cl);
	}
    }

  for (i = 0; i < ttab->nentries; i++)
    {
      uentry current = ttab->entries[i];
      
      DPRINTF (("ttab: %s", uentry_unparseFull (current)));
      
      if (!uentry_isUndefined (current)) 
	{
	  uentry old = usymtab_lookupAux (env, uentry_rawName (current));

	  DPRINTF (("Old: %s", uentry_unparseFull (old)));

	  if (uentry_isUndefined (old))
	    {
	      llcontbug (message ("Undefined entry: %s", uentry_rawName (current)));
	      continue;
	    }

	  if (mustReturnF)
	    {
	      uentry_mergeUses (current, old); 
	      uentry_setState (old, current);
	    }
	  else
	    {
	      /*
              ** Assumes false branch is a fall-through if
              ** fbranch is not defined.  This is true, unless
              ** where was some greivous error in processing
              ** the else branch of an if-then, in which case
              ** this is probably the right thing to do anyway.
              */

	      uentry_mergeState (old, current, loc, mustReturnT, 
				 FALSE, isOpt, cl);
	    }

	  DPRINTF (("==> %s", uentry_unparseFull (old)));
	}
    }
  
  /*
  ** Plain levelUnion doesn't work, since we need to use the sRef's in env->aliases
  ** if they are present.
  */

  llassert (NOALIAS (env->aliases, ttab->aliases));
  llassert (NOALIAS (env->aliases, ftab->aliases));

  aliasTable_free (env->aliases);  

  env->aliases = aliasTable_levelUnionNew (ttab->aliases, 
					   ftab->aliases, env->lexlevel);

  aliasTable_fixSrefs (env->aliases);

  DPRINTF (("Aliases: %s", aliasTable_unparse (env->aliases)));
  
  /* exit true and false scopes */
  usymtab_quietPlainExitScope ();
  usymtab_quietPlainExitScope ();
  
  if (mustReturnT)
    {
      utab->guards = guardSet_levelUnionFree 
	(utab->guards, 
	 guardSet_invert (exprNode_getGuards (pred)), 
	 utab->lexlevel);
    }

  if (mustReturnF)
    {
      utab->guards = guardSet_levelUnion (utab->guards, 
					  exprNode_getGuards (pred), 
					  utab->lexlevel);
    }

  DPRINTF (("Here."));
}

static void usymtab_fixCases (void) /*@modifies utab@*/ {
  while (utab->kind == US_CBRANCH)
    {
      usymtab_quietPlainExitScope ();
    }

  llassert (utab->kind != US_CBRANCH);
}

void
usymtab_altBranch (/*@only@*/ guardSet guards)
  /*@modifies utab@*/
{
  usymtab t;
  usymtab parent = utab->env;

  t = usymtab_create (US_FBRANCH, utab, FALSE);

  /*
  ** If we are in a case, need to close it.  The C syntax
  ** is very liberal, so this kludge is necessary.
  */

  usymtab_fixCases ();

  DPRINTF (("Current kind: %s", usymtab_unparseStack ()));

  llassert (utab->kind == US_TBRANCH);
  llassert (parent != GLOBAL_ENV);
  
  guardSet_free (t->guards);
  t->guards = guards;
  
  aliasTable_free (t->aliases);
  t->aliases = aliasTable_copy (parent->aliases);
  
  utab = t;
}

void
usymtab_allDefined (void)
   /*@globals utab, globtab@*/
{
  int i;

  llassert (utab == globtab);

  for (i = 0; i < utab->nentries; i++)
    {
      uentry e = utab->entries[i];

      if (uentry_isPriv (e))
	{
	 ; /* no need to define it */
	}
      else
	{
	  if (context_getFlag (FLG_SPECUNDECL))
	    {
	      fileloc sloc = uentry_whereSpecified (e);
	      fileloc dloc = uentry_whereDeclared (e);

	      if (fileloc_isDefined (sloc) 
		  && !uentry_isFakeTag (e)
		  && !fileloc_isDefined (dloc))
		{
		  voptgenerror 
		    (FLG_SPECUNDECL,
		     message ("%s %q specified but not declared",
			      ekind_capName (uentry_getKind (e)),
			      uentry_getName (e)),
		     sloc);
		}
	    }
	  
	  if (!uentry_isCodeDefined (e))
	    {
	      fileloc dloc = uentry_whereDeclared (e);
	      
	      if (fileloc_isLib (dloc) || fileloc_isXHFile (dloc))
		{
		 ;
		}
	      else if (fileloc_isDefined (dloc))
		{
		  if (!uentry_isAnyTag (e))
		    {
		      if (fileloc_isUser (dloc))
			{
			  voptgenerror 
			    (FLG_DECLUNDEF,
			     message ("%s %q declared but not defined",
				      ekind_capName (uentry_getKind (e)),
				      uentry_getName (e)),
			     dloc);
			  DPRINTF (("decl: %s", uentry_unparseFull (e)));
			}
		    }
		}
	      else
		{
		  fileloc sloc = uentry_whereSpecified (e);

		  if (fileloc_isDefined (sloc) 
		      && !fileloc_isImport (sloc)
		      && !fileloc_isLib (sloc)
		      && !fileloc_isPreproc (sloc)
		      && !uentry_isFakeTag (e))
		    {
		      if (uentry_isVariable (e) || uentry_isFunction (e))
			{
			  voptgenerror 
			    (FLG_SPECUNDEF,
			     message ("%s %q specified but not declared or defined",
				      ekind_capName (uentry_getKind (e)),
				      uentry_getName (e)),
			     sloc);
			}
		      else
			{
			  voptgenerror 
			    (FLG_SPECUNDEF,
			     message ("%s %q specified but not defined",
				      ekind_capName (uentry_getKind (e)),
				      uentry_getName (e)),
			     sloc);
			}
		    }
		}
	    }
	}
    }
}

void usymtab_exportHeader (void)
     /*@globals utab@*/
{
  int i;

  for (i = 0; i < utab->nentries; i++)
    {
      uentry ce = utab->entries[i];

      if (!uentry_isDatatype (ce) 
	  && !uentry_isAnyTag (ce) 
	  && !uentry_isEitherConstant (ce) && !uentry_isStatic (ce)
	  && !uentry_isExternal (ce)
	  && !uentry_isForward (ce))
	{
	  fileloc fwhere = uentry_whereDeclared (ce);

	  if (fileloc_isUndefined (fwhere)
	      && uentry_isFunction (ce))
	    {
	      fwhere = uentry_whereDefined (ce);
	    }

	  if (fileloc_isDefined (fwhere) 
	      && !fileloc_isHeader (fwhere)
	      && !fileloc_isXHFile (fwhere)
	      && !(fileloc_isSpecialFile (fwhere)
		   && !context_getFlag (FLG_UNUSEDSPECIAL)))
	    {
	      if (uentry_isVariable (ce))
		{
		  if (optgenerror
		      (FLG_EXPORTHEADERVAR,
		       message ("%s %q exported but not declared in header file", 
				ekind_capName (uentry_getKind (ce)),
				uentry_getName (ce)),
		       fwhere))
		    {
		      uentry_showDefSpecInfo (ce, fwhere);
		    }
		}
	      else
		{
		  if (!uentry_isIter (ce)
		      && !uentry_isEndIter (ce)
		      && !uentry_isExpandedMacro (ce))
		    {
		      if (uentry_isFunction (ce) 
			  && cstring_equalLit (uentry_rawName (ce), "main"))
			{
			  ; /* no error for main */
			}
		      else
			{
			  if (optgenerror
			      (FLG_EXPORTHEADER,
			       message ("%s %q exported but not declared "
					"in header file", 
					ekind_capName (uentry_getKind (ce)),
					uentry_getName (ce)),
			       fwhere))
			    {
			      uentry_showDefSpecInfo (ce, fwhere);
			    }
			}
		    }
		}
	    }
	}
    }
}

void usymtab_exportLocal (void)
   /*@globals utab@*/
{
  int i;
  
  for (i = 0; i < utab->nentries; i++)
    {
      uentry ce = utab->entries[i];
      
      if (!uentry_isDatatype (ce) && !uentry_isAnyTag (ce) 
	  && !uentry_isEitherConstant (ce) 
	  && !uentry_isIter (ce)
	  && !uentry_isEndIter (ce)
	  && !uentry_isExpandedMacro (ce)
	  && uentry_isUsed (ce))
	{
	  /* check static uses */
	  filelocList fuses = uentry_getUses (ce);
	  fileloc mod = uentry_whereDefined (ce);
	  bool ok = filelocList_isEmpty (fuses);
	  fileloc fwhere = uentry_whereDeclared (ce);

	  if (fileloc_isSpecialFile (fwhere)
	      && !context_getFlag (FLG_UNUSEDSPECIAL))
	    {
	      ok = TRUE; /* no errors for special files */
	    }
	  else
	    {
	      filelocList_elements (fuses, uloc)
		{
		  if (fileloc_isUndefined (uloc) || !fileloc_sameModule (uloc, mod))
		    {
		      ok = TRUE;
		      /*@innerbreak@*/ break;
		    }
		} end_filelocList_elements;
	    }

	  if (!ok)
	    {
	      if (optgenerror
		  (FLG_EXPORTLOCAL,
		   message ("%s exported but not used outside %s: %q", 
			    ekind_capName (uentry_getKind (ce)),
			    fileloc_getBase (mod),
			    uentry_getName (ce)),
		   fwhere))
		{
		  uentry_showDefSpecInfo (ce, fwhere);
		}
	    }
	}
    }
}

void
usymtab_allUsed (void)
  /*@globals utab@*/
{
  int i;
  bool isFileStatic = usymtab_inFileScope ();
  cstring last_file = cstring_undefined;

  for (i = 0; i < utab->nentries; i++)
    {
      bool hasError = FALSE;
      uentry ce = utab->entries[i];
      fileloc fwhere = uentry_whereDeclared (ce);

      if (fileloc_isUndefined (fwhere))
	{
	  fwhere = uentry_whereDefined (ce);
	}

      if (fileloc_isInvalid (fwhere) 
	  || fileloc_isLib (fwhere) 
	  || fileloc_isBuiltin (fwhere)
	  || ((fileloc_isSpecialFile (fwhere)
	       || fileloc_isSpecialFile (uentry_whereDefined (ce)))
	      && !context_getFlag (FLG_UNUSEDSPECIAL)))
	{
	  ;
	}
      else if (!uentry_wasUsed (ce) && !uentry_isAnyTag (ce))
	{
	  cstring fname = fileloc_filename (fwhere);

	  if (cstring_isUndefined (last_file))
	    {
	      last_file = fname;
	    }
	  else if (cstring_equal (fname, last_file))
	    {
	    }
	  else
	    {
	      cleanupMessages ();
	      last_file = fname;
	    } 
	  
	  if (uentry_isParam (ce))
	    {
	      if (context_inMacro ())
		{
		  sRef cref = uentry_getSref (ce);

		  if (uentry_isYield (ce))
		    {
		      ; /* no checks (for now) */
		    }
		  else if (sRef_isSafe (cref))
		    {
		      ; /* no error */
		    }
		  else
		    {
		      if (uentry_hasRealName (ce))
			{
			  hasError = 
			    optgenerror (FLG_MACROPARAMS,
					 message ("Macro parameter %q not used", 
						  uentry_getName (ce)),
					 fwhere);
			}
		    }
		}
	      else 
		{
		  if (cstring_equalFree (uentry_getName (ce), 
					 cstring_makeLiteral ("...")))
		    {
		      ;
		    }
		  else
		    {
		      hasError = optgenerror (FLG_PARAMUNUSED,
					      message ("Parameter %q not used",
						       uentry_getName (ce)),
					      fwhere);
		    }
		}
	    } /* isParam */
	  else if (uentry_isFunction (ce) || uentry_isIter (ce))
	    {
	      if (fileloc_isUser (fwhere))
		{
		  hasError = optgenerror
		    (FLG_FUNCUNUSED, 
		     message ("%q %q declared but not used", 
			      cstring_makeLiteral 
			      (uentry_isIter (ce) ? "Iterator" 
			       : (isFileStatic ? "File static function" : "Function")),
			      uentry_getName (ce)),
		     fwhere);
		}
	    }
	  else if (uentry_isEndIter (ce))
	    {
	      ; /* no error (already reported for iter */
	    }
	  else if (uentry_isEnumConstant (ce))
	    {
	      if (fileloc_isUser (fwhere))
		{
		  hasError = optgenerror
		    (FLG_ENUMMEMUNUSED,
		     message ("Enum member %q not used", 
			      uentry_getName (ce)),
		     fwhere);
		}
	    }
	  else if (uentry_isConstant (ce))
	    {
	      if (fileloc_isUser (fwhere))
		{
		  hasError = optgenerror
		    (FLG_CONSTUNUSED,
		     message ("Constant %q declared but not used", 
			      uentry_getName (ce)),
		     fwhere);
		}
	    }
	  else if (uentry_isDatatype (ce))
	    {
	      if (fileloc_isUser (fwhere))
		{
		  hasError = optgenerror
		    (FLG_TYPEUNUSED,
		     message ("Type %q declared but not used", 
			      uentry_getName (ce)),
		     fwhere);
		}
	    }
	  else if (!uentry_isRefParam (ce) && !uentry_isExpandedMacro (ce))
	    { /* errors for ref params will be reported in the next scope */
	      llassertprint (uentry_isVar (ce), 
			     ("ce: %s", uentry_unparseFull (ce)));

	      if (ctype_isFunction (uentry_getType (ce)))
		{
		  if (fileloc_isUser (fwhere))
		    {
		      hasError = optgenerror
			(FLG_FUNCUNUSED,
			 message ("%q %q declared but not used", 
				  cstring_makeLiteral 
				  (isFileStatic ? "File static function" 
				   : "Function"),
				  uentry_getName (ce)),
			 fwhere);
		    }
		}
	      else 
		{
		  if (fileloc_isUser (fwhere))
		    {
		      
		      
		      hasError = optgenerror 
			(FLG_VARUNUSED,
			 message ("%q %q declared but not used", 
				  cstring_makeLiteral 
				  (isFileStatic ? "File static variable" 
				   : "Variable"), 
				  uentry_getName (ce)),
			 fwhere);
		    }
		}
	    }
	  else
	    {
	      ; /* no errors */
	    }
	} /* unused */
      else if ((uentry_isDatatype (ce) || uentry_isAnyTag (ce)))
	{ /* check all fields */
	  ctype ct = uentry_getRealType (ce); 

	  
	  while (ctype_isAP (ct))
	    {
	      ct = ctype_getBaseType (ct);
	    }

	  if (ctype_isSU (ct))
	    {
	      uentryList fields = ctype_getFields (ct);

	      uentryList_elements (fields, field)
		{
		  if (!uentry_isUsed (field))
		    {
		      if (uentry_hasName (ce))
			{
			  hasError |= optgenerror 
			    (FLG_FIELDUNUSED,
			     message ("Field %q of %s %q declared but not used", 
				      uentry_getName (field),
				      cstring_makeLiteralTemp
				      (ctype_isStruct (ct) ? "structure" : "union"),
				      uentry_getName (ce)),
			     uentry_whereEarliest (field));
			}
		      else
			{
			  /*
			  ** evans 2001-06-08
			  ** Can't report these errors for unnamed structs.
			  ** No way to tell when there are multiple consistent
			  ** unnamed structure types.  (Could go through table
			  ** and mark them all unused...)

			  hasError |= optgenerror 
			    (FLG_FIELDUNUSED,
			     message ("Field %q of unnamed %s declared but not used", 
				      uentry_getName (field),
				      cstring_makeLiteralTemp
				      (ctype_isStruct (ct) ? "structure" : "union")),
			     uentry_whereEarliest (field));

			  */
			}
		      
		      uentry_setUsed (field, fileloc_undefined);
		    }
		} end_uentryList_elements;
	    }
	}
      else
	{
	  ; /* no errors */
	}

      if (hasError)
	{
	  if (uentry_isParam (ce) && context_inMacro ())
	    {
	      if (fileloc_isDefined (uentry_whereSpecified (ce)))
		{
		  uentry_showWhereSpecified (ce);
		}
	    }
	  else
	    {
	      uentry_showDefSpecInfo (ce, fwhere);
	    }

	  uentry_setUsed (ce, fileloc_undefined);
	}
    }
}

static void
checkGlobalReturn (uentry glob, sRef orig)
{
  sRef sr = uentry_getSref (glob);
  
  DPRINTF (("Check global return: %s / orig: %s / sr: %s",
	    uentry_unparseFull (glob),
	    sRef_unparseFull (orig),
	    sRef_unparseFull (sr)));

  DPRINTF (("Is killed: %s", bool_unparse (sRef_isKilledGlob (orig))));

  if (context_getFlag (FLG_GLOBSTATE))
    {
      DPRINTF (("Is killed: %s", sRef_unparseFull (orig)));
      
      if (sRef_isKilledGlob (orig))
	{
	  if (sRef_isStateUndefined (sr)
	      || sRef_isUnuseable (sr)
	      || sRef_isStateUnknown (sr)
	      || sRef_isDead (sr))
	    {
	      ;
	    }
	  else
	    {
	      ctype ct = ctype_realType (uentry_getType (glob));
	      
	      DPRINTF (("Check global destroyed: %s", uentry_unparseFull (glob)));

	      if (ctype_isVisiblySharable (ct))
		{
		  if (optgenerror 
		      (FLG_GLOBSTATE,
		       message 
		       ("Killed global %q (type %s) not released before return",
			uentry_getName (glob),
			ctype_unparse (ct)),
		       g_currentloc))
		    {
		      sRef_showStateInfo (sr);
		    }
		}
	      else
		{
		  sRef_protectDerivs ();
		  (void) transferChecks_globalDestroyed (sr, g_currentloc);
		  sRef_clearProtectDerivs ();
		}
	    }
	}
      else
	{
	  if (sRef_isStateUndefined (sr))
	    {
	      if (optgenerror (FLG_GLOBSTATE,
			       message 
			       ("Function returns with global %q undefined",
				uentry_getName (glob)),
			       g_currentloc))
		{
		  sRef_showStateInfo (sr);
		}
	    }
	  else 
	    {
	      if (sRef_isDead (sr) || sRef_isKept (sr))
		{
		  if (optgenerror 
		      (FLG_GLOBSTATE,
		       message ("Function returns with global %q "
				"referencing %s storage",
				uentry_getName (glob),
				cstring_makeLiteralTemp (sRef_isDead (sr) ? "released" : "kept")),
		       g_currentloc))
		    {
		      if (sRef_isKept (sr))
			{
			  sRef_showAliasInfo (sr);      
			}
		      else
			{
			  sRef_showStateInfo (sr);
			}

		      sRef_setDefState (sr, SS_UNKNOWN, fileloc_undefined);
		    }
		}
	      
	      DPRINTF (("Here: %s / %s",
			uentry_unparseFull (glob),
			sRef_unparseFull (sr)));

	      if (ctype_isRealPointer (uentry_getType (glob))
		  && sRef_possiblyNull (sr)
		  && !uentry_possiblyNull (glob))
		{
		  if (optgenerror 
		      (FLG_GLOBSTATE,
		       message ("Function returns with non-null global %q "
				"referencing null storage",
				uentry_getName (glob)),
		       g_currentloc))
		    {
		      sRef_showNullInfo (sr);
		    }
		}
	      else
		{
		  DPRINTF (("Check transfer: %s", uentry_unparseFull (glob)));
		  transferChecks_globalReturn (glob);
		}
	    }
	}
    }
}

/*
** remember: check alias globals
*/

void usymtab_checkFinalScope (bool isReturn)
  /*@globals utab@*/
{
  bool mustDefine = context_getFlag (FLG_MUSTDEFINE);
  sRefSet checked = sRefSet_new ();
  usymtab stab = utab;
  int i;
    
  /*
  ** need to check all scopes out to function parameters.
  */

  do 
    {
      for (i = 0; i < stab->nentries; i++)
	{
	  uentry ce = stab->entries[i];
	  sRef sr = uentry_getSref (ce);
	  sRef rb = sRef_getRootBase (sr);
	  valueTable tvalues;

	  /*
	  ** Shouldn't check if shadow checked in deeper scope:
	  */

	  if (stab != utab)
	    {
	      uentry oue = usymtab_lookupQuietNoAlt (utab, uentry_observeRealName (ce));

	      if (!uentry_sameObject (ce, oue))
		{
		  /* what if it is one an alternate branch? */
		  /*@innercontinue@*/ continue;
		}
	    }
	  
	  DPRINTF (("Here check final scope: %s", uentry_unparseFull (ce)));
	  
	  if (ctype_isFunction (uentry_getType (ce)))
	    {
	      /*@innercontinue@*/ continue;
	    }

	  if (uentry_isAnyParam (ce)
	      || uentry_isRefParam (ce)
	      || sRef_isFileOrGlobalScope (rb))
	    {
	      /* Don't do the loseref check...but should check state! */
	      DPRINTF (("Skipping check 1"));
	    }
	  else if (sRef_isDefinitelyNull (sr)
		   || usymtab_isDefinitelyNull (sr))
	    {
	      /*
	      ** No state reference errors for definitely null references.
	      */

	      DPRINTF (("Skipping check 2"));
	    }
	  else
	    {
	      DPRINTF (("Lose ref: %s / %s", uentry_unparse (ce), 
			sRef_unparseFull (sr)));
	      
	      tvalues = sRef_getValueTable (sr);
	      
	      valueTable_elements (tvalues, fkey, fval) {
		metaStateInfo minfo;
		cstring msg = cstring_undefined;
		int nval;
		
		minfo = context_lookupMetaStateInfo (fkey);
		llassert (metaStateInfo_isDefined (minfo));
		
		if (stateValue_isError (fval)
		    || sRef_isStateUndefined (sr)) /* No errors for undefined state */
		  {
		    DPRINTF (("Skipping check 3"));
		  }
		else 
		  {
		    DPRINTF (("Check: %s / %s / %s", fkey,
			      metaStateInfo_unparse (minfo),
			      stateValue_unparse (fval)));
		    
		    minfo = context_lookupMetaStateInfo (fkey);
		    
		    nval = stateCombinationTable_lookupLoseReference 
		      (metaStateInfo_getTransferTable (minfo), 
		       stateValue_getValue (fval), &msg);
		    
		    if (cstring_isDefined (msg)) 
		      {
			if (optgenerror 
			    (FLG_STATETRANSFER,
			     message
			     ("%s loses reference %q in invalid state %q (%s)",
			      cstring_makeLiteralTemp (isReturn ? "Return" : "Scope exit"),
			      uentry_getName (ce),
			      stateValue_unparseValue (fval, minfo),
			      msg),
			     g_currentloc))
			  {
			    stateValue_show (fval, minfo);
			  }
			else
			  {
			    DPRINTF (("Suppressed transfer error: %s", msg));
			  }
		      }
		  }
	      } end_valueTable_elements;
	    }

	  DPRINTF (("Check mustfree entry: %s", uentry_unparseFull (ce)));
	  
	  if (!sRefSet_member (checked, sr) && !sRef_isFileOrGlobalScope (rb))
	    {
	      if (ctype_isRealSU (uentry_getType (ce))
		  && !uentry_isAnyParam (ce)
		  && !uentry_isRefParam (ce)
		  && !uentry_isStatic (ce)
		  && !sRef_isDependent (sr)
		  && !sRef_isOwned (sr))
		{
		  sRefSet als = usymtab_allAliases (sr);
		  
		  if (sRefSet_isEmpty (als))
		    {
		      transferChecks_localDestroyed (sr, g_currentloc);
		    }
		  else
		    {
		      /* aliased, no problem */ ;
		    }
		  
		  sRefSet_free (als);
		}
	      else if
		(!uentry_isStatic (ce)
		 && ((sRef_isNewRef (sr))
		     || (((sRef_isOnly (sr) || sRef_isFresh (sr) 
			   || sRef_isKeep (sr) || sRef_isOwned (sr))
			  && !sRef_isDead (sr))
			 && (!sRef_definitelyNull (sr))
			 && (!usymtab_isDefinitelyNull (sr)))))
		{
		  bool hasError = TRUE;
		  
		  DPRINTF (("Checking: %s", sRef_unparseFull (sr)));
		  
		  /*
		  ** If its a scope exit, check if there is an alias.
		  ** If so, make it only.  If not, there is an error.
		  */
		  
		  if (!isReturn)
		    {
		      if (transferChecks_canLoseReference (sr, g_currentloc))
			{
			  DPRINTF (("Can lose!"));
			  hasError = FALSE;
			}
		    }
		  
		  if (hasError)
		    {
		      if (sRef_hasLastReference (sr))
			{
			  sRef ar = sRef_getAliasInfoRef (sr);
			  
			  if (optgenerror 
			      (sRef_isFresh (ar) ? FLG_MUSTFREEFRESH : FLG_MUSTFREEONLY,
			       message
			       ("Last reference %q to %s storage %qnot %q before %q",
				sRef_unparse (sr),
				alkind_unparse (sRef_getAliasKind (sr)),
				sRef_unparseOpt (ar),
				cstring_makeLiteral (sRef_isKeep (sr) 
						     ? "transferred" : "released"),
				cstring_makeLiteral (isReturn 
						     ? "return" : "scope exit")),
			       g_currentloc))
			    {
			      sRef_showRefLost (sr);
			    }
			}
		      else if (sRef_isNewRef (sr))
			{
			  if (optgenerror
			      (sRef_isFresh (sr) ? FLG_MUSTFREEFRESH : FLG_MUSTFREEONLY,
			       message 
			       ("%q %q not released before %q",
				cstring_makeLiteral 
				(alkind_isKillRef (sRef_getOrigAliasKind (sr))
				 ? "Kill reference parameter" : "New reference"),
				uentry_getName (ce),
				cstring_makeLiteral (isReturn
						     ? "return" : "scope exit")),
			       g_currentloc))
			    {
			      sRef_showAliasInfo (sr);
			    }
			}
		      else 
			{
			  if (ctype_isRealSU (sRef_getType (sr)))
			    {
			      transferChecks_structDestroyed (sr, g_currentloc);
			    }
			  else
			    {
			      if (optgenerror
				  (sRef_isFresh (sr) ? FLG_MUSTFREEFRESH : FLG_MUSTFREEONLY,
				   message 
				   ("%s storage %q not %q before %q",
				    alkind_capName (sRef_getAliasKind (sr)),
				    uentry_getName (ce),
				    cstring_makeLiteral (sRef_isKeep (sr) 
							 ? "transferred" : "released"),
				    cstring_makeLiteral (isReturn 
							 ? "return" : "scope exit")),
				   g_currentloc))
				{
				  sRef_showAliasInfo (sr);
				  DPRINTF (("Storage: %s", sRef_unparseFull (sr)));
				}
			    }
			}
		    }
		}
	      else
		{
		  ;
		}
	    }

	  if (mustDefine && uentry_isOut (ce))
	    {
	      /* No error if its dead (either only or error already reported */
	      if (!sRef_isReallyDefined (sr) && !sRef_isDead (sr))
		{
		  voptgenerror 
		    (FLG_MUSTDEFINE,
		     message ("Out storage %q not defined before %q",
			      uentry_getName (ce),
			      cstring_makeLiteral 
			      (isReturn ? "return" : "scope exit")),
		     g_currentloc);

		  DPRINTF (("sr: %s", sRef_unparseFull (sr)));
		}
	    }
	  
	  /*
	  ** also check state is okay
	  */
	  
	  if (usymtab_lexicalLevel () > functionScope
	      && uentry_isVariable (ce)
	      && (sRef_isLocalVar (sr)
		  && (sRef_isDependent (sr) || sRef_isLocalState (sr))))
	    {
	      sRefSet ab = usymtab_aliasedBy (sr);
	      
	      /* should do something more efficient here */
	      
	      if (sRefSet_isEmpty (ab))
		{
		  /* and no local ref */
		  DPRINTF (("Check lose ref: %s", uentry_unparseFull (ce)));
		  transferChecks_loseReference (ce);
		}
	      else
		{
		  ;
		}
	      
	      sRefSet_free (ab);
	    }
	  else 
	    {
	      ;
	    }
	  
	  checked = sRefSet_insert (checked, sr);
	}

      llassert (usymtab_isDefined (stab->env));

      if (usymtab_isBranch (stab))
	{
	  stab = usymtab_dropEnv (stab);
	}
      else
	{
	  stab = stab->env;
	}
      
      llassert (stab != usymtab_undefined);
    } while (isReturn && (stab->lexlevel >= paramsScope));
  
  sRefSet_free (checked);
  
  /*
  ** for returns:
  **      all globals are appropriately defined
  **      all parameters are appropriately defined
  **      special clauses are followed
  */

  if (isReturn || (utab->lexlevel == paramsScope))
    {
      uentry fcn = context_getHeader ();
      uentryList params = context_getParams ();
      globSet uglobs = context_getUsedGlobs ();
      globSet sglobs = context_getGlobs ();
            
      if (isReturn && context_maybeSet (FLG_GLOBALIAS))
	{ 
	  aliasTable_checkGlobs (utab->aliases); 
	}

      /*
      ** state clauses (ensures, defines, sets, allocates, releases) 
      */
      
      if (uentry_hasStateClauseList (fcn))
	{
	  stateClauseList clauses = uentry_getStateClauseList (fcn);

	  stateClauseList_elements (clauses, cl)
	    {
	      if (stateClause_isAfter (cl) && !stateClause_isGlobal (cl)) 
		{ 
		  if (stateClause_setsMetaState (cl)) 
		    {
		      sRefSet rfs = stateClause_getRefs (cl);
		      qual q = stateClause_getMetaQual (cl);
		      annotationInfo ainfo = qual_getAnnotationInfo (q);
		      metaStateInfo minfo = annotationInfo_getState (ainfo);
		      cstring key = metaStateInfo_getName (minfo);
		      int mvalue = annotationInfo_getValue (ainfo);

		      DPRINTF (("Post meta state clause: %s", stateClause_unparse (cl)));

		      sRefSet_elements (rfs, el)
			{
			  sRef base = sRef_getRootBase (el);
			  
			  if (sRef_isResult (base))
			    {
			      /* 
			      ** This is checked for return transfers. 
			      */
			      ;
			    }
			  else if (sRef_isParam (base) || sRef_isGlobalMarker (base))
			    {
			      sRef sr = sRef_updateSref (base);
			      sr = sRef_fixBase (el, sr);
			      
			      if (!sRef_checkMetaStateValue (sr, key, mvalue))
				{
				  if (optgenerror 
				      (FLG_STATETRANSFER,
				       message
				       ("Ensures clause not satisfied%q (state is %q): %q",
					sRef_isGlobalMarker (sr) 
					? message ("") 
					: message (" by %q", sRef_unparse (sr)),
					stateValue_unparseValue (sRef_getMetaStateValue (sr, key), 
								 minfo),
					stateClause_unparse (cl)),
				       g_currentloc))
				    {
				      sRef_showMetaStateInfo (sr, key);
				    }  
				}
			    }
			  else
			    {
			      if (sRef_isMeaningful (el))
				{
				  BADBRANCH;
				}
			    }
			} end_sRefSet_elements ;
		    }
		  else
		    {
		      /* evs - 2000 07 10 - added this */
		      sRefTest tst = stateClause_getPostTestFunction (cl);
		      sRefSet rfs = stateClause_getRefs (cl);
		      
		      sRefSet_elements (rfs, el)
			{
			  sRef base = sRef_getRootBase (el);
			  
			  if (sRef_isResult (base))
			    {
			      /* 
			      ** This is checked for return transfers. 
			      */

			      ; 
			    }
			  else if (sRef_isParam (base))
			    {
			      sRef sr = sRef_updateSref (base);
			      sr = sRef_fixBase (el, sr);
			      
			      if (tst != NULL && !tst (sr))
				{
				  if (optgenerror 
				      (stateClause_postErrorCode (cl),
				       message ("%s storage %qcorresponds to "
						"storage listed in %q clause",
						stateClause_postErrorString (cl, sr),
						sRef_unparseOpt (sr),
						stateClause_unparseKind (cl)),
				       g_currentloc))
				    {
				      sRefShower ss = stateClause_getPostTestShower (cl);
				      
				      if (ss != NULL)
					{
					  ss (sr);
					}
				      
				      DPRINTF (("Storage: %s", sRef_unparseFull (sr)));
				    }  
				}
			    }
			  else
			    {
			      if (sRef_isMeaningful (el))
				{
				  BADBRANCH;
				}
			    }
			} end_sRefSet_elements ;
		    }
		}
	    } end_stateClauseList_elements ;
	}
      
      /*
      ** check parameters on return
      */

      uentryList_elements (params, arg)
	{
	  if (!uentry_isElipsisMarker (arg))
	    {
	      ctype rt = ctype_realType (uentry_getType (arg));

	      if (ctype_isMutable (rt) || ctype_isSU (rt))
		{
		  uentry param = usymtab_lookupQuiet (utab, uentry_rawName (arg));
		  DPRINTF (("Check param return: %s", uentry_unparseFull (param)));
		  transferChecks_paramReturn (param);
		}
	    }
	} end_uentryList_elements;
      
      DPRINTF (("Check global return: %s",
		globSet_unparse (sglobs)));

      globSet_allElements (sglobs, el)
	{
	  sRef orig = el; /*! sRef_updateSref (el); */ /*!!!*/
	  uentry current = sRef_getUentry (el);

	  DPRINTF (("Check global return: %s / %s", sRef_unparseFull (el),
		    uentry_unparseFull (current)));
	  
	  if (uentry_isVariable (current) && !uentry_isRealFunction (current))
	    {
	      checkGlobalReturn (current, orig);
	    }
	} end_globSet_allElements;

      globSet_allElements (uglobs, el)
	{
	  if (!globSet_member (sglobs, el))
	    {
	      uentry current = sRef_getUentry (el);
	      
	      if (uentry_isVariable (current)
		  && !uentry_isRealFunction (current))
		{
		  checkGlobalReturn (current, sRef_undefined);
		}
	    }
	} end_globSet_allElements;
    }  
}

void
usymtab_quietExitScope (fileloc loc) 
   /*@globals utab, globtab, filetab; @*/ 
   /*@modifies utab@*/
{
  usymtab t = utab->env;

  DPRINTF (("Quiet exit scope [%p]", utab));

  if (utab->reftable != NULL)
    {
      int i;

      for (i = 0; i < utab->nentries; i++)
	{
	  uentry current = utab->entries[i];	  
	  uentry old = usymtab_lookupAux (t, uentry_rawName (current));

	  uentry_mergeState (old, current, loc, FALSE, FALSE, FALSE, NOCLAUSE);
	}
    }

  llassert (t != NULL);

  if (t->lexlevel > paramsScope)
    {
      t->guards = guardSet_levelUnion (t->guards, utab->guards, t->lexlevel);
      t->aliases = aliasTable_levelUnionSeq (t->aliases, utab->aliases, 
					     t->lexlevel);
      utab->aliases = aliasTable_undefined;
    }

  t->mustBreak = utab->mustBreak;
  t->exitCode = utab->exitCode;

  DPRINTF (("Free level [%p]", utab));
  usymtab_freeLevel (utab);

  utab = t;

# ifdef DEBUGSPLINT
  usymtab_checkAllValid ();
# endif
}

/*
** Exit a scope with no checking, lose alias states.
** (When should this be used?)
*/

void usymtab_quietPlainExitScope (void)
     /*@globals utab, globtab, filetab@*/
     /*@modifies utab@*/
{
  usymtab t = utab->env;

  llassert (t != NULL);
  llassert (NOALIAS (utab->aliases, t->aliases));
  usymtab_freeLevel (utab);
  utab = t;
}

void usymtab_exitScope (exprNode expr)
  /*@globals utab, filetab, globtab@*/
  /*@modifies utab, globtab@*/
{
  usymtab ctab = usymtab_undefined;
  usymtab lctab = usymtab_undefined;
  bool mustReturn = exprNode_mustEscape (expr);

  DPRINTF (("Exit scope [%p]", utab));

  if (utab->kind == US_CBRANCH)
    {
      /*
      ** save the case branches, remove the first non-cbranch
      */

      ctab = utab;

      while (utab->kind == US_CBRANCH) 
	{
	  lctab = utab;
	  utab = utab->env;
	  llassert (utab != GLOBAL_ENV);
	}
    }
  
  if (utab->kind == US_TBRANCH || utab->kind == US_FBRANCH
      || utab->kind == US_CBRANCH || utab->kind == US_SWITCH) 
    {
      if (context_inMacro ()) 
	{
	  /* evs 2000-07-25 */
	  /* Unparseable macro may end inside nested scope.  Deal with it. */
	  
	  llerror (FLG_SYNTAX, 
		   message ("Problem parsing macro body of %s (unbalanced scopes). "
			    "Attempting to recover, recommend /*@notfunction@*/ before "
			    "macro definition.", 
			    context_inFunctionName ()));
	  
	  while (utab->kind == US_TBRANCH
		 || utab->kind == US_FBRANCH
		 || utab->kind == US_CBRANCH
		 || utab->kind == US_SWITCH) 
	    {
	      utab = utab->env;
	      llassert (utab != GLOBAL_ENV);
	    }
	} else 
	  {
	    llcontbug (message ("exitScope: in branch: %q", usymtab_unparseStack ()));
	    /*@-branchstate@*/ 
	  } /*@=branchstate@*/
    }
  
  /*
  ** check all variables in scope were used
  */

  /*
  ** bogus errors if this is the normal inside a switch,
  ** since cases have not been merged yet.  Should probably
  ** still check this, but I'm too lazy at the moment...
  */

  llassertfatal (utab->env != GLOBAL_ENV);

  if (utab->env->kind != US_SWITCH)
    {
      usymtab_allUsed ();
    }

  /*
  ** check aliasing: all only params are released (dead)
  **     definition: all out params are defined, all modified params 
  **                     are completely defined
  **
  ** NOTE: note for exiting paramsScope, since checkReturn should be
  ** called first.
  */
  
  if (!mustReturn && (usymtab_lexicalLevel () > functionScope))
    {
      /*
      ** should only call this is end of scope is reachable...
      */

      usymtab_checkFinalScope (FALSE);
    }

  if (usymtab_lexicalLevel () == paramsScope && context_inFunctionLike ())
    {
      /*
      ** leaving a function, need to fix up globals
      */

      uentryList params = context_getParams ();
      globSet    globs = context_getUsedGlobs ();

      uentryList_elements (params, ue)
	{
	  uentry_fixupSref (ue);
	} end_uentryList_elements;

      clearFunctionTypes ();

      DPRINTF (("Fixing up globals: %s", globSet_unparse (globs)));

      globSet_allElements (globs, el)
	{
	  DPRINTF (("Fix: %s", sRef_unparseDebug (el)));

	  if (sRef_isCvar (el))
	    {
	      uentry current;
	      usymId index = sRef_getScopeIndex (el);
	      
	      if (sRef_isFileStatic (el))
		{
		  llassert (usymtab_isDefined (filetab));
		  current = usymtab_fetchIndex (filetab, index);
		}
	      else
		{
		  current = usymtab_fetchIndex (globtab, index);
		}
	      
	      if (uentry_isVariable (current))
		{
		  DPRINTF (("Fixup: %s", uentry_unparse (current)));
		  uentry_fixupSref (current);
		}
	      else
		{
		  DPRINTF (("Clear: %s", uentry_getSref (current)));
		  sRef_clearDerived (uentry_getSref (current));
		}
	    }

	  sRef_clearDerived (el); /* evans 2002-03-14 - this is the likely source of many crashes! */
	} end_globSet_allElements;
    }
  
  usymtab_quietExitScope (exprNode_loc (expr));
  
  if (lctab != usymtab_undefined)
    {
      /*@i@*/ lctab->env = utab;  
      /*@i@*/ utab = ctab;
    /*@-branchstate@*/ } /*@=branchstate@*/
  /*@-globstate@*/


# ifdef DEBUGSPLINT
  usymtab_checkAllValid ();
# endif
}
/*@=globstate@*/

/*
** yikes!  don't let the '170 kids see this one...
*/

usymId
usymtab_directParamNo (uentry ue)
{
  if (uentry_isVar (ue))
    {
      sRef sr = uentry_getSref (ue);

      if (sRef_lexLevel (sr) == functionScope)
	{
	  usymId index = sRef_getScopeIndex (sr);

	  if (index < usymId_fromInt (uentryList_size (context_getParams ())))
	    {
	      return index;
	    }
	}
    }
  return usymId_invalid;
}

/*@dependent@*/ /*@exposed@*/ uentry
  usymtab_getParam (int paramno)
  /*@globals utab@*/
{
  /*
  ** requires in a function context (checked)
  **
  ** depends on no nested functions --- the function
  ** parameters are ALWAYS one scope inside the global scope
  ** and entered in order!
  */
  usymtab s = utab;

  if (!context_inFunctionLike ())
    llfatalbug (message ("usymtab_getParam: not in function context: %q", 
			 context_unparse ()));

  while (s->lexlevel > paramsScope) 
    {
      s = s->env;
    }

  llassert (usymtab_isDefined (s));

  if (paramno >= s->nentries)
    {
      /*  
      ** Parse errors lead to this. 
      */

      uentry err = uentry_makeVariableLoc (cstring_makeLiteralTemp ("<error>"),
					   ctype_unknown);
      
      uentry_markOwned (err);
      return (err);
    }

  return (s->entries[paramno]);
}

static /*@dependent@*/ /*@exposed@*/ uentry 
usymtab_getRefTab (/*@notnull@*/ usymtab u, int level, usymId index)
{
  uentry ue;

  ue = usymtab_getRefNoisy (u, level, index);

  if (uentry_isUndefined (ue))
    {
      llbug (message ("usymtab_getRef: out of range: %d. level = %d",
		    index, level));
    }
  
  return ue;
}

static /*@dependent@*/ /*@exposed@*/ usymtab 
  usymtab_dropEnv (/*@notnull@*/ usymtab s)
{
  if (s->kind == US_CBRANCH)
    {
      usymtab t = s;

      do 
	{
	  t = s;
	  s = s->env;
	  llassert (s != GLOBAL_ENV);
	} while (s->kind == US_CBRANCH); 
      /* drop all cases (except in nested scopes */ 

      s = t;
      llassert (s != GLOBAL_ENV);
    }

  if (s->kind == US_FBRANCH)
    {
      s = s->env; /* skip the true branch */
      llassert (usymtab_isDefined (s));
      llassert (s->kind == US_TBRANCH);
    }

  llassert (s != GLOBAL_ENV);
  s = s->env;

  return s;
}

/*@dependent@*/ /*@exposed@*/ uentry
  usymtab_getRefQuiet (int level, usymId index)
  /*@globals utab@*/
{
  usymtab s = utab;
  
  
  llassert (s != NULL);
  llassert (index >= 0);

  if (level > s->lexlevel)
    {            
      return uentry_undefined;
    }

  llassertprint (level <= s->lexlevel, ("level: %d / lexlevel: %d", 
					level, s->lexlevel)); 

  while (s->lexlevel > level)
    {
      if (usymtab_isBranch (s))
	{
	  int eindex = refTable_lookup (s, level, index);

	  if (eindex != NOT_FOUND)
	    {
	      return (s->entries[eindex]);
	    }
	}

      s = usymtab_dropEnv (s);
    }

  while (usymtab_isBranch (s) && s->lexlevel == level)
    {
      int eindex = refTable_lookup (s, level, index);
      
      if (eindex != NOT_FOUND)
	{
	  return (s->entries[eindex]);
	}

      s = usymtab_dropEnv (s);
    }
 
  if (index >= usymId_fromInt (s->nentries))
    {
      return uentry_undefined;
    }

  llassert (!uentry_isUndefined (s->entries[index]));

  return s->entries[index];
}

static /*@dependent@*/ /*@exposed@*/ uentry
usymtab_getRefNoisy (/*@notnull@*/ usymtab s, int level, usymId index)
{
  usymtab otab = s;
  uentry ue = uentry_undefined;
  
  llassert (index >= 0);
  
  while (s->lexlevel > level)
    {
      if (usymtab_isBranch (s))
	{
	  int eindex = refTable_lookup (s, level, index);

	  if (eindex != NOT_FOUND)
	    {
	      ue = s->entries[eindex];

	      if (s != otab)
		{
		  while (!usymtab_isBranch (otab))
		    {
		      otab = usymtab_dropEnv (otab);
		      llassert (otab != GLOBAL_ENV);
		    }
		
		  if (refTable_lookup (otab, level, index) == NOT_FOUND)
		    {
		      ue = usymtab_addRefEntry (otab, uentry_rawName (ue));
		    }
		  else
		    {
		      ;
		    }
		}
	      
	      return ue;
	    }
	}

      s = usymtab_dropEnv (s);
    }

  llassert (usymtab_isDefined (s));

  while (usymtab_isBranch (s) && s->lexlevel == level)
    {
      int eindex = refTable_lookup (s, level, index);
      
      if (eindex != NOT_FOUND)
	{
	  ue = s->entries[eindex];
	  
	  if (s != otab)
	    {
	      while (!usymtab_isBranch (otab))
		{
		  otab = usymtab_dropEnv (otab);
		  llassert (otab != GLOBAL_ENV);
		}

	      ue = usymtab_addRefEntry (otab, uentry_rawName (ue));
	    }
	  else
	    {
	      ;
	    } 

	  return ue;
	}

      s = usymtab_dropEnv (s);
          }

  if (s->lexlevel == level && (index < usymId_fromInt (s->nentries)))
    {
      ue = s->entries[index];
      
      if (uentry_isValid (ue))
	{
	  if (s != otab)
	    {
	      while (!usymtab_isBranch (otab))
		{
		  otab = usymtab_dropEnv (otab);
		  
		  if (otab == GLOBAL_ENV)
		    {
		      return ue;
		    }
		}
	      
	      ue = usymtab_addRefEntry (otab, uentry_rawName (ue));
	    }
	  else
	    {
	    }
	}
      
      return ue;
    }

  
  if (index >= usymId_fromInt (s->nentries))
    {
      return uentry_undefined;
    }

  llassert (!uentry_isUndefined (s->entries[index]));

  return s->entries[index];
}

/*
** looking up entries
**
** If entry is inside a branch, then copy it, and put it into 
** the branch table.
*/

static
int refTable_lookup (/*@notnull@*/ usymtab ut, int level, usymId index)
{
  refTable rt = ut->reftable;
  int i;

  llassert (rt != NULL);

  for (i = 0; i < ut->nentries; i++)
    {
      if (rt[i]->level == level && rt[i]->index == usymId_toInt (index))
	{
	  return i;
	}
    }
  
  return NOT_FOUND;
}
  
static
/*@only@*/ refentry refentry_create (int level, int index)
{
  refentry r = (refentry) dmalloc (sizeof (*r));

  r->level = level;
  r->index = index;

  return r;
}

static /*@dependent@*/ /*@exposed@*/ uentry
usymtab_addRefEntry (/*@notnull@*/ usymtab s, cstring k)
{
  usymtab ut = s;

  if (ut->reftable == NULL) 
    {
      DPRINTF (("Adding ref entry without reftable: %s", k));
      return uentry_undefined;
    }

  llassert (ut->reftable != NULL);
  
  while (s != GLOBAL_ENV)
    {
      usymId eindex = usymtab_getIndex (s, k);
      
      if (usymtab_indexFound (eindex))
	{
	  uentry current = s->entries[eindex];

	  if (uentry_isVar (current) && !ctype_isFunction (uentry_getType (current)))
	    {
	      uentry ue;

	      DPRINTF (("Here: copying %s", uentry_unparse (current)));
	      if (uentry_isNonLocal (current))
		{
		  ue = uentry_copy (current);
		}
	      else
		{
		  ue = uentry_copyNoSave (current);
		}

	      DPRINTF (("Here: copying %s", uentry_unparse (ue)));
	      usymtab_addEntryQuiet (ut, ue);
	      DPRINTF (("Okay..."));
	      
	      if (s->reftable != NULL)
		{
		  refentry ref = s->reftable[eindex];
		  
		  ut->reftable[ut->nentries - 1] 
		    = refentry_create (ref->level, ref->index);
		}
	      else
		{
		  ut->reftable[ut->nentries - 1] 
		    = refentry_create (s->lexlevel, usymId_toInt (eindex));
		}
	      
	      return (ue);
	    }
	  else
	    {
	      return (current);
	    }
	}

      s = usymtab_dropEnv (s);
    }

  return uentry_undefined;
}

static uentry usymtab_lookupAux (usymtab s, cstring k)
{
  DPRINTF (("Lookup: %s", k));

  while (s != GLOBAL_ENV)
    {
      usymId eindex = usymtab_getIndex (s, k);

      if (usymtab_indexFound (eindex))
	{
	  uentry ret = s->entries[eindex];
# if 0	  

	  
	  if (s->kind == US_TBRANCH 
	      || s->kind == US_FBRANCH
	      || s->kind == US_CBRANCH)
	      /* uentry_isGlobalVariable (ret) && os->lexlevel > fileScope) */
	    {
	      uentry ret;
	      DPRINTF (("Adding global ref entry: %s", k));
	      ret = usymtab_addRefEntry (os, k);
	      DPRINTF (("Adding ref entry: %s", uentry_unparseFull (ret)));
	      return ret;
	    }

# endif
	  DPRINTF (("Found: %s", uentry_unparseFull (ret)));
	  return (ret);
	}
      
      if (s->kind == US_TBRANCH || s->kind == US_FBRANCH 
	  || s->kind == US_CBRANCH)
	{
	  /* why isn't this os??? */
	  uentry ret = usymtab_addRefEntry (s, k);
	  DPRINTF (("Adding ref entry: %s", uentry_unparseFull (ret)));
	  return ret;
	}
      
      s = s->env;
    }
  
  return uentry_undefined;
}

static /*@dependent@*/ /*@exposed@*/ uentry
usymtab_lookupQuietAux (usymtab s, cstring k, bool noalt)
{
  while (s != GLOBAL_ENV)
    {
      usymId eindex = usymtab_getIndex (s, k);
      
      if (usymtab_indexFound (eindex))
	{
	  uentry ret = s->entries[eindex];
	  return (ret);
	}
      
      if (noalt && usymtab_isBranch (s))
	{
	  s = usymtab_dropEnv (s);
	}
      else
	{
	  llassert (s != NULL); 
	  s = s->env;
	}
    }

  return uentry_undefined;
}

static /*@exposed@*/ /*@dependent@*/ uentry 
usymtab_lookupQuiet (usymtab s, cstring k)
{
  return usymtab_lookupQuietAux (s, k, FALSE);
}

static /*@exposed@*/ /*@dependent@*/ uentry 
usymtab_lookupQuietNoAlt (usymtab s, cstring k)
{
  return usymtab_lookupQuietAux (s, k, TRUE);
}

/*@dependent@*/ /*@observer@*/ uentry
  usymtab_lookupSafe (cstring k)
  /*@globals utab@*/
{
  DPRINTF (("Lookup safe: %s", k));
  return (usymtab_lookupAux (utab, k));
}

/*@dependent@*/ /*@observer@*/ uentry
  usymtab_lookupSafeScope (cstring k, int lexlevel)
  /*@globals utab@*/
{
  /*
  ** This is necessary to deal with shadowed variables that are referenced
  ** through aliases inside the shadowed scope.  It would be better if
  ** lookup could take an sRef as a parameter.
  */

  usymtab tab = utab;

  while (tab != GLOBAL_ENV && tab->lexlevel > lexlevel) {
    uentry ret = usymtab_lookupAux (tab, k);
    
    if (uentry_isValid (ret)) {
      sRef sr = uentry_getSref (ret);
      
      if (sRef_isCvar (sr) && sRef_lexLevel (sr) > lexlevel) {
	tab = usymtab_dropEnv (tab);
      } else {
	return ret;
      }
    }
  }

  return uentry_undefined;
}

uentry
  usymtab_lookupExpose (cstring k)
  /*@globals utab@*/
{
  uentry ce = usymtab_lookupAux (utab, k);

  if (uentry_isUndefined (ce))
    {
      llfatalbug (message ("usymtab_lookup: not found: *%s*", k));
    }

  if (uentry_isPriv (ce))
    {
      llfatalbug (message ("usymtab_lookup: private: *%s*", k));
    }

  return ce;
}

uentry usymtab_lookupExposeGlob (cstring k)
{
  return (usymtab_lookupGlobSafe (k));
}

uentry usymtab_lookupGlob (cstring k)
  /*@globals globtab@*/
{
  uentry ce = usymtab_lookupAux (globtab, k);

  if (uentry_isUndefined (ce))
    llfatalbug (message ("usymtab_lookup: not found: %s", k));

  if (uentry_isPriv (ce))
    llfatalbug (message ("usymtab_lookup: private: %s", k));

  DPRINTF (("Lookup global: %s", uentry_unparseFull (ce)));
  return ce;
}

/*@observer@*/ uentry
  usymtab_lookupGlobSafe (cstring k)
  /*@globals globtab@*/
{
  uentry ce = usymtab_lookupAux (globtab, k);
  DPRINTF (("Lookup global: %s", uentry_unparseFull (ce)));
  return ce;
}

uentry usymtab_lookupEither (cstring k)
   /*@globals utab@*/
{
  uentry ce = usymtab_lookupSafe (k);

  if (uentry_isUndefined (ce))
    llfatalerror (message ("usymtab_lookup: not found: %s", k));

  DPRINTF (("Lookup either: %s", uentry_unparseFull (ce)));
  return ce;
}

ctype
usymtab_lookupType (cstring k)
   /*@globals globtab@*/
{
  typeId uid = usymtab_getTypeId (k);

  if (typeId_isInvalid (uid))
    {
      llcontbug (message ("usymtab_lookupType: not found: %s", k));
      return ctype_unknown;
    }
  
  return (uentry_getRealType (usymtab_getTypeEntry (uid)));
}

ctype
usymtab_lookupAbstractType (cstring k) /*@globals globtab@*/
{
  typeId uid = usymtab_getTypeId (k);

  if (typeId_isInvalid (uid))
    {
      llcontbug (message ("usymtab_lookupType: not found: %s", k));
      return ctype_unknown; 
    }
  
  return (uentry_getAbstractType (usymtab_getTypeEntry (uid)));
}
  
/*
** if there is an unnamed lcl-specified struct tag matching
** the uentryList, return its datatype.  Otherwise, returns
** ctype_undefined.
*/

ctype
usymtab_structFieldsType (uentryList f)
   /*@globals globtab@*/
{
  return (usymtab_suFieldsType (f, TRUE));
}

ctype
usymtab_unionFieldsType (uentryList f)
   /*@globals globtab@*/
{
  return (usymtab_suFieldsType (f, FALSE));
}

static ctype
usymtab_suFieldsType (uentryList f, bool isStruct)
  /*@globals globtab@*/
{
  int i;

  DPRINTF (("Fields: %s", uentryList_unparse (f)));

  if (fileloc_isSpec (g_currentloc)) 
    {
      return (ctype_undefined);
    }

  for (i = 0; i < globtab->nentries; i++)
    {
      uentry current = globtab->entries[i];

      if ((isStruct 
	   ? uentry_isStructTag (current) : uentry_isUnionTag (current)))
	{
	  if (isFakeTag (uentry_rawName (current)))
	    {
	      ctype ct = uentry_getType (current);
	      
	      DPRINTF (("Check: %s", ctype_unparse (ct)));

	      if ((isStruct ? ctype_isStruct (ct) : ctype_isUnion (ct))
		  && 
		  (uentry_isSpecified (current)
		   && uentryList_equivFields (f, ctype_getFields (ct))))
		{
		  return uentry_getAbstractType (current);
		}
	      else
		{
		  ;
		}
	    }
	}
    }
  
    return ctype_undefined;
}

ctype
  usymtab_enumEnumNameListType (enumNameList f)
  /*@globals globtab@*/
{
  int i;

  for (i = 0; i < globtab->nentries; i++)
    {
      uentry current = globtab->entries[i];

      if (uentry_isEnumTag (current))
	{
	  if (isFakeTag (uentry_rawName (current)))
	    {
	      ctype ct = uentry_getType (current);

	      if (ctype_isEnum (ct) && (enumNameList_match (f, ctype_elist (ct))))
		{
		  return uentry_getType (current);
		}
	    }
	}
    }
  
  return ctype_undefined;
}

bool
usymtab_exists (cstring k)
   /*@globals utab@*/
{
  uentry ce = usymtab_lookupSafe (k);
  return (!(uentry_isUndefined (ce)) && !(uentry_isPriv (ce)));
}

bool
usymtab_existsReal (cstring k)
   /*@globals utab@*/
{
  uentry ce = usymtab_lookupSafe (k);

  return (!(uentry_isUndefined (ce)) 
	  && !(uentry_isPriv (ce))
	  && !(uentry_isExpandedMacro (ce)));
}

bool
  usymtab_existsGlob (cstring k)
  /*@globals globtab@*/
{
  uentry ce = usymtab_lookupAux (globtab, k);

  return (!(uentry_isUndefined (ce)) && !(uentry_isPriv (ce)));
}

bool
usymtab_existsEither (cstring k)
  /*@globals utab@*/
{
  uentry ce = usymtab_lookupAux (utab, k);
  
  return (uentry_isValid (ce));
}

bool
  usymtab_existsGlobEither (cstring k)
  /*@globals globtab@*/
{
  uentry ce = usymtab_lookupAux (globtab, k);
  
  return (uentry_isValid (ce));
}

bool
usymtab_existsType (cstring k)
  /*@globals globtab@*/
{
  uentry ce = usymtab_lookupAux (globtab, k);

  return (!(uentry_isUndefined (ce)) && !(uentry_isPriv (ce)) && uentry_isDatatype (ce));
}

bool
usymtab_existsTypeEither (cstring k)
  /*@globals globtab@*/
{
  uentry ce;
  ce = usymtab_lookupAux (globtab, k);
  return (uentry_isValid (ce) && uentry_isDatatype (ce));
}

bool
usymtab_existsStructTag (cstring k) /*@globals globtab@*/
{
  cstring sname = makeStruct (k);
  uentry ce = usymtab_lookupAux (globtab, sname);
  cstring_free (sname);  
  return (!(uentry_isUndefined (ce)) && !(uentry_isPriv (ce)));
}

bool
usymtab_existsUnionTag (cstring k) /*@globals globtab@*/
{
  cstring uname = makeUnion (k);
  uentry ce = usymtab_lookupAux (globtab, uname);

  cstring_free (uname);

  return (!(uentry_isUndefined (ce)) && !(uentry_isPriv (ce)));
}

bool
usymtab_existsEnumTag (cstring k) /*@globals globtab@*/
{
  cstring ename = makeEnum (k);
  uentry ce = usymtab_lookupAux (globtab, ename);

  cstring_free (ename);
  return (!(uentry_isUndefined (ce)) && !(uentry_isPriv (ce)));
}

bool usymtab_existsVar (cstring k)
   /*@globals utab@*/
{
  uentry ce = usymtab_lookupSafe (k);

  return (!(uentry_isUndefined (ce)) && !(uentry_isPriv (ce)) && (uentry_isVar (ce)));
}

/*
** destructors
*/

static void
refTable_free (/*@only@*/ /*@null@*/ refTable x, int nentries)
{
  if (x != NULL)
    {
      int i;

      for (i = 0; i < nentries; i++)
	{
	  sfree (x[i]);
	}
      
      sfree (x);
    }
}
  
static void
usymtab_freeLevel (/*@notnull@*/ /*@only@*/ usymtab u)
  /*@globals globtab, utab, filetab@*/
{
  int i;

  DPRINTF (("Free level [%p]", u));
  aliasTable_free (u->aliases);

  refTable_free (u->reftable, u->nentries);

  if (u == filetab || u == globtab)
    {
      for (i = 0; i < u->nentries; i++)
	{
	  DPRINTF (("Free complete: %d", i));
	  DPRINTF (("Uentry: %s", uentry_unparse (u->entries[i])));
	  uentry_freeComplete (u->entries[i]);
	  u->entries[i] = uentry_undefined;
	}
    }
  else
    {
      for (i = 0; i < u->nentries; i++)
	{
	  uentry_free (u->entries[i]);
	  u->entries[i] = uentry_undefined;
	}
    }

  guardSet_free (u->guards);
  sfree (u->entries);

  if (u != globtab 
      && u != utab
      && u != filetab)
    {
      llassert (!cstringTable_isDefined (u->htable));
    }

  sfree (u); /* evans 2002-07-12: was inside if */
}

static void
usymtab_freeAux (/*@only@*/ usymtab u)
   /*@globals globtab, utab, filetab@*/
   /*@modifies u@*/
{
  while (u != GLOBAL_ENV)
    {
      usymtab t = u->env;
      usymtab_freeLevel (u);
      u = t;
      /*@-branchstate@*/ 
    } 
  /*@=branchstate@*/
}

void usymtab_free () 
  /*@globals killed utab, globtab, filetab@*/ 
  /*@modifies utab@*/
{
  dbgfree = TRUE;
  usymtab_freeAux (utab);
  utab = usymtab_undefined;
  /*@-globstate@*/
} /*@=globstate@*/ /* Splint cannot tell that utab is killed */

static int usymtab_lexicalLevel (void) /*@globals utab@*/
{
  return (utab->lexlevel);
}

bool usymtab_inGlobalScope () /*@globals utab, globtab@*/
{
  return (utab == globtab);
}

bool usymtab_inFileScope () /*@globals utab@*/
{
  return (utab->lexlevel == fileScope);
}

bool usymtab_inFunctionScope () /*@globals utab@*/
{
  return (utab->lexlevel == functionScope);
}

void
usymtab_replaceEntry (uentry s)
  /*@globals utab, globtab@*/
  /*@modifies utab, s@*/
{
  usymtab_replaceEntryAux (utab, s);
}

bool
usymtab_matchForwardStruct (typeId u1, typeId u2)
   /*@globals globtab@*/
{
  uentry ue1 = usymtab_getTypeEntry (u1);
  uentry ue2 = usymtab_getTypeEntry (u2);

  if (uentry_isAnyTag (ue2))
    {
      ctype reptype = uentry_getType (ue1);
      
      if (ctype_isPointer (reptype))
	{
	  ctype repbase = ctype_getBaseType (reptype);
  
	  if (ctype_isUA (repbase))
	    {
	      typeId rtuid = ctype_typeId (repbase);

	      if (u2 == rtuid) return TRUE;
	      
	      if (typeId_isValid (rtuid))
		{
		  reptype = uentry_getType (usymtab_getTypeEntry (rtuid));		  
		  return (ctype_isUA (reptype) && (u2 == (ctype_typeId (reptype))));
		}
	    }
	}
    }
  
  return FALSE;
}

void usymtab_addGuards (guardSet guards)
  /*@modifies utab@*/
{
  utab->guards = guardSet_union (utab->guards, guards);
  }

static bool usymtab_isGuardedAux (sRef s)
  /*@globals utab@*/
{
  usymtab tab = utab;
  sRef base = sRef_getRootBase (s);
  int lowlevel = paramsScope;
  int baselevel = sRef_lexLevel (base);

  if (sRef_isCvar (base))
    {
      lowlevel = baselevel;
      if (lowlevel < paramsScope) lowlevel = paramsScope;
    }
  
  while (tab->lexlevel >= lowlevel)
    {
      DPRINTF (("Is guarded? [%s] %s", 
		guardSet_unparse (tab->guards),
		sRef_unparseFull (s)));

      if (guardSet_isGuarded (tab->guards, s))
	{
	  /*
	  if (!sRef_definitelyNull (s))
	    {
	      sRef_setNotNull (s, fileloc_undefined);
	    }
	    */
	  return TRUE;
	}

      tab = usymtab_dropEnv (tab);
    }
  
  return FALSE;
}

void usymtab_unguard (sRef s) /*@modifies utab@*/
{
  usymtab tab = utab;
  sRef base = sRef_getRootBase (s);
  int lowlevel = paramsScope;
  int baselevel = sRef_lexLevel (base);
  
  if (sRef_isCvar (base))
    {
      lowlevel = baselevel;
      if (lowlevel < paramsScope) lowlevel = paramsScope;
    }

  while (tab->lexlevel >= lowlevel)
    {
      if (guardSet_isGuarded (tab->guards, s))
	{
	  guardSet_delete (tab->guards, s);
	}
      
      tab = usymtab_dropEnv (tab);
    }
}

bool usymtab_isGuarded (sRef s)
{
  DPRINTF (("Is guarded? %s", sRef_unparseFull (s)));
  return (sRef_aliasCompleteSimplePred (usymtab_isGuardedAux, s));
}

bool usymtab_isDefinitelyNull (sRef s)
{
  return (sRef_aliasCheckSimplePred (usymtab_isDefinitelyNullAux, s));
}

bool usymtab_isDefinitelyNullDeep (sRef s)
{
  return (sRef_deepPred (usymtab_isDefinitelyNull, s));
}

static bool usymtab_isDefinitelyNullAux (sRef s)
  /*@globals utab@*/
{
  usymtab tab = utab;
  sRef base = sRef_getRootBase (s);
  int  lowlevel = paramsScope;
  
  if (sRef_isCvar (base))
    {
      lowlevel = sRef_lexLevel (base);
      if (lowlevel < paramsScope) lowlevel = paramsScope;
    }
  
  while (tab->lexlevel >= lowlevel)
    {
      if (guardSet_mustBeNull (tab->guards, s))
	{
	  return TRUE;
	}
      
      while (tab->kind == US_CBRANCH) 
	{
	  tab = tab->env;
	}

      llassert (usymtab_isDefined (tab));

      if (tab->kind == US_FBRANCH)
	{
	  tab = tab->env;
	  llassert (tab->kind == US_TBRANCH);
	}
      
      tab = tab->env;
    }

  return FALSE;
}

void
usymtab_printGuards ()
  /*@globals utab, globtab@*/
{
  usymtab ttab = utab;

  while (ttab != globtab)
    {
      llmsg (message ("Guards [%d]: %q", ttab->lexlevel,
		      guardSet_unparse (ttab->guards)));
      ttab = ttab->env;
    }
}

void
usymtab_displayAllUses ()
  /*@globals utab, globtab@*/
{
  usymtab copy;

  /* only in top scope */
  llassert (utab == globtab);

  /* need a copy, so order is not messed up by sort! */  
  copy = usymtab_shallowCopy (globtab); 
  
  qsort (copy->entries, (size_t)copy->nentries, 
	 sizeof (*copy->entries), (int (*)(const void *, const void *)) uentry_xcompareuses);

  usymtab_entries (copy, ue)
    {
      if (uentry_isValid (ue) && !uentry_isGlobalMarker (ue))
	{
	  filelocList uses = uentry_getUses (ue);
	  int size = filelocList_realSize (uses);

	  if (fileloc_isDefined (uentry_whereDefined (ue)) 
	      && !fileloc_isLib (uentry_whereDefined (ue))
	      && (size > 0))
	    {
	      llmsg (message ("%q (%q), %d use%&:\n   %q", 
			      uentry_getName (ue),
			      fileloc_unparse (uentry_whereDefined (ue)),
			      size, filelocList_unparseUses (uses)));
	    }
	}
    } end_usymtab_entries;
  
  usymtab_shallowFree (copy);
}

static /*@dependent@*/ /*@exposed@*/ usymtab
usymtab_getFileTab ()
  /*@globals filetab@*/
{
  llassert (filetab != NULL);

  return filetab;
}

/*@only@*/ cstring
usymtab_unparseStack ()
  /*@globals utab@*/
{
  return (usymtab_unparseStackTab (utab));
}
 
static /*@only@*/ cstring
usymtab_unparseStackTab (usymtab t)
{
  bool firstOne = TRUE;
  cstring ret = cstring_makeLiteral ("[");

  while (t != GLOBAL_ENV)
    {
      if (firstOne)
	{
	  ret = message ("%q %q", ret, usymtab_typeName (t));
	  firstOne = FALSE;
	}
      else
	{
	  ret = message ("%q, %q", ret, usymtab_typeName (t));
	}
      t = t->env;
    }

  ret = message ("%q ]", ret);
  return ret;
}

static /*@only@*/ cstring
usymtab_typeName (/*@notnull@*/ usymtab t)
{
  switch (t->kind)
    {
    case US_GLOBAL:  return cstring_makeLiteral ("global");
    case US_NORMAL:  return cstring_makeLiteral ("normal");
    case US_TBRANCH: return cstring_makeLiteral ("true");
    case US_FBRANCH: return cstring_makeLiteral ("false");
    case US_CBRANCH: return cstring_makeLiteral ("case");
    case US_SWITCH:  return cstring_makeLiteral ("switch");
    }
  
  BADEXIT;
}

void usymtab_addMustAlias (/*@exposed@*/ sRef s, /*@exposed@*/ sRef al)
  /*@modifies utab@*/
{
  if (!sRef_similar (s, al))
    {
      usymtab_addForceMustAlias (s, al);
    }
}

/*
** Same as usymtab_addMustAlias, except does not check sRef_isSimilar.
*/

void usymtab_addForceMustAlias (/*@exposed@*/ sRef s, /*@exposed@*/ sRef al)
  /*@modifies utab@*/
{
  /* evans 2002-03-3: was sRef_isMeaningful -- but we need to keep aliases for new storage also! */
  if (sRef_isMeaningful (s) 
      && sRef_isMeaningful (al)
      && !(sRef_isConst (s) || sRef_isConst (al))
      && !(sRef_isAddress (al) && sRef_isDirectParam (sRef_getBase (al))))
    {
      utab->aliases = aliasTable_addMustAlias (utab->aliases, s, al); 
      DPRINTF (("Must alias: %s", aliasTable_unparse (utab->aliases)));

      /*
      ** for local variable, aliasing is symmetric 
      */
      
      if (sRef_isLocalVar (s) && sRef_isLocalVar (al))
	{
	  utab->aliases = aliasTable_addMustAlias (utab->aliases, al, s); 
	}
    }
  else
    {
      DPRINTF (("Not aliasing! %s / %s", sRef_unparseFull (s), sRef_unparseFull (al)));
      DPRINTF (("meaningful: %d %d", sRef_isMeaningful (s), sRef_isMeaningful (al)));
    }
}

void usymtab_addReallyForceMustAlias (/*@exposed@*/ sRef s, /*@exposed@*/ sRef al)
  /*@modifies utab@*/
{
  utab->aliases = aliasTable_addMustAlias (utab->aliases, s, al); 
}

void usymtab_clearAlias (sRef s)
  /*@modifies utab, s@*/
{
  
  aliasTable_clearAliases (utab->aliases, s); 
}

sRefSet usymtab_allAliases (sRef s)
   /*@globals utab@*/  
{
  if (sRef_isSomewhatMeaningful (s))
    {
      sRefSet ret;
            
      ret = sRefSet_unionFree (aliasTable_aliasedBy (utab->aliases, s),
			       aliasTable_canAlias (utab->aliases, s));
      return (ret);
    }
  else
    {
      DPRINTF (("NOT A MEANINGFUL SREF!"));
      return sRefSet_undefined;
    }
}

/*@only@*/ sRefSet usymtab_canAlias (sRef s)
     /*@globals utab@*/
{
  if (sRef_isSomewhatMeaningful (s))
    {
      sRefSet res = aliasTable_canAlias (utab->aliases, s);
      return res;
    }
  
  return sRefSet_undefined;
}

/*@only@*/ sRefSet usymtab_aliasedBy (sRef s)
     /*@globals utab@*/
{
  return (aliasTable_aliasedBy (utab->aliases, s));
}

/*@only@*/ cstring usymtab_unparseAliases ()
  /*@globals utab@*/
{
  return (aliasTable_unparse (utab->aliases));
}

/*
** Debugging routines:
**    okay to leak storage here, only for debugging 
*/

/*@-mustfree@*/ 

void
usymtab_printOut (void)
  /*@globals utab@*/
{
  int i;
  usymtab s = utab;
  int depth = 0;
  char *ind = mstring_copy ("               ");

  fprintf (g_warningstream, "<<< [symbol table] >>>\n");
  
  while (s != GLOBAL_ENV && s->env != GLOBAL_ENV)
    {
      cstring tname = usymtab_typeName (s);

      if (depth < 5)
	{
	  ind[depth * 3 + 1] = '\0';
	}
     
      fprintf (g_warningstream, "level: %d (%s)\n", s->lexlevel,
	       cstring_toCharsSafe (tname));

      cstring_free (tname);

      for (i = 0; i < s->nentries; i++)
	{
	  cstring us = uentry_unparseFull (s->entries[i]);
	  fprintf (g_warningstream, "%s\n", cstring_toCharsSafe (us));
	  cstring_free (us);
	}
      
      if (s->reftable != NULL && s->nentries > 0)
	{
	  fprintf (g_warningstream, "\t<< Ref table >>\n");

	  for (i = 0; i < s->nentries; i++)
	    {
	      fprintf (g_warningstream, "\t%s %3d: %d, %d\n", ind, i, 
		       s->reftable[i]->level,
		       s->reftable[i]->index);
	    }
	}

      ind[depth * 3 + 1] = ' ';
      depth++;
      s = s->env;
    }
  fprintf (g_warningstream, "<<< end usymtab >>>\n");
  mstring_free (ind);
  return;
}

void
usymtab_printTypes ()
  /*@globals globtab@*/
{
  usymtab_printAllAux (globtab);
}

void 
usymtab_printAll (void)
  /*@globals utab@*/
{
  usymtab_printAllAux (utab);
}

static void
usymtab_printAllAux (usymtab s)
   /*@modifies g_warningstream@*/
{
  int i;
  int depth = 0;
  char *ind = mstring_copy ("               ");

  printf ("[[[ usymtab ]]]");

  while (s != GLOBAL_ENV)
    {
      if (depth < 5)
	ind[depth * 3 + 1] = '\0';
      
      if (s->env == GLOBAL_ENV)
	{
	  int looplow;

	  printf ("level: %d / break: %s / exit: %s\n", s->lexlevel,
		  cstring_toCharsSafe (bool_unparse (s->mustBreak)), 
		  cstring_toCharsSafe (exitkind_unparse (s->exitCode)));

	  looplow = 0;

	  for (i = looplow; i < s->nentries; i++)
	    {
	      printf ("%s%3d. %s\n", ind, i, 
		      cstring_toCharsSafe (uentry_unparseFull (s->entries[i])));
	    }
	}
      else
	{
	  printf ("level: %d / break: %s / exit: %s\n", s->lexlevel,
		  cstring_toCharsSafe (bool_unparse (s->mustBreak)),
		  cstring_toCharsSafe (exitkind_unparse (s->exitCode)));

	  for (i = 0; i < s->nentries; i++)
	    {
	      printf ("%s%3d %s\n", ind, i, 
		     cstring_toCharsSafe (uentry_unparseFull (s->entries[i])));
	    }
	}
      
      ind[depth * 3 + 1] = ' ';
      depth++;
      s = s->env;
    }
  printf ("----------\n");
}

void
usymtab_printComplete ()
  /*@globals utab@*/
{
  int i;
  int depth = 0;
  char *ind = mstring_copy ("               ");
  usymtab s = utab;

  while (s != GLOBAL_ENV)
    {
      if (depth < 5)
	{
	  ind[depth * 3 + 1] = '\0';
	}
      
      if (s->env == GLOBAL_ENV)
	{
	  int looplow;

	  printf ("level: %d\n", s->lexlevel);

	  looplow = 0;

	  for (i = looplow; i < s->nentries; i++)
	    {
	      printf ("%s%3d %s\n", ind, i, 
		      cstring_toCharsSafe (uentry_unparseFull (s->entries[i])));
	    }
	}
      else
	{
	  printf ("level: %d\n", s->lexlevel);
	  for (i = 0; i < s->nentries; i++)
	    {
	      printf ("%s%3d %s\n", ind, i, 
		     cstring_toCharsSafe (uentry_unparseFull (s->entries[i])));
	    }
	}
      
      ind[depth * 3 + 1] = ' ';
      depth++;
      s = s->env;
    }

  printf ("----------\n");
  mstring_free (ind);
}

# ifdef S_SPLINT_S
static /*@only@*/ cstring /*@unused@*/ 
usymtab_unparseLocalAux (/*@notnull@*/ usymtab s)
{
  cstring c = message ("lexlevel: %d\n", s->lexlevel);
  int i;

  for (i = 0; i < s->nentries; i++)
    {
      c = message ("%q\n%q", c, uentry_unparseFull (s->entries[i]));
    }

  c = message ("%q\n=========", c);
  return (c);
}

static cstring /*@unused@*/ /*@only@*/ 
usymtab_unparseLocalList (/*@notnull@*/ usymtab s)
{
  cstring c = message ("[%d/%s/%s] ", s->lexlevel, 
		       bool_unparse (s->mustBreak), 
		       exitkind_unparse (s->exitCode));
  int i;

  for (i = 0; i < s->nentries; i++)
    {
      sRef sr = uentry_getSref (s->entries[i]);

      if (i == 0)
	{
	  c = message ("%q: %q [%b]", c, uentry_getName (s->entries[i]), 
		       sRef_isStateDefined (sr));
	}
      else
	{
	  c = message ("%q, %q [%b]", c, uentry_getName (s->entries[i]), 
		       sRef_isStateDefined (sr));
	}

    }

  return (c);
}
# endif

void
usymtab_printLocal (void)
  /*@globals utab@*/
{
  int i;
  usymtab s = utab;

  printf ("lexlevel: %d\n", s->lexlevel);

  for (i = 0; i < s->nentries; i++)
    {
      printf ("%s\n", cstring_toCharsSafe (uentry_unparseFull (s->entries[i])));
    }
  
  while (s->lexlevel > 1)
    {
      s = s->env;
    }

  llassert (usymtab_isDefined (s));

  printf ("Params:\n");

  for (i = 0; i < s->nentries; i++)
    {
      printf ("%d: %s\n", i, 
	      cstring_toCharsSafe (uentry_unparseFull (s->entries[i])));
    }
}
/*@=mustfree@*/

static bool checkDistinctExternalName (uentry e)
  /*@globals globtab@*/
  /*@modifies *g_warningstream@*/
{
  size_t checklen = size_fromInt (context_getValue (FLG_EXTERNALNAMELEN));
  bool ignorecase = context_getFlag (FLG_EXTERNALNAMECASEINSENSITIVE);
  bool gotone = FALSE;
  bool extras = FALSE;
  bool hasError = FALSE;
  cstring name = uentry_rawName (e);
  usymtab st = globtab;

  if (checklen == 0)
    {
      ;
    }
  else
    {
      if (uentry_isAnyTag (e)) 
	{
	  checklen++;  /* the tag marker doesn't count */
	}
    }

  usymtab_entries (st, oe)
    {
      if (uentry_sameObject (oe, e))
	{
	  continue;
	}

      if (checklen == 0)
	{
	  if (cstring_equalCaseInsensitive (uentry_rawName (oe), name))
	    {
	      if (gotone)
		{
		  extras = TRUE;
		  break;
		}
	      
	      if (optgenerror 
		  (FLG_DISTINCTEXTERNALNAMES,
		   message 
		   ("External identifier %q is not distinguishable from %q "
		    "because alphabetical case is ignored",
		    uentry_getName (e),
		    uentry_getName (oe)),
		   uentry_whereLast (e)))
		{
		  uentry_showWhereAny (oe);
		  uentry_setHasNameError (oe);
		  gotone = TRUE;
		}
	    }
	}
      else
	{
	  if (ignorecase)
	    {
	      if (cstring_equalLenCaseInsensitive (uentry_rawName (oe),
						   name, checklen))
		{
		  if (gotone)
		    {
		      extras = TRUE;
		      break;
		    }
		  
		  if (cstring_equalLen (uentry_rawName (oe), name, checklen))
		    {
		      if (optgenerror 
			  (FLG_DISTINCTEXTERNALNAMES,
			   /*@-sefparams@*/
			   message 
			   ("External identifier %q is not distinguishable from %q "
			    "in the first %d characters (%q)",
			    uentry_getName (e),
			    uentry_getName (oe),
			    size_toInt (checklen),
			    cstring_clip (uentry_getName (e), checklen)),
			   /*@=sefparams@*/
			   uentry_whereLast (e)))
			{
			  uentry_showWhereAny (oe);
			  uentry_setHasNameError (oe);
			  gotone = TRUE;
			}
		    }
		  else
		    {
		      if (gotone)
			{
			  extras = TRUE;
			  break;
			}
		      
		      if (optgenerror 
			  (FLG_DISTINCTEXTERNALNAMES,
			   message 
			   ("External identifier %q is not distinguishable from %q "
			    "in the first %d characters because alphabetical case "
			    "is ignored",
			    uentry_getName (e),
			    uentry_getName (oe),
			    size_toInt (checklen)),
			   uentry_whereLast (e)))
			{
			  uentry_showWhereAny (oe);
			  uentry_setHasNameError (oe);
			  gotone = TRUE;
			}
		    }
		}
	    }
	  else if (cstring_equalLen (uentry_rawName (oe), name, checklen))
	    {
	      if (gotone)
		{
		  extras = TRUE;
		  break;
		}
	      
	      if (optgenerror 
		  (FLG_DISTINCTEXTERNALNAMES,
		   /*@-sefparams@*/
		   message 
		   ("External identifier %q is not distinguishable from %q "
		    "in the first %d characters (%q)",
		    uentry_getName (e),
		    uentry_getName (oe),
		    size_toInt (checklen),
		    cstring_clip (uentry_getName (e), checklen)),
		   /*@=sefparams@*/
		   uentry_whereLast (e)))
		{
		  uentry_showWhereAny (oe);
		  uentry_setHasNameError (oe);
		  gotone = TRUE;
		}
	    }
	  else
	    {
	      ; /* okay */
	    }
	}
    } end_usymtab_entries ;

  hasError = gotone;
  
  if (extras)
    {
      llgenindentmsgnoloc
	(cstring_makeLiteral ("One or more additional "
			      "indistinguishable external "
			      "names not reported"));
    }

  return hasError;
}

static bool checkDistinctInternalName (uentry e)
  /*@globals utab@*/
  /*@modifies *g_warningstream@*/
{
  usymtab ttab = utab;
  cstring name = uentry_rawName (e);
  size_t numchars = size_fromInt (context_getValue (FLG_INTERNALNAMELEN));
  bool caseinsensitive = context_getFlag (FLG_INTERNALNAMECASEINSENSITIVE);
  bool lookalike = context_getFlag (FLG_INTERNALNAMELOOKALIKE);

  if (uentry_isAnyTag (e) && (numchars != 0))
    {
      numchars++;  /* the tag marker doesn't count */
    }
  
  while (usymtab_isDefined (ttab))
    {
      usymtab_entries (ttab, oe)
	{
	  if (uentry_sameObject (oe, e))
	    {
	      /*@innercontinue@*/ continue;
	    }

	  switch (cstring_genericEqual
		  (name, uentry_rawName (oe),
		   numchars, caseinsensitive, lookalike))
	    {
	    case CGE_DISTINCT: /* okay */
	      /*@switchbreak@*/ 
	      break;
	    case CGE_SAME:
	      if (cstring_equal (name, uentry_rawName (oe)))
		{
		  ; /* got a shadow error */
		}
	      else
		{
		  if (optgenerror 
		      (FLG_DISTINCTINTERNALNAMES,
		       /*@-sefparams@*/
		       message 
		       ("Internal identifier %q is not distinguishable from %q "
			"in the first %d characters (%q)",
			uentry_getName (e),
			uentry_getName (oe),
			size_toInt (numchars),
			cstring_clip (uentry_getName (e), numchars)),
		       /*@=sefparams@*/
		       uentry_whereLast (e)))
		    {
		      uentry_showWhereAny (oe);
		      uentry_setHasNameError (oe);
		      return TRUE;
		    }
		}
	      /*@switchbreak@*/
	      break;
	    case CGE_CASE:
	      if (numchars == 0 || (cstring_length (name) <= numchars))
		{
		  if (optgenerror 
		      (FLG_DISTINCTINTERNALNAMES,
		       message 
		       ("Internal identifier %q is not distinguishable from %q "
			"without case sensitivity",
			uentry_getName (e),
			uentry_getName (oe)),
		       uentry_whereLast (e)))
		    {
		      uentry_showWhereAny (oe);
		      uentry_setHasNameError (oe);
		      return TRUE;
		    }
		}
	      else 
		{
		  if (optgenerror 
		      (FLG_DISTINCTINTERNALNAMES,
		       message 
		       ("Internal identifier %q is not distinguishable from %q "
			"in the first %d characters without case sensitivity",
			uentry_getName (e),
			uentry_getName (oe),
			size_toInt (numchars)),
		       uentry_whereLast (e)))
		    {
		      uentry_showWhereAny (oe);
		      uentry_setHasNameError (oe);
		      return TRUE;
		    }
		}
	      /*@switchbreak@*/ 
	      break;
	    case CGE_LOOKALIKE:
	      if (numchars == 0 
		  || (cstring_length (name) <= numchars))
		{
		  if (optgenerror 
		      (FLG_DISTINCTINTERNALNAMES,
		       message 
		       ("Internal identifier %q is not distinguishable from %q "
			"except by lookalike characters",
			uentry_getName (e),
			uentry_getName (oe)),
		       uentry_whereLast (e)))
		    {
		      uentry_showWhereAny (oe);
		      uentry_setHasNameError (oe);
		      return TRUE;
		    }
		}
	      else 
		{
		  if (optgenerror 
		      (FLG_DISTINCTINTERNALNAMES,
		       message 
		       ("Internal identifier %q is not distinguishable from %q "
			"in the first %d characters except by lookalike characters",
			uentry_getName (e),
			uentry_getName (oe),
			size_toInt (numchars)),
		       uentry_whereLast (e)))
		    {
		      uentry_showWhereAny (oe);
		      uentry_setHasNameError (oe);
		      return TRUE;
		    }
		}
	    }
	} end_usymtab_entries ;
      
      ttab = ttab->env;
    }

  return FALSE;
}

void usymtab_checkDistinctName (uentry e, int scope)
   /*@globals utab, globtab@*/
{
  bool hasError = FALSE;
  fileloc where = uentry_whereLast (e);

  if (!fileloc_isPreproc (where)  && !fileloc_isBuiltin (where))
    {
      if (scope == globScope)
	{
	  if (context_getFlag (FLG_DISTINCTEXTERNALNAMES))
	    {
	      hasError = checkDistinctExternalName (e);
	    }
	}
      
      if (!hasError && context_getFlag (FLG_DISTINCTINTERNALNAMES))
	{
	  hasError = checkDistinctInternalName (e);
	}
    }

  if (hasError)
    {
      uentry_setHasNameError (e);
    }
}

/*@exposed@*/ sRef usymtab_lookupGlobalMarker (void) /*@globals utab@*/
{
  uentry ue;

  ue = usymtab_lookupAux (utab, GLOBAL_MARKER_NAME);
  llassert (uentry_isValid (ue));

  return uentry_getSref (ue);
}


# ifdef DEBUGSPLINT
/*
** For debugging only
*/

void
usymtab_checkAllValid () /*@globals utab@*/ 
{
  usymtab tab = utab;

  while (tab != GLOBAL_ENV)
    {
      int i;

      for (i = 0; i < utab->nentries; i++)
	{
	  uentry e = utab->entries[i];
	  
	  uentry_checkValid (e);
	}

      aliasTable_checkValid (tab->aliases);
      tab = tab->env;
    }
}
# endif






