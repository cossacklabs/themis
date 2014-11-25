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
** clabstract.c
**
** ASTs for C grammar
**
*/

# include "splintMacros.nf"
# include "basic.h"
# include "cgrammar.h"
# include "usymtab_interface.h"

# include "structNames.h"
# include "nameChecks.h"

# include "cscannerHelp.h"

# ifdef SANITIZER
# include "sgrammar_tokens.h"
# else
# include "cgrammar_tokens.h"
# endif

/*
** Lots of variables are needed because of interactions with the
** parser.  This is easier than restructuring the grammar so the
** right values are available in the right place.
*/

/*drl*/
static /*@only@*/ constraintList implicitFcnConstraints = NULL;

static void clabstract_prepareFunction (uentry p_e) /*@modifies p_e@*/ ;
static bool fcnNoGlobals = FALSE;
static void processVariable (/*@temp@*/ idDecl p_t) /*@modifies internalState@*/ ;

static bool s_processingVars = FALSE;
static bool s_processingParams = FALSE;
static bool s_processingGlobals = FALSE;
static bool s_processingTypedef = FALSE;
static bool s_processingIterVars = FALSE;
static /*@only@*/ qtype processingType = qtype_undefined;
static uentry currentIter = uentry_undefined;
static /*@dependent@*/ uentryList saveParamList;  /* for old style functions */
static /*@owned@*/ uentry saveFunction = uentry_undefined;
static int saveIterParamNo;
static idDecl fixStructDecl (/*@returned@*/ idDecl p_d);
static void checkTypeDecl (uentry p_e, ctype p_rep);
static /*@dependent@*/ fileloc saveStoreLoc = fileloc_undefined;
static storageClassCode storageClass = SCNONE;
static void declareEnumList (/*@temp@*/ enumNameList p_el, ctype p_c, fileloc p_loc);
static void resetGlobals (void);
static /*@null@*/ qual specialFunctionCode;
static bool argsUsed = FALSE;

extern void clabstract_initMod () 
{
  specialFunctionCode = qual_createUnknown ();
  DPRINTF (("Initialized: %s", qual_unparse (specialFunctionCode)));
}

static bool hasSpecialCode (void)
{
  return (!qual_isUnknown (specialFunctionCode));
}

extern void setArgsUsed (void)
{
  if (argsUsed)
    {
      voptgenerror
	(FLG_SYNTAX,
	 cstring_makeLiteral ("Multiple ARGSUSED comments for one function"),
	 g_currentloc);
    }

  argsUsed = TRUE;
}

static void reflectArgsUsed (uentry ue)
{
  if (argsUsed)
    {
      if (uentry_isFunction (ue))
	{
	  uentryList params = uentry_getParams (ue);

	  uentryList_elements (params, el)
	    {
	      uentry_setUsed (el, fileloc_undefined);
	    } end_uentryList_elements ;
	}

      argsUsed = FALSE;
    }
}
	      
extern void setSpecialFunction (qual qu)
{
  if (!qual_isUnknown (specialFunctionCode))
    {
      voptgenerror (FLG_SYNTAX,
		    message ("Multiple special function codes: %s, %s "
			     "(first code is ignored)",
			     qual_unparse (specialFunctionCode),
			     qual_unparse (qu)),
		    g_currentloc);
    }

  specialFunctionCode = qu;
}

static void reflectSpecialCode (uentry ue)
{
  if (qual_isUnknown (specialFunctionCode)) {
    ;
  } else if (qual_isPrintfLike (specialFunctionCode)) {
    uentry_setPrintfLike (ue);
  } else if (qual_isScanfLike (specialFunctionCode)) {
    uentry_setScanfLike (ue);
  } else if (qual_isMessageLike (specialFunctionCode)) {
    uentry_setMessageLike (ue);
  } else {
    BADBRANCH;
  }

  specialFunctionCode = qual_createUnknown ();
}

static void resetStorageClass (void)
{
  qtype_free (processingType);
  processingType = qtype_undefined;
  storageClass = SCNONE;
}

static void reflectStorageClass (uentry u)
{
  if (storageClass == SCSTATIC)
    {
      uentry_setStatic (u);
    }
  else if (storageClass == SCEXTERN)
    {
      uentry_setExtern (u);
    }
  else
    {
      ; /* no storage class */
    }

  }

void storeLoc ()
{
  saveStoreLoc = g_currentloc;
}

void setFunctionNoGlobals (void)
{
  fcnNoGlobals = TRUE;
}

static void reflectGlobalQualifiers (sRef sr, qualList quals)
{
  DPRINTF (("Reflect global qualifiers: %s / %s", 
	    sRef_unparseFull (sr), qualList_unparse (quals)));

  qualList_elements (quals, qel)
    {
      if (qual_isGlobalQual (qel)) /* undef, killed */
	{
	  sstate oldstate = sRef_getDefState (sr);
	  sstate defstate = sstate_fromQual (qel);
	  
	  if ((oldstate == SS_UNDEFGLOB && defstate == SS_KILLED)
	      || (oldstate == SS_KILLED && defstate == SS_UNDEFGLOB))
	    {
	      defstate = SS_UNDEFKILLED;
	    }
	  else 
	    {
	      ; /* any errors? */
	    }
	  
	  sRef_setDefState (sr, defstate, fileloc_undefined);
	  DPRINTF (("State: %s", sRef_unparseFull (sr)));
	}
      else if (qual_isAllocQual (qel)) /* out, partial, reldef, etc. */
	{
	  ctype realType = sRef_getType (sr);
	  sstate defstate = sstate_fromQual (qel);
	  
	  if (qual_isRelDef (qel))
	    {
	      ; /* okay anywhere */
	    }
	  else
	    {
	      if (!ctype_isAP (realType) 
		  && !ctype_isSU (realType)
		  && !ctype_isUnknown (realType)
		  && !ctype_isAbstract (sRef_getType (sr)))
		{
		  llerror 
		    (FLG_SYNTAX, 
		     message ("Qualifier %s used on non-pointer or struct: %q",
			      qual_unparse (qel), sRef_unparse (sr)));
		  
		}
	    }
	  
	  sRef_setDefState (sr, defstate, fileloc_undefined);
	}
      else if (qual_isNull (qel))
	{
	  sRef_setNullState (sr, NS_POSNULL, fileloc_undefined);
	}
      else if (qual_isRelNull (qel))
	{
	  sRef_setNullState (sr, NS_RELNULL, fileloc_undefined);
	}
      else if (qual_isNotNull (qel))
	{
	  sRef_setNullState (sr, NS_MNOTNULL, fileloc_undefined);
	}
      else
	{
	  if (qual_isCQual (qel))
	    {
	      ; /* okay */
	    }
	  else
	    {
	      llerror (FLG_SYNTAX,
		       message ("Qualifier %s cannot be used in a globals list",
				qual_unparse (qel)));
	    }
	}
    } end_qualList_elements;
}

sRef clabstract_createGlobal (sRef sr, qualList quals)
{
  sRef res;

  if (sRef_isValid (sr))
    {
      res = sRef_copy (sr);
      DPRINTF (("Reflecting quals: %s / %s", sRef_unparse (sr), qualList_unparse (quals)));
      reflectGlobalQualifiers (res, quals);
      DPRINTF (("==> %s", sRef_unparseFull (res)));
    }
  else
    {
      res = sRef_undefined;
    }

  qualList_free (quals);
  return res;
}

extern void declareCIter (cstring name, /*@owned@*/ uentryList params)
{
  uentry ue;

  ue = uentry_makeIter (name, 
			ctype_makeFunction (ctype_void, params), 
			fileloc_copy (g_currentloc));

  usymtab_supEntry (uentry_makeEndIter (name, fileloc_copy (g_currentloc)));
  ue = usymtab_supGlobalEntryReturn (ue);
}

extern void nextIterParam (void)
{
  llassert (s_processingIterVars);
  saveIterParamNo++;
}

extern int iterParamNo (void)
{
  llassert (s_processingIterVars);
  return saveIterParamNo;
}

/*
** yucky hacks to put it in the right place
*/

