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
** usymtab_interface.c
**
** Grammar interface to symtab.
**
** The Splint parser will build symbol tables for abstract types and
** function declarations.
**
*/

# include "splintMacros.nf"
# include "basic.h"
# include "gram.h"
# include "lclscan.h"
# include "lclsyntable.h"
# include "lslparse.h"
# include "usymtab_interface.h"
# include "structNames.h"

static void 
  declareFcnAux (fcnNode p_f, /*@only@*/ qtype p_qt, ctype p_ct, typeId p_tn, 
		 bool p_priv, bool p_spec);

static uentryList paramNodeList_toUentryList (paramNodeList p_p);
static /*@observer@*/ cstring getVarName (/*@null@*/ typeExpr p_x);
static qtype convertLclTypeSpecNode (/*@null@*/ lclTypeSpecNode p_n);
static ctype convertTypeExpr (ctype p_c, /*@null@*/ typeExpr p_x);
static ctype convertCTypeExpr (ctype p_c, /*@null@*/ typeExpr p_x);
static /*@exposed@*/ sRef fixTermNode (termNode p_n, fcnNode p_f, uentryList p_cl);
static sRefSet fixModifies (fcnNode p_f, uentryList p_cl);

static uentryList
  convertuentryList (stDeclNodeList x)
{
  uentryList fl = uentryList_new ();

  
  stDeclNodeList_elements (x, i)
  {
    declaratorNodeList d = i->declarators;
    qtype q = convertLclTypeSpecNode (i->lcltypespec);

    declaratorNodeList_elements (d, j)
    {
      idDecl id;

      qtype_setType (q, convertTypeExpr (qtype_getType (q), j->type));
      id = idDecl_create (cstring_copy (getVarName (j->type)), qtype_copy (q));
      fl = uentryList_add (fl, uentry_makeIdVariable (id));
      idDecl_free (id);
    } end_declaratorNodeList_elements;

    qtype_free (q);
  } end_stDeclNodeList_elements;

  
  return (fl);
}

static uentryList
  convert_uentryList (paramNodeList x)
{
  uentryList p = uentryList_undefined;
  bool first_one = TRUE;

  
  paramNodeList_elements (x, i)
  {
    if (i != (paramNode) 0)
      {
	if (paramNode_isElipsis (i))
	  {
	    first_one = FALSE;
	    p = uentryList_add (p, uentry_makeElipsisMarker ());
	  }
	else
	  {
	    qtype q = convertLclTypeSpecNode (i->type);
	    typeExpr t = i->paramdecl;
	    
	    qtype_setType (q, convertTypeExpr (qtype_getType (q), t));
	    
	    /* note: has to be like this to hack around void  ???? still */ 

	    if (first_one)
	      {
		if (ctype_isVoid (qtype_getType (q)))
		  {
		    llassert (uentryList_isUndefined (p));
		    qtype_free (q);
		    return (p);
		  }
		
		first_one = FALSE;
	      }
	    
	    /*
	    ** don't do qualifiers here, will get errors later
	    */

	    p = uentryList_add (p, uentry_makeUnnamedVariable (qtype_getType (q)));
	    qtype_free (q);
	  }
      }
    else
      {
	llbug (cstring_makeLiteral ("convertuentryList: null paramNode"));
      }
  } end_paramNodeList_elements;
  
  if (first_one)
    {
      llassert (uentryList_isUndefined (p));
      
      p = uentryList_makeMissingParams ();
    }

  return p;
}

/*
** convertTypeExpr
**
** modify c with pointer, array, function
**
** (based on printTypeExpr2 from abstract.c)
**
*/

static ctype
convertTypeExpr (ctype c, typeExpr x)
{
  if (x == (typeExpr) 0)
    {
      return c;
    }
  
  switch (x->kind)
    {
    case TEXPR_BASE:
      return (c);
    case TEXPR_PTR:
      return (convertTypeExpr (ctype_makePointer (c), x->content.pointer));
    case TEXPR_ARRAY:
      return (convertTypeExpr (ctype_makeArray (c), x->content.array.elementtype));
    case TEXPR_FCN:
      {
	ctype rv = convertTypeExpr (c, x->content.function.returntype);
	uentryList p = paramNodeList_toUentryList (x->content.function.args);
	
	if (x->content.function.returntype != NULL
	    && x->content.function.returntype->wrapped == 1 
	    && ctype_isPointer (rv))
	  {
	    rv = ctype_baseArrayPtr (rv);
	  }

	return (ctype_makeParamsFunction (rv, p));
      }
    default:
      {
	llfatalbug (message ("convertTypeExpr: unknown typeExprKind: %d",
			     (int) x->kind));
      }
    }

  BADEXIT;
}

static 
ctype convertCTypeExpr (ctype c, typeExpr x)
{
  if (x == (typeExpr) 0)
    {
      return c;
    }

  switch (x->kind)
    {
    case TEXPR_BASE:     return (c);
    case TEXPR_PTR:  return (convertCTypeExpr (ctype_makePointer (c), 
						 x->content.pointer));
    case TEXPR_ARRAY:    return (convertCTypeExpr (ctype_makeArray (c), 
						 x->content.array.elementtype));
    case TEXPR_FCN:
      {
	ctype rv = convertCTypeExpr (c, x->content.function.returntype);
	uentryList p = convert_uentryList (x->content.function.args);

	return (ctype_makeParamsFunction (rv, p));
      }
    default:
      {
	llfatalbug (message ("convertCTypeExpr: unknown typeExprKind: %d", (int) x->kind));
      }
    }
  BADEXIT;
}

/*
** convertLclTypeSpecNode
**
** LclTypeSpecNode --> ctype
** this is the base type only!
*/

/*
** convertLeaves
**
** for now, assume only last leaf is relevant.
** this should be a safe assumption in general???
*/

static ctype
  convertLeaves (ltokenList f)
{
  ctype c = ctype_unknown;
  
  ltokenList_reset (f);
  
  ltokenList_elements (f, current)
    {
      switch (ltoken_getCode (current))
	{
	case LLT_TYPEDEF_NAME:
	  {
	    cstring tn = ltoken_getRawString (current);

	    if (usymtab_existsTypeEither (tn))
	      {
		c = ctype_combine (uentry_getAbstractType 
				   (usymtab_lookupEither (tn)), c);
	      }
	    else if (cstring_equalLit (tn, "bool"))
	      {
		/*
		** Bogus...keep consistent with old lcl builtin.
		*/
		c = ctype_bool;
	      }
	    else
	      {
		fileloc loc = fileloc_fromTok (current);

		voptgenerror (FLG_UNRECOG, 
			      message ("Unrecognized type: %s", tn), loc);
		fileloc_free (loc);
		
		usymtab_supEntry 
		  (uentry_makeDatatype 
		   (tn, ctype_unknown, MAYBE, qual_createConcrete (), 
		    fileloc_getBuiltin ()));
		
	      }
	    /*@switchbreak@*/ break;
	  }
	case LLT_CHAR:
	  c = ctype_combine (ctype_char, c);
	  /*@switchbreak@*/ break;
	  
	case LLT_DOUBLE:
	  c = ctype_combine (ctype_double, c);
	  /*@switchbreak@*/ break;
	case LLT_FLOAT:     
	  c = ctype_combine (ctype_float, c);
	  /*@switchbreak@*/ break;
	case LLT_CONST:    
	case LLT_VOLATILE: 
	  /*@switchbreak@*/ break;
	case LLT_INT:      
	  c = ctype_combine (ctype_int, c);
	  /*@switchbreak@*/ break;
	case LLT_LONG:  
	  c = ctype_combine (c, ctype_lint); 
	  /*@switchbreak@*/ break;
	case LLT_SHORT:    
	  c = ctype_combine (c, ctype_sint);
	  /*@switchbreak@*/ break;
	case LLT_SIGNED:    
	  c = ctype_combine (c, ctype_int); 
	  /*@switchbreak@*/ break;
	case LLT_UNSIGNED:  
	  c = ctype_combine (c, ctype_uint);
	  /*@switchbreak@*/ break;
	case LLT_UNKNOWN:   
	  c = ctype_combine (ctype_unknown, c);
	  /*@switchbreak@*/ break;
	case LLT_VOID:      
	  c = ctype_combine (ctype_void, c); 
	  /*@switchbreak@*/ break;
	case LLT_ENUM:
	  llcontbug (cstring_makeLiteral ("convertLeaves: enum"));
	  c = ctype_int; 
	  /*@switchbreak@*/ break;
	default:
	  llfatalbug (message ("convertLeaves: bad token: %q", 
			       ltoken_unparseCodeName (current)));
	}
    } end_ltokenList_elements;

  return c;
}

static enumNameList
  convertEnumList (ltokenList enums)
{
  enumNameList el = enumNameList_new ();

  if (ltokenList_isDefined (enums))
    {
      ltokenList_elements (enums, i)
	{
	  enumNameList_addh 
	    (el, enumName_create (cstring_copy (ltoken_unparse (i))));
	} end_ltokenList_elements;
    }

  return el;
}