/*@only@*/ uentry 
makeCurrentParam (idDecl t)
{
  uentry ue;

  saveStoreLoc = fileloc_undefined;

  /* param number unknown */

  ue = uentry_makeParam (t, 0);
  return ue;
}

ctype
declareUnnamedEnum (enumNameList el)
{
  ctype ret = usymtab_enumEnumNameListType (el);
  ctype rt;
  uentry e;

  if (ctype_isDefined (ret))
    {
      rt = ret;
      e = uentry_makeEnumTagLoc (ctype_enumTag (rt), ret);

      reflectStorageClass (e);
      usymtab_supGlobalEntry (e);
      
      declareEnumList (el, ret, g_currentloc);    
      enumNameList_free (el);
    }
  else
    {
      ctype ct = ctype_createEnum (fakeTag (), el);

      e = uentry_makeEnumTagLoc (ctype_enumTag (ctype_realType (ct)), ct);
      reflectStorageClass (e);

      e = usymtab_supGlobalEntryReturn (e);
      rt = uentry_getAbstractType (e);
      declareEnumList (el, ct, g_currentloc);    
    }
  
  return (rt);
}

ctype
declareEnum (cstring ename, enumNameList el)
{
  ctype cet;
  uentry e;

  llassert (cstring_isDefined (ename));

  cet = ctype_createEnum (ename, el);
  e = uentry_makeEnumTagLoc (ename, cet);
  reflectStorageClass (e);
  e = usymtab_supGlobalEntryReturn (e);
  cet = uentry_getType (e);
  declareEnumList (el, cet, uentry_whereLast (e));    
  return (uentry_getAbstractType (e));
}

static void
declareEnumList (enumNameList el, ctype c, fileloc loc)
{
  bool boolnames = FALSE;
  bool othernames = FALSE;

  (void) context_getSaveLocation (); /* undefine it */

  if (context_maybeSet (FLG_NUMENUMMEMBERS))
    {
      int maxnum = context_getValue (FLG_NUMENUMMEMBERS);
      int num = enumNameList_size (el);

      if (num > maxnum)
	{
	  voptgenerror 
	    (FLG_NUMENUMMEMBERS,
	     message ("Enumerator %s declared with %d members (limit is set to %d)",
		      ctype_unparse (c), num, maxnum),
	     loc);
	}
    }

  enumNameList_elements (el, e)
    {
      uentry ue = usymtab_lookupExposeGlob (e);
      ctype ct = uentry_getType (ue);

      llassert (uentry_isEnumConstant (ue));

      if (ctype_isUnknown (ct))
	{
	  uentry_setType (ue, c);
	}
      else
	{
	  if (cstring_equal (e, context_getFalseName ())
	      || cstring_equal (e, context_getTrueName ()))
	    {
	      if (othernames) 
		{
		  if (optgenerror 
		      (FLG_INCONDEFS,
		       message ("Enumerator mixes boolean name (%s) with "
				"non-boolean names",
				e),
		       uentry_whereLast (ue)))
		    {
		      ;
		    }
		}
	      
	      boolnames = TRUE;
	      uentry_setType (ue, ctype_bool);
	      DPRINTF (("Set type: %s / %s",
			uentry_unparse (ue), ctype_unparse (ctype_bool)));
	    }
	  else 
	    {
	      if (boolnames) 
		{
		  if (optgenerror 
		      (FLG_INCONDEFS,
		       message ("Enumerator mixes boolean names (%s, %s) with "
				"non-boolean name: %s",
				context_getTrueName (),
				context_getFalseName (),
				e),
		       uentry_whereLast (ue)))
		    {
		      ;
		    }
		}

	      othernames = TRUE;
	    }

	  if (!ctype_match (c, ct))
	    {
	      if (ctype_isDirectBool (ct))
		{
		  if (cstring_equal (e, context_getFalseName ())
		      || cstring_equal (e, context_getTrueName ()))
		    {
		      DPRINTF (("Here we are!"));
		    }
		  else
		    {
		      if (optgenerror 
			  (FLG_INCONDEFS,
			   message ("Enumerator member %s declared with "
				    "inconsistent type: %s",
				    e, ctype_unparse (c)),
			   uentry_whereLast (ue)))
			{
			  uentry_showWhereSpecifiedExtra 
			    (ue, cstring_copy (ctype_unparse (ct)));
			}
		    }
		}
	      else
		{
		  if (optgenerror 
		      (FLG_INCONDEFS,
		       message ("Enumerator member %s declared with "
				"inconsistent type: %s",
				e, ctype_unparse (c)),
		       uentry_whereLast (ue)))
		    {
		      uentry_showWhereSpecifiedExtra 
			(ue, cstring_copy (ctype_unparse (ct)));
		    }
		}
	    }
	}
    } end_enumNameList_elements;
}

static /*@dependent@*/ uentryList currentParamList;

/*drl added 3-28-2002*/
/* this function takes a list of paramentar and generates a list
   of constraints.
*/

/* drl modified 10/23/2002

The current semantics are generated constraints of the form MaxSet(p) >= 0 and MaxRead(p) >= 0 for all pointers
unless the @out@ annotation has been applied to a parameter, then we only want to generate maxSet(p) > = 0
*/

void setImplicitfcnConstraints (void)
{
  uentryList params;
  sRef s;
  constraint c;
  params = currentParamList;

  if (constraintList_isDefined (implicitFcnConstraints))
    {
      constraintList_free (implicitFcnConstraints);
    }
  
  implicitFcnConstraints = constraintList_makeNew();
  
  uentryList_elements (params, el)
    {
      DPRINTF (("setImplicitfcnConstraints doing: %s", uentry_unparse(el)));
      
      if (uentry_isVariable (el))
	{
	  s = uentry_getSref(el);
	  
	  if (sRef_isReference (s))
	    {
	      DPRINTF((message ("%s is a pointer", sRef_unparse(s) ) ));
	      /*drl 4/26/01
		chagned this from MaxSet(s) == 0 to MaxSet(s) >= 0 */
	      c = constraint_makeSRefWriteSafeInt (s, 0);
	      
	      implicitFcnConstraints = constraintList_add (implicitFcnConstraints , c);
	      
	      /*drl 10/23/2002 added support for out*/
	      
	      if (!uentry_isOut(el))
		{
		  c = constraint_makeSRefReadSafeInt (s, 0);
		  implicitFcnConstraints = constraintList_add (implicitFcnConstraints , c);
		}
	    }
	  else
	    {
	      DPRINTF((message ("%s is NOT a pointer", sRef_unparse(s) ) ));
	    }
	} /*end uentry_isVariable*/
      else if (uentry_isElipsisMarker (el) )      
	{
	  /* just ignore these*/
	  ;
	}
      
      else
	{
	  /* just ignore this
	     I'm not sure if this is possible though
	  */
	  ;
	}
    } end_uentryList_elements;
}


/*@observer@*/ constraintList getImplicitFcnConstraints (void)
{
  return implicitFcnConstraints;
}

void setCurrentParams (/*@dependent@*/ uentryList ue)
{
  currentParamList = ue;
}

void clearCurrentParams (void)
{
  currentParamList = uentryList_undefined;
}

/*
** requires: uentry_isFunction (e)
**           parameter names for current function are in currentParamList
*/

static void enterFunctionParams (uentryList params)
{
  int paramno = 0;

  uentryList_elements (params, current)
    {
      if (uentry_hasName (current)) 
	{
	  uentry_setParamNo (current, paramno);
	  usymtab_supEntry (uentry_copy (current));
	}
      
      paramno++;
    } end_uentryList_elements; 
}
 

extern void enterParamsTemp (void)
{
  usymtab_enterScope ();
  enterFunctionParams (currentParamList);
}

extern void exitParamsTemp (void)
{
  usymtab_quietPlainExitScope ();
}