static /*@only@*/ qtype 
  convertLclTypeSpecNode (/*@null@*/ lclTypeSpecNode n)
{
  
  if (n != (lclTypeSpecNode) 0)
    {
      qtype result;
      
      switch (n->kind)
	{
	case LTS_CONJ:
	  {
	    qtype c1 = convertLclTypeSpecNode (n->content.conj->a);
	    qtype c2 = convertLclTypeSpecNode (n->content.conj->b);

	    /*
	    ** Is it explicit?
	    */

	    if (fileloc_isLib (g_currentloc)
		|| fileloc_isStandardLibrary (g_currentloc))
	      {
		result = qtype_mergeImplicitAlt (c1, c2);
	      }
	    else
	      {
		result = qtype_mergeAlt (c1, c2);
	      }
	    
	    break;
	  }
	case LTS_TYPE:
	  llassert (n->content.type != NULL);
	  result = qtype_create (convertLeaves (n->content.type->ctypes));
	  break;
	case LTS_STRUCTUNION:
	  {
	    strOrUnionNode sn;
	    cstring cn = cstring_undefined;

	    sn = n->content.structorunion;

	    llassert (sn != (strOrUnionNode) 0);

	    if (!ltoken_isUndefined (sn->opttagid))
	      {
		cn = cstring_copy (ltoken_getRawString (sn->opttagid));
	      }
	    else
	      {
		cn = fakeTag ();
	      }

	    switch (sn->kind)
	      {
	      case SU_STRUCT:
		if (usymtab_existsStructTag (cn))
		  {
		    
		    result = qtype_create (uentry_getAbstractType
					   (usymtab_lookupStructTag (cn)));
		    cstring_free (cn);
		  }
		else
		  {
		    uentryList fl = convertuentryList (sn->structdecls);
		    ctype ct;

		    		    ct = ctype_createStruct (cstring_copy (cn), fl);

		    /*
		    ** If it was a forward declaration, this could add it to
		    ** the table.  Need to check if it exists again...
		    */

		    if (usymtab_existsStructTag (cn))
		      {
			result = qtype_create (uentry_getAbstractType
					       (usymtab_lookupStructTag (cn)));
		      }
		    else
		      {
			fileloc loc = fileloc_fromTok (n->content.structorunion->tok);
			uentry ue = uentry_makeStructTag (cn, ct, loc);

			result = qtype_create (usymtab_supTypeEntry (ue));
		      }
			
		    cstring_free (cn);
		  }
		/*@switchbreak@*/ break;
	      case SU_UNION:
		if (usymtab_existsUnionTag (cn))
		  {
		    
		    result = qtype_create (uentry_getAbstractType
					   (usymtab_lookupUnionTag (cn)));
		    cstring_free (cn);
		  }
		else
		  {
		    uentryList fl;
		    ctype ct;
		    
		    fl = convertuentryList (sn->structdecls);
		    ct = ctype_createUnion (cstring_copy (cn), fl);

		    /*
		    ** If it was a forward declaration, this could add it to
		    ** the table.  Need to check if it exists again...
		    */

		    
		    
		    if (usymtab_existsUnionTag (cn))
		      {
			
			result = qtype_create (uentry_getAbstractType
					       (usymtab_lookupUnionTag (cn)));
		      }
		    else
		      {
			fileloc loc = fileloc_fromTok (n->content.structorunion->tok);
			uentry ue = uentry_makeUnionTag (cn, ct, loc);

			result = qtype_create (usymtab_supTypeEntry (ue));
		      }
			
		    cstring_free (cn);
		  }
		/*@switchbreak@*/ break;
	      BADDEFAULT
	      }
	    break;
	  }
	case LTS_ENUM:
	  {
	    enumSpecNode e = n->content.enumspec;
	    enumNameList el;
	    cstring ename;
	    bool first = TRUE;
	    ctype ta;
	    ctype cet;

	    llassert (e != NULL);
	    el = convertEnumList (e->enums);

	    if (!ltoken_isUndefined (e->opttagid))	/* named enumerator */
	      {
		ename = cstring_copy (ltoken_getRawString (e->opttagid));
	      }
	    else
	      {
		ename = fakeTag ();
	      }

	    cet = ctype_createEnum (ename, el);
	    
	    if (usymtab_existsEnumTag (ename))
	      {
		ta = uentry_getAbstractType (usymtab_lookupEnumTag (ename));
	      }
	    else
	      {	
		fileloc loc = fileloc_fromTok (e->tok);
		uentry ue = uentry_makeEnumTag (ename, cet, loc);

		ta = usymtab_supTypeEntry (ue);
	      }

	    enumNameList_elements (el, en)
	      {
		uentry ue;
		fileloc loc;

		if (first)
		  {
		    ltokenList_reset (e->enums);
		    first = FALSE;
		  }
		else
		  {
		    ltokenList_advance (e->enums);
		  }

		loc = fileloc_fromTok (ltokenList_current (e->enums));
		ue = uentry_makeSpecEnumConstant (en, cet, loc);

		/*
		** Can't check name here, might not have
		** type yet.  Will check in .lh file?
		*/
		
		ue = usymtab_supGlobalEntryReturn (ue);
		
		if (context_inLCLLib ())
		  {
		    uentry_setDefined (ue, loc);
		  }
	      } end_enumNameList_elements;
	    
	    result = qtype_create (ta);
	  }
	  break;
	default:
	  {
	    llfatalbug (message ("convertLclTypeSpecNode: unknown lclTypeSpec kind: %d",
				 (int) n->kind));
	  }
	}
      
      result = qtype_addQualList (result, n->quals);

      if (pointers_isDefined (n->pointers))
	{
	  qtype_adjustPointers (n->pointers, result);
	}

      return result;
    }
  else
    {
      llcontbug (cstring_makeLiteral ("convertLclTypeSpecNode: null"));
      return qtype_unknown (); 
    }
  BADEXIT;
} 

static /*@only@*/ multiVal
  literalValue (ctype ct, ltoken lit)
{
  cstring text = cstring_fromChars (lsymbol_toChars (ltoken_getText (lit)));
  char first;

  if (cstring_length (text) > 0)
    {
      first = cstring_firstChar (text);
    }
  else
    {
      return multiVal_unknown ();
    }

  
  if /*@-usedef@*/ (first == '\"') /*@=usedef@*/
    {
      size_t len = cstring_length (text) - 2;
      char *val = mstring_create (len);
      
      llassert (cstring_lastChar (text) == '\"');
      strncpy (val, cstring_toCharsSafe (text) + 1, len);
      return (multiVal_makeString (cstring_fromCharsO (val)));
    }

  if (ctype_isDirectInt (ct) || ctype_isPointer (ct))
    {
      long val = 0;
     
      if (sscanf (cstring_toCharsSafe (text), "%ld", &val) == 1)
	{
	  return multiVal_makeInt (val);
	}
    }

  return multiVal_unknown ();
}


/*
** declareConstant
**
** unfortunately, because the abstract types are different, this
** cannot be easily subsumed into declareVar.
*/