static /*@exposed@*/ uentry clabstract_globalDeclareFunction (idDecl tid) 
{
  ctype deftype = idDecl_getCtype (tid);
  ctype rettype;
  uentry ue;
  
  DPRINTF (("Global function: %s", idDecl_unparse (tid)));

  if (ctype_isFunction (deftype))
    {
      rettype = ctype_getReturnType (deftype);
    }
  else
    {
      rettype = ctype_unknown;
    }

  /*
  ** check has been moved here...
  */

  if (ctype_isFunction (idDecl_getCtype (tid)))
    {
      ue = uentry_makeIdFunction (tid);
      reflectSpecialCode (ue);
      reflectArgsUsed (ue);
      reflectStorageClass (ue);
      uentry_checkParams (ue);
      
      DPRINTF (("Supercede function: %s", uentry_unparseFull (ue)));
      
      ue = usymtab_supGlobalEntryReturn (ue);
      DPRINTF (("After supercede function: %s", uentry_unparseFull (ue)));
      
      DPRINTF (("Enter function: %s", uentry_unparseFull (ue)));
      context_enterFunction (ue);
      enterFunctionParams (uentry_getParams (ue));
      
      resetStorageClass ();
      DPRINTF (("Function: %s", uentry_unparseFull (ue)));
      return (ue);
    }
  else
    {    
      llparseerror (message ("Non-function declaration: %q",
			     idDecl_unparse (tid)));
      return (uentry_undefined);
    }
}

/*
** for now, no type checking
** (must check later though!)
*/

static /*@only@*/ uentry globalDeclareOldStyleFunction (idDecl tid)
{
  uentry ue;

  /*
  ** check has been moved here...
  */

  if (cstring_equalLit (idDecl_observeId (tid), "main"))
    {
      context_setFlagTemp (FLG_MAINTYPE, FALSE);
    }

  ue = uentry_makeIdFunction (tid);
  reflectStorageClass (ue);
  reflectSpecialCode (ue);
  reflectArgsUsed (ue);
  uentry_setDefined (ue, g_currentloc);
  uentry_checkParams (ue);
  resetStorageClass ();

  /* context_enterOldStyleScope (); */

  return (ue);
}

static void oldStyleDeclareFunction (/*@only@*/ uentry e)
{
  uentryList params = saveParamList;
  ctype rt = uentry_getType (e);

  llassert (ctype_isFunction (rt));

  if (uentry_hasStateClauseList (e) 
      || uentry_hasConditions (e))
    {
      llfatalerror (message ("%q: Old-style function declaration uses a clause (rewrite with function parameters): %q",
			     fileloc_unparse (g_currentloc), uentry_unparse (e)));
    }

  e = usymtab_supGlobalEntryReturn (e);

  context_enterFunction (e);
  enterFunctionParams (params);
  saveParamList = uentryList_undefined;
  resetStorageClass ();
}

static void oldStyleCompleteFunction (/*@only@*/ uentry e)
{
  uentryList params = saveParamList;
  ctype rt = uentry_getType (e);

  llassert (ctype_isFunction (rt));

  if (uentry_hasStateClauseList (e) 
      || uentry_hasConditions (e))
    {
      llfatalerror (message ("%q: Old-style function declaration uses a clause (rewrite with function parameters): %q",
			     fileloc_unparse (g_currentloc), uentry_unparse (e)));
    }

  e = usymtab_supGlobalEntryReturn (e);

  context_completeOldStyleFunction (e);
  enterFunctionParams (params);
  saveParamList = uentryList_undefined;
  resetStorageClass ();
}

void clabstract_declareFunction (idDecl tid) /*@globals undef saveFunction; @*/
{
  uentry ue;

  DPRINTF (("Declare function: %s", idDecl_unparse (tid)));
  
  if (ctype_isUnknown (idDecl_getCtype (tid)))
    {
      /*
      ** No type, its really a plain name (int) declaration
      */

      voptgenerror (FLG_IMPTYPE,
		    message ("No type before declaration name (implicit int type): %q",
			     idDecl_unparse (tid)),
		    g_currentloc);
      tid = idDecl_replaceCtype (tid, ctype_int);
      processVariable (tid);
      saveFunction = uentry_undefined;
    }
  else
    {
      if (s_processingParams)
	{
	  ue = globalDeclareOldStyleFunction (tid);
	  saveFunction = ue;
	  DPRINTF (("Set save function: %s", uentry_unparseFull (ue)));
	}
      else
	{
	  saveFunction = uentry_undefined;
	  
	  if (context_inRealFunction ())
	    {
	      ue = uentry_makeVariableLoc (idDecl_observeId (tid), ctype_unknown);
	      
	      llparseerror (message ("Function declared inside function: %q",
				     idDecl_unparse (tid)));
	      
	      context_quietExitFunction ();
	      ue = usymtab_supEntryReturn (ue);
	    }
	  else
	    {
	      if (context_inInnerScope ())
		{
		  llparseerror (message ("Declaration in inner context: %q",
					 idDecl_unparse (tid)));
		  
		  sRef_setGlobalScope ();
		  ue = uentry_makeVariableLoc (idDecl_observeId (tid), 
					       ctype_unknown);
		  ue = usymtab_supGlobalEntryReturn (ue);
		  sRef_clearGlobalScope ();
		}
	      else
		{
		  ue = clabstract_globalDeclareFunction (tid);
		}
	    }
	  
	  resetGlobals ();
	}

      resetStorageClass ();
    }

  idDecl_free (tid);
}

void declareStaticFunction (idDecl tid) /*@globals undef saveFunction; @*/
{
  uentry ue;

  DPRINTF (("Declare static funciton: %s", idDecl_unparse (tid)));

  if (s_processingParams)
    {
      ue = globalDeclareOldStyleFunction (tid);
      saveFunction = ue;
    }
  else
    {
      saveFunction = uentry_undefined;

      if (context_inRealFunction ())
	{
	  ue = uentry_makeVariableLoc (idDecl_observeId (tid), ctype_unknown);

	  llparseerror (message ("Function declared inside function: %q",
				 idDecl_unparse (tid)));
	  
	  context_quietExitFunction ();
	  ue = usymtab_supEntryReturn (ue);
	}
      else
	{
	  if (context_inInnerScope ())
	    {
	      llparseerror (message ("Declaration in inner context: %q",
				     idDecl_unparse (tid)));
	      
	      sRef_setGlobalScope ();
	      ue = uentry_makeVariableLoc (idDecl_observeId (tid), 
					   ctype_unknown);
	      ue = usymtab_supGlobalEntryReturn (ue);
	      sRef_clearGlobalScope ();
	    }
	  else
	    {
	      ctype deftype = idDecl_getCtype (tid);
	      ctype rettype;
	      
	      if (ctype_isFunction (deftype))
		{
		  rettype = ctype_getReturnType (deftype);
		}
	      else
		{
		  rettype = ctype_unknown;
		}
	      
	      /*
	      ** check has been moved here...
	      */
	      
	      if (ctype_isFunction (idDecl_getCtype (tid)))
		{
		  ue = uentry_makeIdFunction (tid);
		  reflectSpecialCode (ue);
		  reflectArgsUsed (ue);
		}
	      else
		{    
		  DPRINTF (("Here we are!"));
		  llparseerror (message ("Inconsistent function declaration: %q",
					 idDecl_unparse (tid)));
		  
		  tid = idDecl_replaceCtype 
		    (tid, ctype_makeFunction (ctype_unknown, uentryList_undefined));
		  ue = uentry_makeIdFunction (tid);
		}
	      
	      reflectStorageClass (ue);
	      uentry_setStatic (ue);

	      uentry_checkParams (ue);
	
	      DPRINTF (("Sub global entry: %s", uentry_unparse (ue)));
	      ue = usymtab_supGlobalEntryReturn (ue);

	      context_enterFunction (ue);
	      enterFunctionParams (uentry_getParams (ue));
	      resetStorageClass ();
	    }
	}
      
      resetGlobals ();
    }
  
  resetStorageClass ();
  idDecl_free (tid);
}