void
doDeclareConstant (constDeclarationNode c, bool priv)
{
  lclTypeSpecNode t;
  ctype ctx;
  qtype qt;
  
  if (c == (constDeclarationNode) 0)
    {
      return;
    }

  t = c->type;
  qt = convertLclTypeSpecNode (t);

  ctx = qtype_getType (qt);

  initDeclNodeList_elements (c->decls, i)
    {
      ctype ct = convertTypeExpr (ctx, i->declarator->type);
      cstring s = getVarName (i->declarator->type);

      if (ctype_isFunction (ct))
	{
	  fcnNode fcn = fcnNode_fromDeclarator (lclTypeSpecNode_copy (t), 
						declaratorNode_copy (i->declarator));

	  /* FALSE == unspecified function, only a declaration */

	  doDeclareFcn (fcn, typeId_invalid, priv, FALSE);
	  fcnNode_free (fcn);
	}
      else
	{
	  uentry ue;
	  fileloc loc = fileloc_fromTok (i->declarator->id);

	  if (i->value != (termNode)0 &&
	      i->value->kind == TRM_LITERAL)
	    {
	      ue = uentry_makeConstantValue (s, ct, loc, priv, literalValue (ct, i->value->literal));
	    }
	  else
	    {
	      ue = uentry_makeConstantValue (s, ct, loc, priv, multiVal_unknown ());
	    }
	  
	  uentry_reflectQualifiers (ue, qtype_getQuals (qt));
	  
	  if (context_inLCLLib () && !priv)
	    {
	      uentry_setDefined (ue, loc);
	    }

	  usymtab_supGlobalEntry (ue);
	}
    } end_initDeclNodeList_elements;

  qtype_free (qt);
}

static cstring
getVarName (/*@null@*/ typeExpr x)
{
  cstring s = cstring_undefined;

  if (x != (typeExpr) 0)
    {
      switch (x->kind)
	{
	case TEXPR_BASE:
	  s = ltoken_getRawString (x->content.base);
	  break;
	case TEXPR_PTR:
	  s = getVarName (x->content.pointer);
	  break;
	case TEXPR_ARRAY:
	  s = getVarName (x->content.array.elementtype);
	  break;
	case TEXPR_FCN:
	  s = getVarName (x->content.function.returntype);
	  break;
	default:
	  llfatalbug (message ("getVarName: unknown typeExprKind: %d", (int) x->kind));
	}
    }

  return s;
}

void
doDeclareVar (varDeclarationNode v, bool priv)
{
  lclTypeSpecNode t;
  qtype c;

  if (v == (varDeclarationNode) 0)
    {
      return;
    }

  t = v->type;
  c = convertLclTypeSpecNode (t);

  initDeclNodeList_elements (v->decls, i)
    {
      ctype ct = convertTypeExpr (qtype_getType (c), i->declarator->type);
      cstring s = getVarName (i->declarator->type);

      qtype_setType (c, ct);

      if (ctype_isFunction (ct))
	{
	  fcnNode fcn;

	  	  
	  fcn = fcnNode_fromDeclarator (lclTypeSpecNode_copy (t), 
					declaratorNode_copy (i->declarator));

	  /* FALSE == unspecified function, only a declaration */
	  declareFcnAux (fcn, qtype_unknown (), ct,
			 typeId_invalid, priv, FALSE);
	  fcnNode_free (fcn);
	}
      else
	{
	  fileloc loc = fileloc_fromTok (i->declarator->id);
	  uentry le = uentry_makeVariable (s, ct, loc, priv);
	  
	  uentry_reflectQualifiers (le, qtype_getQuals (c));
	  
	  if (uentry_isCheckedUnknown (le))
	    {
	      if (context_getFlag (FLG_IMPCHECKEDSTRICTSPECGLOBALS))
		{
		  uentry_setCheckedStrict (le);
		}
	      else if (context_getFlag (FLG_IMPCHECKEDSPECGLOBALS))
		{
		  uentry_setChecked (le);
		}
	      else if (context_getFlag (FLG_IMPCHECKMODSPECGLOBALS))
		{
		  uentry_setCheckMod (le);
		}
	      else
		{
		  ; /* okay */
		}
	    }

	  if (context_inLCLLib () && !priv)
	    {
	      uentry_setDefined (le, loc);
	    }

	  if (initDeclNode_isRedeclaration (i))
	    {
	      usymtab_replaceEntry (le);
	    }
	  else
	    {
	      le = usymtab_supEntrySrefReturn (le);
	    }
	}
    } end_initDeclNodeList_elements;

  qtype_free (c);
}

static globSet
processGlob (/*@returned@*/ globSet globs, varDeclarationNode v)
{
  if (v == (varDeclarationNode) 0)
    {
      return globs;
    }

  if (v->isSpecial)
    {
      globs = globSet_insert (globs, v->sref);
    }
  else
    {
      lclTypeSpecNode t = v->type;
      qtype qt = convertLclTypeSpecNode (t);
      ctype c = qtype_getType (qt);
      cstring s;      

      initDeclNodeList_elements (v->decls, i)
	{
	  ctype ct;
	  uentry ue;
	  qualList quals = qtype_getQuals (qt);
	  
	  s = getVarName (i->declarator->type);
	  ue = usymtab_lookupGlobSafe (s);
	  
	  if (uentry_isInvalid (ue))
	    {
	      ; /* error already reported */ 
	    }
	  else
	    {
	      if (uentry_isPriv (ue))
		{
		  globs = globSet_insert (globs, sRef_makeSpecState ());
		}
	      else
		{
		  uentry ce = uentry_copy (ue);
		  ctype lt = uentry_getType (ce);
		  fileloc loc = fileloc_fromTok (i->declarator->id);
		  
		  ct = convertTypeExpr (c, i->declarator->type);
		  
		  if (!ctype_match (lt, ct))
		    {
		      (void) gentypeerror
			(lt, exprNode_undefined,
			 ct, exprNode_undefined,
			 message ("Global type mismatch %s (%t, %t)",
				  s, lt, ct), 
			 loc);
		    }
		  
		  uentry_reflectQualifiers (ce, quals);
		  globs = globSet_insert (globs,
					  sRef_copy (uentry_getSref (ce)));
		  fileloc_free (loc);
		  uentry_free (ce);
		}
	    }
	} end_initDeclNodeList_elements;

      qtype_free (qt);
    }

  return globs;
}