void
checkTypeDecl (uentry e, ctype rep)
{
  cstring n = uentry_getName (e);

  DPRINTF (("Check type decl: %s", uentry_unparseFull (e)));

  if (cstring_equal (context_getBoolName (), n))
    {
      ctype rrep = ctype_realType (rep);
      
      /*
      ** for abstract enum types, we need to fix the enum members:
      ** they should have the abstract type, not the rep type.
      */
      
      if (ctype_isEnum (ctype_realType (rrep)))
	{
	  enumNameList el = ctype_elist (rrep);
	  
	  enumNameList_elements (el, ye)
	    {
	      if (usymtab_existsGlob (ye))
		{
		  uentry ue = usymtab_lookupSafe (ye);
		  uentry_setType (ue, ctype_bool);
		}

	      if (cstring_equal (context_getTrueName (), ye)
		  || cstring_equal (context_getFalseName (), ye))
		{
		  ;
		}
	      else
		{
		  vgenhinterror 
		    (FLG_SYNTAX,
		     message ("Member of boolean enumerated type definition "
			      "does not match name set to represent true "
			      "or false: %s",
			      ye),
		     message ("Use -boolfalse and -booltrue to set the "
			      "name of false and true boolean values."),
		     uentry_whereDefined (e));
		}
	    } end_enumNameList_elements;
	}
    }

  if (usymtab_exists (n))
    {
      usymId llm = usymtab_getId (n);
      uentry le  = usymtab_getTypeEntry (typeId_fromUsymId (llm));

      uentry_setDeclared (e, g_currentloc); 
      uentry_setSref (e, sRef_makeGlobal (llm, uentry_getType (le), stateInfo_currentLoc ()));

      DPRINTF (("Here we are: %s / %s",
		n, context_getBoolName ()));
      
      if (uentry_isAbstractDatatype (le))
	{
	  ctype rrep = ctype_realType (rep);

	  DPRINTF (("Abstract type: %s", uentry_unparseFull (le)));

	  /*
	  ** for abstract enum types, we need to fix the enum members:
	  ** they should have the abstract type, not the rep type.
	  */

	  if (ctype_isEnum (ctype_realType (rrep)))
	    {
	      ctype at = uentry_getAbstractType (le);
	      enumNameList el = ctype_elist (rrep);

	      enumNameList_elements (el, ye)
		{
		  if (usymtab_existsGlob (ye))
		    {
		      uentry ue = usymtab_lookupSafe (ye);

		      llassert (uentry_isEitherConstant (ue));

		      /* evans 2002-04-22 */
		      if (ctype_isBool (uentry_getType (ue)))
			{
			  /*
			  ** If set using -booltrue or -boolfalse, don't change the type.
			  */
			}
		      else
			{
			  llassertprint (ctype_match (uentry_getType (ue), rrep),
					 ("Bad enum: %s / %s",
					  uentry_unparse (ue),
					  ctype_unparse (rrep)));
			  
			  uentry_setType (ue, at);
			}
		    }
		} end_enumNameList_elements;
	    }
	  
	  if (uentry_isMutableDatatype (le))
	    {
	      /* maybe more complicated if abstract and immutable ? */

	      if (!ctype_isRealPointer (rep) && !ctype_isRealAbstract (rep))
		{
		  voptgenerror 
		    (FLG_MUTREP,
		     message ("Mutable abstract type %s declared without pointer "
			      "indirection: %s (violates assignment semantics)",
			      n, ctype_unparse (rep)),
		     uentry_whereDefined (e));
		  
		  uentry_setMutable (e);
		}
	    }
	}
    }
  else
    {
      fileloc fl = uentry_whereDeclared (e);

      if (context_getFlag (FLG_LIKELYBOOL)
	  && !context_getFlag (FLG_BOOLINT))
	{
	  if ((cstring_equalLit (n, "BOOL")
	       || cstring_equalLit (n, "Bool")
	       || cstring_equalLit (n, "bool")
	       || cstring_equalLit (n, "boolean")
	       || cstring_equalLit (n, "Boolean")
	       || cstring_equalLit (n, "BOOLEAN"))
	      && !(cstring_equal (n, context_getBoolName ())))
	    {
	      if (context_setBoolName ()) {
		voptgenerror 
		  (FLG_LIKELYBOOL,
		   message ("Type %s is probably meant as a boolean type, but does "
			    "not match the boolean type name \"%s\".",
			    n,
			    context_getBoolName ()),
		   fl);
	      } else
		voptgenerror 
		  (FLG_LIKELYBOOL,
		   message ("Type %s is probably meant as a boolean type, "
			    "but the boolean type name is not set. "
			    "Use -booltype %s to set it.",
			    n,
			    n),
		   fl);
		}
	}

      if (!uentry_isStatic (e)
	  && !ctype_isFunction (uentry_getType (e)) 
	  && !fileloc_isLib (fl) 
	  && !fileloc_isImport (fl)
	  && fileloc_isHeader (fl))
	{
	  voptgenerror (FLG_EXPORTTYPE,
			message ("Type exported, but not specified: %s\n", n),
			fl);
	}
    }

  cstring_free (n);
}

uentryList
fixUentryList (idDeclList tl, qtype q)
{
  uentryList f = uentryList_new ();
  
  idDeclList_elements (tl, i)
  {
    if (idDecl_isDefined (i))
      {
	uentry ue;
	uentry old;
	ctype rt;

	(void) idDecl_fixBase (i, q);

	/*
	** implicit annotations 
	*/

	(void) fixStructDecl (i);

	ue = uentry_makeIdVariable (i);
	rt = ctype_realType (uentry_getType (ue));

	/*
	** where is this here???

	if (ctype_isArray (rt) || ctype_isSU (rt))
	  {
	    sRef_setAllocated (uentry_getSref (ue), uentry_whereDefined (ue));
	  }

        **
	*/

	if (uentry_isValid (old = uentryList_lookupField (f, uentry_rawName (ue))))
	  {
	    if (optgenerror (FLG_SYNTAX,
			     message ("Field name reused: %s", uentry_rawName (ue)),
			     uentry_whereDefined (ue)))
	      {
		llgenmsg (message ("Previous use of %s", uentry_rawName (ue)),
			  uentry_whereDefined (old));
	      }
	  }
	
	f = uentryList_add (f, ue);
      }
  } end_idDeclList_elements;

  idDeclList_free (tl);
  return (f);
}

/*
** This is a hack to support unnamed struct/union fields as done by
** Microsoft VC++.  It is not supported by the ANSI standard.  
**
** The inner fields are added to the outer structure.  This is meaningful
** for nesting structs inside unions, but Splint does no related 
** checking.
*/

uentryList
fixUnnamedDecl (qtype q)
{
  ctype ct = ctype_realType (qtype_getType (q));

  if (ctype_isStruct (ct) || ctype_isUnion (ct))
    {
      return uentryList_single (uentry_makeUnnamedVariable (ct));
    }
  else if (ctype_isEnum (ct))
    {
      /* evans 2002-02-05: nothing to do for unnamed enum lists */
      return uentryList_undefined;
    }
  else
    { 
      voptgenerror 
	(FLG_SYNTAX,
	 message ("Type name in field declarations: %s", qtype_unparse (q)),
	 g_currentloc);
    }

  return uentryList_undefined;
}

void setStorageClass (storageClassCode sc)
{
  storageClass = sc;
}

void
setProcessingIterVars (uentry iter)
{
  s_processingIterVars = TRUE;
  currentIter = iter;
  saveIterParamNo = 0;
}

void
setProcessingGlobalsList ()
{
  s_processingGlobals = TRUE;
  fcnNoGlobals = FALSE;
}

static bool ProcessingGlobMods = FALSE;

void
setProcessingGlobMods ()
{
  ProcessingGlobMods = TRUE;
}

void
clearProcessingGlobMods ()
{
  ProcessingGlobMods = FALSE;
}

bool
isProcessingGlobMods ()
{
  return (ProcessingGlobMods);
}

static void resetGlobals (void)
{
  s_processingGlobals = FALSE;
  fcnNoGlobals = FALSE;
}

void
unsetProcessingGlobals ()
{
  s_processingGlobals = FALSE;
}

void
setProcessingVars (/*@only@*/ qtype q)
{
  s_processingVars = TRUE;
  qtype_free (processingType);
  processingType = q;
}

static void
setGenericParamList (/*@dependent@*/ uentryList pm)
{
  s_processingParams = TRUE;
  saveParamList = pm;
}

void
setProcessingTypedef (qtype q)
{
  s_processingTypedef = TRUE;

  qtype_free (processingType);
  processingType = q;
}

void
unsetProcessingVars ()
{
  resetStorageClass ();
  s_processingVars = FALSE;
}