static void
declareAbstractType (abstractNode n, bool priv)
{
  cstring tn;
  fileloc loc;
  uentry ue;
  typeId uid;
  abstBodyNode ab;

  if (n == (abstractNode) 0)
    {
      return;
    }

  
  tn = ltoken_getRawString (n->name);

  loc = fileloc_fromTok (n->tok);
  
  ue = uentry_makeDatatypeAux (tn, ctype_unknown, 
			       ynm_fromBool (n->isMutable), 
			       qual_createAbstract (), 
			       loc, priv);

  if (n->isRefCounted)
    {
      uentry_setRefCounted (ue);
    }

  if (context_inLCLLib () && !priv)
    {
      uentry_setDefined (ue, loc);
    }

  uid = usymtab_supAbstractTypeEntry (ue, context_inLCLLib() && !priv);
  

  if (!priv && (ab = n->body) != (abstBodyNode) 0)
    {
      fcnNodeList ops = ab->fcns;

      if (!fcnNodeList_isEmpty (ops))
	{
	  fcnNodeList_elements (ops, i)
	  {
	    if (i->typespec == (lclTypeSpecNode) 0)
	      {
		cstring fname = ltoken_getRawString (i->name);

		if (usymtab_exists (fname))
		  {
		    uentry e = usymtab_lookup (fname);
		    fileloc floc = fileloc_fromTok (i->declarator->id);

		    if (uentry_isForward (e))
		      {
			usymtab_supEntry  
			  (uentry_makeTypeListFunction 
			   (fname, typeIdSet_insert (uentry_accessType (e), uid), 
			    floc));
		      }
		    else
		      {
			usymtab_supEntry 
			  (uentry_makeSpecFunction 
			   (fname, uentry_getType (e),
			    typeIdSet_insert (uentry_accessType (e), uid),
			    globSet_undefined,
			    sRefSet_undefined,
			    floc));
			
			if (context_inLCLLib ())
			  {
			    llbuglit ("Jolly jeepers Wilma, it ain't dead after all!");
			  }
		      }
		  }
		else
		  {
		    usymtab_supEntry 
		      (uentry_makeForwardFunction (fname, uid, loc));
		  }
	      }
	    else
	      {
		declareFcn (i, uid);
	      }
	  } end_fcnNodeList_elements;
	}
    }
}

static void declareExposedType (exposedNode n, bool priv)
{
  qtype c;
  cstring s;

  
  if (n == (exposedNode) 0)
    {
      return;
    }

  c = convertLclTypeSpecNode (n->type);

  declaratorInvNodeList_elements (n->decls, i)
  {
    ctype realType = convertTypeExpr (qtype_getType (c), i->declarator->type);
    fileloc loc = fileloc_fromTok (i->declarator->id);
    uentry ue;

    s = getVarName (i->declarator->type);

    ue = uentry_makeDatatypeAux (s, realType, MAYBE, qual_createConcrete (), 
				 loc, priv);

    uentry_reflectQualifiers (ue, qtype_getQuals (c));
    
    if (context_inLCLLib () && !priv)
      {
	uentry_setDefined (ue, loc);
      }

    (void) usymtab_supExposedTypeEntry (ue, context_inLCLLib () && !priv);
  } end_declaratorInvNodeList_elements;

  qtype_free (c);
}

/*
** ah...remember ye old days...
**
** wow...same thing in THREE symbol tables!  talk about space efficiency
** (or as Joe Theory once said, its only a constant factor)
*/

void
doDeclareType (typeNode t, bool priv)
{
  
  if (t != (typeNode) 0)
    {
      switch (t->kind)
	{
	case TK_ABSTRACT:
	  declareAbstractType (t->content.abstract, priv);
	  break;
	  
	case TK_EXPOSED:
	  declareExposedType (t->content.exposed, priv);
	  break;
	  
	case TK_UNION:
	default:
	  {
	    llfatalbug (message ("declareType: unknown kind: %d",
				 (int) t->kind));
	  }
	}
    }
  
}

extern void
declareIter (iterNode iter)
{
  fileloc loc = fileloc_fromTok (iter->name);
  uentry ue = 
    uentry_makeIter (ltoken_unparse (iter->name), 
		     ctype_makeFunction 
		     (ctype_void,
		      paramNodeList_toUentryList (iter->params)), 
		     fileloc_copy (loc));
  
  usymtab_supEntry (ue);
  usymtab_supEntry 
    (uentry_makeEndIter (ltoken_unparse (iter->name), loc));
}

/*
** declareFcn
*/