void 
oldStyleDoneParams ()
{  
  if (s_processingParams)
    {
      if (uentry_isInvalid (saveFunction))
	{
	  llbuglit ("unsetProcessingVars: no saved function\n");
	}
      else
	{
	  ctype ct = ctype_getReturnType (uentry_getType (saveFunction));
	  uentryList params = uentryList_copy (saveParamList);
	  ctype ct2 = ctype_makeFunction (ct, params);

	  uentry_setType (saveFunction, ct2);
	  s_processingParams = FALSE;

	  oldStyleCompleteFunction (saveFunction);
	  saveFunction = uentry_undefined;
	  resetGlobals ();
	}
    }
  else
    {
      /*
      ** If the paramlist used a type name, we could be here.
      */

      llfatalerror (message ("%q: Old-style function parameter list uses a "
			     "type name.", fileloc_unparse (g_currentloc)));
    }
}

void 
checkDoneParams ()
{
  if (uentry_isValid (saveFunction))
    {
      /*
      ** old style declaration
      */

      ctype ct = ctype_getReturnType (uentry_getType (saveFunction));
      ctype ct2;

      DPRINTF (("save function: %s", uentry_unparseFull (saveFunction)));

      uentryList_elements (saveParamList, current)
	{
	  uentry_setType (current, ctype_int); /* all params are ints */
	} end_uentryList_elements; 

      ct2 = ctype_makeParamsFunction (ct, uentryList_copy (saveParamList));
      
      uentry_setType (saveFunction, ct2);
      s_processingParams = FALSE;
      
      oldStyleDeclareFunction (saveFunction);
      saveFunction = uentry_undefined;
    }
}

void clabstract_declareType (/*@only@*/ exprNodeList decls, /*@only@*/ warnClause warn)
{
  llassert (s_processingTypedef);

  DPRINTF (("Declare type: %s", exprNodeList_unparse (decls)));

  if (warnClause_isDefined (warn))
    {
      DPRINTF (("Has a warn clause!"));
      DPRINTF (("Warn: %s", warnClause_unparse (warn)));

      exprNodeList_elements (decls, el)
	{
	  uentry ue = exprNode_getUentry (el);
	  cstring uname = uentry_getName (ue);

	  DPRINTF (("Entry: %s", exprNode_unparse (el)));

	  /*
	  ** Need to lookup again to make sure we have the right one...
	  */

	  ue = usymtab_lookupExposeGlob (uname);

	  llassert (uentry_isValid (ue));
	  llassert (uentry_isDatatype (ue));

	  DPRINTF (("Warning for %s: %s",
		    uentry_unparse (ue), warnClause_unparse (warn)));

	  uentry_addWarning (ue, warnClause_copy (warn));
	  DPRINTF (("After add warning: %s", uentry_unparseFull (ue)));
	  cstring_free (uname);
	} end_exprNodeList_elements;
    }

  warnClause_free (warn);
  exprNodeList_free (decls);
  unsetProcessingTypedef ();
}

void
unsetProcessingTypedef ()
{
  s_processingTypedef = FALSE;
}

void checkConstant (qtype t, idDecl id) 
{
  uentry e;
  
  id = idDecl_fixBase (id, t);
  e = uentry_makeIdConstant (id);
  
  reflectStorageClass (e);
  resetStorageClass ();

  DPRINTF (("Constant: %s", uentry_unparseFull (e)));
  usymtab_supGlobalEntry (e);
}

void checkValueConstant (qtype t, idDecl id, exprNode e) 
{
  uentry ue;

  id = idDecl_fixBase (id, t);
  ue = uentry_makeIdConstant (id);
  reflectStorageClass (ue);
  resetStorageClass ();

  if (exprNode_isDefined (e))
    {
      if (!exprNode_matchType (uentry_getType (ue), e))
	{
	  (void) gentypeerror 
	    (exprNode_getType (e), e,
	     uentry_getType (ue), exprNode_undefined,
	     message ("Constant %q initialized to type %t, expects %t: %s",
		      uentry_getName (ue),  
		      exprNode_getType (e), 
		      uentry_getType (ue),
		      exprNode_unparse (e)),
	     exprNode_loc (e));
	}
      else
	{
	  if (exprNode_hasValue (e))
	    {
	      uentry_mergeConstantValue (ue, multiVal_copy (exprNode_getValue (e)));
	    }
	  else
	    {
	      DPRINTF (("No value: %s", exprNode_unparse (e)));
	    }
	}
    }

  DPRINTF (("Constant value: %s", uentry_unparseFull (ue)));
  usymtab_supGlobalEntry (ue);
}

static void processVariable (idDecl t)
{
  uentry e;
  ctype ct;
  
  ct = ctype_realType (idDecl_getCtype (t));
  
  if (s_processingParams)
    {
      cstring id = idDecl_getName (t);
      int paramno = uentryList_lookupRealName (saveParamList, id);
      
      if (paramno >= 0)
	{
	  uentry cparam = uentryList_getN (saveParamList, paramno);
	  
	  DPRINTF (("Processing param: %s", uentry_unparseFull (cparam)));
	  uentry_setType (cparam, idDecl_getCtype (t));
	  uentry_reflectQualifiers (cparam, idDecl_getQuals (t));
	  uentry_setDeclaredOnly (cparam, context_getSaveLocation ());
	  DPRINTF (("Processing param: %s", uentry_unparseFull (cparam)));
	}
      else
	{
	  llfatalerrorLoc
	    (message ("Old style declaration uses unlisted parameter: %s", 
		      id));
	}
    }
  else
    {
      fileloc loc;
      
      if (context_inIterDef ())
	{
	  cstring pname = makeParam (idDecl_observeId (t));
	  uentry p = usymtab_lookupSafe (pname);
	  
	  cstring_free (pname);
	  
	  if (uentry_isYield (p))
	    {
	      e = uentry_makeParam (t, sRef_getParam (uentry_getSref (p)));
	      uentry_checkYieldParam (p, e);
	      usymtab_supEntrySref (e);
	      return;
	    }
	}
      
      if ((hasSpecialCode () || argsUsed)
	  && ctype_isFunction (idDecl_getCtype (t)))
	{
	  e = uentry_makeIdFunction (t);
	  reflectSpecialCode (e);
	  reflectArgsUsed (e);
	}
      else
	{
	  e = uentry_makeIdVariable (t);
	}
      
      loc = uentry_whereDeclared (e);
      
      /*
	if (context_inGlobalScope ())
	{
	uentry_checkParams was here!
	}
      */
      
      if (ctype_isFunction (uentry_getType (e)))
	{
	  clabstract_prepareFunction (e);
	}
      
      DPRINTF (("Superceding... %s", uentry_unparseFull (e)));
      e = usymtab_supEntrySrefReturn (e);
      DPRINTF (("After superceding... %s", uentry_unparseFull (e)));	  
      
      if (uentry_isExtern (e) && !context_inGlobalScope ())
	{
	  voptgenerror 
	    (FLG_NESTEDEXTERN,
	     message ("Declaration using extern inside function scope: %q",
		      uentry_unparse (e)),
	     g_currentloc);
	  
	  uentry_setDefined (e, fileloc_getExternal ());
	  sRef_setDefined (uentry_getSref (e), fileloc_getExternal ());
	}
      
      if (uentry_isFunction (e))
	{
	  if (!context_inXHFile ())
	    {
	      checkParamNames (e);
	    }
	}
      
      if (uentry_isVar (e) && uentry_isCheckedUnknown (e))
	{
	  sRef sr = uentry_getSref (e);
	  
	  if (sRef_isLocalVar (sr))
	    {
	      if (context_getFlag (FLG_IMPCHECKMODINTERNALS))
		{
		  uentry_setCheckMod (e);
		}
	      else
		{
		  uentry_setUnchecked (e);
		}
	    }
	  else if (sRef_isFileStatic (sr))
	    {
	      if (context_getFlag (FLG_IMPCHECKEDSTRICTSTATICS))
		{
		  uentry_setCheckedStrict (e);
		}
	      else if (context_getFlag (FLG_IMPCHECKEDSTATICS))
		{
		  uentry_setChecked (e);
		}
	      else if (context_getFlag (FLG_IMPCHECKMODSTATICS))
		{
		  uentry_setCheckMod (e);
		}
	      else
		{
		  ;
		}
	    }
	  else /* real global */
	    {
	      llassert (sRef_isRealGlobal (sr));
	      
	      if (context_getFlag (FLG_IMPCHECKEDSTRICTGLOBALS))
		{
		  uentry_setCheckedStrict (e);
		}
	      else if (context_getFlag (FLG_IMPCHECKEDGLOBALS))
		{
		  uentry_setChecked (e);
		}
	      else if (context_getFlag (FLG_IMPCHECKMODGLOBALS))
		{
		  uentry_setCheckMod (e);
		}
	      else
		{
		  ;
		}
	    }
	}
    }
}

void processNamedDecl (idDecl t)
{
  if (qtype_isUndefined (processingType))
    {
      processingType = qtype_create (ctype_int);
      t = idDecl_fixBase (t, processingType);

      voptgenerror (FLG_IMPTYPE,
		    message ("No type before declaration name (implicit int type): %q",
			     idDecl_unparse (t)),
		    g_currentloc);
    }
  else
    {
      t = idDecl_fixBase (t, processingType);
    }

  DPRINTF (("Declare: %s", idDecl_unparse (t)));
  
  if (s_processingGlobals)
    {
      cstring id = idDecl_getName (t);
      uentry ue = usymtab_lookupSafe (id);
      
      if (!uentry_isValid (ue))
	{
	  llerror (FLG_UNRECOG,
		   message ("Variable used in globals list is undeclared: %s", id));
	}
      else
	{
	  if (!ctype_match (uentry_getType (ue), idDecl_getCtype (t)))
	    {
	      voptgenerror 
		(FLG_INCONDEFS,
		 message ("Variable %s used in globals list declared %s, "
			  "but listed as %s", 
			  id, ctype_unparse (uentry_getType (ue)), 
			  ctype_unparse (idDecl_getCtype (t))),
		 g_currentloc);
	    }
	  else
	    {
	      sRef sr = sRef_copy (uentry_getSref (ue));
	      reflectGlobalQualifiers (sr, idDecl_getQuals (t));
	    }
	}
    }
  else if (s_processingVars)
    {
      processVariable (t);
    }
  else if (s_processingTypedef)
    {
      ctype ct = idDecl_getCtype (t);
      uentry e;
      
      DPRINTF (("Processing typedef: %s", ctype_unparse (ct)));
      
      e = uentry_makeIdDatatype (t);

      if (cstring_equal (idDecl_getName (t), context_getBoolName ())) {
	ctype rt = ctype_realType (ct);
	
	if (ctype_isEnum (rt)) {
	  ;
	} else {
	  if (!(ctype_isInt (rt)
		|| ctype_isUnknown (rt)
		|| ctype_isChar (rt))) {
	    (void) llgenerror
	      (FLG_BOOLTYPE, 
	       message ("Boolean type %s defined using non-standard type %s (integral, char or enum type expected)",
			context_getBoolName (),
			ctype_unparse (ct)),
	       uentry_whereLast (e));
	  }
	  
	  ct = ctype_bool;
	  uentry_setType (e, ct);
	}
      }

      reflectStorageClass (e);
      checkTypeDecl (e, ct);
      
      e = usymtab_supReturnTypeEntry (e);
    }
  else
    {
      llparseerror (message ("Suspect missing struct or union keyword: %q",
			     idDecl_unparse (t)));
    }

  }

/*
** moved from grammar
*/

static idDecl fixStructDecl (/*@returned@*/ idDecl d)
{
  if (ctype_isVisiblySharable (idDecl_getCtype (d)) 
      && context_getFlag (FLG_STRUCTIMPONLY))
    {
      if (!qualList_hasAliasQualifier (idDecl_getQuals (d)))
	{
	  if (qualList_hasExposureQualifier (idDecl_getQuals (d)))
	    {
	      idDecl_addQual (d, qual_createDependent ());
	    }
	  else
	    {
	      idDecl_addQual (d, qual_createImpOnly ());
	    }
	}
    }

  return d;
}

ctype
declareUnnamedStruct (/*@only@*/ uentryList f)
{
  DPRINTF (("Unnamed struct: %s", uentryList_unparse (f)));

  if (context_maybeSet (FLG_NUMSTRUCTFIELDS))
    {
      int num = uentryList_size (f);
      int max = context_getValue (FLG_NUMSTRUCTFIELDS);

      if (num > max)
	{
	  voptgenerror 
	    (FLG_NUMSTRUCTFIELDS,
	     message ("Structure declared with %d fields "
		      "(limit is set to %d)",
		      num, max),
	     g_currentloc);
	}
    }

  return (ctype_createUnnamedStruct (f));
}

ctype
declareUnnamedUnion (/*@only@*/ uentryList f)
{
  DPRINTF (("Unnamed union: %s", uentryList_unparse (f)));

  if (context_maybeSet (FLG_NUMSTRUCTFIELDS))
    {
      int num = uentryList_size (f);
      int max = context_getValue (FLG_NUMSTRUCTFIELDS);

      if (num > max)
	{
	  voptgenerror 
	    (FLG_NUMSTRUCTFIELDS,
	     message ("Union declared with %d fields "
		      "(limit is set to %d)",
		      num, max),
	     g_currentloc);
	}
    }

  return (ctype_createUnnamedUnion (f));
}

ctype declareStruct (cstring id, /*@only@*/ uentryList f)
{
  ctype ct; 
  uentry ue;
  int num = uentryList_size (f);

  DPRINTF (("Declare struct: %s / %s [%d]", id, uentryList_unparse (f),
	    uentryList_size (f)));

  ct = ctype_createStruct (cstring_copy (id), f);

  DPRINTF (("Ctype: %s", ctype_unparse (ct)));

  ue = uentry_makeStructTagLoc (id, ct);

  DPRINTF (("ue: %s", uentry_unparseFull (ue)));

  if (context_maybeSet (FLG_NUMSTRUCTFIELDS))
    {
      int max = context_getValue (FLG_NUMSTRUCTFIELDS);

      if (num > max)
	{
	  voptgenerror 
	    (FLG_NUMSTRUCTFIELDS,
	     message ("Structure %q declared with %d fields "
		      "(limit is set to %d)",
		      uentry_getName (ue), num, max),
	     uentry_whereLast (ue));
	}
    }

  return (usymtab_supTypeEntry (ue));
}

ctype declareUnion (cstring id, uentryList f)
{
  ctype ct; 
  uentry ue;
  int num = uentryList_size (f);

  ct = ctype_createUnion (cstring_copy (id), f);
  ue = uentry_makeUnionTagLoc (id, ct);

  if (context_maybeSet (FLG_NUMSTRUCTFIELDS))
    {
      int max = context_getValue (FLG_NUMSTRUCTFIELDS);

      if (num > max)
	{
	  voptgenerror 
	    (FLG_NUMSTRUCTFIELDS,
	     message ("Union %q declared with %d fields "
		      "(limit is set to %d)",
		      uentry_getName (ue), num, max),
	     uentry_whereLast (ue));
	}
    }

  return (usymtab_supTypeEntry (ue));
}

ctype handleStruct (/*@only@*/ cstring id)
{
  if (usymtab_existsStructTag (id))
    {
      ctype ct = uentry_getAbstractType (usymtab_lookupStructTag (id));

      cstring_free (id);
      return ct;
    }
  else
    {
      return (ctype_createForwardStruct (id));
    }
}

ctype handleUnion (/*@only@*/ cstring id)
{
  if (usymtab_existsUnionTag (id))
    {
      ctype ret = uentry_getAbstractType (usymtab_lookupUnionTag (id));
      cstring_free (id);
      return (ret);
    }
  else
    {
      return (ctype_createForwardUnion (id));
    }
}

ctype
handleEnum (cstring id)
{
  if (usymtab_existsEnumTag (id))
    {
      ctype ret = uentry_getAbstractType (usymtab_lookupEnumTag (id));
      cstring_free (id);
      return ret;
    }
  else
    {
      return (ctype_createForwardEnum (id));
    }
}

bool processingIterVars (void) 
{ 
  return s_processingIterVars; 
}

uentry getCurrentIter (void) 
{
  return currentIter; 
}