static void
declareFcnAux (fcnNode f, /*@only@*/ qtype qt, ctype ct,
	       typeId tn, bool priv, bool spec)
{
  globalList globals;
  typeIdSet acct;
  sRefSet sl = sRefSet_undefined;
  globSet globlist = globSet_undefined;
  cstring s = getVarName (f->declarator->type);
  fileloc loc = fileloc_fromTok (f->declarator->id);
  uentryList args;

  /*
  ** type conversion generates args 
  */

  if (ctype_isFunction (ct))
    {
      args = ctype_argsFunction (ct);
    }
  else
    {
      llcontbug (message ("Not function: %s", ctype_unparse (ct)));
      args = uentryList_undefined;
    }

  
  fileloc_setColumnUndefined (loc);

  if (spec)
    {
      globals = f->globals;
      
      sl = fixModifies (f, args);

      /*
      ** Bind let declarations in modifies list 
      */

      varDeclarationNodeList_elements (globals, glob)
	{
	  globlist = processGlob (globlist, glob);
	} end_varDeclarationNodeList_elements;
      
      
      if (f->checks != (lclPredicateNode) 0)
	/* push stderr on globalList */
	/* modifies *stderr^ */
	{
	  uentry ue;

	  if (!(usymtab_existsVar (cstring_makeLiteralTemp ("stderr"))))
	    {
	      ctype tfile;
	      
	      llmsglit ("Global stderr implied by checks clause, "
			"not declared in initializations.");

	      tfile = usymtab_lookupType (cstring_makeLiteralTemp ("FILE"));
	      
	      if (ctype_isUndefined (tfile))
		{
		  llmsglit ("FILE datatype implied by checks clause not defined.");
		  tfile = ctype_unknown;
		}
	      
	      usymtab_supGlobalEntry 
		(uentry_makeVariable (cstring_makeLiteralTemp ("stderr"), 
				     tfile, fileloc_getBuiltin (), FALSE));
	    }
	  
	  ue = usymtab_lookupGlob (cstring_makeLiteralTemp ("stderr"));
	  
	  globlist = globSet_insert (globlist, sRef_copy (uentry_getSref (ue)));
	  sl = sRefSet_insert (sl, sRef_buildPointer (uentry_getSref (ue)));
	}
    }

  if (typeId_isInvalid (tn))
    {
      acct = context_fileAccessTypes ();
    }
  else
    {
      acct = typeIdSet_single (tn);
    }

  if (usymtab_exists (s))
    {
      uentry l = usymtab_lookup (s);
      uentry ue;

      if (uentry_isForward (l) || (fileloc_isLib (uentry_whereSpecified (l))))
	{
	  typeIdSet accessType;

	  if (uentry_isFunction (l))
	    {
	      accessType = typeIdSet_union (uentry_accessType (l), 
					    context_fileAccessTypes ());
	    }
	  else
	    {
	      accessType = context_fileAccessTypes ();
	    }

	  if (spec)
	    {
	      ue = uentry_makeSpecFunction (s, ct, accessType, globlist, sl, loc);
	    }
	  else
	    {
	      sRefSet_free (sl);
	      globSet_free (globlist);

	      ue = uentry_makeUnspecFunction (s, ct, accessType, loc);
	    }

	  uentry_reflectQualifiers (ue, qtype_getQuals (qt));
	  usymtab_supEntry (ue);
	}
      else
	{
	  /*
	  ** error reported by symtable already
          **
	  ** llgenerror (message ("Function redeclared: %s (previous declaration: %s)", s,
	  **		      fileloc_unparse (uentry_whereSpecified (l))),
	  **	      loc);
          */

	  fileloc_free (loc);
	  sRefSet_free (sl);
	  globSet_free (globlist);
	}
    }
  else
    {
      uentry le;
      
      if (spec)
	{
	  if (priv)
	    {
	      le = uentry_makePrivFunction2 (s, ct, acct, globlist, sl, loc);
	    }
	  else
	    {
	      le = uentry_makeSpecFunction (s, ct, acct, globlist, sl, loc);
	    }
	}
      else
	{
	  le = uentry_makeUnspecFunction (s, ct, acct, loc);

	  sRefSet_free (sl);
	  globSet_free (globlist);
	}

      if (context_inLCLLib () && !priv)
	{
	  uentry_setDefined (le, loc);
	}      

      uentry_reflectQualifiers (le, qtype_getQuals (qt));

      if (qual_isUnknown (f->special)) {
	;
      } else if (qual_isPrintfLike (f->special)) {
	uentry_setPrintfLike (le); 
      } else if (qual_isScanfLike (f->special)) {
	uentry_setScanfLike (le); 
      } else if (qual_isMessageLike (f->special)) {
	uentry_setMessageLike (le); 
      } else {
	BADBRANCH;
      }

      usymtab_supEntry (le);
    }

  qtype_free (qt);
}

extern void
doDeclareFcn (fcnNode f, typeId tn, bool priv, bool spec)
{
  qtype qt = convertLclTypeSpecNode (f->typespec);
  ctype ct = convertTypeExpr (qtype_getType (qt), f->declarator->type);

  declareFcnAux (f, qt, ct, tn, priv, spec);
}

/*
** is s is an argument to f, return its arg no.
** otherwise, return 0
*/

static int
getParamNo (cstring s, fcnNode f)
{
  /* gasp, maybe should do run-time checks here */
  paramNodeList params;
  typeExpr fd = f->declarator->type;
  
  /* is this a bug in the LCL grammar? */

  while (fd != NULL && (fd->kind == TEXPR_PTR || fd->kind == TEXPR_ARRAY))
    {
      if (fd->kind == TEXPR_PTR)
	{
	  fd = fd->content.pointer;
	}
      else
	{
	  /*@-null@*/ fd = fd->content.array.elementtype; /*@=null@*/
         
          /*
          ** This is a bug in checking, that I should eventually fix.
	  ** Need some way of deleting the guard from the true branch,
	  ** but adding it back in the false branch...
	  */
	}
    }

  llassert (fd != NULL);

  if (fd->kind != TEXPR_FCN)
    {
      llfatalbug (message ("getParamNo: not a function: %q (%d)",
			   typeExpr_unparse (fd), (int) fd->kind));
    }

  params = fd->content.function.args;

  if (paramNodeList_empty (params))
    {
      return -1;
    }
  else
    {
      int pno = 0;

      paramNodeList_elements (params, i)
	{
	  if (i->paramdecl != (typeExpr) 0)	/* handle (void) */
	    {
	      if (cstring_equal (s, getVarName (i->paramdecl)))
		{
		  return pno;
		}
	    }
	  pno++;
	} end_paramNodeList_elements;
      return -1;
    }
}