static bool flipOldStyle = FALSE;
static bool flipNewStyle = TRUE;

void setFlipOldStyle ()          { flipOldStyle = TRUE; }
bool isFlipOldStyle ()           { return flipOldStyle; }
bool isNewStyle ()               { return flipNewStyle; }
void setNewStyle ()              { flipNewStyle = TRUE; }

/*@dependent@*/ uentryList handleParamIdList (/*@dependent@*/ uentryList params)
{  
  int paramno = 0;

  /*
  ** this is a really YUCKY hack to handle old style
  ** declarations.
  */
  
  voptgenerror (FLG_OLDSTYLE,
		cstring_makeLiteral ("Old style function declaration"),
		g_currentloc); 

  DPRINTF (("Handle old style params: %s", uentryList_unparseFull (params)));

  uentryList_elements (params, current)
    {
      uentry_setParam (current);
      uentry_setSref (current, sRef_makeParam 
		      (paramno, ctype_unknown, 
		       stateInfo_makeLoc (uentry_whereLast (current), SA_DECLARED)));
      paramno++;
    } end_uentryList_elements;

  setGenericParamList (params);
  cscannerHelp_setExpectingTypeName ();

  return params;
}

/*@dependent@*/ uentryList handleParamTypeList (/*@returned@*/ uentryList params)
{
  if (flipOldStyle)
    {
      uentryList_fixMissingNames (params);

      voptgenerror (FLG_OLDSTYLE, 
		    cstring_makeLiteral ("Old style function declaration."), 
		    g_currentloc); 
      
      setGenericParamList (params);
      flipOldStyle = FALSE;
      cscannerHelp_setExpectingTypeName ();
    }
 
  return (params); 
}

void
doVaDcl ()
{
  ctype c = ctype_unknown;
  cstring id = cstring_makeLiteral ("va_alist");
  uentry e;

  if (s_processingParams)
    {
      int i = uentryList_lookupRealName (saveParamList, id);
      
      if (i >= 0)
	{
	  fileloc loc = context_getSaveLocation ();
	  e = uentry_makeVariableSrefParam 
	    (id, c, loc, 
	     sRef_makeParam (i, c, stateInfo_makeLoc (loc, SA_DECLARED)));
	}
      else
	{
	  e = uentry_undefined; /* suppress gcc message */
	  llfatalerrorLoc (cstring_makeLiteral ("va_dcl used without va_alist"));
	}
    }
  else
    {	 
      llerror (FLG_SYNTAX, cstring_makeLiteral ("va_dcl used outside of function declaration"));
      e = uentry_makeVariableLoc (id, c);
    }

  cstring_free (id);
  uentry_setUsed (e, g_currentloc);  
  usymtab_supEntrySref (e);
}

/*@exposed@*/ sRef modListPointer (/*@exposed@*/ sRef s)
{
  ctype ct = sRef_getType (s);
  ctype rt = ctype_realType (ct);
  
  if (ctype_isAP (rt))
    {
      if (context_inHeader () && ctype_isAbstract (ct))
	{
	  voptgenerror 
	    (FLG_ABSTRACT,
	     message
	     ("Modifies clause in header file dereferences abstract "
	      "type %s (interface modifies clause should not depend "
	      "on or expose type representation): %q",
	      ctype_unparse (ct),
	      sRef_unparse (s)),
	     g_currentloc);
	}

      return (sRef_constructPointer (s));
    }
  else
    {
      if (ctype_isKnown (rt))
	{
	  voptgenerror 
	    (FLG_TYPE,
	     message ("Implementation modifies clause dereferences non-pointer (type %s): %q",
		      ctype_unparse (rt),
		      sRef_unparse (s)),
	     g_currentloc);
	}

      return s;
    }
}

/*@exposed@*/ sRef modListFieldAccess (sRef s, cstring f)
{
  ctype ct = sRef_getType (s);
  ctype rt = ctype_realType (ct);
  
  if (ctype_isStructorUnion (rt))
    {
      uentry tf = uentryList_lookupField (ctype_getFields (rt), f);
      
      if (uentry_isUndefined (tf))
	{
	  voptgenerror (FLG_TYPE,
			message ("Modifies list accesses non-existent "
				 "field %s of %t: %q", f, ct, 
				 sRef_unparse (s)),
			g_currentloc);
	  
	  cstring_free (f);
	  return sRef_undefined;
	}
      else 
	{
	  if (ctype_isAbstract (ct) && context_inHeader ())
	    {
	      voptgenerror 
		(FLG_ABSTRACT,
		 message
		 ("Modifies clause in header file accesses abstract "
		  "type %s (interface modifies clause should not depend "
		  "on or expose type representation): %q",
		  ctype_unparse (ct),
		  sRef_unparse (s)),
		 g_currentloc);
	    }
	}
      
      cstring_markOwned (f);
      return (sRef_makeField (s, f));
    }
  else
    {
      voptgenerror 
	(FLG_TYPE,
	 message ("Modifies clause dereferences non-pointer (type %s): %q",
		  ctype_unparse (rt),
		  sRef_unparse (s)),
	 g_currentloc);
      
      cstring_free (f);
      return s;
    }
}

/*@dependent@*/ sRef clabstract_unrecognizedGlobal (cstring s)
{
  if (cstring_equalLit (s, "nothing"))
    {
      return sRef_makeNothing ();
    }
  else if (cstring_equalLit (s, "internalState"))
    {
      return sRef_makeInternalState ();
    }
  else if (cstring_equalLit (s, "fileSystem")
	   || cstring_equalLit (s, "systemState"))
    {
      return sRef_makeSystemState ();
    }
  else
    {
      voptgenerror 
	(FLG_UNRECOG, 
	 message ("Unrecognized identifier in globals list: %s", s), 
	 g_currentloc);
      
      return sRef_undefined;
    }
}

/*@exposed@*/ sRef modListArrowAccess (sRef s, cstring f)
{
  ctype ct = sRef_getType (s);
  ctype rt = ctype_realType (ct);

  if (ctype_isRealPointer (rt))
    {
      ctype b = ctype_baseArrayPtr (rt);
      ctype rb = ctype_realType (b);

      if (ctype_isStructorUnion (rb))
	{
	  uentry tf = uentryList_lookupField (ctype_getFields (rb), f);
      
	  if (uentry_isUndefined (tf))
	    {
	      voptgenerror (FLG_TYPE,
			    message ("Modifies list arrow accesses non-existent "
				     "field %s of %t: %q", f, b, 
				     sRef_unparse (s)),
			    g_currentloc);
	      
	      cstring_free (f);
	      return sRef_undefined;
	    }
	  else 
	    {
	      if (context_inHeader ())
		{
		  if (ctype_isAbstract (b))
		    {
		      voptgenerror 
			(FLG_ABSTRACT,
			 message
			 ("Modifies clause in header file arrow accesses abstract "
			  "type %s (interface modifies clause should not depend "
			  "on or expose type representation): %q",
			  ctype_unparse (b),
			  sRef_unparse (s)),
			 g_currentloc);
		    }
		}
	      else 
		{
		  if (ctype_isAbstract (rt))
		    {
		      voptgenerror 
			(FLG_ABSTRACT,
			 message
			 ("Modifies clause arrow accesses inaccessible abstract "
			  "type %s (interface modifies clause should not depend "
			  "on or expose type representation): %q",
			  ctype_unparse (rt),
			  sRef_unparse (s)),
			 g_currentloc);
		    }
		}
	    }

	  cstring_markOwned (f);
	  return (sRef_makeArrow (s, f));
	}
      else
	{
	  voptgenerror 
	    (FLG_TYPE,
	     message ("Modifies clause arrow accesses pointer to "
		      "non-structure (type %s): %q",
		      ctype_unparse (rt),
		      sRef_unparse (s)),
	     g_currentloc);
	}
    }
  else
    {
      voptgenerror 
	(FLG_TYPE,
	 message ("Modifies clause arrow accesses non-pointer (type %s): %q",
		  ctype_unparse (rt),
		  sRef_unparse (s)),
	 g_currentloc);
    }

  cstring_free (f);
  return s;
}