static /*@null@*/ /*@observer@*/ termNode
getLetDecl (cstring s, fcnNode f)
{
  letDeclNodeList x = f->lets;

  letDeclNodeList_elements (x, i)
  {
    if (cstring_equal (s, ltoken_getRawString (i->varid)))
      {
	if (i->sortspec != NULL)
	  {
	    llbuglit ("getLetDecl: cannot return sort!");
	  }
	else
	  {			/* is a termNode */
	    return i->term;
	  }
      }
  } end_letDeclNodeList_elements;

  return (termNode) 0;
}

/*
** processTermNode --- based on printTermNode2
*/

static /*@exposed@*/ sRef 
  processTermNode (/*@null@*/ opFormNode op, termNodeList args, 
		   fcnNode f, uentryList cl)
{
  if (op != (opFormNode) 0)
    {
      switch (op->kind)
	{
	case OPF_IF:
	  llcontbuglit ("processTermNode: OPF_IF: not handled");
	  break;
	case OPF_ANYOP:
	  llcontbuglit ("processTermNode: OPF_ANYOP: not handled");
	  break;
	case OPF_MANYOP:
	  {
	    int size = termNodeList_size (args);

	    if (size == 1 
		&& (cstring_equalLit (ltoken_getRawString (op->content.anyop), "'") ||
		    cstring_equalLit (ltoken_getRawString (op->content.anyop), "^")))
	      {
		return (fixTermNode (termNodeList_head (args), f, cl));
	      }
	    else 
	      {
		;
	      }
	    break;
	  }
	case OPF_ANYOPM:
	  {
	    int size = termNodeList_size (args);

	    if (size == 1 
		&& (cstring_equalLit (ltoken_getRawString (op->content.anyop), "*")))
	      {
		sRef ft;
		sRef res;

		ft = fixTermNode (termNodeList_head (args), f, cl);
		res = sRef_buildPointer (ft);
		return (res);
	      }
	    else
	      {
		;
	      }
	    break;
	  }
	case OPF_MANYOPM:
	  llcontbuglit ("OPF_MANYOPM: not handled\n");
	  break;
	case OPF_MIDDLE:
	  llcontbuglit ("OPF_MIDDLE: not handled\n");
	  break;
	case OPF_MMIDDLE:
	  llcontbuglit ("OPF_MMIDDLE: not handled\n");
	  break;
	case OPF_MIDDLEM:
	  llcontbuglit ("OPF_MIDDLEM: not handled\n");
	  break;
	case OPF_MMIDDLEM:
	  llcontbuglit ("OPF_MMIDDLEM: not handled\n");
	  break;
	case OPF_BMIDDLE:
	  if (op->content.middle == 1)
	    llbug (message ("array fetch: [%q]",
			    termNodeList_unparse (args)));
	  else
	    llcontbuglit ("OPF_BMIDDLE: bad\n");
	  break;

	case OPF_BMMIDDLE:
	  if (op->content.middle <= 1)
	    {
	      sRef arr = fixTermNode (termNodeList_head (args), f, cl);
	      sRef ret;

	      if (op->content.middle == 1)
		{
		  termNode t = (termNodeList_reset (args),
				termNodeList_advance (args),
				termNodeList_current (args));
		  
		  if (t->kind == TRM_LITERAL)
		    {
		      int i;

		      if (sscanf 
			  (cstring_toCharsSafe 
			   (ltoken_getRawString (t->literal)),
			   "%d", &i) == 1)
			{
			  ret = sRef_buildArrayFetchKnown (arr, i);
			}
		      else
			{
			  ret = sRef_buildArrayFetch (arr);
			}

		      return (ret);
		    }
		}
	      	      
	      /* unknown index */

	      ret = sRef_buildArrayFetch (arr);

	      return (ret);
	    }
	  else 
	    {
	      llcontbug (message ("op->content.middle = %d", 
				  op->content.middle));
	      break;
	    }

	case OPF_BMIDDLEM:
	  llcontbuglit ("OPF_BMIDDLEM not handled");
	  break; 

	case OPF_BMMIDDLEM:
	  llcontbuglit ("OPF_BMMIDDLEM not handled");
	  break;

	case OPF_SELECT:
	  llcontbug (message ("select: .%s", 
			      ltoken_getRawString (op->content.id)));
	  break; 

	case OPF_MAP:
	  llcontbug (message ("map: .%s", 
			      ltoken_getRawString (op->content.id)));
	  break;

	case OPF_MSELECT:
	  {
	    sRef rec = fixTermNode (termNodeList_head (args), f, cl);
	    sRef ret; 
	    ctype ct = ctype_realType (sRef_deriveType (rec, cl));
	    cstring fieldname = ltoken_getRawString (op->content.id);

	    ct = ctype_realType (ct);

	    /*
            ** does it correspond to a typedef struct field
            **
            ** (kind of kludgey, but there is no direct way to
            **  tell if it is an lsl operator instead)
	    */
	    
	    if (ctype_isStructorUnion (ct) &&
		uentry_isValid 
		(uentryList_lookupField (ctype_getFields (ct), fieldname)))
	      {
		cstring fname = cstring_copy (fieldname);

		ret = sRef_buildField (rec, fname);
		cstring_markOwned (fname);
	      }
	    else
	      {
		ret = sRef_undefined;
	      }

	    return ret;
	  }
	case OPF_MMAP: 
	  {
	    sRef rec = fixTermNode (termNodeList_head (args), f, cl);
	    sRef ret = sRef_undefined;
	    ctype ct = ctype_realType (sRef_deriveType (rec, cl));
	    cstring fieldname = ltoken_getRawString (op->content.id);

	    /*
	    ** does it correspond to a typedef struct field
	    */
	    
	    if (ctype_isPointer (ct))
	      {
		ctype ctb = ctype_realType (ctype_baseArrayPtr (ct));

		if (ctype_isStructorUnion (ctb) &&
		    uentry_isValid (uentryList_lookupField
				    (ctype_getFields (ctb), fieldname)))
		  {
		    cstring fname = cstring_copy (fieldname);

 		    ret = sRef_buildArrow (rec, fname);
		    cstring_markOwned (fname);
		  }
	      }

	    return ret;
	  }
	}
    }

  return sRef_undefined;
}