sRef checkStateClausesId (uentry ue)
{
  cstring s = uentry_rawName (ue);

  if (sRef_isFileOrGlobalScope (uentry_getSref (ue)))
    {
      voptgenerror 
	(FLG_COMMENTERROR,
	 message ("Global variable %s used state clause.  (Global variables "
		  "are not recognized in state clauses.  If they are present "
		  "they are ignored. "
		  " If there is "
		  "sufficient interest in support for this, it may be "
		  "added to a future release.  Send mail to "
		  "info@splint.org.)",
		  s),
	 g_currentloc);
      
      return sRef_undefined;
    }
  else
    {
      if (cstring_equalLit (s, "result"))
	{
	  if (optgenerror 
	      (FLG_SYNTAX, 
	       message ("Special clause list uses %s which is a variable and has special "
			"meaning in a modifies list.  (Special meaning assumed.)", s), 
	       g_currentloc))
	    {
	      uentry_showWhereDeclared (ue);
	    }
	}

      return uentry_getSref (ue);
    }
}
/*drl:1/19/2001
  oops to 1/8/2000
  date is wronge ..
  don;t know what the real date is...
  
*/

/*drl
  added 1/8/2000
  based on checkSpecClausesId
  called by grammar
*/

sRef checkbufferConstraintClausesId (uentry ue)
{
  sRef sr;
  cstring s = uentry_rawName (ue);

  if (cstring_equalLit (s, "result"))
    {
      if (optgenerror 
	  (FLG_SYNTAX, 
	   message ("Function clause list uses %s which is a variable and has special "
		    "meaning in a modifies list.  (Special meaning assumed.)", s), 
	   g_currentloc))
	{
	  uentry_showWhereDeclared (ue);
	}
    }
  
  sr = uentry_getSref (ue);

  if (sRef_isInvalid (sr))
    {
      llfatalerrorLoc (cstring_makeLiteral ("Macro defined constants can not be used in function "
					    "constraints unless they are specifed with the constant "
					    "annotation. To use a macro defined constant include an "
					    "annotation of the form /*@constant <type> <name>=<value>@*/ "
					    "somewhere before the function constraint. This restriction "
					    "may be removed in future releases."));
    }

  /* saveCopy to used to mitigate danger of accessing freed memory*/
  return sRef_saveCopy (sr); 
}

void checkModifiesId (uentry ue)
{
  cstring s = uentry_rawName (ue);

  if (cstring_equalLit (s, "nothing")
      || cstring_equalLit (s, "internalState")
      || cstring_equalLit (s, "systemState")
      || (cstring_equalLit (s, "fileSystem")))
    {
      if (optgenerror 
	  (FLG_SYNTAX, 
	   message ("Modifies list uses %s which is a variable and has special "
		    "meaning in a modifies list.  (Special meaning assumed.)", s), 
	   g_currentloc))
	{
	  uentry_showWhereDeclared (ue);
	}
    }
}

/*@exposed@*/ sRef fixModifiesId (cstring s) 
{
  sRef ret;
  cstring pname = makeParam (s);
  uentry ue = usymtab_lookupSafe (pname);

  cstring_free (pname);

  if (cstring_equalLit (s, "nothing"))
    {
      ret = sRef_makeNothing ();
    }
  else if (cstring_equalLit (s, "internalState"))
    {
      ret = sRef_makeInternalState ();
    }
  else if (cstring_equalLit (s, "fileSystem")
	   || cstring_equalLit (s, "systemState"))
    {
      ret = sRef_makeSystemState ();
    }
  else
    {
      ret = sRef_undefined;
    }

  if (sRef_isValid (ret))
    {
      if (uentry_isValid (ue))
	{
	  voptgenerror 
	    (FLG_SYNTAX, 
	     message ("Modifies list uses %s which is a parameter and has special "
		      "meaning in a modifies list.  (Special meaning assumed.)", s), 
	     g_currentloc);
	}
    }
  else
    {
      if (uentry_isValid (ue))
	{
	  ret = uentry_getSref (ue);
	}
      else
	{
	  fileloc loc = fileloc_decColumn (g_currentloc, size_toInt (cstring_length (s)));
	  ret = sRef_undefined;

	  voptgenerror 
	    (FLG_UNRECOG, 
	     message ("Unrecognized identifier in modifies comment: %s", s), 
	     loc);

	  fileloc_free (loc);
	}
    }
  
  return ret;
}

sRef fixStateClausesId (cstring s) 
{
  sRef ret;
  cstring pname = makeParam (s);
  uentry ue = usymtab_lookupSafe (pname);

  cstring_free (pname);

  if (cstring_equalLit (s, "result"))
    {
      ret = sRef_makeResult (ctype_unknown);
    }
  else
    {
      ret = sRef_undefined;
    }

  if (sRef_isValid (ret))
    {
      if (uentry_isValid (ue))
	{
	  voptgenerror 
	    (FLG_SYNTAX, 
	     message ("Function clause uses %s which is a parameter and has special "
		      "meaning in a function clause.  (Special meaning assumed.)", s), 
	     g_currentloc);
	}
    }
  else
    {
      if (uentry_isValid (ue))
	{
	  ret = uentry_getSref (ue);

	  if (sRef_isFileOrGlobalScope (ret))
	    {
	      voptgenerror 
		(FLG_SYNTAX, 
		 message ("Global variable %s used in function clause.  (Global variables "
			  "are not recognized in function clauses.  If there is "
			  "sufficient interest in support for this, it may be "
			  "added to a future release.  Send mail to "
			  "info@splint.org.)",
			  s), 
		 g_currentloc);
	      
	      ret = sRef_undefined;
	    }
	}
      else
	{

	  /* drl This is the code for structure invariants

	  It is no yet stable enough to be included in a Splint release.
	  */

	  /*check that we're in a structure */
#if 0
		  /*@unused@*/	  uentryList ueL;
	  /*@unused@*/ uentry ue2;
	  /*@unused@*/ ctype ct;
#endif
	  fileloc loc = fileloc_decColumn (g_currentloc, size_toInt (cstring_length (s)));
	  ret = sRef_undefined; 
# if 0
	  
	  ct = context_getLastStruct ( ct );

	  llassert( ctype_isStruct(ct) );

	  ueL =  ctype_getFields (ct);

	  ue2 = uentryList_lookupField (ueL, s);

	  if (!uentry_isUndefined(ue2) )
	    {
	      ret = uentry_getSref(ue2);
	      
	      DPRINTF((
		       message("Got field in structure in the annotation constraint: %s (or sref: %s)", s, sRef_unparse(ret) )
		       ));
	      
	      return ret;
	    }
	  
#endif

	  voptgenerror 
	    (FLG_UNRECOG, 
	     message ("Unrecognized identifier in function clause: %s", s), 
	     loc);

	  fileloc_free (loc);
	}
    }
  
  return ret;
}

sRef modListArrayFetch (/*@exposed@*/ sRef s, /*@unused@*/ sRef mexp)
{
  ctype ct = sRef_getType (s);
  ctype rt = ctype_realType (ct);

  if (ctype_isAP (rt))
    {
      if (context_inHeader () && ctype_isAbstract (ct))
	{
	  voptgenerror 
	    (FLG_ABSTRACT,
	     message
	     ("Modifies clause in header file indexes abstract "
	      "type %s (interface modifies clause should not depend "
	      "on or expose type representation): %q",
	      ctype_unparse (ct),
	      sRef_unparse (s)),
	     g_currentloc);
	}
      
      return (sRef_makeAnyArrayFetch (s));
    }
  else
    {
      voptgenerror
	(FLG_TYPE,
	 message
	 ("Implementation modifies clause uses array fetch on non-array (type %s): %q",
	  ctype_unparse (ct), sRef_unparse (s)),
	 g_currentloc);
      return s;
    }
}

static void clabstract_prepareFunction (uentry e)
{
  uentry_checkParams (e);
  DPRINTF (("After prepare: %s", uentry_unparseFull (e)));
}

sRef clabstract_checkGlobal (exprNode e)
{
  sRef s;
  llassert (exprNode_isInitializer (e));

  s = exprNode_getSref (e);
  DPRINTF (("Initializer: %s -> %s", exprNode_unparse (e), sRef_unparse (s)));

  exprNode_free (e);
  return sRef_copy (s);
}