/*
** fixModifies
**
** o replace anything in modifies that is bound with let with value
** o replace spec variables with internal state 
** o replace paramaters with paramno identifiers
** o replace globals with their usymid's
** o make everything sRefs
*/

static /*@exposed@*/ sRef fixTermNode (termNode n, fcnNode f, uentryList cl)
{
  if (n != (termNode) 0)
    {
      switch (n->kind)
	{
	case TRM_LITERAL:
	  break;
	case TRM_CONST:
	case TRM_VAR:
	case TRM_ZEROARY:
	  {
	    cstring s = ltoken_getRawString (n->literal);
	    termNode tl = getLetDecl (s, f);

	    if (tl != (termNode) 0)
	      {
		return (fixTermNode (tl, f, cl));
	      }
	    else
	      {
		int i = getParamNo (s, f);

		if (i < 0)
		  {
		    usymId usym = usymtab_getId (s);
		    
		    if (usymId_isInvalid (usym))
		      {
			if (usymtab_existsEither (s))
			  {
			    return sRef_makeSpecState ();
			  }
			else
			  {
			    llcontbuglit ("Invalid symbol in modifies list");
			    return sRef_undefined;
			  }
		      }
		    else
		      return (sRef_makeGlobal (usym, ctype_unknown, stateInfo_currentLoc ()));
		  }
		
		else
		  {
		    sRef p = sRef_makeParam (i, ctype_unknown, stateInfo_currentLoc ());
		    return (p);
		  }
	      }
	  }
	case TRM_APPLICATION:
	  {
	    nameNode nn = n->name;

	    if (nn != (nameNode) 0)
	      {
		if (nn->isOpId)
		  {
		    /* must we handle n->given ? skip for now */

		    llfatalbug 
		      (message ("fixTermNode: expect non-empty nameNode: "
				"TRM_APPLICATION: %q",
				nameNode_unparse (nn)));
		  }
		else
		  {
		    sRef sr;

		    sr = processTermNode (nn->content.opform, n->args, f, cl);
		    		    return (sr);
		  }
	      }
	    
	    return sRef_undefined;
	  }
	case TRM_UNCHANGEDALL:      
	case TRM_UNCHANGEDOTHERS:
	case TRM_SIZEOF:
	case TRM_QUANTIFIER:
	  return sRef_undefined;
	}
    }

  return sRef_undefined;
}

static 
/*@only@*/ sRefSet fixModifies (fcnNode f, uentryList cl)
{
  static bool shownWarning = FALSE;
  modifyNode m = f->modify;
  sRefSet sl = sRefSet_new ();

  if (m != (modifyNode) 0)
    {
      if (m->hasStoreRefList)
	{
	  storeRefNodeList srefs = m->list;

	  storeRefNodeList_elements (srefs, i)
	    {
	      if (storeRefNode_isObj (i) || storeRefNode_isType (i))
		{
		  if (!shownWarning)
		    {
		      fileloc loc = fileloc_fromTok (f->name);
		  
		      llmsg (message
			     ("%q: Warning: object and type modifications "
			      "not understood by Splint",
			      fileloc_unparse (loc)));
		      fileloc_free (loc);
		      shownWarning = TRUE;
		    }
		}
	      else if (storeRefNode_isSpecial (i))
		{
		  sl = sRefSet_insert (sl, i->content.ref);
		}
	      else if (storeRefNode_isTerm (i))
		{
		  sRef s = fixTermNode (i->content.term, f, cl);

		  if (sRef_isKnown (s)) 
		    {
		      sl = sRefSet_insert (sl, s);
		    }
		}
	      else
		{
		  BADEXIT;
		}
	    } end_storeRefNodeList_elements;
	  
	}
    }

  return sl;
}

static /*@only@*/ cstring
paramNode_name (paramNode x)
{
  return (typeExpr_name (x->paramdecl));
}

static /*@only@*/ uentry
paramNode_toUentry (paramNode p)
{
  if (p != (paramNode) 0)
    {
      if (p->kind == PELIPSIS)
	{
	  return uentry_makeElipsisMarker ();
	}
      else
	{
	  qtype ct = convertLclTypeSpecNode (p->type);
	  ctype cr = convertTypeExpr (qtype_getType (ct), p->paramdecl);
	  cstring pname = (p->paramdecl == (typeExpr)0) ? cstring_undefined 
	                                                : paramNode_name (p);
	  uentry ue = uentry_makeVariableParam (pname, cr, g_currentloc);

	  uentry_reflectQualifiers (ue, qtype_getQuals (ct));
	  qtype_free (ct);
	  return (ue);
	}
    }
  else
    {
      llcontbuglit ("paramNode_toUentry: NULL");
      return uentry_undefined;
    }
  BADEXIT;
}

static uentryList
  paramNodeList_toUentryList (paramNodeList p)
{
  uentryList cl = uentryList_new ();

  if (paramNodeList_isNull (p)) return (cl);

  paramNodeList_elements (p, current)
    {
      cl = uentryList_add (cl, paramNode_toUentry (current));
    } end_paramNodeList_elements;

  return cl;
}


