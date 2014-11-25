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
** abstract.c
**
** Module for building abstract syntax trees for LCL.
**
** This module is too close to the surface syntax of LCL.
** Suffices for now.
**
** AUTHOR:
**	Yang Meng Tan,
**         Massachusetts Institute of Technology
*/

# include "splintMacros.nf"
# include "basic.h"
# include "lslparse.h"
# include "llgrammar.h"	/* need simpleOp, MULOP and logicalOp in makeInfixTermNode */
# include "lclscan.h"
# include "lh.h"
# include "imports.h"

static lsymbol lsymbol_Bool;
static lsymbol lsymbol_bool;
static lsymbol lsymbol_TRUE;
static lsymbol lsymbol_FALSE;

static void lclPredicateNode_free (/*@only@*/ /*@null@*/ lclPredicateNode p_x) ;
static void exposedNode_free (/*@only@*/ /*@null@*/ exposedNode p_x) ;
static void CTypesNode_free (/*@null@*/ /*@only@*/ CTypesNode p_x);
static /*@null@*/ CTypesNode CTypesNode_copy (/*@null@*/ CTypesNode p_x) /*@*/ ;
static void 
  constDeclarationNode_free (/*@only@*/ /*@null@*/ constDeclarationNode p_x);
static void claimNode_free (/*@only@*/ /*@null@*/ claimNode p_x);
static void iterNode_free (/*@only@*/ /*@null@*/ iterNode p_x);
static void abstBodyNode_free (/*@only@*/ /*@null@*/ abstBodyNode p_n);
static void abstractNode_free (/*@only@*/ /*@null@*/ abstractNode p_x);
static void taggedUnionNode_free (/*@only@*/ /*@null@*/ taggedUnionNode p_x);
static void typeNode_free (/*@only@*/ /*@null@*/ typeNode p_t);
static /*@null@*/ strOrUnionNode 
  strOrUnionNode_copy (/*@null@*/ strOrUnionNode p_n) /*@*/ ;
static void strOrUnionNode_free (/*@null@*/ /*@only@*/ strOrUnionNode p_n)
  /*@modifies *p_n @*/ ;

static void enumSpecNode_free (/*@null@*/ /*@only@*/ enumSpecNode p_x);
static /*@only@*/ /*@null@*/ enumSpecNode
  enumSpecNode_copy (/*@null@*/ enumSpecNode p_x) /*@*/ ;
static /*@only@*/ lclTypeSpecNode
  lclTypeSpecNode_copySafe (lclTypeSpecNode p_n) /*@*/ ;
static void lclTypeSpecNode_free (/*@null@*/ /*@only@*/ lclTypeSpecNode p_n);
static void typeNamePack_free (/*@only@*/ /*@null@*/ typeNamePack p_x);
static void opFormNode_free (/*@only@*/ /*@null@*/ opFormNode p_op);
static quantifiedTermNode quantifiedTermNode_copy (quantifiedTermNode p_q) /*@*/ ;
static void nameAndReplaceNode_free (/*@only@*/ /*@null@*/ nameAndReplaceNode p_x);
static void renamingNode_free (/*@only@*/ /*@null@*/ renamingNode p_x);
static void exportNode_free (/*@null@*/ /*@only@*/ exportNode p_x);
static void privateNode_free (/*@only@*/ /*@null@*/ privateNode p_x);
static /*@null@*/ termNode termNode_copy (/*@null@*/ termNode p_t) /*@*/ ;
static void 
  stmtNode_free (/*@only@*/ /*@null@*/ stmtNode p_x) /*@modifies *p_x@*/ ;
static /*@null@*/ typeExpr typeExpr_copy (/*@null@*/ typeExpr p_x) /*@*/ ;

static lsymbol ConditionalSymbol;
static lsymbol equalSymbol;
static lsymbol eqSymbol;
static lclTypeSpecNode exposedType;

static /*@only@*/ cstring abstDeclaratorNode_unparse (abstDeclaratorNode p_x);
static pairNodeList extractParams (/*@null@*/ typeExpr p_te);
static sort extractReturnSort (lclTypeSpecNode p_t, declaratorNode p_d);
static void checkAssociativity (termNode p_x, ltoken p_op);
static void LCLBootstrap (void);
static cstring printMiddle (int p_j);
static void paramNode_checkQualifiers (lclTypeSpecNode p_t, typeExpr p_d);

void
resetImports (cstring current)
{
  lsymbolSet_free (g_currentImports); 

  g_currentImports = lsymbolSet_new ();	/* equal_symbol; */
  (void) lsymbolSet_insert (g_currentImports, 
			      lsymbol_fromString (current));
}

void
abstract_init ()
{
  typeInfo ti = (typeInfo) dmalloc (sizeof (*ti));
  nameNode nn;
  ltoken dom, range;
  sigNode sign;
  opFormNode opform;
  ltokenList domain = ltokenList_new ();
  ltokenList domain2;

  equalSymbol = lsymbol_fromChars ("=");
  eqSymbol = lsymbol_fromChars ("\\eq");

  /*
  ** not: cstring_toCharsSafe (context_getBoolName ())
  ** we use the hard wired "bool" name.
  */

  lsymbol_bool = lsymbol_fromChars ("bool");
  lsymbol_Bool = lsymbol_fromChars ("Bool");

  lsymbol_TRUE = lsymbol_fromChars ("TRUE");
  lsymbol_FALSE = lsymbol_fromChars ("FALSE");

  ConditionalSymbol = lsymbol_fromChars ("if__then__else__");

  /* generate operators for
  **    __ \not, __ \implies __ , __ \and __, __ \or __ 
  */

  range = ltoken_create (simpleId, lsymbol_bool);
  dom = ltoken_create (simpleId, lsymbol_bool);

  ltokenList_addh (domain, ltoken_copy (dom));

  domain2 = ltokenList_copy (domain);  /* moved this here (before release) */

  sign = makesigNode (ltoken_undefined, domain, ltoken_copy (range));

  opform = makeOpFormNode (ltoken_undefined, OPF_ANYOPM, 
			   opFormUnion_createAnyOp (ltoken_not),
			   ltoken_undefined);
  nn = makeNameNodeForm (opform);
    symtable_enterOp (g_symtab, nn, sign);

  ltokenList_addh (domain2, dom);

  sign = makesigNode (ltoken_undefined, domain2, range);

  opform = makeOpFormNode (ltoken_undefined, OPF_MANYOPM, 
			   opFormUnion_createAnyOp (ltoken_and), 
			   ltoken_undefined);

  nn = makeNameNodeForm (opform);
  symtable_enterOp (g_symtab, nn, sigNode_copy (sign));

  opform = makeOpFormNode (ltoken_undefined, OPF_MANYOPM, 
			   opFormUnion_createAnyOp (ltoken_or),
			   ltoken_undefined);

  nn = makeNameNodeForm (opform);
  symtable_enterOp (g_symtab, nn, sigNode_copy (sign));

  opform = makeOpFormNode (ltoken_undefined, OPF_MANYOPM, 
			   opFormUnion_createAnyOp (ltoken_implies),
			   ltoken_undefined);
  nn = makeNameNodeForm (opform);
  symtable_enterOp (g_symtab, nn, sign);
  
  /* from lclscanline.c's init procedure */
  /* comment out so we can add in lclinit.lci: synonym double float */
  /* ReserveToken (FLOAT,		    "float"); */
  /* But we need to make the scanner parse "float" not as a simpleId, but
     as a TYPEDEF_NAME.  This is done later in abstract_init  */
  
  ti->id = ltoken_createType (LLT_TYPEDEF_NAME, SID_TYPE, lsymbol_fromChars ("float"));

  ti->modifiable = FALSE;
  ti->abstract = FALSE;
  ti->export = FALSE;		/* this is implicit, not exported */
  ti->basedOn = g_sortFloat;
  symtable_enterType (g_symtab, ti);
}

void 
declareForwardType (declaratorNode declare)
{
  typeInfo ti = (typeInfo) dmalloc (sizeof (*ti));
  sort tsort, handle;
  lsymbol typedefname;

  typedefname = ltoken_getText (declare->id);
  ti->id = ltoken_copy (declare->id);

  ltoken_setCode (ti->id, LLT_TYPEDEF_NAME);
  ltoken_setIdType (ti->id, SID_TYPE);

  ti->modifiable = FALSE;
  ti->abstract = FALSE;
  tsort = lclTypeSpecNode2sort (exposedType);
  handle = typeExpr2ptrSort (tsort, declare->type);
  ti->basedOn = sort_makeSyn (declare->id, handle, typedefname);
  ti->export = FALSE;

  symtable_enterType (g_symtab, ti);
}

void LCLBuiltins (void)
{
  typeInfo ti = (typeInfo) dmalloc (sizeof (*ti));
  varInfo vi = (varInfo) dmalloc (sizeof (*vi));
  
  /* immutable type bool;
     uses CTrait;
     constant bool FALSE = false;
     constant bool TRUE  = true; */
  
  /* the following defines the builtin LSL sorts and operators */
  LCLBootstrap ();
  
  /* now LCL builtin proper */
  /* do "immutable type bool;" */
  
  ti->id = ltoken_copy (ltoken_bool);

  ltoken_setCode (ti->id, LLT_TYPEDEF_NAME);
  ltoken_setIdType (ti->id, SID_TYPE);

  ti->modifiable = FALSE;
  ti->abstract = TRUE;
  ti->basedOn = g_sortBool;
  ti->export = FALSE; /* this wasn't set (detected by Splint) */
  symtable_enterType (g_symtab, ti);
  
  /* do "constant bool FALSE = false;" */
  vi->id = ltoken_createType (simpleId, SID_VAR, lsymbol_fromChars ("FALSE"));

  vi->kind = VRK_CONST;
  vi->sort = g_sortBool;
  vi->export = TRUE;

  (void) symtable_enterVar (g_symtab, vi);
  
  /* do "constant bool TRUE  = true;"  */
  /* vi->id = ltoken_copy (vi->id); */
  ltoken_setText (vi->id, lsymbol_fromChars ("TRUE"));
  (void) symtable_enterVar (g_symtab, vi);

  varInfo_free (vi);
  
  importCTrait ();
}

static void
LCLBootstrap (void)
{
  nameNode nn1, nn2;
  ltoken range;
  sigNode sign;
  sort s;

 /*
 ** Minimal we need to bootstrap is to provide the sort
 ** "bool" and 2 bool constants "true" and "false". 
 ** sort_init should already have been called, and hence
 ** the bool and Bool sorts defined.
 */
 
  s = sort_makeImmutable (ltoken_undefined, lsymbol_bool);
  range = ltoken_create (simpleId, lsymbol_bool);
  sign = makesigNode (ltoken_undefined, ltokenList_new (),  range);

  nn1 = (nameNode) dmalloc (sizeof (*nn1));
  nn1->isOpId = TRUE;

  nn1->content.opid = ltoken_create (simpleId, lsymbol_fromChars ("true"));

  symtable_enterOp (g_symtab, nn1, sign);
  
  nn2 = (nameNode) dmalloc (sizeof (*nn2));
  nn2->isOpId = TRUE;
  nn2->content.opid = ltoken_create (simpleId, lsymbol_fromChars ("false"));

  symtable_enterOp (g_symtab, nn2, sigNode_copy (sign));
}

interfaceNodeList
consInterfaceNode (/*@only@*/ interfaceNode n, /*@returned@*/ interfaceNodeList ns)
{
  /* n is never empty, but ns may be empty */
  interfaceNodeList_addl (ns, n);
  return (ns);
}

/*@only@*/ interfaceNode
makeInterfaceNodeImports (/*@only@*/ importNodeList x)
{
  interfaceNode i = (interfaceNode) dmalloc (sizeof (*i));
  lsymbol importSymbol;

  i->kind = INF_IMPORTS;
  i->content.imports = x;	/* an importNodeList */
  
  importNodeList_elements (x, imp)
    {
      importSymbol = ltoken_getRawText (imp->val);
      
      if (lsymbolSet_member (g_currentImports, importSymbol))
	{
	  lclerror (imp->val, 
		    message ("Circular imports: %s", 
			     cstring_fromChars (lsymbol_toChars (importSymbol))));
	}      
      else
	{
	  processImport (importSymbol, imp->val, imp->kind);
	}
    } end_importNodeList_elements;

  lhOutLine (cstring_undefined);
  return (i);
}

/*@only@*/ interfaceNode
makeInterfaceNodeUses (/*@only@*/ traitRefNodeList x)
{
  interfaceNode i = (interfaceNode) dmalloc (sizeof (*i));

  i->kind = INF_USES;
  i->content.uses = x;
  /* read in LSL traits */

  return (i);
}

/*@only@*/ interfaceNode
interfaceNode_makeConst (/*@only@*/ constDeclarationNode x)
{
  interfaceNode i = (interfaceNode) dmalloc (sizeof (*i));
  exportNode e = (exportNode) dmalloc (sizeof (*e));

  e->kind = XPK_CONST;
  e->content.constdeclaration = x;
  i->kind = INF_EXPORT;
  i->content.export = e;

  return (i);
}

/*@only@*/ interfaceNode
interfaceNode_makeVar (/*@only@*/ varDeclarationNode x)
{
  interfaceNode i = (interfaceNode) dmalloc (sizeof (*i));
  exportNode e = (exportNode) dmalloc (sizeof (*e));

  e->kind = XPK_VAR;
  e->content.vardeclaration = x;
  i->kind = INF_EXPORT;
  i->content.export = e;
  
  if (context_msgLh ())
    {
      lhOutLine (lhVarDecl (x->type, x->decls, x->qualifier));
    }

    return (i);
}

/*@only@*/ interfaceNode
interfaceNode_makeType (/*@only@*/ typeNode x)
{
  interfaceNode i = (interfaceNode) dmalloc (sizeof (*i));
  exportNode e = (exportNode) dmalloc (sizeof (*e));
  e->kind = XPK_TYPE;
  e->content.type = x;
  i->kind = INF_EXPORT;
  i->content.export = e;

  if (context_msgLh ())
    {
      
      lhOutLine (lhType (x));
    }

  return (i);
}

/*@only@*/ interfaceNode
interfaceNode_makeFcn (/*@only@*/ fcnNode x)
{
  interfaceNode i = (interfaceNode) dmalloc (sizeof (*i));
  exportNode e = (exportNode) dmalloc (sizeof (*e));

  e->kind = XPK_FCN;
  e->content.fcn = x;
  i->kind = INF_EXPORT;
  i->content.export = e;

  if (context_msgLh ())
    {
      llassert (x->typespec != NULL);
      llassert (x->declarator != NULL);

      lhOutLine (lhFunction (x->typespec, x->declarator));
    }

  return (i);
}

/*@only@*/ interfaceNode
interfaceNode_makeClaim (/*@only@*/ claimNode x)
{
  interfaceNode i = (interfaceNode) dmalloc (sizeof (*i));
  exportNode e = (exportNode) dmalloc (sizeof (*e));

  e->kind = XPK_CLAIM;
  e->content.claim = x;
  i->kind = INF_EXPORT;
  i->content.export = e;
  return (i);
}

/*@only@*/ interfaceNode
interfaceNode_makeIter (/*@only@*/ iterNode x)
{
  interfaceNode i = (interfaceNode) dmalloc (sizeof (*i));
  exportNode e = (exportNode) dmalloc (sizeof (*e));

  e->kind = XPK_ITER;
  e->content.iter = x;
  i->kind = INF_EXPORT;
  i->content.export = e;
  return (i);
}

/*@only@*/ interfaceNode
interfaceNode_makePrivConst (/*@only@*/ constDeclarationNode x)
{
  interfaceNode i = (interfaceNode) dmalloc (sizeof (*i));
  privateNode e = (privateNode) dmalloc (sizeof (*e));

  e->kind = PRIV_CONST;
  e->content.constdeclaration = x;
  i->kind = INF_PRIVATE;
  i->content.private = e;
  return (i);
}

/*@only@*/ interfaceNode
interfaceNode_makePrivVar (/*@only@*/ varDeclarationNode x)
{
  interfaceNode i = (interfaceNode) dmalloc (sizeof (*i));
  privateNode e = (privateNode) dmalloc (sizeof (*e));
  
  e->kind = PRIV_VAR;
  e->content.vardeclaration = x;
  i->kind = INF_PRIVATE;
  i->content.private = e;
  return (i);
}

/*@only@*/ interfaceNode
interfaceNode_makePrivType (/*@only@*/ typeNode x)
{
  interfaceNode i = (interfaceNode) dmalloc (sizeof (*i));
  privateNode e = (privateNode) dmalloc (sizeof (*e));

  e->kind = PRIV_TYPE;
  e->content.type = x;
  i->kind = INF_PRIVATE;
  i->content.private = e;
  return (i);
}

/*@only@*/ interfaceNode
interfaceNode_makePrivFcn (/*@only@*/ fcnNode x)
{
  interfaceNode i = (interfaceNode) dmalloc (sizeof (*i));
  privateNode e = (privateNode) dmalloc (sizeof (*e));

  /*
  ** bug detected by enum checking
  ** e->kind = XPK_FCN;
  */

  e->kind = PRIV_FUNCTION;
  e->content.fcn = x;
  i->kind = INF_PRIVATE;
  i->content.private = e;
  return (i);
}

/*@only@*/ cstring
exportNode_unparse (exportNode n)
{
  if (n != (exportNode) 0)
    {
      switch (n->kind)
	{
	case XPK_CONST:
	  return (message 
		  ("%q\n", 
		   constDeclarationNode_unparse (n->content.constdeclaration)));
	case XPK_VAR:
	  return (message 
		  ("%q\n", 
		   varDeclarationNode_unparse (n->content.vardeclaration)));
	case XPK_TYPE:
	  return (message ("%q\n", typeNode_unparse (n->content.type)));
	case XPK_FCN:
	  return (fcnNode_unparse (n->content.fcn));
	case XPK_CLAIM:
	  return (claimNode_unparse (n->content.claim));
	case XPK_ITER:
	  return (iterNode_unparse (n->content.iter));
	default:
	  llfatalbug (message ("exportNode_unparse: unknown kind: %d", (int) n->kind));
	}
    }
  return cstring_undefined;
}

/*@only@*/ cstring
privateNode_unparse (privateNode n)
{
  if (n != (privateNode) 0)
    {
      switch (n->kind)
	{
	case PRIV_CONST:
	  return (constDeclarationNode_unparse (n->content.constdeclaration));
	case PRIV_VAR:
	  return (varDeclarationNode_unparse (n->content.vardeclaration));
	case PRIV_TYPE:
	  return (typeNode_unparse (n->content.type));
	case PRIV_FUNCTION:
	  return (fcnNode_unparse (n->content.fcn));
	default:
	  llfatalbug (message ("privateNode_unparse: unknown kind: %d", 
			       (int) n->kind));
	}
    }
  return cstring_undefined;
}

void lclPredicateNode_free (/*@null@*/ /*@only@*/ lclPredicateNode x)
{
  if (x != NULL)
    {
      termNode_free (x->predicate);
      ltoken_free (x->tok);
      sfree (x);
    }
}

static /*@only@*/ cstring
lclPredicateNode_unparse (/*@null@*/ lclPredicateNode p) /*@*/ 
{
  if (p != (lclPredicateNode) 0)
    {
      cstring st = cstring_undefined;
      
      switch (p->kind)
	{
	case LPD_REQUIRES:
	  st = cstring_makeLiteral ("  requires ");
	  break;
	case LPD_CHECKS:
	  st = cstring_makeLiteral ("  checks "); 
	  break;
	case LPD_ENSURES:
	  st = cstring_makeLiteral ("  ensures ");
	  break;
	case LPD_INTRACLAIM:
	  st = cstring_makeLiteral ("  claims ");
	  break;
	case LPD_CONSTRAINT:
	  st = cstring_makeLiteral ("constraint ");
	  break;
	case LPD_INITIALLY:
	  st = cstring_makeLiteral ("initially ");
	  break;
	case LPD_PLAIN:
	  break;
	default:
	  llfatalbug (message ("lclPredicateNode_unparse: unknown kind: %d", 
			       (int) p->kind));
	}
      return (message ("%q%q;\n", st, termNode_unparse (p->predicate)));
    }
  return cstring_undefined;
}

bool
ltoken_similar (ltoken t1, ltoken t2)
{
  lsymbol sym1 = ltoken_getText (t1);
  lsymbol sym2 = ltoken_getText (t2);
  
  if (sym1 == sym2)
    {
      return TRUE;
    }

  if ((sym1 == eqSymbol && sym2 == equalSymbol) ||
      (sym2 == eqSymbol && sym1 == equalSymbol))
    {
      return TRUE;
    }

  if ((sym1 == lsymbol_bool && sym2 == lsymbol_Bool) ||
      (sym2 == lsymbol_bool && sym1 == lsymbol_Bool))
    {
      return TRUE;
    }

  return FALSE;
}

/*@only@*/ cstring
iterNode_unparse (/*@null@*/ iterNode i)
{
  if (i != (iterNode) 0)
    {
      return (message ("iter %s %q", ltoken_unparse (i->name), 
		       paramNodeList_unparse (i->params)));
    }
  return cstring_undefined;
}


/*@only@*/ cstring
fcnNode_unparse (/*@null@*/ fcnNode f)
{
  if (f != (fcnNode) 0)
    {
      return (message ("%q %q%q{\n%q%q%q%q%q%q}\n",
		       lclTypeSpecNode_unparse (f->typespec),
		       declaratorNode_unparse (f->declarator),
		       varDeclarationNodeList_unparse (f->globals),
		       varDeclarationNodeList_unparse (f->inits),
		       letDeclNodeList_unparse (f->lets),
		       lclPredicateNode_unparse (f->require),
		       modifyNode_unparse (f->modify),
		       lclPredicateNode_unparse (f->ensures),
		       lclPredicateNode_unparse (f->claim)));
    }
  return cstring_undefined;
}

/*@only@*/ cstring
varDeclarationNode_unparse (/*@null@*/ varDeclarationNode x)
{
  if (x != (varDeclarationNode) 0)
    {
      cstring st;

      if (x->isSpecial)
	{
	  return (sRef_unparse (x->sref));
	}
      else
	{
	  switch (x->qualifier)
	    {
	    case QLF_NONE:
	      st = cstring_undefined;
	      break;
	    case QLF_CONST:
	      st = cstring_makeLiteral ("const ");
	      break;
	    case QLF_VOLATILE:
	      st = cstring_makeLiteral ("volatile ");
	      break;
	      BADDEFAULT;
	    }
	  
	  st = message ("%q%q %q", st, lclTypeSpecNode_unparse (x->type),
			initDeclNodeList_unparse (x->decls));
	  return (st);
	}
    }

  return cstring_undefined;
}

/*@only@*/ cstring
typeNode_unparse (/*@null@*/ typeNode t)
{
  if (t != (typeNode) 0)
    {
      switch (t->kind)
	{
	case TK_ABSTRACT:
	  return (abstractNode_unparse (t->content.abstract));
	case TK_EXPOSED:
	  return (exposedNode_unparse (t->content.exposed));
	case TK_UNION:
	  return (taggedUnionNode_unparse (t->content.taggedunion));
	default:
	  llfatalbug (message ("typeNode_unparse: unknown kind: %d", (int)t->kind));
	}
    }
  return cstring_undefined;
}

/*@only@*/ cstring
constDeclarationNode_unparse (/*@null@*/ constDeclarationNode x)
{
  if (x != (constDeclarationNode) 0)
    {
      return (message ("constant %q %q", lclTypeSpecNode_unparse (x->type),
		       initDeclNodeList_unparse (x->decls)));
    }

  return cstring_undefined;
}

/*@only@*/ storeRefNode
makeStoreRefNodeTerm (/*@only@*/ termNode t)
{
  storeRefNode x = (storeRefNode) dmalloc (sizeof (*x));

  x->kind = SRN_TERM;
  x->content.term = t;
  return (x);
}

/*@only@*/ storeRefNode
makeStoreRefNodeType (/*@only@*/ lclTypeSpecNode t, bool isObj)
{
  storeRefNode x = (storeRefNode) dmalloc (sizeof (*x));

  x->kind = isObj ? SRN_OBJ : SRN_TYPE;
  x->content.type = t;
  return (x);
}

storeRefNode
makeStoreRefNodeInternal (void)
{
  storeRefNode x = (storeRefNode) dmalloc (sizeof (*x));

  x->kind = SRN_SPECIAL;
  x->content.ref = sRef_makeInternalState ();
  return (x);
}

storeRefNode
makeStoreRefNodeSystem (void)
{
  storeRefNode x = (storeRefNode) dmalloc (sizeof (*x));

  x->kind = SRN_SPECIAL;
  x->content.ref = sRef_makeSystemState ();
  return (x);
}

/*@only@*/ modifyNode
makeModifyNodeSpecial (/*@only@*/ ltoken t, bool modifiesNothing)
{
  modifyNode x = (modifyNode) dmalloc (sizeof (*x));

  x->tok = t;
  x->modifiesNothing = modifiesNothing;
  x->hasStoreRefList = FALSE;
  return (x);
}

/*@only@*/ modifyNode
makeModifyNodeRef (/*@only@*/ ltoken t, /*@only@*/ storeRefNodeList y)
{
  modifyNode x = (modifyNode) dmalloc (sizeof (*x));
  sort sort;
  
  x->tok = t;
  x->hasStoreRefList = TRUE;
  x->modifiesNothing = FALSE;
  x->list = y;
  /* check that all storeRef's are modifiable */
  
  storeRefNodeList_elements (y, sr)
    {
      if (storeRefNode_isTerm (sr))
	{
	  sort = sr->content.term->sort;

	  if (!sort_mutable (sort) && sort_isValidSort (sort))
	    {
	      ltoken errtok = termNode_errorToken (sr->content.term);
	      lclerror (errtok, 
			message ("Term denoting immutable object used in modifies list: %q",
				 termNode_unparse (sr->content.term)));
	    }
	}
      else 
	{
	  if (!storeRefNode_isSpecial (sr))
	    {
	      sort = lclTypeSpecNode2sort (sr->content.type);
	      
	      if (storeRefNode_isObj (sr))
		{
		  sort = sort_makeObj (sort);
		}
	      
	      if (!sort_mutable (sort))
		{
		  ltoken errtok = lclTypeSpecNode_errorToken (sr->content.type);
		  lclerror (errtok, 
			    message ("Immutable type used in modifies list: %q",
				     sort_unparse (sort)));
		}
	    }
	}
    } end_storeRefNodeList_elements;
  return (x);
}

/*@observer@*/ ltoken
termNode_errorToken (/*@null@*/ termNode n)
{
  if (n != (termNode) 0)
    {
      switch (n->kind)
	{
	case TRM_LITERAL:
	case TRM_UNCHANGEDALL:
	case TRM_UNCHANGEDOTHERS:
	case TRM_SIZEOF:
	case TRM_CONST:
	case TRM_VAR:
	case TRM_ZEROARY:	/* also the default kind, when no in symbol table */
	  return n->literal;
	case TRM_QUANTIFIER:
	  return n->quantified->open;
	case TRM_APPLICATION:
	  if (n->name != NULL)
	    {
	      if (n->name->isOpId)
		{
		  return n->name->content.opid;
		}
	      else
		{
		  llassert (n->name->content.opform != NULL);
		  return n->name->content.opform->tok;
		}
	    }
	  else
	    {
	      return ltoken_undefined;
	    }
	}
    }
  return ltoken_undefined;
}

/*@observer@*/ ltoken
nameNode_errorToken (/*@null@*/ nameNode nn)
{
  if (nn != (nameNode) 0)
    {
      if (nn->isOpId)
	{
	  return nn->content.opid;
	}
      else
	{
	  if (nn->content.opform != NULL)
	    {
	      return nn->content.opform->tok;
	    }
	}
    }

  return ltoken_undefined;
}

/*@observer@*/ ltoken
lclTypeSpecNode_errorToken (/*@null@*/ lclTypeSpecNode t)
{
  if (t != (lclTypeSpecNode) 0)
    {
      switch (t->kind)
	{
	case LTS_TYPE:
	  {
	    llassert (t->content.type != NULL);

	    if (ltokenList_empty (t->content.type->ctypes))
	      break;
	    else
	      return (ltokenList_head (t->content.type->ctypes));
	  }
	case LTS_STRUCTUNION:
	  llassert (t->content.structorunion != NULL);
	  return t->content.structorunion->tok;
	case LTS_ENUM:
	  llassert (t->content.enumspec != NULL);
	  return t->content.enumspec->tok;
	case LTS_CONJ:
	  return (lclTypeSpecNode_errorToken (t->content.conj->a));
	}
    }

  return ltoken_undefined;
}

static bool
sort_member_modulo_cstring (sort s, /*@null@*/ termNode t)
{
  
  if (t != (termNode) 0)
    {
      if (t->kind == TRM_LITERAL)
	{ /* allow multiple types */
	  sortNode sn;

	  sortSet_elements (t->possibleSorts, el)
	    {
	      if (sort_compatible_modulo_cstring (s, el))
		{
		  return TRUE;
		}
	    } end_sortSet_elements;

	  sn = sort_lookup (s);

	  if (sn->kind == SRT_PTR)
	    {
	      char *lit = lsymbol_toChars (ltoken_getText (t->literal));
	      
	      if (lit != NULL)
		{
		  long val = 0;
		  
		  if (sscanf (lit, "%ld", &val) == 1)
		    {
		      if (val == 0) return TRUE;
		    }
		}
	    }
	  
	  return FALSE;
	}
      else
	{
	  return sort_compatible_modulo_cstring (s, t->sort);
	}
    }
  return FALSE;
}

/*@only@*/ letDeclNode
  makeLetDeclNode (ltoken varid, /*@only@*/ /*@null@*/ lclTypeSpecNode t, 
		   /*@only@*/ termNode term)
{
  letDeclNode x = (letDeclNode) dmalloc (sizeof (*x));
  varInfo vi = (varInfo) dmalloc (sizeof (*vi));
  ltoken errtok;
  sort s, termsort;

  if (t != (lclTypeSpecNode) 0)
    {
      /* check varid has the same sort as term */
      s = lclTypeSpecNode2sort (t);
      termsort = term->sort;
      /* should keep the arguments in order */
      if (!sort_member_modulo_cstring (s, term) &&
	  !term->error_reported)
	{
	  errtok = termNode_errorToken (term);
	  
	  /*      errorShowPoint (inputStream_thisLine (lclsource), ltoken_getCol (errtok)); */
	  /*      sprintf (ERRMSG, "expect `%s' type but given term has `%s' type",
		  sort_unparse (s), sort_unparse (termsort)); */
	  
	  lclerror (errtok, 
		    message ("Let declaration expects type %q", sort_unparse (s)));
	  /* evs --- don't know how to generated this message or what it means? */
	}
    }
  else
    {
      s = term->sort;
    }
  /* assign variable its type and sort, store in symbol table */
  vi->id = ltoken_copy (varid);
  vi->kind = VRK_LET;
  vi->sort = s;
  vi->export = TRUE;

  (void) symtable_enterVar (g_symtab, vi);
  varInfo_free (vi);

  x->varid = varid;
  x->sortspec = t;
  x->term = term;
  x->sort = sort_makeNoSort ();

  return (x);
}

/*@only@*/ programNode
makeProgramNodeAction (/*@only@*/ programNodeList x, actionKind k)
{
  programNode n = (programNode) dmalloc (sizeof (*n));
  n->wrapped = 0;
  n->kind = k;
  n->content.args = x;
  return (n);
}

/*@only@*/ programNode
makeProgramNode (/*@only@*/ stmtNode x)
{
  programNode n = (programNode) dmalloc (sizeof (*n));

  n->wrapped = 0;
  n->kind = ACT_SELF;
  n->content.self = x;
  return (n);
}

/*@only@*/ typeNode
makeAbstractTypeNode (/*@only@*/ abstractNode x)
{
  typeNode n = (typeNode) dmalloc (sizeof (*n));
  
  n->kind = TK_ABSTRACT;
  n->content.abstract = x;
  
    return (n);
}

/*@only@*/ typeNode
makeExposedTypeNode (/*@only@*/ exposedNode x)
{
  typeNode n = (typeNode) dmalloc (sizeof (*n));

  n->kind = TK_EXPOSED;
  n->content.exposed = x;
  return (n);
}

/*
** evs added 8 Sept 1993
*/

/*@only@*/ importNode
importNode_makePlain (/*@only@*/ ltoken t)
{
  importNode imp = (importNode) dmalloc (sizeof (*imp));

  imp->kind = IMPPLAIN;
  imp->val = t;
  return (imp);
}

/*@only@*/ importNode
importNode_makeBracketed (/*@only@*/ ltoken t)
{
  importNode imp = (importNode) dmalloc (sizeof (*imp));

  imp->kind = IMPBRACKET;
  imp->val = t;
  return (imp);
}

static cstring extractQuote (/*@only@*/ cstring s)
{
  size_t len = cstring_length (s);
  char *sc = cstring_toCharsSafe (s);
  cstring t;

  llassert (len > 1);
  *(sc + len - 1) = '\0';
  t = cstring_fromChars (mstring_copy (sc + 1));
  cstring_free (s);
  return (t);
}

/*@only@*/ importNode
importNode_makeQuoted (/*@only@*/ ltoken t)
{
  importNode imp = (importNode) dmalloc (sizeof (*imp));
  cstring q = extractQuote (cstring_copy (ltoken_getRawString (t)));

  imp->kind = IMPQUOTE;

  ltoken_setRawText (t, lsymbol_fromString (q));

  imp->val = t;  

  cstring_free (q);
  return (imp);
}

/*
** check that is it '<' and '>'
** should probably be in a different file?
*/

static void cylerror (/*@only@*/ char *s)
{
  ylerror(s);
  sfree (s);
}

void
checkBrackets (ltoken lb, ltoken rb)
{
  /* no attempt at error recovery...not really necessary */
  cstring tname;

  tname = ltoken_getRawString (lb);

  if (!cstring_equalLit (tname, "<"))
    {
      cylerror (cstring_toCharsSafeO (message ("Invalid import token: %s", tname)));
    }

  tname = ltoken_getRawString (rb);

  if (!cstring_equalLit (tname, ">"))
    {
      cylerror (cstring_toCharsSafeO (message ("Invalid import token: %s", tname)));
    }
}

/*@only@*/ traitRefNode
makeTraitRefNode (/*@only@*/ ltokenList fl, /*@only@*/ renamingNode r)
{
  traitRefNode n = (traitRefNode) dmalloc (sizeof (*n));

  n->traitid = fl;
  n->rename = r;
  return (n);
}

/*
** printLeaves: no commas
*/

static /*@only@*/ cstring
printLeaves (ltokenList f)
{
  bool firstone = TRUE;
  cstring s = cstring_undefined;

  ltokenList_elements (f, i)
  {
    if (firstone)
      {
	s = cstring_copy (ltoken_unparse (i));
	firstone = FALSE;
      }
    else
      {
	s = message ("%q %s", s, ltoken_unparse (i));
      }
  } end_ltokenList_elements;

  return s;
}


/*@only@*/ cstring
printLeaves2 (ltokenList f)
{
  return (ltokenList_unparse (f));
}

/*@only@*/ cstring
printRawLeaves2 (ltokenList f)
{
  bool first = TRUE;
  cstring s = cstring_undefined;

  ltokenList_elements (f, i)
  {
    if (first)
      {
	s = message ("%s", ltoken_getRawString (i));
	first = FALSE;
      }
    else
      s = message ("%q, %s", s, ltoken_getRawString (i));
  } end_ltokenList_elements;

  return s;
}

/*@only@*/ renamingNode
makeRenamingNode (/*@only@*/ typeNameNodeList n, /*@only@*/ replaceNodeList r)
{
   renamingNode ren = (renamingNode) dmalloc (sizeof (*ren));

  if (typeNameNodeList_empty (n))
    {
      ren->is_replace = TRUE;
      ren->content.replace = r;
      typeNameNodeList_free (n);
    }
  else
    {
      nameAndReplaceNode nr = (nameAndReplaceNode) dmalloc (sizeof (*nr));
      nr->replacelist = r;
      nr->namelist = n;
      ren->is_replace = FALSE;
      ren->content.name = nr;
    }

  return (ren);
}

/*@only@*/ cstring
renamingNode_unparse (/*@null@*/ renamingNode x)
{
  if (x != (renamingNode) 0)
    {
      if (x->is_replace)
	{
	  return (replaceNodeList_unparse (x->content.replace));
	}
      else
	{
	  return (message ("%q%q", typeNameNodeList_unparse (x->content.name->namelist),
		   replaceNodeList_unparse (x->content.name->replacelist)));
	}
    }
  return cstring_undefined;
}

/*@only@*/ replaceNode
makeReplaceNameNode (ltoken t, typeNameNode tn, nameNode nn)
{
  replaceNode r = (replaceNode) dmalloc (sizeof (*r));

  r->tok = t;
  r->isCType = FALSE;
  r->typename = tn;
  r->content.renamesortname.name = nn;
  r->content.renamesortname.signature = (sigNode) NULL;
  
  return (r);
}

/*@only@*/ replaceNode
makeReplaceNode (ltoken t, typeNameNode tn,
		 bool is_ctype, ltoken ct,
		 nameNode nn, sigNode sn)
{
  replaceNode r = (replaceNode) dmalloc (sizeof (*r));
  
  r->tok = t;
  r->isCType = is_ctype;
  r->typename = tn;

  if (is_ctype)
    {
      r->content.ctype = ct;
      sigNode_free (sn);
      nameNode_free (nn);
    }
  else
    {
      r->content.renamesortname.name = nn;
      r->content.renamesortname.signature = sn;
      ltoken_free (ct);
    }

  return (r);
}

/*@only@*/ cstring
replaceNode_unparse (/*@null@*/ replaceNode x)
{
  if (x != (replaceNode) 0)
    {
      cstring st;

      st = message ("%q for ", typeNameNode_unparse (x->typename));

      if (x->isCType)
	{
	  st = message ("%q%s", st, ltoken_getRawString (x->content.ctype));
	}
      else
	{
	  st = message ("%q%q%q", st, nameNode_unparse (x->content.renamesortname.name),
	       sigNode_unparse (x->content.renamesortname.signature));
	}
      return st;
    }
  return cstring_undefined;
}

/*@only@*/ nameNode
makeNameNodeForm (/*@only@*/ /*@null@*/ opFormNode opform)
{
  nameNode nn = (nameNode) dmalloc (sizeof (*nn));
  
  nn->isOpId = FALSE;
  nn->content.opform = opform;

  return (nn);
}

/*@only@*/ nameNode
makeNameNodeId (/*@only@*/ ltoken opid)
{
  nameNode nn = (nameNode) dmalloc (sizeof (*nn));
  
  /* 
  ** current LSL -syms output bug produces "if_then_else_" rather
  ** than 6 separate tokens 
  */
  
  if (ltoken_getText (opid) == ConditionalSymbol)
    {
      opFormNode opform = makeOpFormNode (ltoken_undefined, OPF_IF, 
					  opFormUnion_createMiddle (0),
					  ltoken_undefined);
      nn->isOpId = FALSE;
      nn->content.opform = opform;
      ltoken_free (opid);
    }
  else
    {
      nn->isOpId = TRUE;
      nn->content.opid = opid;
    }

  return (nn);
}

/*@only@*/ cstring
nameNode_unparse (/*@null@*/ nameNode n)
{
  if (n != (nameNode) 0)
    {
      if (n->isOpId)
	{
	  return (cstring_copy (ltoken_getRawString (n->content.opid))); /*!!!*/
	}
      else
	{
	  return (opFormNode_unparse (n->content.opform));
	}
    }
  return cstring_undefined;
}

/*@only@*/ sigNode
makesigNode (ltoken t, /*@only@*/ ltokenList domain, ltoken range)
{
  sigNode s = (sigNode) dmalloc (sizeof (*s));
  unsigned long int key;

  /*
  ** Assign a hash key here to speed up lookup of operators.
  */
  
  s->tok = t;
  s->domain = domain;
  s->range = range;
  key = MASH (0, ltoken_getText (range));
  
  ltokenList_elements (domain, id)
    {
      lsymbol sym = ltoken_getText (id);
      key = MASH (key, sym);
    } end_ltokenList_elements;
  
  s->key = key;
  return (s);
}

cstring sigNode_unparse (/*@null@*/ sigNode n)
{
  if (n != (sigNode) 0)
    {
      return (message (":%q -> %s", printLeaves2 (n->domain),
		       ltoken_unparse (n->range)));
    }

  return cstring_undefined;
}

void sigNode_markOwned (sigNode n)
{
    sfreeEventually (n);
}

/*@only@*/ cstring
sigNode_unparseText (/*@null@*/ sigNode n)
{
  if (n != (sigNode) 0)
    {
      return (message ("%q -> %s", printLeaves2 (n->domain), 
		       ltoken_unparse (n->range)));
    }
  return cstring_undefined;
}

static unsigned long opFormNode2key (opFormNode op, opFormKind k)
{
  unsigned long int key;

  switch (k)
    {
    case OPF_IF:
      /* OPF_IF is the first enum, so it's 0 */

      /*@-type@*/ 
      key = MASH (k, k + 1);
      /*@=type@*/
      
      break;
    case OPF_ANYOP:
    case OPF_MANYOP:
    case OPF_ANYOPM:
    case OPF_MANYOPM:
      {				/* treat eq and = the same */
	lsymbol sym = ltoken_getText (op->content.anyop);

	if (sym == equalSymbol)
	  {		      
	    key = MASH (k, eqSymbol);
	  }
	else
	  {
	    key = MASH (k, ltoken_getText (op->content.anyop));
	  }
	break;
      }
    case OPF_MIDDLE:
    case OPF_MMIDDLE:
    case OPF_MIDDLEM:
    case OPF_MMIDDLEM:
    case OPF_BMIDDLE:
    case OPF_BMMIDDLE:
    case OPF_BMIDDLEM:
    case OPF_BMMIDDLEM:
      key = MASH (k, op->content.middle);
      key = MASH (key, ltoken_getRawText (op->tok));
      break;
    case OPF_SELECT:
    case OPF_MAP:
    case OPF_MSELECT:
    case OPF_MMAP:
      key = MASH (k, ltoken_getRawText (op->content.id));
      break;
    default:
      key = 0;
    }

  return key;
}

/*@only@*/ opFormNode
makeOpFormNode (ltoken t, opFormKind k, opFormUnion u,
		ltoken close)
{
  opFormNode n = (opFormNode) dmalloc (sizeof (*n));
  unsigned long int key = 0;

  /*
  ** Assign a hash key here to speed up lookup of operators.
  */

  n->tok = t;
  n->close = close;
  n->kind = k;
  
  switch (k)
    {
    case OPF_IF:
      n->content.middle = 0;
      /* OPF_IF is the first enum, so it's 0 */
      key = MASH /*@+enumint@*/ (k, k + 1) /*@=enumint@*/;
      break;
    case OPF_ANYOP:
    case OPF_MANYOP:
    case OPF_ANYOPM:
    case OPF_MANYOPM:
      {				/* treat eq and = the same */
	lsymbol sym = ltoken_getText (u.anyop);

	if (sym == equalSymbol)
	  {		
	    key = MASH (k, eqSymbol);
	  }
	else
	  {
	    key = MASH (k, ltoken_getText (u.anyop));
	  }

	n->content = u;
	break;
      }
    case OPF_MIDDLE:
    case OPF_MMIDDLE:
    case OPF_MIDDLEM:
    case OPF_MMIDDLEM:
    case OPF_BMIDDLE:
    case OPF_BMMIDDLE:
    case OPF_BMIDDLEM:
    case OPF_BMMIDDLEM:
      n->content = u;
      key = MASH (k, u.middle);
      key = MASH (key, ltoken_getRawText (t));
      break;
    case OPF_SELECT:
    case OPF_MAP:
    case OPF_MSELECT:
    case OPF_MMAP:
      key = MASH (k, ltoken_getRawText (u.id));
      n->content = u;
      break;
    default:
      {
	llbug (message ("makeOpFormNode: unknown opFormKind: %d", (int) k));
      }
    }
  n->key = key;
  return (n);
}

static cstring printMiddle (int j)
{
  int i;
  char *s = mstring_createEmpty ();

  for (i = j; i >= 1; i--)
    {
      s = mstring_concatFree1 (s, "__");

      if (i != 1)
	{
	  s = mstring_concatFree1 (s, ", ");
	}
    }

  return cstring_fromCharsO (s);
}

/*@only@*/ cstring
opFormNode_unparse (/*@null@*/ opFormNode n)
{
  if (n != (opFormNode) 0)
    {
      switch (n->kind)
	{
	case OPF_IF:
	  return (cstring_makeLiteral ("if __ then __ else __ "));
	case OPF_ANYOP:
	  return (cstring_copy (ltoken_getRawString (n->content.anyop)));
	case OPF_MANYOP:
	  return (message ("__ %s", ltoken_getRawString (n->content.anyop)));
	case OPF_ANYOPM:
	  return (message ("%s __ ", ltoken_getRawString (n->content.anyop)));
	case OPF_MANYOPM:
	  return (message ("__ %s __ ", ltoken_getRawString (n->content.anyop)));
	case OPF_MIDDLE:
	  return (message ("%s %q %s", 
			   ltoken_getRawString (n->tok),
			   printMiddle (n->content.middle),
			   ltoken_getRawString (n->close)));
	case OPF_MMIDDLE:
	  return (message ("__ %s %q %s", 
			   ltoken_getRawString (n->tok),
			   printMiddle (n->content.middle),
			   ltoken_getRawString (n->close)));
	case OPF_MIDDLEM:
	  return (message ("%s %q %s __", 
			   ltoken_getRawString (n->tok),
			   printMiddle (n->content.middle), 
			   ltoken_getRawString (n->close)));
	case OPF_MMIDDLEM:
	  return (message ("__ %s%q %s __", 
			   ltoken_getRawString (n->tok),
			   printMiddle (n->content.middle),
			   ltoken_getRawString (n->close)));
	case OPF_BMIDDLE:
	  return (message ("[%q]", printMiddle (n->content.middle)));
	case OPF_BMMIDDLE:
	  return (message ("__ [%q]", printMiddle (n->content.middle)));
	case OPF_BMIDDLEM:
	  return (message ("[%q] __", printMiddle (n->content.middle)));
	case OPF_BMMIDDLEM:
	  return (message ("__ [%q] __", printMiddle (n->content.middle)));
	case OPF_SELECT:
	  return (message (" \\select %s", ltoken_getRawString (n->content.id)));
	case OPF_MAP:
	  return (message (" \\field_arrow%s", ltoken_getRawString (n->content.id)));
	case OPF_MSELECT:
	  return (message ("__ \\select %s", ltoken_getRawString (n->content.id)));
	case OPF_MMAP:
	  return (message ("__ \\field_arrow %s", ltoken_getRawString (n->content.id)));
	default:
	  llfatalbug (message ("opFormNodeUnparse: unknown kind: %d",
			       (int) n->kind));
	}
    }
  return cstring_undefined;
}

/*@only@*/ typeNameNode
makeTypeNameNode (bool isObj, lclTypeSpecNode t, abstDeclaratorNode n)
{
  typeNameNode tn = (typeNameNode) dmalloc (sizeof (*tn));
  typeNamePack p = (typeNamePack) dmalloc (sizeof (*p));

  tn->isTypeName = TRUE;
  p->isObj = isObj;
  p->type = t;
  p->abst = n;
  tn->opform = (opFormNode) 0;
  tn->typename = p;
  return (tn);
}

/*@only@*/ typeNameNode
makeTypeNameNodeOp (opFormNode n)
{
  typeNameNode t = (typeNameNode) dmalloc (sizeof (*t));
  t->typename = (typeNamePack) 0;
  t->opform = n;
  t->isTypeName = FALSE;
  return (t);
}

/*@only@*/ cstring
typeNameNode_unparse (/*@null@*/ typeNameNode n)
{
  if (n != (typeNameNode) 0)
    {
      if (n->isTypeName)
	{
	  cstring st = cstring_undefined;
	  typeNamePack p = n->typename;

	  llassert (p != NULL);

	  if (p->isObj)
	    st = cstring_makeLiteral ("obj ");

	  return (message ("%q%q%q", st, lclTypeSpecNode_unparse (p->type),
			   abstDeclaratorNode_unparse (p->abst)));

	}
      else
	return (opFormNode_unparse (n->opform));
    }
  return cstring_undefined;
}

/*@only@*/ lclTypeSpecNode
makeLclTypeSpecNodeConj (/*@null@*/ lclTypeSpecNode a, /*@null@*/ lclTypeSpecNode b)
{
  lclTypeSpecNode n = (lclTypeSpecNode) dmalloc (sizeof (*n));

  n->kind = LTS_CONJ;
  n->pointers = pointers_undefined;
  n->quals = qualList_new ();
  n->content.conj = (lclconj) dmalloc (sizeof (*n->content.conj));
  n->content.conj->a = a;
  n->content.conj->b = b;

  return (n);
}

/*@only@*/ lclTypeSpecNode
makeLclTypeSpecNodeType (/*@null@*/ CTypesNode x)
{
  lclTypeSpecNode n = (lclTypeSpecNode) dmalloc (sizeof (*n));

  n->kind = LTS_TYPE;
  n->pointers = pointers_undefined;
  n->content.type = x;
  n->quals = qualList_new ();
  return (n);
}

/*@only@*/ lclTypeSpecNode
makeLclTypeSpecNodeSU (/*@null@*/ strOrUnionNode x)
{
  lclTypeSpecNode n = (lclTypeSpecNode) dmalloc (sizeof (*n));

  n->kind = LTS_STRUCTUNION;
  n->pointers = pointers_undefined;
  n->content.structorunion = x;
  n->quals = qualList_new ();
  return (n);
}

/*@only@*/ lclTypeSpecNode
makeLclTypeSpecNodeEnum (/*@null@*/ enumSpecNode x)
{
  lclTypeSpecNode n = (lclTypeSpecNode) dmalloc (sizeof (*n));

  n->quals = qualList_new ();
  n->kind = LTS_ENUM;
  n->pointers = pointers_undefined;
  n->content.enumspec = x;
  return (n);
}

lclTypeSpecNode
lclTypeSpecNode_addQual (lclTypeSpecNode n, qual q)
{
  llassert (lclTypeSpecNode_isDefined (n));
  n->quals = qualList_add (n->quals, q);
  return n;
}

/*@only@*/ cstring
lclTypeSpecNode_unparse (/*@null@*/ lclTypeSpecNode n)
{
  if (n != (lclTypeSpecNode) 0)
    {
      switch (n->kind)
	{
	case LTS_TYPE:
	  llassert (n->content.type != NULL);
	  return (printLeaves (n->content.type->ctypes));
	case LTS_STRUCTUNION:
	  return (strOrUnionNode_unparse (n->content.structorunion));
	case LTS_ENUM:
	  return (enumSpecNode_unparse (n->content.enumspec));
	case LTS_CONJ:
	  return (lclTypeSpecNode_unparse (n->content.conj->a));
	default:
	  llfatalbug (message ("lclTypeSpecNode_unparse: unknown lclTypeSpec kind: %d",
			       (int) n->kind));
	}
    }
  return cstring_undefined;
}

/*@only@*/ enumSpecNode
makeEnumSpecNode (ltoken t, ltoken optTagId,
		  /*@owned@*/ ltokenList enums)
{
  enumSpecNode n = (enumSpecNode) dmalloc (sizeof (*n));
  tagInfo ti;
  smemberInfo *top = smemberInfo_undefined;

  n->tok = t;
  n->opttagid = ltoken_copy (optTagId);
    n->enums = enums;

  /* generate sort for this LCL type */
  n->sort = sort_makeEnum (optTagId);
  
  if (!ltoken_isUndefined (optTagId))
    {
      /* First, check to see if tag is already defined */
      ti = symtable_tagInfo (g_symtab, ltoken_getText (optTagId));

      if (tagInfo_exists (ti))
	{
	  if (ti->kind == TAG_ENUM)
	    {
	      /* 23 Sep 1995 --- had been noting here...is this right? */

	      ti->content.enums = enums;
	      ti->sort = n->sort;
	      ti->imported = context_inImport ();
	    }
	  else
	    {
	      lclerror (optTagId, 
			message ("Tag %s previously defined as %q, redefined as enum",
				 ltoken_getRawString (optTagId),	
				 tagKind_unparse (ti->kind)));
	      
	      /* evs --- shouldn't they be in different name spaces? */
	    }

	  ltoken_free (optTagId);
	}
      else
	{
	  ti = (tagInfo) dmalloc (sizeof (*ti));

	  ti->kind = TAG_ENUM;
	  ti->id = optTagId;
	  ti->content.enums = enums;
	  ti->sort = n->sort;
	  ti->imported = context_inImport ();
	  /* First, store tag info in symbol table */
	  (void) symtable_enterTag (g_symtab, ti);
	}
    }

  /* check that enumeration constants are unique */
  
  ltokenList_reset (enums);

  while (!ltokenList_isFinished (enums))
    {
      ltoken c = ltokenList_current (enums);
      smemberInfo *ei = (smemberInfo *) dmalloc (sizeof (*ei));

      ei->name = ltoken_getText (c);
      ei->next = top;
      ei->sort = n->sort;
      top = ei;
      
      if (!varInfo_exists (symtable_varInfo (g_symtab, ltoken_getText (c))))
	{				/* put info into symbol table */
 	  varInfo vi = (varInfo) dmalloc (sizeof (*vi));
	  
	  vi->id = ltoken_copy (c);
	  vi->kind = VRK_ENUM;
	  vi->sort = n->sort;
	  vi->export = TRUE;

	  (void) symtable_enterVar (g_symtab, vi);
	  varInfo_free (vi);
	}
      else
	{
	  lclerror (c, message ("Enumerated value redeclared: %s", 
				ltoken_getRawString (c)));
	  ltokenList_removeCurrent (enums);
	}
      ltokenList_advance (enums);
      /*@-branchstate@*/
    }
  /*@=branchstate@*/
  
  (void) sort_updateEnum (n->sort, top);
  return (n);
}

/*@only@*/ enumSpecNode
makeEnumSpecNode2 (ltoken t, ltoken tagid)
{
  /* a reference, not a definition */
  enumSpecNode n = (enumSpecNode) dmalloc (sizeof (*n));
  tagInfo ti = symtable_tagInfo (g_symtab, ltoken_getText (tagid));
  
  n->tok = t;
  n->opttagid = tagid;
  n->enums = ltokenList_new ();
  
  if (tagInfo_exists (ti))
    {
      if (ti->kind == TAG_ENUM)
	{
	  n->sort = ti->sort;
	}
      else
	{
	  n->sort = sort_makeNoSort ();
	  lclerror (tagid, message ("Tag %s defined as %q, used as enum",
				    ltoken_getRawString (tagid),
				    tagKind_unparse (ti->kind)));
	}
    }
  else
    {
      n->sort = sort_makeNoSort ();
      lclerror (t, message ("Undefined type: enum %s", 
			    ltoken_getRawString (tagid)));
    }

  return (n);
}

/*@only@*/ cstring
enumSpecNode_unparse (/*@null@*/ enumSpecNode n)
{
  if (n != (enumSpecNode) 0)
    {
      cstring s = cstring_makeLiteral ("enum ");

      if (!ltoken_isUndefined (n->opttagid))
	{
	  s = message ("%q%s ", s, ltoken_getRawString (n->opttagid));
	}

      s = message ("%q{%q}", s, printLeaves2 (n->enums));
      return s;
    }
  return cstring_undefined;
}

/*@only@*/ strOrUnionNode
makestrOrUnionNode (ltoken str, suKind k, ltoken opttagid,
		       /*@only@*/ stDeclNodeList x)
{
  strOrUnionNode n = (strOrUnionNode) dmalloc (sizeof (*n));
  lsymbolSet set = lsymbolSet_new ();
  declaratorNodeList declarators;
  sort fieldsort, tsort1, tsort2;
  smemberInfo *mi, *top = smemberInfo_undefined;
  bool doTag = FALSE;
  bool isStruct = (k == SU_STRUCT);
  tagInfo t;

  
  n->kind = k;
  n->tok = str;
  n->opttagid = ltoken_copy (opttagid);
  n->structdecls = x;
  n->sort = isStruct ? sort_makeStr (opttagid) : sort_makeUnion (opttagid);

  if (!ltoken_isUndefined (opttagid))
    {
      /* First, check to see if tag is already defined */
      t = symtable_tagInfo (g_symtab, ltoken_getText (opttagid));

      if (tagInfo_exists (t))
	{
	  if ((t->kind == TAG_FWDUNION && k == SU_UNION) ||
	      (t->kind == TAG_FWDSTRUCT && k == SU_STRUCT))
	    {
	      /* to allow self-recursive types and forward tag declarations */
	      t->content.decls = stDeclNodeList_copy (x); /* update tag info */
	      t->sort = n->sort;
	    }
	  else
	    {
	      lclerror (opttagid, 
			message ("Tag %s previously defined as %q, used as %q",
				 ltoken_getRawString (opttagid),
				 tagKind_unparse (t->kind),
				 cstring_makeLiteral (isStruct ? "struct" : "union")));
	    }
	}
      else
	{
	  doTag = TRUE;
	}
    }
  else
    {
      doTag = TRUE;
    }
  
  if (doTag && !ltoken_isUndefined (opttagid))
    {
      t = (tagInfo) dmalloc (sizeof (*t));

      /* can either override prev defn or use prev defn */
      /* override it so as to detect more errors */

      t->kind = (k == SU_STRUCT) ? TAG_STRUCT : TAG_UNION;
      t->id = opttagid;
      t->content.decls = stDeclNodeList_copy (x);
      t->sort = n->sort;
      t->imported = FALSE;

            /* Next, update tag info in symbol table */
      (void) symtable_enterTagForce (g_symtab, t);
    }
  
  /* check no duplicate field names */
  
  stDeclNodeList_elements (x, i)
    {
      fieldsort = lclTypeSpecNode2sort (i->lcltypespec);
      
      /* need the locations, not values */
      /*  fieldsort = sort_makeObj (fieldsort); */
      /* 2/19/93, was
	 fieldsort = sort_makeGlobal (fieldsort); */
      
      declarators = i->declarators;
      
      declaratorNodeList_elements (declarators, decl)
	{
	  lsymbol fieldname;
 	  mi = (smemberInfo *) dmalloc (sizeof (*mi));
	  /* need to make dynamic copies */
	  fieldname = ltoken_getText (decl->id);
	  
	  /* 2/19/93, added */
	  tsort1 = typeExpr2ptrSort (fieldsort, decl->type);
	  tsort2 = sort_makeGlobal (tsort1);
	  
	  mi->name = fieldname;
	  mi->sort = tsort2;	/* fieldsort; */
	  mi->next = top;
	  top = mi;
	  
	  if (lsymbolSet_member (set, fieldname))
	    {
	      lclerror (decl->id,
			message ("Field name reused: %s", 
				 ltoken_getRawString (decl->id)));
	    }
	  else
	    {
	      (void) lsymbolSet_insert (set, fieldname);
	    }
	  /*@-branchstate@*/ 
	} end_declaratorNodeList_elements; 
      /*@=branchstate@*/
    } end_stDeclNodeList_elements;
  
  if (k == SU_STRUCT)
    {
      (void) sort_updateStr (n->sort, top);
    }
  else
    {
      (void) sort_updateUnion (n->sort, top);
    }

  /* We shall keep the info with both tags and types if any
     of them are present. */
  
  lsymbolSet_free (set);

    return (n);
}

/*@only@*/ strOrUnionNode
makeForwardstrOrUnionNode (ltoken str, suKind k,
			ltoken tagid)
{
  strOrUnionNode n = (strOrUnionNode) dmalloc (sizeof (*n));
  sort sort = sort_makeNoSort ();
  tagInfo t;

  /* a reference, not a definition */
  
  n->kind = k;
  n->tok = str;
  n->opttagid = tagid;
  n->structdecls = stDeclNodeList_new ();
  
  /* get sort for this LCL type */
  t = symtable_tagInfo (g_symtab, ltoken_getText (tagid));

  if (tagInfo_exists (t))
    {
      sort = t->sort;
      
      if (!(((t->kind == TAG_STRUCT || t->kind == TAG_FWDSTRUCT) && k == SU_STRUCT) 
	    || ((t->kind == TAG_UNION || t->kind == TAG_FWDUNION) && k == SU_UNION)))
	{
	  lclerror (tagid, 
		    message ("Tag %s previously defined as %q, used as %q",
			     ltoken_getRawString (tagid),
			     tagKind_unparse (t->kind),
			     cstring_makeLiteral ((k == SU_STRUCT) ? "struct" : "union")));
	}
    }
  else
    {
      /*
      ** changed from error: 31 Mar 1994
      **
      ** lclerror (str, message ("Undefined type: %s %s", s, ltoken_getRawString (tagid));
      **
      */

      /* forward struct's and union's are ok... */

      if (k == SU_STRUCT)
	{
	  (void) checkAndEnterTag (TAG_FWDSTRUCT, ltoken_copy (tagid));
	  lhForwardStruct (tagid);
	  sort = sort_makeStr (tagid);
	}
      else
	{
	  (void) checkAndEnterTag (TAG_FWDUNION, ltoken_copy (tagid));
	  lhForwardUnion (tagid);
	  sort = sort_makeUnion (tagid);
	}
    }
  
  n->sort = sort;
  return (n);
}

/*@only@*/ cstring
strOrUnionNode_unparse (/*@null@*/ strOrUnionNode n)
{
  if (n != (strOrUnionNode) 0)
    {
      cstring s;
      switch (n->kind)
	{
	case SU_STRUCT:
	  s = cstring_makeLiteral ("struct ");
	  break;
	case SU_UNION:
	  s = cstring_makeLiteral ("union ");
	  break;
	BADDEFAULT
	}

      if (!ltoken_isUndefined (n->opttagid))
	{
	  s = message ("%q%s ", s, ltoken_getRawString (n->opttagid));
	}
      s = message ("%q{%q}", s, stDeclNodeList_unparse (n->structdecls));
      return s;
    }
  return cstring_undefined;
}

/*@only@*/ stDeclNode
makestDeclNode (lclTypeSpecNode s,
		declaratorNodeList x)
{
  stDeclNode n = (stDeclNode) dmalloc (sizeof (*n));

  n->lcltypespec = s;
  n->declarators = x;
  return n;
}

/*@only@*/ typeExpr
makeFunctionNode (typeExpr x, paramNodeList p)
{
  typeExpr y = (typeExpr) dmalloc (sizeof (*y));

  y->wrapped = 0;
  y->kind = TEXPR_FCN;
  y->content.function.returntype = x;
  y->content.function.args = p;
  y->sort = sort_makeNoSort ();

  return (y);
}

static /*@observer@*/ ltoken
  extractDeclarator (/*@null@*/ typeExpr t)
{
  if (t != (typeExpr) 0)
    {
      switch (t->kind)
	{
	case TEXPR_BASE:
	  	  return t->content.base;
	case TEXPR_PTR:
	  return (extractDeclarator (t->content.pointer));
	case TEXPR_ARRAY:
	  return (extractDeclarator (t->content.array.elementtype));
	case TEXPR_FCN:
	  return (extractDeclarator (t->content.function.returntype));
	}
    }

  return ltoken_undefined;
}

/*@only@*/ typeExpr
makeTypeExpr (ltoken t)
{
  typeExpr x = (typeExpr) dmalloc (sizeof (*x));
  
  
  x->wrapped = 0;
  x->kind = TEXPR_BASE;
  x->content.base = t;
  x->sort = sort_makeNoSort ();

  return (x);
}


/*@only@*/ declaratorNode
makeDeclaratorNode (typeExpr t)
{
  declaratorNode x = (declaratorNode) dmalloc (sizeof (*x));
  
  x->id = ltoken_copy (extractDeclarator (t));
  x->type = t;
  x->isRedecl = FALSE;

    return (x);
}

static /*@only@*/ declaratorNode
makeUnknownDeclaratorNode (/*@only@*/ ltoken t)
{
  declaratorNode x = (declaratorNode) dmalloc (sizeof (*x));

  x->id = t;
  x->type = (typeExpr) 0;
  x->isRedecl = FALSE;

  return (x);
}

static /*@only@*/ cstring
printTypeExpr2 (/*@null@*/ typeExpr x)
{
  paramNodeList params;

  if (x != (typeExpr) 0)
    {
      cstring s;		/* print out types in reverse order */

      switch (x->kind)
	{
	case TEXPR_BASE:
	  return (message ("%s ", ltoken_getRawString (x->content.base)));
	case TEXPR_PTR:
	  return (message ("%qptr to ", printTypeExpr2 (x->content.pointer)));
	case TEXPR_ARRAY:
	  return (message ("array[%q] of %q",
			   termNode_unparse (x->content.array.size),
			   printTypeExpr2 (x->content.array.elementtype)));
	case TEXPR_FCN:
	  s = printTypeExpr2 (x->content.function.returntype);
	  params = x->content.function.args;
	  if (!paramNodeList_empty (params))
	    {
	      s = message ("%qfcn with args: (%q)", s,
			   paramNodeList_unparse (x->content.function.args));
	    }
	  else
	    s = message ("%qfcn with no args", s);
	  return s;
	default:
	  llfatalbug (message ("printTypeExpr2: unknown typeExprKind: %d", (int) x->kind));
	}
    }
  return cstring_undefined;
}

/*@only@*/ cstring
declaratorNode_unparse (declaratorNode x)
{
  return (typeExpr_unparse (x->type));
}

/*@only@*/ declaratorNode
  declaratorNode_copy (declaratorNode x)
{
  declaratorNode ret = (declaratorNode) dmalloc (sizeof (*ret));

    ret->type = typeExpr_copy (x->type);
  ret->id = ltoken_copy (x->id);
  ret->isRedecl = x->isRedecl; 

    return (ret);
}

static /*@null@*/ typeExpr typeExpr_copy (/*@null@*/ typeExpr x)
{
  if (x == NULL)
    {
      return NULL;
    }
  else
    {
      typeExpr ret = (typeExpr) dmalloc (sizeof (*ret));
      
      ret->wrapped = x->wrapped;
      ret->kind = x->kind;

      switch (ret->kind)
	{
	case TEXPR_BASE:     
	  ret->content.base = ltoken_copy (x->content.base);
	  break;
	case TEXPR_PTR:  
	  ret->content.pointer = typeExpr_copy (x->content.pointer);
	  break;
	case TEXPR_ARRAY:    
	  ret->content.array.elementtype = typeExpr_copy (x->content.array.elementtype);
	  ret->content.array.size = termNode_copy (x->content.array.size);
	  break;
	case TEXPR_FCN:
	  ret->content.function.returntype = typeExpr_copy (x->content.function.returntype);
	  ret->content.function.args = paramNodeList_copy (x->content.function.args);
	  break;
	}

      ret->sort = x->sort;
      return ret;
    }
}

static /*@only@*/ cstring 
  typeExpr_unparseCode (/*@null@*/ typeExpr x)
{
  /* print out types in order of appearance in source */
  cstring s = cstring_undefined;

  if (x != (typeExpr) 0)
    {
      switch (x->kind)
	{
	case TEXPR_BASE:
	  return (cstring_copy (ltoken_getRawString (x->content.base)));
	case TEXPR_PTR:
	  return (typeExpr_unparseCode (x->content.pointer));
	case TEXPR_ARRAY:
	  return (typeExpr_unparseCode (x->content.array.elementtype));
	case TEXPR_FCN:
	  return (typeExpr_unparseCode (x->content.function.returntype));
	}
    }
  return s;
}

void typeExpr_free (/*@only@*/ /*@null@*/ typeExpr x)
{
  if (x != (typeExpr) 0)
    {
      switch (x->kind)
	{
	case TEXPR_BASE:
	  break;
	case TEXPR_PTR:
	  typeExpr_free (x->content.pointer);
	  break;
	case TEXPR_ARRAY:
	  typeExpr_free (x->content.array.elementtype);
	  termNode_free (x->content.array.size);
	  break;
	case TEXPR_FCN:
	  typeExpr_free (x->content.function.returntype);
	  paramNodeList_free (x->content.function.args);
	  break;
	  /*@-branchstate@*/ 
	} 
      /*@=branchstate@*/

      sfree (x);
    }
}


/*@only@*/ cstring
declaratorNode_unparseCode (declaratorNode x)
{
  return (typeExpr_unparseCode (x->type));
}

/*@only@*/ cstring
typeExpr_unparse (/*@null@*/ typeExpr x)
{
  cstring s = cstring_undefined; /* print out types in order of appearance in source */
  paramNodeList params;
  int i;

  if (x != (typeExpr) 0)
    {
      cstring front = cstring_undefined;
      cstring back  = cstring_undefined;

      llassert (x->wrapped < 100);

      for (i = x->wrapped; i >= 1; i--)
	{
	  front = cstring_appendChar (front, '(');
	  back = cstring_appendChar (back, ')');
	}
      
      switch (x->kind)
	{
	case TEXPR_BASE:
	  s = message ("%q%s", s, ltoken_getRawString (x->content.base));
	  break;
	case TEXPR_PTR:
	  s = message ("%q*%q", s, typeExpr_unparse (x->content.pointer));
	  break;
	case TEXPR_ARRAY:
	  s = message ("%q%q[%q]", s, 
		       typeExpr_unparse (x->content.array.elementtype),
		       termNode_unparse (x->content.array.size));
	  break;
	case TEXPR_FCN:
	  s = message ("%q%q (", s, 
		       typeExpr_unparse (x->content.function.returntype));
	  params = x->content.function.args;

	  if (!paramNodeList_empty (params))
	    {
	      s = message ("%q%q", s, 
			   paramNodeList_unparse (x->content.function.args));
	    }

	  s = message ("%q)", s);
	  break;
	}
      s = message ("%q%q%q", front, s, back);
    }
  else
    {
      s = cstring_makeLiteral ("?");
    }

  return s;
}

/*@only@*/ cstring
typeExpr_unparseNoBase (/*@null@*/ typeExpr x)
{
  cstring s = cstring_undefined; /* print out types in order of appearance in source */
  paramNodeList params;
  int i;

  if (x != (typeExpr) 0)
    {
      cstring front = cstring_undefined;
      cstring back  = cstring_undefined;

      llassert (x->wrapped < 100);

      for (i = x->wrapped; i >= 1; i--)
	{
	  front = cstring_appendChar (front, '(');
	  back = cstring_appendChar (back, ')');
	}
      
      switch (x->kind)
	{
	case TEXPR_BASE:
	  s = message ("%q /* %s */", s, ltoken_getRawString (x->content.base));
	  break;
	case TEXPR_PTR:
	  s = message ("%q*%q", s, typeExpr_unparseNoBase (x->content.pointer));
	  break;
	case TEXPR_ARRAY:
	  s = message ("%q%q[%q]", s, 
		       typeExpr_unparseNoBase (x->content.array.elementtype),
		       termNode_unparse (x->content.array.size));
	  break;
	case TEXPR_FCN:
	  s = message ("%q%q (", s, 
		       typeExpr_unparseNoBase (x->content.function.returntype));
	  params = x->content.function.args;

	  if (!paramNodeList_empty (params))
	    {
	      s = message ("%q%q", s, 
			   paramNodeList_unparse (x->content.function.args));
	    }

	  s = message ("%q)", s);
	  break;
	}
      s = message ("%q%q%q", front, s, back);
    }
  else
    {
      s = cstring_makeLiteral ("?");
    }

  return s;
}

cstring
typeExpr_name (/*@null@*/ typeExpr x)
{
  if (x != (typeExpr) 0)
    {
      switch (x->kind)
	{
	case TEXPR_BASE:
	  return (cstring_copy (ltoken_getRawString (x->content.base)));
	case TEXPR_PTR:
	  return (typeExpr_name (x->content.pointer));
	case TEXPR_ARRAY:
	  return (typeExpr_name (x->content.array.elementtype));
	case TEXPR_FCN:
	  return (typeExpr_name (x->content.function.returntype));
	}
    }

  /* evs --- 14 Mar 1995
  ** not a bug: its okay to have empty parameter names
  **   llbug ("typeExpr_name: null");
  */

  return cstring_undefined;
}

/*@only@*/ typeExpr
  makePointerNode (ltoken star, /*@only@*/ /*@returned@*/ typeExpr x)
{
  if (x != (typeExpr)0 && (x->kind == TEXPR_FCN && (x->wrapped == 0)))
    {
      x->content.function.returntype = makePointerNode (star, x->content.function.returntype);
      return x;
    }
  else
    {
      typeExpr y = (typeExpr) dmalloc (sizeof (*y));

      y->wrapped = 0;
      y->kind = TEXPR_PTR;
      y->content.pointer = x;
      y->sort = sort_makeNoSort ();
      ltoken_free (star);

      return y;
    }
}

typeExpr makeArrayNode (/*@returned@*/ typeExpr x,
			/*@only@*/ arrayQualNode a)
{
  if (x != (typeExpr)0 && (x->kind == TEXPR_FCN && (x->wrapped == 0)))
    {
      /*
      ** Spurious errors reported here, because of referencing
      ** in makeArrayNode.
      */

      /*@-usereleased@*/
      x->content.function.returntype = makeArrayNode (x, a);
      /*@=usereleased@*/ 
      /*@-kepttrans@*/
      return x;
      /*@=kepttrans@*/ 
    }
  else
    {
      typeExpr y = (typeExpr) dmalloc (sizeof (*y));
      y->wrapped = 0;
      y->kind = TEXPR_ARRAY;

      if (a == (arrayQualNode) 0)
	{
	  y->content.array.size = (termNode) 0;
	}
      else
	{
	  y->content.array.size = a->term;
	  ltoken_free (a->tok);
	  sfree (a);
	}

      y->content.array.elementtype = x;
      y->sort = sort_makeNoSort ();

      return (y);
    }
}

/*@only@*/ constDeclarationNode
makeConstDeclarationNode (lclTypeSpecNode t, initDeclNodeList decls)
{
  constDeclarationNode n = (constDeclarationNode) dmalloc (sizeof (*n));
  sort s, s2, initValueSort;
  ltoken varid, errtok;
  termNode initValue;

  s = lclTypeSpecNode2sort (t);  

  initDeclNodeList_elements (decls, init)
    {
      declaratorNode vdnode = init->declarator;
      varInfo vi = (varInfo) dmalloc (sizeof (*vi));

      varid = ltoken_copy (vdnode->id);
      s2 = typeExpr2ptrSort (s, vdnode->type);
      initValue = init->value;
      
      if (termNode_isDefined (initValue) && !initValue->error_reported)
	{
	  initValueSort = initValue->sort;

	  /* should keep the arguments in order */
	  if (!sort_member_modulo_cstring (s2, initValue)
	      && !initValue->error_reported)
	    {
	      errtok = termNode_errorToken (initValue);
	      
	      lclerror 
		(errtok, 
		 message ("Constant %s declared type %q, initialized to %q: %q",
			  ltoken_unparse (varid), 
			  sort_unparse (s2), 
			  sort_unparse (initValueSort),
			  termNode_unparse (initValue)));
	    }
	}
      
      vi->id = varid;
      vi->kind = VRK_CONST;
      vi->sort = s2;
      vi->export = TRUE;

      (void) symtable_enterVar (g_symtab, vi);
      varInfo_free (vi);

    } end_initDeclNodeList_elements;

  n->type = t;
  n->decls = decls;
  
  return n;
}

varDeclarationNode makeInternalStateNode (void)
{
  varDeclarationNode n = (varDeclarationNode) dmalloc (sizeof (*n));

  n->isSpecial = TRUE;
  n->sref = sRef_makeInternalState ();

  /*@-compdef@*/ return n; /*@=compdef@*/
}

varDeclarationNode makeFileSystemNode (void)
{
  varDeclarationNode n = (varDeclarationNode) dmalloc (sizeof (*n));

  n->isSpecial = TRUE;
  n->sref = sRef_makeSystemState ();

  /*@-compdef@*/ return n; /*@=compdef@*/
}

/*@only@*/ varDeclarationNode
makeVarDeclarationNode (lclTypeSpecNode t, initDeclNodeList x,
			bool isGlobal, bool isPrivate)
{
  varDeclarationNode n = (varDeclarationNode) dmalloc (sizeof (*n));
  sort s, s2, initValueSort;
  ltoken varid, errtok;
  termNode initValue;
  declaratorNode vdnode;

  n->isSpecial = FALSE;
  n->qualifier = QLF_NONE;
  n->isGlobal = isGlobal;
  n->isPrivate = isPrivate;
  n->decls = x;

  s = lclTypeSpecNode2sort (t);

  /* t is an lclTypeSpec, its sort may not be assigned yet */

  initDeclNodeList_elements (x, init)
    {
      vdnode = init->declarator;
      varid = vdnode->id;
      s2 = typeExpr2ptrSort (s, vdnode->type);
      initValue = init->value;

      if (termNode_isDefined (initValue) && !initValue->error_reported)
	{
	  initValueSort = initValue->sort;
	  /* should keep the arguments in order */
	  if (!sort_member_modulo_cstring (s2, initValue)
	      && !initValue->error_reported)
	    {
	      errtok = termNode_errorToken (initValue);
	      
	      lclerror (errtok, 
			message ("Variable %s declared type %q, initialized to %q",
				 ltoken_unparse (varid), 
				 sort_unparse (s2), 
				 sort_unparse (initValueSort)));
	    }
	}
      
      /*
      ** If global, check that it has been declared already, don't push
      ** onto symbol table yet (wrong scope, done in enteringFcnScope 
      */

      if (isGlobal)
	{
	  varInfo vi = symtable_varInfo (g_symtab, ltoken_getText (varid));
	  
	  if (!varInfo_exists (vi))
	    {
	      lclerror (varid,
			message ("Undeclared global variable: %s",
				 ltoken_getRawString (varid)));	    
	    }
	  else
	    {
	      if (vi->kind == VRK_CONST)
		{
		  lclerror (varid,
			    message ("Constant used in global list: %s",
				     ltoken_getRawString (varid)));
		}
	    }
	}
      else
	{
	  varInfo vi = (varInfo) dmalloc (sizeof (*vi));
	  
	  vi->id = ltoken_copy (varid);
	  if (isPrivate)
	    {
	      vi->kind = VRK_PRIVATE;
	      /* check that initValue is not empty */
	      if (initValue == (termNode) 0)
		{
		  lclerror (varid,
			    message ("Private variable must have initialization: %s",
				     ltoken_getRawString (varid)));
		}
	    }
	  else
	    {
	      vi->kind = VRK_VAR;
	    }
	  
	  vi->sort = sort_makeGlobal (s2);
	  vi->export = TRUE;
	  
	  vdnode->isRedecl = symtable_enterVar (g_symtab, vi);
	  varInfo_free (vi);
	}
    } end_initDeclNodeList_elements;
  
  n->type = t;

  return n;
}

/*@only@*/ initDeclNode
makeInitDeclNode (declaratorNode d, termNode x)
{
  initDeclNode n = (initDeclNode) dmalloc (sizeof (*n));

  n->declarator = d;
  n->value = x;
  return n;
}

/*@only@*/ abstractNode
makeAbstractNode (ltoken t, ltoken name,
		  bool isMutable, bool isRefCounted, abstBodyNode a)
{
  abstractNode n = (abstractNode) dmalloc (sizeof (*n));
  sort handle;
  typeInfo ti = (typeInfo) dmalloc (sizeof (*ti));
  
  n->tok = t;
  n->isMutable = isMutable;
  n->name = name;
  n->body = a;
  n->isRefCounted = isRefCounted;

  if (isMutable)
    handle = sort_makeMutable (name, ltoken_getText (name));
  else
    handle = sort_makeImmutable (name, ltoken_getText (name));
  n->sort = handle;
  
  ti->id = ltoken_createType (ltoken_getCode (ltoken_typename), SID_TYPE, 
				ltoken_getText (name));
  ti->modifiable = isMutable;
  ti->abstract = TRUE;
  ti->basedOn = handle;
  ti->export = TRUE;

  symtable_enterType (g_symtab, ti);

  
    return n;
}

/*@only@*/ cstring
abstractNode_unparse (abstractNode n)
{
  if (n != (abstractNode) 0)
    {
      cstring s;

      if (n->isMutable)
	s = cstring_makeLiteral ("mutable");
      else
	s = cstring_makeLiteral ("immutable");

      return (message ("%q type %s%q;", s, ltoken_getRawString (n->name),
		       abstBodyNode_unparse (n->body)));
    }
  return cstring_undefined;
}

void
setExposedType (lclTypeSpecNode s)
{
  exposedType = s;
}

/*@only@*/ exposedNode
makeExposedNode (ltoken t, lclTypeSpecNode s,
		 declaratorInvNodeList d)
{
  exposedNode n = (exposedNode) dmalloc (sizeof (*n));
  
  n->tok = t;
  n->type = s;
  n->decls = d;

    return n;
}

/*@only@*/ cstring
exposedNode_unparse (exposedNode n)
{
  if (n != (exposedNode) 0)
    {
      return (message ("typedef %q %q;",
		       lclTypeSpecNode_unparse (n->type),
		       declaratorInvNodeList_unparse (n->decls)));
    }
  return cstring_undefined;
}

/*@only@*/ declaratorInvNode
makeDeclaratorInvNode (declaratorNode d, abstBodyNode b)
{
  declaratorInvNode n = (declaratorInvNode) dmalloc (sizeof (*n));
  n->declarator = d;
  n->body = b;

  return (n);
}

/*@only@*/ cstring
declaratorInvNode_unparse (declaratorInvNode d)
{
  return (message ("%q%q", declaratorNode_unparse (d->declarator),
		   abstBodyNode_unparseExposed (d->body)));
}

/*@only@*/ cstring
abstBodyNode_unparse (abstBodyNode n)
{
  if (n != (abstBodyNode) 0)
    {
      return (lclPredicateNode_unparse (n->typeinv));
    }
  return cstring_undefined;
}

/*@only@*/ cstring
abstBodyNode_unparseExposed (abstBodyNode n)
{
  if (n != (abstBodyNode) 0)
    {
      return (message ("%q", lclPredicateNode_unparse (n->typeinv)));
    }
  return cstring_undefined;
}

/*@only@*/ cstring
taggedUnionNode_unparse (taggedUnionNode n)
{
  if (n != (taggedUnionNode) 0)
    {
      return (message ("tagged union {%q}%q;\n",
		       stDeclNodeList_unparse (n->structdecls),
		       declaratorNode_unparse (n->declarator)));
    }
  return cstring_undefined;
}

static /*@observer@*/ paramNodeList
  typeExpr_toParamNodeList (/*@null@*/ typeExpr te)
{
  if (te != (typeExpr) 0)
    {
      switch (te->kind)
	{
	case TEXPR_FCN:
	  return te->content.function.args;
	case TEXPR_PTR:
	  return typeExpr_toParamNodeList (te->content.pointer);
	case TEXPR_ARRAY:
	 /* return typeExpr_toParamNodeList (te->content.array.elementtype); */
	case TEXPR_BASE:
	  return paramNodeList_undefined;
	}
    }
  return paramNodeList_undefined;
}

/*@only@*/ fcnNode
  fcnNode_fromDeclarator (/*@only@*/ /*@null@*/ lclTypeSpecNode t, 
			  /*@only@*/ declaratorNode d)
{
  return (makeFcnNode (qual_createUnknown (),  t, d,
		       varDeclarationNodeList_new (), 
		       varDeclarationNodeList_new (), 
		       letDeclNodeList_new (), 
		       (lclPredicateNode) 0,
		       (lclPredicateNode) 0,
		       (modifyNode) 0,
		       (lclPredicateNode) 0,
		       (lclPredicateNode) 0));
}

/*@only@*/ iterNode
makeIterNode (ltoken id, paramNodeList p)
{
  iterNode x = (iterNode) dmalloc (sizeof (*x));
  bool hasYield = FALSE;
  
  x->name = id;
  x->params = p;
  
  /* check there is at least one yield param */
  
  paramNodeList_elements (p, pe)
    {
      if (paramNode_isYield (pe)) 
	{
	  hasYield = TRUE; 
	  break; 
	}
    } end_paramNodeList_elements 
      
  if (!hasYield)
    {
      lclerror (id, message ("Iterator has no yield parameters: %s", 
			     ltoken_getRawString (id)));
    }

  return (x);
}

/*@only@*/ fcnNode
makeFcnNode (qual specQual,
	     /*@null@*/ lclTypeSpecNode t,
	                declaratorNode d,
	     /*@null@*/ globalList g, 
	     /*@null@*/ varDeclarationNodeList privateinits,
	     /*@null@*/ letDeclNodeList lets,
	     /*@null@*/ lclPredicateNode checks,
	     /*@null@*/ lclPredicateNode requires, 
	     /*@null@*/ modifyNode m,
	     /*@null@*/ lclPredicateNode ensures, 
	     /*@null@*/ lclPredicateNode claims)
{
  fcnNode x = (fcnNode) dmalloc (sizeof (*x));

  if (d->type != (typeExpr)0 && (d->type)->kind != TEXPR_FCN)
    {
      lclerror (d->id, cstring_makeLiteral 
		("Attempt to specify function without parameter list"));
      d->type = makeFunctionNode (d->type, paramNodeList_new ());
    }
    
  x->special = specQual;
  x->typespec = t;
  x->declarator = d;
  x->globals = g;
  x->inits = privateinits;
  x->lets = lets;
  x->checks = checks;
  x->require = requires;
  x->modify = m;
  x->ensures = ensures;
  x->claim = claims;
  
  /* extract info to fill in x->name =;  x->signature =; */
  x->name = ltoken_copy (d->id);
  
  return (x);
}

/*@only@*/ claimNode
makeClaimNode (ltoken id, paramNodeList p,
	       globalList g, letDeclNodeList lets, lclPredicateNode requires,
	       programNode b, lclPredicateNode ensures)
{
  claimNode x = (claimNode) dmalloc (sizeof (*x));

  
  x->name = id;
  x->params = p;
  x->globals = g;
  x->lets = lets;
  x->require = requires;
  x->body = b;
  x->ensures = ensures;
  return (x);
}

/*@only@*/ lclPredicateNode
makeIntraClaimNode (ltoken t, lclPredicateNode n)
{
  ltoken_free (n->tok);
  n->tok = t;
  n->kind = LPD_INTRACLAIM;
  return (n);
}

/*@only@*/ lclPredicateNode
makeRequiresNode (ltoken t, lclPredicateNode n)
{
  ltoken_free (n->tok);
  n->tok = t;
  n->kind = LPD_REQUIRES;
  return (n);
}

/*@only@*/ lclPredicateNode
makeChecksNode (ltoken t, lclPredicateNode n)
{
  ltoken_free (n->tok);
  n->tok = t;
  n->kind = LPD_CHECKS;
  return (n);
}

/*@only@*/ lclPredicateNode
makeEnsuresNode (ltoken t, lclPredicateNode n)
{
  ltoken_free (n->tok);
  n->tok = t;
  n->kind = LPD_ENSURES;
  return (n);
}

/*@only@*/ lclPredicateNode
makeLclPredicateNode (ltoken t, termNode n,
		      lclPredicateKind k)
{
  lclPredicateNode x = (lclPredicateNode) dmalloc (sizeof (*x));

  x->tok = t;
  x->predicate = n;
  x->kind = k;
  return (x);
}

/*@only@*/ quantifierNode
makeQuantifierNode (varNodeList v, ltoken quant)
{
  quantifierNode x = (quantifierNode) dmalloc (sizeof (*x));

  x->quant = quant;
  x->vars = v;
  x->isForall = cstring_equalLit (ltoken_unparse (quant), "\forall");

  return (x);
}

/*@only@*/ arrayQualNode
makeArrayQualNode (ltoken t, termNode term)
{
  arrayQualNode x = (arrayQualNode) dmalloc (sizeof (*x));

  x->tok = t;
  x->term = term;
  return (x);
}

/*@only@*/ varNode
makeVarNode (/*@only@*/ ltoken varid, bool isObj, lclTypeSpecNode t)
{
  varNode x = (varNode) dmalloc (sizeof (*x));
  varInfo vi = (varInfo) dmalloc (sizeof (*vi));
  sort sort;
  
  vi->id = ltoken_copy (varid);
  sort = lclTypeSpecNode2sort (t);
  
  /* 9/3/93, The following is needed because we want value sorts to be
     the default, object sort is generated only if there is "obj" qualifier.
     There are 2 cases: (1) for immutable types (including C primitive types),
     we need to generate the object sort if qualifier is present; (2) for
     array, struct and union types, they are already in their object sorts. 
     */
  
  sort = sort_makeVal (sort);	/* both cases are now value sorts */
  
  if (isObj)
    {
      sort = sort_makeObj (sort);
    }
  
    
  vi->sort = sort;
  vi->kind = VRK_QUANT;
  vi->export = TRUE;

  (void) symtable_enterVar (g_symtab, vi);
  varInfo_free (vi);

  x->varid = varid;
  x->isObj = isObj;
  x->type = t;
  x->sort = sort_makeNoSort ();

  return (x);
}

/*@only@*/ abstBodyNode
makeAbstBodyNode (ltoken t, fcnNodeList f)
{
  abstBodyNode x = (abstBodyNode) dmalloc (sizeof (*x));

  x->tok = t;
  x->typeinv = (lclPredicateNode)0;
  x->fcns = f;
  return (x);
}

/*@only@*/ abstBodyNode
makeExposedBodyNode (ltoken t, lclPredicateNode inv)
{
  abstBodyNode x = (abstBodyNode) dmalloc (sizeof (*x));

  x->tok = t;
  x->typeinv = inv;
  x->fcns = fcnNodeList_undefined;
  return (x);
}

/*@only@*/ abstBodyNode
makeAbstBodyNode2 (ltoken t, ltokenList ops)
{
  abstBodyNode x = (abstBodyNode) dmalloc (sizeof (*x));

  x->tok = t;
  x->typeinv = (lclPredicateNode) 0;

  x->fcns = fcnNodeList_new ();

  ltokenList_elements (ops, op)
    {
      x->fcns = fcnNodeList_add
	(x->fcns,
	 fcnNode_fromDeclarator (lclTypeSpecNode_undefined,
				 makeUnknownDeclaratorNode (ltoken_copy (op))));
    } end_ltokenList_elements;
  
  ltokenList_free (ops);

  return (x);
}

/*@only@*/ stmtNode
  makeStmtNode (ltoken varId, ltoken fcnId, /*@only@*/ termNodeList v)
{
  stmtNode n = (stmtNode) dmalloc (sizeof (*n));

  n->lhs = varId;
  n->operator = fcnId;
  n->args = v;
  return (n);
}

/* printDeclarators -> declaratorNodeList_unparse */

static cstring abstDeclaratorNode_unparse (abstDeclaratorNode x)
{
  return (typeExpr_unparse ((typeExpr) x));
}

/*@only@*/ paramNode
makeParamNode (lclTypeSpecNode t, typeExpr d)
{
  paramNode x = (paramNode) dmalloc (sizeof (*x));
  
  paramNode_checkQualifiers (t, d);

  x->type = t;
  x->paramdecl = d;
  x->kind = PNORMAL; /*< forgot this! >*/

  return (x);
}
  
/*@only@*/ paramNode
paramNode_elipsis (void)
{
  paramNode x = (paramNode) dmalloc (sizeof (*x));

  x->type = (lclTypeSpecNode) 0;
  x->paramdecl = (typeExpr) 0;
  x->kind = PELIPSIS;

  return (x);  
}

static /*@observer@*/ ltoken typeExpr_getTok (typeExpr d)
{
  while (d != (typeExpr)0)
    {
      if (d->kind == TEXPR_BASE)
	{
	  return (d->content.base);
	}
      else
	{
	  if (d->kind == TEXPR_PTR)
	    {
	      d = d->content.pointer;
	    }
	  else if (d->kind == TEXPR_ARRAY)
	    {
	      d = d->content.array.elementtype;
	    }
	  else if (d->kind == TEXPR_FCN) 
	    {
	      d = d->content.function.returntype;
	    }
	  else
	    {
	      BADBRANCH;
	    }
	}
    }

  llfatalerror (cstring_makeLiteral ("typeExpr_getTok: unreachable code"));
  BADEXIT;
}

void
paramNode_checkQualifiers (lclTypeSpecNode t, typeExpr d)
{
  bool isPointer = FALSE;
  bool isUser = FALSE;
  bool hasAlloc = FALSE;
  bool hasAlias = FALSE;

  llassert (lclTypeSpecNode_isDefined (t));

  if (pointers_isUndefined (t->pointers)
      && (d != (typeExpr)0 && d->kind != TEXPR_PTR) && d->kind != TEXPR_ARRAY)
    {
      if (t->kind == LTS_TYPE)
	{
	  sortNode sn;

	  llassert (t->content.type != NULL);

	  sn = sort_quietLookup (sort_getUnderlying ((t->content.type)->sort));

	  if (sn->kind == SRT_PTR || sn->kind == SRT_ARRAY 
	      || sn->kind == SRT_HOF || sn->kind == SRT_NONE)
	    {
	      isPointer = TRUE;
	    }
	}
    }
  else
    {
      isPointer = TRUE;
    }

  if (d != (typeExpr)0 && d->kind != TEXPR_BASE)
    {
      if (t->kind == LTS_TYPE)
	{
	  sortNode sn;

	  llassert (t->content.type != NULL);
	  sn = sort_quietLookup (sort_getUnderlying ((t->content.type)->sort));

	  if (sn->kind == SRT_PTR || sn->kind == SRT_ARRAY
	      || sn->kind == SRT_HOF || sn->kind == SRT_NONE)
	    {
	      isUser = TRUE;
	    }
	}
    }
  else
    {
      isPointer = TRUE;
    }
  
  if (d != (typeExpr)NULL)
    {
      qualList_elements (t->quals, q)
	{
	  if (qual_isAllocQual (q))
	    {
	      if (hasAlloc)
		{
		  ltoken tok  = typeExpr_getTok (d); 
		  lclerror (tok, message ("Parameter declared with multiple allocation "
					  "qualifiers: %q", typeExpr_unparse (d)));
		}
	      hasAlloc = TRUE;
	      
	      if (!isPointer)
		{
		  ltoken tok  = typeExpr_getTok (d); 
		  lclerror (tok, message ("Non-pointer declared as %s parameter: %q", 
					  qual_unparse (q),
					  typeExpr_unparse (d)));
		}
	    }
	  if (qual_isAliasQual (q))
	    {
	      if (hasAlias)
		{
		  ltoken tok  = typeExpr_getTok (d); 
		  lclerror (tok, message ("Parameter declared with multiple alias qualifiers: %q", 
					  typeExpr_unparse (d)));
		}
	      hasAlias = TRUE;
	      
	      if (!(isPointer || isUser))
		{
		  ltoken tok  = typeExpr_getTok (d); 
		  lclerror (tok, message ("Unsharable type declared as %s parameter: %q", 
					  qual_unparse (q),
					  typeExpr_unparse (d)));
		}
	    }
	} end_qualList_elements;
    }
}

/*@only@*/ cstring
paramNode_unparse (paramNode x)
{
  if (x != (paramNode) 0)
    {
      if (x->kind == PELIPSIS)
	{
	  return (cstring_makeLiteral ("..."));
	}

      if (x->paramdecl != (typeExpr) 0)
	{ /* handle (void) */
	  return (message ("%q %q", lclTypeSpecNode_unparse (x->type),
			   typeExpr_unparse (x->paramdecl)));
	}
      else
	{
	  return (lclTypeSpecNode_unparse (x->type));
	}
    }
  return cstring_undefined;
}

static cstring 
lclTypeSpecNode_unparseAltComments (/*@null@*/ lclTypeSpecNode typespec) /*@*/
{
  if (typespec != (lclTypeSpecNode) 0)
    {
      cstring s = qualList_toCComments (typespec->quals);

      switch (typespec->kind)
	{
	case LTS_TYPE:
	  {
	    llassert (typespec->content.type != NULL);

	    return (cstring_concatFree 
		    (s, printLeaves (typespec->content.type->ctypes)));
	  }
	case LTS_ENUM:
	  {
	    bool first = TRUE;
	    enumSpecNode n = typespec->content.enumspec;
	    
	    s = cstring_concatFree (s, cstring_makeLiteral ("enum"));
	    llassert (n != NULL);

	    if (!ltoken_isUndefined (n->opttagid))
	      {
		s = message ("%q %s", s, ltoken_unparse (n->opttagid));
	      }
	    s = message ("%q {", s); 

	    ltokenList_elements (n->enums, e)
	    {
	      if (first)
		{
		  first = FALSE;
		  s = message ("%q%s", s, ltoken_getRawString (e));
		}
	      else
		s = message ("%q, %s", s, ltoken_getRawString (e));
	    } end_ltokenList_elements;
	    
	    return (message ("%q}", s));
	  }
	case LTS_STRUCTUNION:
	  {
	    strOrUnionNode n = typespec->content.structorunion;
	    stDeclNodeList decls;

	    llassert (n != NULL);

	    switch (n->kind)
	      {
	      case SU_STRUCT:
		s = cstring_concatFree (s, cstring_makeLiteral ("struct "));
		/*@switchbreak@*/ break;
	      case SU_UNION:
		s = cstring_concatFree (s, cstring_makeLiteral ("union "));
		/*@switchbreak@*/ break;
	      }

	    if (!ltoken_isUndefined (n->opttagid))
	      {
		if (stDeclNodeList_size (n->structdecls) == 0)
		  {
		    return (message ("%q%s", s, ltoken_unparse (n->opttagid)));
		  }

		s = message ("%q%s {\n\t", s, ltoken_unparse (n->opttagid));
	      }
	    else
	      {
		s = message ("%q{\n\t", s);
	      }

	    decls = n->structdecls;

	    stDeclNodeList_elements (decls, f)
	    {
	      s = message ("%q%q %q;\n\t", s, 
			   lclTypeSpecNode_unparseAltComments (f->lcltypespec),
			  declaratorNodeList_unparse (f->declarators));
	    } end_stDeclNodeList_elements;

	    return (message ("%q }", s));
	  }
	case LTS_CONJ:
	  {
	    cstring_free (s);

	    return 
	      (message
	       ("%q, %q",
		lclTypeSpecNode_unparseAltComments (typespec->content.conj->a),
		lclTypeSpecNode_unparseAltComments (typespec->content.conj->b)));
	  }
        BADDEFAULT;
	}
    }
  else
    {
      llcontbuglit ("lclTypeSpecNode_unparseComments: null typespec");
      
      return cstring_undefined;
    }
  
  BADEXIT;
}

cstring lclTypeSpecNode_unparseComments (/*@null@*/ lclTypeSpecNode typespec)
{
  if (typespec != (lclTypeSpecNode) 0)
    {
      cstring s = qualList_toCComments (typespec->quals);

      switch (typespec->kind)
	{
	case LTS_TYPE:
	  {
	    llassert (typespec->content.type != NULL);

	    return (cstring_concatFree 
		    (s, printLeaves (typespec->content.type->ctypes)));
	  }
	case LTS_ENUM:
	  {
	    bool first = TRUE;
	    enumSpecNode n = typespec->content.enumspec;
	    
	    s = cstring_concatFree (s, cstring_makeLiteral ("enum"));
	    llassert (n != NULL);

	    if (!ltoken_isUndefined (n->opttagid))
	      {
		s = message ("%q %s", s, ltoken_unparse (n->opttagid));
	      }
	    s = message ("%q {", s); 

	    ltokenList_elements (n->enums, e)
	    {
	      if (first)
		{
		  first = FALSE;
		  s = message ("%q%s", s, ltoken_getRawString (e));
		}
	      else
		s = message ("%q, %s", s, ltoken_getRawString (e));
	    } end_ltokenList_elements;
	    
	    return (message ("%q}", s));
	  }
	case LTS_STRUCTUNION:
	  {
	    strOrUnionNode n = typespec->content.structorunion;
	    stDeclNodeList decls;

	    llassert (n != NULL);

	    switch (n->kind)
	      {
	      case SU_STRUCT:
		s = cstring_concatFree (s, cstring_makeLiteral ("struct "));
		/*@switchbreak@*/ break;
	      case SU_UNION:
		s = cstring_concatFree (s, cstring_makeLiteral ("union "));
		/*@switchbreak@*/ break;
	      }

	    if (!ltoken_isUndefined (n->opttagid))
	      {
		if (stDeclNodeList_size (n->structdecls) == 0)
		  {
		    return (message ("%q%s", s, ltoken_unparse (n->opttagid)));
		  }

		s = message ("%q%s {\n\t", s, ltoken_unparse (n->opttagid));
	      }
	    else
	      {
		s = message ("%q{\n\t", s);
	      }

	    decls = n->structdecls;

	    stDeclNodeList_elements (decls, f)
	    {
	      s = message ("%q%q %q;\n\t", s, 
			   lclTypeSpecNode_unparseComments (f->lcltypespec),
			  declaratorNodeList_unparse (f->declarators));
	    } end_stDeclNodeList_elements;

	    return (message ("%q }", s));
	  }
	case LTS_CONJ:
	  {
	    cstring_free (s);

	    return 
	      (message
	       ("%q /*@alt %q@*/",
		lclTypeSpecNode_unparseComments (typespec->content.conj->a),
		lclTypeSpecNode_unparseAltComments (typespec->content.conj->b)));
	     }
        BADDEFAULT;
	}
    }
  else
    {
      llcontbuglit ("lclTypeSpecNode_unparseComments: null typespec");
      
      return cstring_undefined;
    }
  
  BADEXIT;
}

/*@only@*/ cstring
paramNode_unparseComments (paramNode x)
{
  if (x != (paramNode) 0)
    {
      if (x->kind == PELIPSIS)
	{
	  return (cstring_makeLiteral ("..."));
	}

      if (x->paramdecl != (typeExpr) 0)
	{			/* handle (void) */
	  return (message ("%q %q", 
			   lclTypeSpecNode_unparseComments (x->type),
			   typeExpr_unparseNoBase (x->paramdecl)));
	}
      else
	{
	  return (lclTypeSpecNode_unparseComments (x->type));
	}
    }
  return cstring_undefined;
}

/*@only@*/ termNode
makeIfTermNode (ltoken ift, termNode ifn, ltoken thent, 
		termNode thenn, ltoken elset, 
		termNode elsen)
{
  termNode t = (termNode) dmalloc (sizeof (*t));
  opFormNode opform = makeOpFormNode (ift, OPF_IF, opFormUnion_createMiddle (0),
				      ltoken_undefined);
  nameNode nn = makeNameNodeForm (opform);
  termNodeList args = termNodeList_new ();

  t->error_reported = FALSE;
  t->wrapped = 0;
  termNodeList_addh (args, ifn);
  termNodeList_addh (args, thenn);
  termNodeList_addh (args, elsen);
  t->name = nn;
  t->args = args;
  t->kind = TRM_APPLICATION;
  t->sort = sort_makeNoSort ();
  t->given = t->sort;
  t->possibleSorts = sortSet_new ();
  t->possibleOps = lslOpSet_new ();
  
  ltoken_free (thent);
  ltoken_free (elset);

  return (t);
}

static /*@observer@*/ ltoken
  nameNode2anyOp (nameNode n)
{
  if (n != (nameNode) 0)
    {
      opFormNode opnode = n->content.opform;
      opFormKind kind;

      llassert (opnode != NULL);

      kind = opnode->kind;

      if (kind == OPF_MANYOPM || kind == OPF_ANYOP ||
	  kind == OPF_MANYOP || kind == OPF_ANYOPM)
	{
	  opFormUnion u;

	  u = opnode->content;
	  return u.anyop;
	}
    }
  return ltoken_undefined;
}

/*@only@*/ termNode
makeInfixTermNode (termNode x, ltoken op, termNode y)
{
  termNode t = (termNode) dmalloc (sizeof (*t));
  opFormNode opform;
  nameNode nn;
  termNodeList args = termNodeList_new ();
  
  checkAssociativity (x, op);

  opform = makeOpFormNode (op, OPF_MANYOPM,
			   opFormUnion_createAnyOp (op), 
			   ltoken_undefined);

  nn = makeNameNodeForm (opform);

  t->error_reported = FALSE;
  t->wrapped = 0;
  termNodeList_addh (args, x);
  termNodeList_addh (args, y);
  t->name = nn;
  t->args = args;
  t->kind = TRM_APPLICATION;
  t->sort = sort_makeNoSort ();
  t->given = t->sort;
  t->possibleSorts = sortSet_new (); /* sort_equal */
  t->possibleOps = lslOpSet_new ();
  return (t);
}

/*@only@*/ quantifiedTermNode
  quantifiedTermNode_copy (quantifiedTermNode q)
{
  quantifiedTermNode ret = (quantifiedTermNode) dmalloc (sizeof (*ret));

  ret->quantifiers = quantifierNodeList_copy (q->quantifiers);
  ret->open = ltoken_copy (q->open);
  ret->close = ltoken_copy (q->close);
  ret->body = termNode_copySafe (q->body);

  return (ret);
}

/*@only@*/ termNode
makeQuantifiedTermNode (quantifierNodeList qn, ltoken open,
			termNode t, ltoken close)
{
  sort sort;
  termNode n = (termNode) dmalloc (sizeof (*n));
  quantifiedTermNode q = (quantifiedTermNode) dmalloc (sizeof (*q));

  n->name = NULL; /*> missing this --- detected by splint <*/
  n->error_reported = FALSE;
  n->wrapped = 0;
  n->error_reported = FALSE;
  n->kind = TRM_QUANTIFIER;
  n->possibleSorts = sortSet_new ();
  n->possibleOps = lslOpSet_new ();
  n->kind = TRM_UNCHANGEDALL;
  n->args = termNodeList_new (); /*< forgot this >*/

  termNodeList_free (t->args);
  t->args = termNodeList_new ();

  sort = g_sortBool;
  n->sort = sort;
  (void) sortSet_insert (n->possibleSorts, sort);

  q->quantifiers = qn;
  q->open = open;
  q->close = close;
  q->body = t;

  n->quantified = q;
  return (n);
}

/*@only@*/ termNode
makePostfixTermNode (/*@returned@*/ /*@only@*/ termNode secondary, ltokenList postfixops)
{
  termNode top = secondary;

  ltokenList_elements (postfixops, op)
    {
      top = makePostfixTermNode2 (top, ltoken_copy (op));
      /*@i@*/ } end_ltokenList_elements;

  ltokenList_free (postfixops);

  return (top); /* dep as only? */
}

/*
** secondary is returned in the args list
*/

/*@only@*/ termNode
makePostfixTermNode2 (/*@returned@*/ /*@only@*/ termNode secondary, 
		      /*@only@*/ ltoken postfixop)
{
  termNode t = (termNode) dmalloc (sizeof (*t));

  opFormNode opform = makeOpFormNode (postfixop,
				      OPF_MANYOP, opFormUnion_createAnyOp (postfixop),
				      ltoken_undefined);
  nameNode nn = makeNameNodeForm (opform);
  termNodeList args = termNodeList_new ();

  t->error_reported = FALSE;
  t->wrapped = 0;
  termNodeList_addh (args, secondary);
  t->name = nn;
  t->args = args;
  t->kind = TRM_APPLICATION;
  t->sort = sort_makeNoSort ();
  t->given = t->sort;
  t->possibleSorts = sortSet_new ();
  t->possibleOps = lslOpSet_new ();
  return t;
}

/*@only@*/ termNode
makePrefixTermNode (ltoken op, termNode arg)
{
  termNode t = (termNode) dmalloc (sizeof (*t));
  termNodeList args = termNodeList_new ();
  opFormNode opform = makeOpFormNode (op, OPF_ANYOPM, opFormUnion_createAnyOp (op),
				      ltoken_undefined);
  nameNode nn = makeNameNodeForm (opform);

  t->error_reported = FALSE;
  t->wrapped = 0;
  t->name = nn;
  termNodeList_addh (args, arg);
  t->args = args;
  t->kind = TRM_APPLICATION;
  t->sort = sort_makeNoSort ();
  t->given = t->sort;
  t->possibleSorts = sortSet_new ();
  t->possibleOps = lslOpSet_new ();
  return t;
}

/*@only@*/ termNode
makeOpCallTermNode (ltoken op, ltoken open,
		    termNodeList args, ltoken close)
{
  /* like prefixTerm, but with opId LPAR termNodeList  RPAR */
  termNode t = (termNode) dmalloc (sizeof (*t));
  nameNode nn = makeNameNodeId (op);
  
  t->error_reported = FALSE;
  t->wrapped = 0;
  t->name = nn;
  t->args = args;
  t->kind = TRM_APPLICATION;
  t->sort = sort_makeNoSort ();
  t->given = t->sort;
  t->possibleSorts = sortSet_new ();
  t->possibleOps = lslOpSet_new ();

  ltoken_free (open);
  ltoken_free (close);

  return t;
}

/*@exposed@*/ termNode
CollapseInfixTermNode (/*@returned@*/ termNode secondary, termNodeList infix)
{
  termNode left = secondary;

  termNodeList_elements (infix, node)
    {
      termNodeList_addl (node->args, termNode_copySafe (left));
      left = node;
      /*    computePossibleSorts (left); */
    } end_termNodeList_elements;

  return (left);
}

static void
checkAssociativity (termNode x, ltoken op)
{
  ltoken lastOpToken;

  if (x->wrapped == 0 &&	/* no parentheses */
      x->kind == TRM_APPLICATION && x->name != (nameNode) 0 &&
      (!x->name->isOpId))
    {
      lastOpToken = nameNode2anyOp (x->name);

      if ((ltoken_getCode (lastOpToken) == logicalOp &&
	   ltoken_getCode (op) == logicalOp) ||
	  ((ltoken_getCode (lastOpToken) == simpleOp ||
	    ltoken_getCode (lastOpToken) == LLT_MULOP) &&
	   (ltoken_getCode (op) == simpleOp ||
	    ltoken_getCode (op) == LLT_MULOP)))
	if (ltoken_getText (lastOpToken) != ltoken_getText (op))
	  {
	    lclerror (op, 
		      message
		      ("Parentheses needed to specify associativity of %s and %s",
		       cstring_fromChars (lsymbol_toChars (ltoken_getText (lastOpToken))),
		       cstring_fromChars (lsymbol_toChars (ltoken_getText (op)))));
	  }
    }
}

termNodeList
pushInfixOpPartNode (/*@returned@*/ termNodeList x, ltoken op,
		     /*@only@*/ termNode secondary)
{
  termNode lastLeftTerm;
  termNodeList args = termNodeList_new ();
  termNode t = (termNode) dmalloc (sizeof (*t));
  opFormNode opform;
  nameNode nn;

  termNodeList_addh (args, secondary);
  
  if (!termNodeList_empty (x))
    {
      termNodeList_reset (x);
      lastLeftTerm = termNodeList_current (x);
      checkAssociativity (lastLeftTerm, op);
    }

  opform = makeOpFormNode (op, OPF_MANYOPM, 
			   opFormUnion_createAnyOp (op), ltoken_undefined);

  nn = makeNameNodeForm (opform);

  t->error_reported = FALSE;
  t->wrapped = 0;
  t->name = nn;
  t->kind = TRM_APPLICATION;
  t->args = args;
  t->sort = sort_makeNoSort ();
  t->given = t->sort;
  t->possibleSorts = sortSet_new ();
  t->possibleOps = lslOpSet_new ();
  termNodeList_addh (x, t);
  /* don't compute sort yet, do it in CollapseInfixTermNode */
  return (x);
}

termNode
updateMatchedNode (/*@only@*/ termNode left, /*@returned@*/ termNode t, 
		   /*@only@*/ termNode right)
{
  opFormNode op;

  if ((t == (termNode) 0) || (t->name == NULL) || t->name->isOpId)
    {
      llbugexitlit ("updateMatchedNode: expect opForm in nameNode");
    }

  op = t->name->content.opform;
  llassert (op != NULL);

  if (left == (termNode) 0)
    {
      if (right == (termNode) 0)
	{
	  /* op->kind is not changed */
	  termNode_free (right);
	}
      else
	{
	  op->kind = OPF_MIDDLEM;
	  op->key = opFormNode2key (op, OPF_MIDDLEM);
	  termNodeList_addh (t->args, right);
	}
    }
  else
    {
      termNodeList_addl (t->args, left);
      if (right == (termNode) 0)
	{
	  op->kind = OPF_MMIDDLE;
	  op->key = opFormNode2key (op, OPF_MMIDDLE);
	}
      else
	{
	  op->kind = OPF_MMIDDLEM;
	  op->key = opFormNode2key (op, OPF_MMIDDLEM);
	  termNodeList_addh (t->args, right);
	}
    }
  return t;
}

/*@only@*/ termNode
  updateSqBracketedNode (/*@only@*/ termNode left,
			 /*@only@*/ /*@returned@*/ termNode t,
			 /*@only@*/ termNode right)
{
  opFormNode op;

  if ((t == (termNode) 0) || (t->name == NULL) || (t->name->isOpId))
    {
      llbugexitlit ("updateSqBracketededNode: expect opForm in nameNode");
    }

  op = t->name->content.opform;
  llassert (op != NULL);

  if (left == (termNode) 0)
    {
      if (right == (termNode) 0)
	{
	  /* op->kind is not changed */
	}
      else
	{
	  op->kind = OPF_BMIDDLEM;
	  op->key = opFormNode2key (op, OPF_BMIDDLEM);
	  termNodeList_addh (t->args, right);
	}
    }
  else
    {
      termNodeList_addl (t->args, left);

      if (right == (termNode) 0)
	{
	  op->kind = OPF_BMMIDDLE;
	  op->key = opFormNode2key (op, OPF_BMMIDDLE);
	}
      else
	{
	  op->kind = OPF_BMMIDDLEM;
	  op->key = opFormNode2key (op, OPF_BMMIDDLEM);
	  termNodeList_addh (t->args, right);
	}
    }
  return t;
}

/*@only@*/ termNode
makeSqBracketedNode (ltoken lbracket,
		     termNodeList args, ltoken rbracket)
{
  termNode t = (termNode) dmalloc (sizeof (*t));
  int size;
  opFormNode opform;
  nameNode nn;

  t->error_reported = FALSE;
  t->wrapped = 0;
  
  size = termNodeList_size (args);
  opform = makeOpFormNode (lbracket, OPF_BMIDDLE, opFormUnion_createMiddle (size),
			   rbracket);
  nn = makeNameNodeForm (opform);
  t->name = nn;
  t->kind = TRM_APPLICATION;
  t->args = args;
  t->sort = sort_makeNoSort ();
  t->given = t->sort;
  t->possibleSorts = sortSet_new ();
  t->possibleOps = lslOpSet_new ();
 /* do sort checking later, not here, incomplete parse */
  return (t);
}

/*@only@*/ termNode
makeMatchedNode (ltoken open, termNodeList args, ltoken close)
{
  /*   matched : open args close */
  termNode t = (termNode) dmalloc (sizeof (*t));
  int size;
  opFormNode opform;
  nameNode nn;

  t->error_reported = FALSE;
  t->wrapped = 0;
  
  size = termNodeList_size (args);
  opform = makeOpFormNode (open, OPF_MIDDLE, opFormUnion_createMiddle (size), close);
  nn = makeNameNodeForm (opform);
  t->name = nn;
  t->kind = TRM_APPLICATION;
  t->args = args;
  t->sort = sort_makeNoSort ();
  t->given = t->sort;
  t->possibleSorts = sortSet_new ();
  t->possibleOps = lslOpSet_new ();
 /* do sort checking later, not here, incomplete parse */
  return (t);
}

/*@only@*/ termNode
makeSimpleTermNode (ltoken varid)
{
  sort theSort = sort_makeNoSort ();
  lsymbol sym;
  opInfo oi;
  varInfo vi;
  termNode n = (termNode) dmalloc (sizeof (*n));
  
  n->error_reported = FALSE;
  n->wrapped = 0;
  n->name = (nameNode) 0;
  n->given = theSort;
  n->args = termNodeList_new ();
  n->possibleSorts = sortSet_new ();
  n->possibleOps = lslOpSet_new ();
  
  sym = ltoken_getText (varid);
  
  /* lookup current scope */
    vi = symtable_varInfoInScope (g_symtab, sym);

  if (varInfo_exists (vi))
    {
      theSort = vi->sort;
      n->kind = TRM_VAR;
      n->sort = theSort;
      n->literal = varid;
      (void) sortSet_insert (n->possibleSorts, theSort);
    }
  else
    {				/* need to handle LCL constants */
      vi = symtable_varInfo (g_symtab, sym);

      if (varInfo_exists (vi) && vi->kind == VRK_CONST)
	{
	  theSort = vi->sort;
	  n->kind = TRM_CONST;
	  n->sort = theSort;
	  n->literal = varid;
	  (void) sortSet_insert (n->possibleSorts, theSort);
	}
      else
	{			/* and LSL operators (true, false, new, nil, etc) */
	  nameNode nn = makeNameNodeId (ltoken_copy (varid));
	  oi = symtable_opInfo (g_symtab, nn);

	  if (opInfo_exists (oi) && (oi->name->isOpId) &&
	      !sigNodeSet_isEmpty (oi->signatures))
	    {
	      sigNodeSet_elements (oi->signatures, x)
		{
		  if (ltokenList_empty (x->domain))
		    /* yes, it really is empty, not not empty */
		    {
		      lslOp op = (lslOp) dmalloc (sizeof (*op));
		      
		      op->name = nameNode_copy (nn);
		      op->signature = x;
		      (void) sortSet_insert (n->possibleSorts, sigNode_rangeSort (x));
		      (void) lslOpSet_insert (n->possibleOps, op);
		    }
		} end_sigNodeSet_elements;
	    }

	  nameNode_free (nn);
	  
	  if (sortSet_size (n->possibleSorts) == 0)
	    {
	      lclerror 
		(varid, 
		 message ("Unrecognized identifier (constant, variable or operator): %s",
			  ltoken_getRawString (varid)));

	    }
	  
	  n->sort = sort_makeNoSort ();
	  n->literal = varid;
	  n->kind = TRM_ZEROARY;
	}
    }

  return (n);
}

/*@only@*/ termNode
makeSelectTermNode (termNode pri, ltoken select, /*@dependent@*/ ltoken id)
{
  termNode t = (termNode) dmalloc (sizeof (*t));
  opFormNode opform = makeOpFormNode (select,
				      OPF_MSELECT, opFormUnion_createAnyOp (id),
				      ltoken_undefined);
  nameNode nn = makeNameNodeForm (opform);
  termNodeList args = termNodeList_new ();

  t->error_reported = FALSE;
  t->wrapped = 0;
  t->name = nn;
  t->kind = TRM_APPLICATION;
  termNodeList_addh (args, pri);
  t->args = args;
  t->kind = TRM_APPLICATION;
  t->sort = sort_makeNoSort ();
  t->given = t->sort;
  t->possibleSorts = sortSet_new ();
  t->possibleOps = lslOpSet_new ();

  return t;
}

/*@only@*/ termNode
makeMapTermNode (termNode pri, ltoken map, /*@dependent@*/ ltoken id)
{
  termNode t = (termNode) dmalloc (sizeof (*t));
  opFormNode opform = makeOpFormNode (map, OPF_MMAP, opFormUnion_createAnyOp (id),
				      ltoken_undefined);
  nameNode nn = makeNameNodeForm (opform);
  termNodeList args = termNodeList_new ();

  t->error_reported = FALSE;
  t->wrapped = 0;
  t->kind = TRM_APPLICATION;
  t->name = nn;
  termNodeList_addh (args, pri);
  t->args = args;
  t->kind = TRM_APPLICATION;
  t->sort = sort_makeNoSort ();
  t->given = t->sort;
  t->possibleSorts = sortSet_new ();
  t->possibleOps = lslOpSet_new ();
  return t;
}

/*@only@*/ termNode
makeLiteralTermNode (ltoken tok, sort s)
{
  nameNode nn = makeNameNodeId (ltoken_copy (tok));
  opInfo oi = symtable_opInfo (g_symtab, nn);
  lslOp op = (lslOp) dmalloc (sizeof (*op));  
  termNode n = (termNode) dmalloc (sizeof (*n));
  sigNode sign;
  ltoken range;

  n->name = nn;
  n->error_reported = FALSE;
  n->wrapped = 0;
  n->kind = TRM_LITERAL;
  n->literal = tok;
  n->given = sort_makeNoSort ();
  n->sort = n->given;
  n->args = termNodeList_new ();
  n->possibleSorts = sortSet_new ();
  n->possibleOps = lslOpSet_new ();

  /* look up signatures for this operator too */
  
  range = ltoken_create (simpleId, sort_getLsymbol (s));
  sign = makesigNode (ltoken_undefined, ltokenList_new (),  
			    ltoken_copy (range));
  
  if (opInfo_exists (oi) && (oi->name->isOpId) 
      && (sigNodeSet_size (oi->signatures) > 0))
    {
      sigNodeSet_elements (oi->signatures, x)
	{
	  if (ltokenList_empty (x->domain))
	    {
	      lslOp opn = (lslOp) dmalloc (sizeof (*opn));
	      sort sort;

	      opn->name = nameNode_copy (nn);
	      opn->signature = x;
	      sort = sigNode_rangeSort (x);
	      (void) sortSet_insert (n->possibleSorts, sort);
	      (void) lslOpSet_insert (n->possibleOps, opn);
	    }
	} end_sigNodeSet_elements;
    }
  
  /* insert into literal term */
  (void) sortSet_insert (n->possibleSorts, s);
  
  op->name = nameNode_copy (nn);
  op->signature = sign;
  (void) lslOpSet_insert (n->possibleOps, op);

  /* enter the literal as an operator into the operator table */
  /* 8/9/93.  C's char constant 'c' syntax conflicts
     with LSL's lslinit.lsi table.  Throw out, because it's not
     needed anyway.  */
  /*  symtable_enterOp (g_symtab, nn, sign); */

  if (s == g_sortInt)
    {
      sigNode osign;
      lslOp opn = (lslOp) dmalloc (sizeof (*opn));

      /* if it is a C int, we should overload it as double too because
	 C allows you to say "x > 2". */
      
      (void) sortSet_insert (n->possibleSorts, g_sortDouble);
      
      ltoken_setText (range, lsymbol_fromChars ("double"));
      osign = makesigNode (ltoken_undefined, ltokenList_new (), range);
      opn->name = nameNode_copy (nn);
      opn->signature = osign;
      (void) lslOpSet_insert (n->possibleOps, opn);
      
      symtable_enterOp (g_symtab, nameNode_copySafe (nn), sigNode_copy (osign));
    }
  else
    {
      ltoken_free (range);
    }
      
  /* future: could overload cstrings to be both char_Vec as well as
     char_ObjPtr */
  
  /*@-mustfree@*/
  return n;
} /*@=mustfree@*/

/*@only@*/ termNode
makeUnchangedTermNode1 (ltoken op, /*@unused@*/ ltoken all)
{
  termNode t = (termNode) dmalloc (sizeof (*t));

  t->error_reported = FALSE;
  t->wrapped = 0;
  t->kind = TRM_UNCHANGEDALL;
  t->sort = g_sortBool;
  t->literal = op;
  t->given = sort_makeNoSort ();
  t->name = NULL; /*< missing this >*/
  t->args = termNodeList_new ();
  t->possibleSorts = sortSet_new ();
  t->possibleOps = lslOpSet_new ();
  (void) sortSet_insert (t->possibleSorts, t->sort);

  ltoken_free (all);

  return t;
}

/*@only@*/ termNode
makeUnchangedTermNode2 (ltoken op, storeRefNodeList x)
{
  termNode t = (termNode) dmalloc (sizeof (*t));
  ltoken errtok;
  sort sort;

  t->name = NULL; /*< missing this >*/
  t->error_reported = FALSE;
  t->wrapped = 0;
  t->kind = TRM_UNCHANGEDOTHERS;
  t->sort = g_sortBool;
  t->literal = op;
  t->unchanged = x;
  t->given = sort_makeNoSort ();
  t->possibleSorts = sortSet_new ();
  t->possibleOps = lslOpSet_new ();
  t->args = termNodeList_new ();

  (void) sortSet_insert (t->possibleSorts, t->sort);
  /* check storeRefNode's are mutable, uses sort of term */
  
  storeRefNodeList_elements (x, sto)
    {
      if (storeRefNode_isTerm (sto))
	{
	  sort = sto->content.term->sort;
	  if (!sort_mutable (sort))
	    {
	      errtok = termNode_errorToken (sto->content.term);
	      lclerror (errtok, 
			message ("Term denoting immutable object used in unchanged list: %q",
				 termNode_unparse (sto->content.term)));
	    }
	}
      else
	{
	  if (storeRefNode_isType (sto))
	    {
	      lclTypeSpecNode type = sto->content.type;
	      sort = lclTypeSpecNode2sort (type);
	      if (!sort_mutable (sort))
		{
		  errtok = lclTypeSpecNode_errorToken (type);
		  lclerror (errtok, message ("Immutable type used in unchanged list: %q",
					     sort_unparse (sort)));
		}
	    }
	}
    } end_storeRefNodeList_elements;
  
  return t;
}

/*@only@*/ termNode
  makeSizeofTermNode (ltoken op, lclTypeSpecNode type)
{
  termNode t = (termNode) dmalloc (sizeof (*t));
  
  t->name = NULL; /*< missing this >*/
  t->error_reported = FALSE;
  t->wrapped = 0;
  t->kind = TRM_SIZEOF;
  t->sort = g_sortInt;
  t->literal = op;
  t->sizeofField = type;
  t->given = sort_makeNoSort ();
  t->possibleSorts = sortSet_new ();
  t->possibleOps = lslOpSet_new ();
  t->args = termNodeList_new (); 

  (void) sortSet_insert (t->possibleSorts, t->sort);
  /* nothing to check */
  return (t);
}

/*@only@*/ cstring
claimNode_unparse (claimNode c)
{
  if (c != (claimNode) 0)
    {
      cstring s = message ("claims (%q)%q{\n%q", 
			   paramNodeList_unparse (c->params),
			   varDeclarationNodeList_unparse (c->globals),
			   lclPredicateNode_unparse (c->require));

      if (c->body != NULL)
	{
	  s = message ("%qbody {%q}\n", s, programNode_unparse (c->body));
	}
      s = message ("%q%q}\n", s, lclPredicateNode_unparse (c->ensures));
      return s;
    }
  return cstring_undefined;
}

static void
WrongArity (ltoken tok, int expect, int size)
{
  lclerror (tok, message ("Expecting %d arguments but given %d", expect, size));
}

static cstring
printTermNode2 (/*@null@*/ opFormNode op, termNodeList args, sort sort)
{
  if (op != (opFormNode) 0)
    {
      cstring s = cstring_undefined;
      cstring sortText;
      cstring sortSpace;

      if (sort != sort_makeNoSort ())
	{
	  sortText = message (": %s", cstring_fromChars (lsymbol_toChars (sort_getLsymbol (sort))));
	  sortSpace = cstring_makeLiteral (" ");
	}
      else
	{
	  sortText = cstring_undefined;
	  sortSpace = cstring_undefined;
	}

      switch (op->kind)
	{
	case OPF_IF:
	  {
	    int size = termNodeList_size (args);

	    if (size == 3)
	      {
		s = message ("if %q then %q else %q\n",
			     termNode_unparse (termNodeList_getN (args, 0)),
			     termNode_unparse (termNodeList_getN (args, 1)),
			     termNode_unparse (termNodeList_getN (args, 2)));
	      }
	    else
	      {
		WrongArity (op->tok, 3, size);
		s = cstring_makeLiteral ("if __ then __ else __");
	      }
	    s = message ("%q%s", s, sortText);
	    break;
	  }
	case OPF_ANYOP:
	  {			/* ymtan ? */
	    s = message ("%s %s", 
			 ltoken_getRawString (op->content.anyop), 
			 sortText);
	    break;
	  }
	case OPF_MANYOP:
	  {
	    int size = termNodeList_size (args);

	    if (size == 1)
	      {
		s = message ("%q ", termNode_unparse (termNodeList_head (args)));
	      }
	    else
	      {
		WrongArity (op->content.anyop, 1, size);
		s = cstring_makeLiteral ("__ ");
	      }
	    s = message ("%q%s%s", s, ltoken_getRawString (op->content.anyop),
			 sortText);
	    break;
	  }
	case OPF_ANYOPM:
	  {
	    int size = termNodeList_size (args);

	    s = message ("%s ", ltoken_getRawString (op->content.anyop));

	    if (size == 1)
	      {
		s = message ("%q%q", s, termNode_unparse (termNodeList_head (args)));
	      }
	    else
	      {
		WrongArity (op->content.anyop, 1, size);
		s = message ("%q__", s);
	      }
	    s = message ("%q%s", s, sortText);
	    break;
	  }
	case OPF_MANYOPM:
	  {
	    int size = termNodeList_size (args);

	    if (size == 2)
	      {
		s = message ("%q %s %q",
			     termNode_unparse (termNodeList_getN (args, 0)),
			     ltoken_getRawString (op->content.anyop),
			     termNode_unparse (termNodeList_getN (args, 1)));
	      }
	    else
	      {
		WrongArity (op->content.anyop, 2, size);
		s = message ("__ %s __", ltoken_getRawString (op->content.anyop));
	      }
	    s = message ("%q%s", s, sortText);
	    break;
	  }
	case OPF_MIDDLE:
	  {
	    int size = termNodeList_size (args);
	    int expect = op->content.middle;
	    
	    /* ymtan ? use { or openSym token ? */
	    
	    if (size == expect)
	      {
		s = message ("{%q}", termNodeList_unparse (args));
	      }
	    else
	      {
		WrongArity (op->tok, expect, size);
		s = cstring_makeLiteral ("{ * }");
	      }

	    s = message ("%q%s", s, sortText);
	    break; 
	  }
	case OPF_MMIDDLE:
	  {
	    int size = termNodeList_size (args);
	    int expect = op->content.middle + 1;

	    if (size == expect)
	      {
		s = message ("%q{%q}",
			     termNode_unparse (termNodeList_head (args)),
			     termNodeList_unparseTail (args));
	      }
	    else
	      {
		WrongArity (op->tok, expect, size);
		s = cstring_makeLiteral ("__ { * }");
	      }

	    s = message ("%q%s", s, sortText);
	    break;
	  }
	case OPF_MIDDLEM:
	  {
	    int size = termNodeList_size (args);
	    int expect = op->content.middle + 1;

	    if (size == expect)
	      {
		termNodeList_finish (args);

		s = message ("{%q}%s%s%q",
			     termNodeList_unparseToCurrent (args),
			     sortText, sortSpace,
			     termNode_unparse (termNodeList_current (args)));
	      }
	    else
	      {
		WrongArity (op->tok, expect, size);

		s = message ("{ * }%s __", sortText);

	       /* used to put in extra space! evs 94-01-05 */
	      }
	    break;
	  }
	case OPF_MMIDDLEM:
	  {
	    int size = termNodeList_size (args);
	    int expect = op->content.middle + 2;

	    if (size == expect)
	      {
		termNodeList_finish (args);

		s = message ("%q {%q} %s%s%q",
			     termNode_unparse (termNodeList_head (args)),
			     termNodeList_unparseSecondToCurrent (args),
			     sortText, sortSpace,
			     termNode_unparse (termNodeList_current (args)));
	      }
	    else
	      {
		WrongArity (op->tok, expect, size);
		s = message ("__ { * } %s __", sortText);

	       /* also had extra space? */
	      }
	    break;
	  }
	case OPF_BMIDDLE:
	  {
	    int size = termNodeList_size (args);
	    int expect = op->content.middle;

	    if (size == expect)
	      {
		s = message ("[%q]", termNodeList_unparse (args));
	      }
	    else
	      {
		WrongArity (op->tok, expect, size);
		s = cstring_makeLiteral ("[ * ]");
	      }
	    s = message ("%q%s", s, sortText);
	    break;
	  }
	case OPF_BMMIDDLE:
	  {
	    int size = termNodeList_size (args);
	    int expect = op->content.middle + 1;

	    if (size == expect)
	      {
		s = message ("%q[%q]",
			     termNode_unparse (termNodeList_head (args)),
			     termNodeList_unparseTail (args));
	      }
	    else
	      {
		WrongArity (op->tok, expect, size);
		s = cstring_makeLiteral ("__ [ * ]");
	      }

	    s = message ("%q%s", s, sortText);
	    break;
	  }
	case OPF_BMMIDDLEM:
	  {
	    int size = termNodeList_size (args);
	    int expect = op->content.middle + 1;

	    if (size == expect)
	      {
		s = message ("%q[%q] __",
			     termNode_unparse (termNodeList_head (args)),
			     termNodeList_unparseTail (args));
	      }
	    else
	      {
		WrongArity (op->tok, expect, size);
		s = cstring_makeLiteral ("__ [ * ] __");
	      }
	    s = message ("%q%s", s, sortText);
	    break;
	  }
	case OPF_BMIDDLEM:
	  {
	    int size = termNodeList_size (args);
	    int expect = op->content.middle + 1;

	    if (size == expect)
	      {
		termNodeList_finish (args);

		s = message ("[%q]%s%s%q",
			     termNodeList_unparseToCurrent (args),
			     sortText, sortSpace,
			     termNode_unparse (termNodeList_current (args)));
	      }
	    else
	      {
		WrongArity (op->tok, expect, size);
		s = cstring_makeLiteral ("[ * ] __");
	      }
	    
	    break;
	  }
	case OPF_SELECT:
	  {			/* ymtan constant, check args ? */
	    s = cstring_prependChar ('.', ltoken_getRawString (op->content.id));
	    break;
	  }
	case OPF_MAP:
	  s = cstring_concat (cstring_makeLiteralTemp ("->"), 
			      ltoken_getRawString (op->content.id));
	  break;
	case OPF_MSELECT:
	  {
	    int size = termNodeList_size (args);

	    if (size == 1)
	      {
		s = message ("%q.%s", termNode_unparse (termNodeList_head (args)),
			     ltoken_getRawString (op->content.id));
	      }
	    else
	      {
		WrongArity (op->content.id, 1, size);
		s = cstring_concat (cstring_makeLiteralTemp ("__."), 
				    ltoken_getRawString (op->content.id));
	      }
	    break;
	  }
	case OPF_MMAP:
	  {
	    int size = termNodeList_size (args);

	    if (size == 1)
	      {
		s = message ("%q->%s", termNode_unparse (termNodeList_head (args)),
			     ltoken_getRawString (op->content.id));
	      }
	    else
	      {
		WrongArity (op->content.id, 1, size);
		s = cstring_concat (cstring_makeLiteralTemp ("__->"), 
				    ltoken_getRawString (op->content.id));
	      }
	    break;
	  }
	}

      cstring_free (sortSpace);
      cstring_free (sortText);
      return s;
    }
  return cstring_undefined;
}

/*@only@*/ cstring
termNode_unparse (/*@null@*/ termNode n)
{
  cstring s = cstring_undefined;
  cstring back = cstring_undefined;
  cstring front = cstring_undefined;
  int count;

  if (n != (termNode) 0)
    {
      for (count = n->wrapped; count > 0; count--)
	{
	  front = cstring_appendChar (front, '(');
	  back = cstring_appendChar (back, ')');
	}

      switch (n->kind)
	{
	case TRM_LITERAL:
	case TRM_CONST:
	case TRM_VAR:
	case TRM_ZEROARY:
	  s = cstring_copy (ltoken_getRawString (n->literal));
	  break;
	case TRM_APPLICATION:
	  {
	    nameNode nn = n->name;
	    if (nn != (nameNode) 0)
	      {
		if (nn->isOpId)
		  {
		    s = message ("%s (%q) ",
				 ltoken_getRawString (nn->content.opid),
				 termNodeList_unparse (n->args));
		   /* must we handle n->given ? skip for now */
		  }
		else
		  {
		    s = message ("%q ", printTermNode2 (nn->content.opform, n->args, n->given));
		  }
	      }
	    else
	      {
		llfatalbug
		  (message ("termNode_unparse: expect non-empty nameNode: TRM_APPLICATION: %q",
			    nameNode_unparse (nn)));
	      }
	    break;
	  }
	case TRM_UNCHANGEDALL:
	  s = cstring_makeLiteral ("unchanged (all)");
	  break;
	case TRM_UNCHANGEDOTHERS:
	  s = message ("unchanged (%q)", storeRefNodeList_unparse (n->unchanged));
	  break;
	case TRM_SIZEOF:
	  s = message ("sizeof (%q)", lclTypeSpecNode_unparse (n->sizeofField));
	  break;
	case TRM_QUANTIFIER:
	  {
	    quantifiedTermNode x = n->quantified;
	    s = message ("%q%s%q%s",
			 quantifierNodeList_unparse (x->quantifiers),
			 ltoken_getRawString (x->open),
			 termNode_unparse (x->body),
			 ltoken_getRawString (x->close));
	    break;
	  }
	}
    }
  return (message ("%q%q%q", front, s, back));
}

static void modifyNode_free (/*@null@*/ /*@only@*/ modifyNode m)
{
  if (m != (modifyNode) 0)
    {
      
      if (m->hasStoreRefList)
	{
	  storeRefNodeList_free (m->list);
	  /*@-branchstate@*/ 
	} 
      /*@=branchstate@*/

      ltoken_free (m->tok);
      sfree (m);
    }
}

/*@only@*/ cstring
modifyNode_unparse (/*@null@*/ modifyNode m)
{
  if (m != (modifyNode) 0)
    {
      if (m->hasStoreRefList)
	{
	  return (message ("  modifies %q; \n", storeRefNodeList_unparse (m->list)));
	}
      else
	{
	  if (m->modifiesNothing)
	    {
	      return (cstring_makeLiteral ("modifies nothing; \n"));
	    }
	  else
	    {
	      return (cstring_makeLiteral ("modifies anything; \n"));
	    }
	}
    }
  return cstring_undefined;
}

/*@only@*/ cstring
programNode_unparse (programNode p)
{
  if (p != (programNode) 0)
    {
      cstring s = cstring_undefined;
      int count;

      switch (p->kind)
	{
	case ACT_SELF:
	  {
	    cstring back = cstring_undefined;
	    
	    for (count = p->wrapped; count > 0; count--)
	      {
		s = cstring_appendChar (s, '(');
		back = cstring_appendChar (back, ')');
	      }
	    s = message ("%q%q%q", s, stmtNode_unparse (p->content.self), back);
	    break;
	  }
	case ACT_ITER:
	  s = message ("*(%q)", programNodeList_unparse (p->content.args));
	  break;
	case ACT_ALTERNATE:
	  s = message ("|(%q)", programNodeList_unparse (p->content.args));
	  break;
	case ACT_SEQUENCE:
	  s = programNodeList_unparse (p->content.args);
	  break;
	}

      return s;
    }
  return cstring_undefined;
}

/*@only@*/ cstring
stmtNode_unparse (stmtNode x)
{
  cstring s = cstring_undefined;

  if (x != (stmtNode) 0)
    {
      if (ltoken_isValid (x->lhs))
	{
	  s = cstring_concat (ltoken_getRawString (x->lhs), 
			      cstring_makeLiteralTemp (" = "));
	}

      s = message ("%q%s (%q)", s,
		   ltoken_getRawString (x->operator),
		   termNodeList_unparse (x->args));
    }

  return s;
}

/*@only@*/ lslOp
  makelslOpNode (/*@only@*/ /*@null@*/ nameNode name, 
		       /*@dependent@*/ sigNode s)
{
  lslOp x = (lslOp) dmalloc (sizeof (*x));

  x->name = name;
  x->signature = s;

  /* enter operator info into symtab */
  /* if not, they may need to be renamed in LCL imports */

  if (g_lslParsingTraits)
    {
      if (name != NULL)
	{
	  symtable_enterOp (g_symtab, nameNode_copySafe (name), sigNode_copy (s));
	}
    }
  else
    {
            /* nameNode_free (name); */  /* YIKES! */
    }

  return x;
}

/*@only@*/ cstring
lslOp_unparse (lslOp x)
{
  char *s = mstring_createEmpty ();

  if (x != (lslOp) 0)
    {
      s = mstring_concatFree (s, cstring_toCharsSafe (nameNode_unparse (x->name)));

      if (x->signature != (sigNode) 0)
	{
	  s = mstring_concatFree (s, cstring_toCharsSafe (sigNode_unparse (x->signature)));
	}
    }

  return cstring_fromCharsO (s);
}

static bool
sameOpFormNode (/*@null@*/ opFormNode n1, /*@null@*/ opFormNode n2)
{
  if (n1 == n2)
    return TRUE;

  if (n1 == 0)
    return FALSE;

  if (n2 == 0)
    return FALSE;

  if (n1->kind == n2->kind)
    {
      switch (n1->kind)
	{
	case OPF_IF:
	  return TRUE;
	case OPF_ANYOP:
	case OPF_MANYOP:
	case OPF_ANYOPM:
	  return (ltoken_similar (n1->content.anyop, n2->content.anyop));
	case OPF_MANYOPM:
	  {
	    /* want to treat eq and = the same */
	    return ltoken_similar (n1->content.anyop, n2->content.anyop);
	  }
	case OPF_MIDDLE:
	case OPF_MMIDDLE:
	case OPF_MIDDLEM:
	case OPF_MMIDDLEM:
	  /* need to check the rawText of openSym and closeSym */
	  if ((int) n1->content.middle == (int) n2->content.middle)
	    {
	      if (lsymbol_equal (ltoken_getRawText (n1->tok),
				   ltoken_getRawText (n2->tok)) &&
		  lsymbol_equal (ltoken_getRawText (n1->close),
				   ltoken_getRawText (n2->close)))
		return TRUE;
	    }
	  return FALSE;
	case OPF_BMIDDLE:
	case OPF_BMMIDDLE:
	case OPF_BMIDDLEM:
	case OPF_BMMIDDLEM:
	  return ((int) n1->content.middle == (int) n2->content.middle);
	case OPF_SELECT:
	case OPF_MAP:
	case OPF_MSELECT:
	case OPF_MMAP:
	  return (ltoken_similar (n1->content.id, n2->content.id));
	}
    }
  return FALSE;
}

bool
sameNameNode (/*@null@*/ nameNode n1, /*@null@*/ nameNode n2)
{
  if (n1 == n2)
    return TRUE;
  if (n1 != (nameNode) 0 && n2 != (nameNode) 0)
    {
      if (bool_equal (n1->isOpId, n2->isOpId))
	{
	  if (n1->isOpId)
	    return (ltoken_similar (n1->content.opid, n2->content.opid));
	  else
	    return sameOpFormNode (n1->content.opform,
				   n2->content.opform);
	}
    }
  return FALSE;
}

void CTypesNode_free (/*@only@*/ /*@null@*/ CTypesNode x)
{
  if (x != NULL)
    {
      ltokenList_free (x->ctypes);
      sfree (x);
    }
}

/*@null@*/ CTypesNode CTypesNode_copy (/*@null@*/ CTypesNode x)
{
  if (x != NULL)
    {
      CTypesNode newnode = (CTypesNode) dmalloc (sizeof (*newnode));
      newnode->intfield = x->intfield;
      newnode->ctypes = ltokenList_copy (x->ctypes);
      newnode->sort = x->sort;
      
      return newnode;
    }
  else
    {
      return NULL;
    }
}  

/*@only@*/ CTypesNode
  makeCTypesNode (/*@only@*/ CTypesNode ctypes, ltoken ct)
{
  /*@only@*/ CTypesNode newnode;
  lsymbol sortname;
  bits sortbits;

  if (ctypes == (CTypesNode) NULL)
    {
      newnode = (CTypesNode) dmalloc (sizeof (*newnode));
      newnode->intfield = 0;
      newnode->ctypes = ltokenList_new ();
      newnode->sort = sort_makeNoSort ();
    }
  else
    {
      newnode = ctypes;
    }

  if ((ltoken_getIntField (ct) & newnode->intfield) != 0)
    {
      lclerror (ct,
		message
		("Duplicate type specifier ignored: %s",
		 cstring_fromChars 
		 (lsymbol_toChars
		  (lclctype_toSortDebug (ltoken_getIntField (ct))))));

      /* evs --- don't know how to generator this error */
     
      /* Use previous value, to keep things consistent  */
      ltoken_free (ct);
      return newnode;
    }

  sortbits = newnode->intfield | ltoken_getIntField (ct);
  sortname = lclctype_toSort (sortbits);

  if (sortname == lsymbol_fromChars ("error"))
    {
      lclerror (ct, cstring_makeLiteral ("Invalid combination of type specifiers"));
    }
  else
    {
      newnode->intfield = sortbits;
    }

  ltokenList_addh (newnode->ctypes, ct);
  
  /*
  ** Sorts are assigned after CTypesNode is created during parsing,
  ** see bison grammar. 
  */

  return newnode;
}

/*@only@*/ CTypesNode          
makeTypeSpecifier (ltoken typedefname)
{
  CTypesNode newnode = (CTypesNode) dmalloc (sizeof (*newnode));
  typeInfo ti = symtable_typeInfo (g_symtab, ltoken_getText (typedefname));

  newnode->intfield = 0;
  newnode->ctypes = ltokenList_singleton (ltoken_copy (typedefname));
  
  /* if we see "bool" include bool.h header file */

  if (ltoken_getText (typedefname) == lsymbol_bool)
    {
      lhIncludeBool ();
    }
  
  if (typeInfo_exists (ti))
    {
      /* must we be concern about whether this type is exported by module?
	 No.  Because all typedef's are exported.  No hiding supported. */
      /* Later, may want to keep types around too */
      /* 3/2/93, use underlying sort */
      newnode->sort = sort_getUnderlying (ti->basedOn);
    }
  else
    {
      lclerror (typedefname, message ("Unrecognized type: %s", 
				      ltoken_getRawString (typedefname)));
      /* evs --- Don't know how to get this message */
      
      newnode->sort = sort_makeNoSort ();
    }
  
  ltoken_free (typedefname);
  return newnode;
}

bool sigNode_equal (sigNode n1, sigNode n2)
{
 /* n1 and n2 are never 0 */

  return ((n1 == n2) ||
	  (n1->key == n2->key &&
	   ltoken_similar (n1->range, n2->range) &&
	   ltokenList_equal (n1->domain, n2->domain)));
}

sort
typeExpr2ptrSort (sort base, /*@null@*/ typeExpr t)
{
  if (t != (typeExpr) 0)
    {
      switch (t->kind)
	{
	case TEXPR_BASE:
	  return base;
	case TEXPR_PTR:
	  return typeExpr2ptrSort (sort_makePtr (ltoken_undefined, base),
				   t->content.pointer);
	case TEXPR_ARRAY:
	  return typeExpr2ptrSort (sort_makeArr (ltoken_undefined, base),
				   t->content.array.elementtype);
	case TEXPR_FCN:
	  /* map all hof types to some sort of SRT_HOF */
	  return sort_makeHOFSort (base);
	}
    }
  return base;
}

static sort
typeExpr2returnSort (sort base, /*@null@*/ typeExpr t)
{
  if (t != (typeExpr) 0)
    {
      switch (t->kind)
	{
	case TEXPR_BASE:
	  return base;
	case TEXPR_PTR:
	  return typeExpr2returnSort (sort_makePtr (ltoken_undefined, base),
				      t->content.pointer);
	case TEXPR_ARRAY:
	  return typeExpr2returnSort (sort_makeArr (ltoken_undefined, base),
				      t->content.array.elementtype);
	case TEXPR_FCN:
	  return typeExpr2returnSort (base, t->content.function.returntype);
	}
    }
  return base;
}

sort
lclTypeSpecNode2sort (lclTypeSpecNode type)
{
  if (type != (lclTypeSpecNode) 0)
    {
      switch (type->kind)
	{
	case LTS_TYPE:
	  llassert (type->content.type != NULL);
	  return sort_makePtrN (type->content.type->sort, type->pointers);
	case LTS_STRUCTUNION:
	  llassert (type->content.structorunion != NULL);
	  return sort_makePtrN (type->content.structorunion->sort, type->pointers);
	case LTS_ENUM:
	  llassert (type->content.enumspec != NULL);
	  return sort_makePtrN (type->content.enumspec->sort, type->pointers);
	case LTS_CONJ:
	  return (lclTypeSpecNode2sort (type->content.conj->a));
	}
    }
  return (sort_makeNoSort ());
}

lsymbol
checkAndEnterTag (tagKind k, ltoken opttagid)
{
  /* should be tagKind, instead of int */
  tagInfo t;
  sort sort = sort_makeNoSort ();
  
  if (!ltoken_isUndefined (opttagid))
    {
      switch (k)
	{
	case TAG_FWDSTRUCT:
	case TAG_STRUCT:
	  sort = sort_makeStr (opttagid);
	  break;
	case TAG_FWDUNION:
	case TAG_UNION:
	  sort = sort_makeUnion (opttagid);
	  break;
	case TAG_ENUM:
	  sort = sort_makeEnum (opttagid);
	  break;
	}      

      /* see if it is already in symbol table */
      t = symtable_tagInfo (g_symtab, ltoken_getText (opttagid));
      
      if (tagInfo_exists (t))
	{
	  if (t->kind == TAG_FWDUNION || t->kind == TAG_FWDSTRUCT)
	    {
	      /* this is fine, for mutually recursive types */
	    }
	  else
	    {			/* this is not good, complain later */
	      cstring s;

	      switch (k)
		{
		case TAG_ENUM:
		  s = cstring_makeLiteral ("Enum");
		  break;
		case TAG_STRUCT:
		case TAG_FWDSTRUCT:
		  s = cstring_makeLiteral ("Struct");
		  break;
		case TAG_UNION:
		case TAG_FWDUNION:
		  s = cstring_makeLiteral ("Union");
		  break;
		}

	      t->sort = sort;
	      t->kind = k;
	      lclerror (opttagid, 
			message ("Tag redefined: %q %s", s, 
				 ltoken_getRawString (opttagid)));
	      
	    }

	  ltoken_free (opttagid);
	}
      else
	{
   	  tagInfo newnode = (tagInfo) dmalloc (sizeof (*newnode));
      
	  newnode->sort = sort;
	  newnode->kind = k;
	  newnode->id = opttagid;
	  newnode->imported = FALSE;
	  newnode->content.decls = stDeclNodeList_new ();

	  (void) symtable_enterTag (g_symtab, newnode);
	}
    }

  return sort_getLsymbol (sort);
}

static sort
extractReturnSort (lclTypeSpecNode t, declaratorNode d)
{
  sort sort;
  sort = lclTypeSpecNode2sort (t);
  sort = typeExpr2returnSort (sort, d->type);
  return sort;
}

void
signNode_free (/*@only@*/ signNode sn)
{
  sortList_free (sn->domain);
  ltoken_free (sn->tok);
  sfree (sn);
}

/*@only@*/ cstring
signNode_unparse (signNode sn)
{
  cstring s = cstring_undefined;

  if (sn != (signNode) 0)
    {
      s = message (": %q -> %s", sortList_unparse (sn->domain),
		   sort_unparseName (sn->range));
    }
  return s;
}

static /*@only@*/ pairNodeList
  globalList_toPairNodeList (globalList g)
{
  /* expect list to be globals, drop private ones */
  pairNodeList result = pairNodeList_new ();
  pairNode p;
  declaratorNode vdnode;
  lclTypeSpecNode type;
  sort sort;
  lsymbol sym;
  initDeclNodeList decls;

  varDeclarationNodeList_elements (g, x)
  {
    if (x->isSpecial)
      {
	;
      }
    else
      {
	if (x->isGlobal && !x->isPrivate)
	  {
	    type = x->type;
	    decls = x->decls;
	    
	    initDeclNodeList_elements (decls, init)
	      {
		p = (pairNode) dmalloc (sizeof (*p));
		
		vdnode = init->declarator;
		sym = ltoken_getText (vdnode->id);
		/* 2/21/93, not sure if it should be extractReturnSort,
		   or some call to typeExpr2ptrSort */
		sort = extractReturnSort (type, vdnode);
		p->sort = sort_makeGlobal (sort);
		/*	if (!sort_isArrayKind (sort)) p->sort = sort_makeObj (sort);
			else p->sort = sort; */
		/*	p->name = sym; */
		p->tok = ltoken_copy (vdnode->id);
		pairNodeList_addh (result, p);
	      } end_initDeclNodeList_elements;
	  }
      }
  } end_varDeclarationNodeList_elements;
  return result;
}

void
enteringFcnScope (lclTypeSpecNode t, declaratorNode d, globalList g)
{
  scopeInfo si = (scopeInfo) dmalloc (sizeof (*si));
  varInfo vi = (varInfo) dmalloc (sizeof (*vi));
  sort returnSort;
  ltoken result = ltoken_copy (ltoken_id);
  pairNodeList paramPairs, globals;
  fctInfo fi    = (fctInfo) dmalloc (sizeof (*fi));
  signNode sign = (signNode) dmalloc (sizeof (*sign));
  sortList domain = sortList_new ();
  unsigned long int key;

  paramPairs = extractParams (d->type);
  returnSort = extractReturnSort (t, d);
  globals = globalList_toPairNodeList (g);

  sign->tok = ltoken_undefined;
  sign->range = returnSort;

  key = MASH (0, sort_getLsymbol (returnSort));

  pairNodeList_elements (paramPairs, p)
  {
    sortList_addh (domain, p->sort);
    key = MASH (key, sort_getLsymbol (p->sort));
  } end_pairNodeList_elements;

  sign->domain = domain;
  sign->key = key;

  /* push fcn onto symbol table stack first */
  fi->id = ltoken_copy (d->id);
  fi->export = TRUE;
  fi->signature = sign;
  fi->globals = globals;

  (void) symtable_enterFct (g_symtab, fi);

  /* push new fcn scope */
  si->kind = SPE_FCN;
  symtable_enterScope (g_symtab, si);

  /* add "result" with return type to current scope */
  ltoken_setText (result, lsymbol_fromChars ("result"));

  vi->id = result;
  vi->sort = sort_makeFormal (returnSort);	/* make appropriate values */
  vi->kind = VRK_PARAM;
  vi->export = TRUE;

  (void) symtable_enterVar (g_symtab, vi);

  /*
  ** evs - 4 Mar 1995 
  **   pust globals first (they are in outer scope)
  */

  /* push onto symbol table the global variables declared in this function,
     together with their respective sorts */

  pairNodeList_elements (globals, gl)
    {
      ltoken_free (vi->id);
      vi->id = ltoken_copy (gl->tok);
      vi->kind = VRK_GLOBAL;
      vi->sort = gl->sort;
      (void) symtable_enterVar (g_symtab, vi);
    } end_pairNodeList_elements;

  /*
  ** could enter a new scope; instead, warn when variable shadows global
  ** that is used
  */

  /*
  ** push onto symbol table the formal parameters of this function,
  ** together with their respective sorts 
  */

  pairNodeList_elements (paramPairs, pair)
    {
      ltoken_free (vi->id);
      vi->id = ltoken_copy (pair->tok);
      vi->sort = pair->sort;
      vi->kind = VRK_PARAM;
      (void) symtable_enterVar (g_symtab, vi);
    } end_pairNodeList_elements;

  pairNodeList_free (paramPairs);
  varInfo_free (vi);
}

void
enteringClaimScope (paramNodeList params, globalList g)
{
  scopeInfo si = (scopeInfo) dmalloc (sizeof (*si));
  pairNodeList globals;
  lclTypeSpecNode paramtype;
  typeExpr paramdecl;
  sort sort;

  globals = globalList_toPairNodeList (g);
  /* push new claim scope */
  si->kind = SPE_CLAIM;

  symtable_enterScope (g_symtab, si);
  
  /* push onto symbol table the formal parameters of this function,
     together with their respective sorts */
  
  paramNodeList_elements (params, param)
    {
      paramdecl = param->paramdecl;
      paramtype = param->type;
      if (paramdecl != (typeExpr) 0 && paramtype != (lclTypeSpecNode) 0)
	{
	  varInfo vi = (varInfo) dmalloc (sizeof (*vi));
	  
	  sort = lclTypeSpecNode2sort (paramtype);
	  sort = sort_makeFormal (sort);
	  vi->sort = typeExpr2ptrSort (sort, paramdecl);
	  vi->id = ltoken_copy (extractDeclarator (paramdecl));
	  vi->kind = VRK_PARAM;
	  vi->export = TRUE;

	  (void) symtable_enterVar (g_symtab, vi);
	  varInfo_free (vi);
	}
    } end_paramNodeList_elements;
  
  /* push onto symbol table the global variables declared in this function,
     together with their respective sorts */

  pairNodeList_elements (globals, g2)
    {
      varInfo vi = (varInfo) dmalloc (sizeof (*vi));
      
      vi->id = ltoken_copy (g2->tok);
      vi->kind = VRK_GLOBAL;
      vi->sort = g2->sort;
      vi->export = TRUE;

      /* should catch duplicates in formals */
      (void) symtable_enterVar (g_symtab, vi);	
      varInfo_free (vi);
    } end_pairNodeList_elements;

  pairNodeList_free (globals);
  /* should not free it here! ltoken_free (claimId); @*/
}

static /*@only@*/ pairNodeList
  extractParams (/*@null@*/ typeExpr te)
{
 /* extract the parameters from a function header declarator's typeExpr */
  sort sort;
  typeExpr paramdecl;
  paramNodeList params;
  lclTypeSpecNode paramtype;
  pairNodeList head = pairNodeList_new ();
  pairNode pair;

  if (te != (typeExpr) 0)
    {
      params = typeExpr_toParamNodeList (te);
      if (paramNodeList_isDefined (params))
	{
	  paramNodeList_elements (params, param)
	  {
	    paramdecl = param->paramdecl;
	    paramtype = param->type;
	    if (paramdecl != (typeExpr) 0 && paramtype != (lclTypeSpecNode) 0)
	      {
   		pair = (pairNode) dmalloc (sizeof (*pair));
		sort = lclTypeSpecNode2sort (paramtype);
		/* 2/17/93, was sort_makeVal (sort) */
		sort = sort_makeFormal (sort);
		pair->sort = typeExpr2ptrSort (sort, paramdecl);
		/* pair->name = ltoken_getText (extractDeclarator (paramdecl)); */
		pair->tok = ltoken_copy (extractDeclarator (paramdecl));
		pairNodeList_addh (head, pair);
	      }
	  } end_paramNodeList_elements;
	}
    }
  return head;
}

sort
sigNode_rangeSort (sigNode sig)
{
  if (sig == (sigNode) 0)
    {
      return sort_makeNoSort ();
    }
  else
    {
      return sort_fromLsymbol (ltoken_getText (sig->range));
    }
}

/*@only@*/ sortList
  sigNode_domain (sigNode sig)
{
  sortList domain = sortList_new ();

  if (sig == (sigNode) 0)
    {
      ;
    }
  else
    {
      ltokenList dom = sig->domain;

      ltokenList_elements (dom, tok)
      {
	sortList_addh (domain, sort_fromLsymbol (ltoken_getText (tok)));
      } end_ltokenList_elements;
    }

  return domain;
}

opFormUnion
opFormUnion_createAnyOp (/*@temp@*/ ltoken t)
{
  opFormUnion u;

  /* do not distinguish between .anyop and .id */
  u.anyop = t;
  return u;
}

opFormUnion
opFormUnion_createMiddle (int middle)
{
  opFormUnion u;
  
  u.middle = middle;
  return u;
}

paramNode
markYieldParamNode (paramNode p)
{
  p->kind = PYIELD;

  llassert (p->type != NULL);
  p->type->quals = qualList_add (p->type->quals, qual_createYield ());

    return (p);
}

/*@only@*/ lclTypeSpecNode
  lclTypeSpecNode_copySafe (lclTypeSpecNode n)
{
  lclTypeSpecNode ret = lclTypeSpecNode_copy (n);
  
  llassert (ret != NULL);
  return ret;
}

/*@null@*/ /*@only@*/ lclTypeSpecNode
  lclTypeSpecNode_copy (/*@null@*/ lclTypeSpecNode n)
{
  if (n != NULL)
    {
      switch (n->kind)
	{
	case LTS_CONJ:
	  return (makeLclTypeSpecNodeConj (lclTypeSpecNode_copy (n->content.conj->a),
					   lclTypeSpecNode_copy (n->content.conj->b)));
	case LTS_TYPE:
	  return (makeLclTypeSpecNodeType (CTypesNode_copy (n->content.type)));
	case LTS_STRUCTUNION:
	  return (makeLclTypeSpecNodeSU (strOrUnionNode_copy (n->content.structorunion)));
	case LTS_ENUM:
	  return (makeLclTypeSpecNodeEnum (enumSpecNode_copy (n->content.enumspec)));
	}
    }
  
  return NULL;
}

void lclTypeSpecNode_free (/*@null@*/ /*@only@*/ lclTypeSpecNode n)
{
  if (n != NULL)
    {
      switch (n->kind)
	{
	case LTS_CONJ:
	  lclTypeSpecNode_free (n->content.conj->a);
	  lclTypeSpecNode_free (n->content.conj->b);
	  break;
	case LTS_TYPE:
	  CTypesNode_free (n->content.type);
	  break;
	case LTS_STRUCTUNION:
	  strOrUnionNode_free (n->content.structorunion);
	  break;
	case LTS_ENUM:
	  enumSpecNode_free (n->content.enumspec);
	  break;
	}

      qualList_free (n->quals);
      sfree (n);
    }
}

static /*@null@*/ opFormNode opFormNode_copy (/*@null@*/ opFormNode op)
{
  if (op != NULL)
    {
      opFormNode ret = (opFormNode) dmalloc (sizeof (*ret));
      
      ret->tok = ltoken_copy (op->tok);
      ret->kind = op->kind;
      ret->content = op->content;
      ret->key = op->key;
      ret->close = ltoken_copy (op->close);
      
      return ret;
    }
  else
    {
      return NULL;
    }
}

void opFormNode_free (/*@null@*/ opFormNode op)
{
  if (op != NULL)
    {
      ltoken_free (op->tok);
      ltoken_free (op->close);
      sfree (op);
    }
}

void nameNode_free (nameNode n)
{
  
  if (n != NULL)
    {
      if (!n->isOpId)
	{
	  opFormNode_free (n->content.opform);
	}
      
      sfree (n);
    }
}

bool
lslOp_equal (lslOp x, lslOp y)
{
  return ((x == y) ||
	  ((x != 0) && (y != 0) &&
	   sameNameNode (x->name, y->name) &&
	   sigNode_equal (x->signature, y->signature)));
}

void lslOp_free (lslOp x)
{
  nameNode_free (x->name);
  sfree (x);
}

void sigNode_free (sigNode x)
{
  if (x != NULL)
    {
      ltokenList_free (x->domain);
      ltoken_free (x->tok);
      ltoken_free (x->range);
      sfree (x);
    }
}

void declaratorNode_free (/*@null@*/ /*@only@*/ declaratorNode x)
{
  if (x != NULL)
    {
      typeExpr_free (x->type);
      ltoken_free (x->id);
      sfree (x);
    }
}

void abstBodyNode_free (/*@null@*/ /*@only@*/ abstBodyNode n)
{
  if (n != NULL)
    {
      lclPredicateNode_free (n->typeinv);
      fcnNodeList_free (n->fcns);
      ltoken_free (n->tok);
      sfree (n);
    }
}

void fcnNode_free (/*@null@*/ /*@only@*/ fcnNode f)
{
  if (f != NULL)
    {
      lclTypeSpecNode_free (f->typespec);
      declaratorNode_free (f->declarator);
      globalList_free (f->globals);
      varDeclarationNodeList_free (f->inits);
      letDeclNodeList_free (f->lets);
      lclPredicateNode_free (f->checks);
      lclPredicateNode_free (f->require);
      lclPredicateNode_free (f->claim);
      lclPredicateNode_free (f->ensures);
      modifyNode_free (f->modify);
      ltoken_free (f->name);
      sfree (f);
    }
}

void declaratorInvNode_free (/*@null@*/ /*@only@*/ declaratorInvNode x)
{
  if (x != NULL)
    {
      declaratorNode_free (x->declarator);
      abstBodyNode_free (x->body);
      sfree (x);
    }
}

/*@only@*/ lslOp lslOp_copy (lslOp x)
{
  return (makelslOpNode (nameNode_copy (x->name), x->signature));
}

sigNode sigNode_copy (sigNode s)
{
  llassert (s != NULL);
  return (makesigNode (ltoken_copy (s->tok), 
			     ltokenList_copy (s->domain), 
			     ltoken_copy (s->range)));
}

/*@null@*/ nameNode nameNode_copy (/*@null@*/ nameNode n)
{
  if (n == NULL) return NULL;
  return nameNode_copySafe (n);
}

nameNode nameNode_copySafe (nameNode n)
{
  if (n->isOpId)
    {
      return (makeNameNodeId (ltoken_copy (n->content.opid)));
    }
  else
    {
      /* error should be detected by splint: forgot to copy opform! */
      return (makeNameNodeForm (opFormNode_copy (n->content.opform)));
    }
}

bool initDeclNode_isRedeclaration (initDeclNode d)
{
  return (d->declarator->isRedecl);
}

void termNode_free (/*@only@*/ /*@null@*/ termNode t)
{
  if (t != NULL) 
    {
      sortSet_free (t->possibleSorts);
      lslOpSet_free (t->possibleOps);
      nameNode_free (t->name);
      termNodeList_free (t->args);
      sfree (t);
    }
}

/*@only@*/ termNode termNode_copySafe (termNode t)
{
  termNode ret = termNode_copy (t);

  llassert (ret != NULL);
  return ret;
}

/*@null@*/ /*@only@*/ termNode termNode_copy (/*@null@*/ termNode t)
{
  if (t != NULL)
    {
      termNode ret = (termNode) dmalloc (sizeof (*ret));

      ret->wrapped = t->wrapped;
      ret->kind = t->kind;
      ret->sort = t->sort;
      ret->given = t->given;
      ret->possibleSorts = sortSet_copy (t->possibleSorts);
      ret->error_reported = t->error_reported;
      ret->possibleOps = lslOpSet_copy (t->possibleOps);
      ret->name = nameNode_copy (t->name);
      ret->args = termNodeList_copy (t->args);
      
      if (t->kind == TRM_LITERAL 
	  || t->kind == TRM_SIZEOF 
	  || t->kind == TRM_VAR
	  || t->kind == TRM_CONST 
	  || t->kind == TRM_ZEROARY)
	{
	  ret->literal = ltoken_copy (t->literal);
	}
      
      if (t->kind == TRM_UNCHANGEDOTHERS)
	{
	  ret->unchanged = storeRefNodeList_copy (t->unchanged);
	}
      
      if (t->kind == TRM_QUANTIFIER)
	{
	  ret->quantified = quantifiedTermNode_copy (t->quantified);
	}
      
      if (t->kind == TRM_SIZEOF)
	{
	  ret->sizeofField = lclTypeSpecNode_copySafe (t->sizeofField);
	}
  
      return ret;
    }
  else
    {

      return NULL;
    }
}

void importNode_free (/*@only@*/ /*@null@*/ importNode x)
{
  if (x != NULL) 
    {
      ltoken_free (x->val);
      sfree (x);
    }
}

void initDeclNode_free (/*@only@*/ /*@null@*/ initDeclNode x)
{
  if (x != NULL)
    {
      declaratorNode_free (x->declarator);
      termNode_free (x->value);
      sfree (x);
    }
}

void letDeclNode_free (/*@only@*/ /*@null@*/ letDeclNode x)
{
  if (x != NULL)
    {
      lclTypeSpecNode_free (x->sortspec);
      termNode_free (x->term);
      ltoken_free (x->varid);
      sfree (x);
    }
}

void pairNode_free (/*@only@*/ /*@null@*/ pairNode x)
{
  if (x != NULL) 
    {
      ltoken_free (x->tok);
      sfree (x);
    }
}

/*@null@*/ paramNode paramNode_copy (/*@null@*/ paramNode p)
{
  if (p != NULL)
    {
      paramNode ret = (paramNode) dmalloc (sizeof (*ret));

      ret->type = lclTypeSpecNode_copy (p->type);
      ret->paramdecl = typeExpr_copy (p->paramdecl);
      ret->kind = p->kind;
      return ret;
    }

  return NULL;
}

void paramNode_free (/*@only@*/ /*@null@*/ paramNode x)
{
  if (x != NULL)
    {
      lclTypeSpecNode_free (x->type);
      typeExpr_free (x->paramdecl);
      sfree (x);
    }
}

void programNode_free (/*@only@*/ /*@null@*/ programNode x)
{
  if (x != NULL)
    {
      switch (x->kind)
	{
	case ACT_SELF: stmtNode_free (x->content.self); break;
	case ACT_ITER:
	case ACT_ALTERNATE:
	case ACT_SEQUENCE: programNodeList_free (x->content.args); break;
	BADDEFAULT;
	}
      sfree (x);
    }
}

quantifierNode quantifierNode_copy (quantifierNode x)
{
  quantifierNode ret = (quantifierNode) dmalloc (sizeof (*ret));
  
  ret->quant = ltoken_copy (x->quant);
  ret->vars = varNodeList_copy (x->vars);
  ret->isForall = x->isForall;
  
  return ret;
}

void quantifierNode_free (/*@null@*/ /*@only@*/ quantifierNode x)
{
  if (x != NULL)
    {
      varNodeList_free (x->vars);
      ltoken_free (x->quant);
      sfree (x);
    }
}

void replaceNode_free (/*@only@*/ /*@null@*/ replaceNode x)
{
  if (x != NULL)
    {
      if (x->isCType)
	{
	  ;
	}
      else
	{
	  nameNode_free (x->content.renamesortname.name);
	  sigNode_free (x->content.renamesortname.signature);
	}

      typeNameNode_free (x->typename);
      ltoken_free (x->tok);
      sfree (x);
    }
}

storeRefNode storeRefNode_copy (storeRefNode x)
{
  storeRefNode ret = (storeRefNode) dmalloc (sizeof (*ret));

  ret->kind = x->kind;

  switch (x->kind)
    {
    case SRN_TERM:
      ret->content.term = termNode_copySafe (x->content.term); 
      break;
    case SRN_OBJ: case SRN_TYPE:
      ret->content.type = lclTypeSpecNode_copy (x->content.type);
      break;
    case SRN_SPECIAL:
      ret->content.ref = sRef_copy (x->content.ref);
      break;
    }

  return ret;
}

void storeRefNode_free (/*@only@*/ /*@null@*/ storeRefNode x)
{
  if (x != NULL)
    {
      if (storeRefNode_isTerm (x))
	{
	  termNode_free (x->content.term);
	}
      else if (storeRefNode_isType (x) || storeRefNode_isObj (x))
	{
	  lclTypeSpecNode_free (x->content.type);
	}
      else
	{
	  /* nothing to free */
	}

      sfree (x);
    }
}

stDeclNode stDeclNode_copy (stDeclNode x)
{
  stDeclNode ret = (stDeclNode) dmalloc (sizeof (*ret));
  
  ret->lcltypespec = lclTypeSpecNode_copySafe (x->lcltypespec);
  ret->declarators = declaratorNodeList_copy (x->declarators);
  
  return ret;
}

void stDeclNode_free (/*@only@*/ /*@null@*/ stDeclNode x)
{
  if (x != NULL)
    {
      lclTypeSpecNode_free (x->lcltypespec);
      declaratorNodeList_free (x->declarators);
      sfree (x);
    }
}

void traitRefNode_free (/*@only@*/ /*@null@*/ traitRefNode x)
{
  if (x != NULL)
    {
      ltokenList_free (x->traitid);
      renamingNode_free (x->rename);
      sfree (x);
    }
}

void typeNameNode_free (/*@only@*/ /*@null@*/ typeNameNode n)
{
  if (n != NULL)
    {
      typeNamePack_free (n->typename);
      opFormNode_free (n->opform);
      sfree (n);
    }
}

void varDeclarationNode_free (/*@only@*/ /*@null@*/ varDeclarationNode x)
{
  if (x != NULL)
    {
      if (x->isSpecial)
	{
	  ;
	}
      else
	{
	  lclTypeSpecNode_free (x->type);
	  initDeclNodeList_free (x->decls);
	  sfree (x);
	}
    }
}

varNode varNode_copy (varNode x)
{
  varNode ret = (varNode) dmalloc (sizeof (*ret));

  ret->varid = ltoken_copy (x->varid);
  ret->isObj = x->isObj;
  ret->type = lclTypeSpecNode_copySafe (x->type);
  ret->sort = x->sort;
  
  return ret;
}

void varNode_free (/*@only@*/ /*@null@*/ varNode x)
{
  if (x != NULL)
    {
      lclTypeSpecNode_free (x->type);
      ltoken_free (x->varid);
      sfree (x);
    }
}

void stmtNode_free (/*@only@*/ /*@null@*/ stmtNode x)
{
  if (x != NULL)
    {
      ltoken_free (x->lhs);
      termNodeList_free (x->args);
      ltoken_free (x->operator);
      sfree (x);
    }
}

void renamingNode_free (/*@only@*/ /*@null@*/ renamingNode x)
{
  if (x != NULL)
    {
      if (x->is_replace)
	{
	  replaceNodeList_free (x->content.replace);
	}
      else
	{
	  nameAndReplaceNode_free (x->content.name);
	}

      sfree (x);
    }
}

void nameAndReplaceNode_free (/*@only@*/ /*@null@*/ nameAndReplaceNode x)
{
  if (x != NULL)
    {
      typeNameNodeList_free (x->namelist);
      replaceNodeList_free (x->replacelist);
      sfree (x);
    }
}

void typeNamePack_free (/*@only@*/ /*@null@*/ typeNamePack x)
{
  if (x != NULL)
    {
      lclTypeSpecNode_free (x->type);
      abstDeclaratorNode_free (x->abst);
      sfree (x);
    }
}

cstring interfaceNode_unparse (interfaceNode x)
{
  if (x != NULL)
    {
      switch (x->kind)
	{
	case INF_IMPORTS:
	  return (message ("[imports] %q", importNodeList_unparse (x->content.imports)));
	case INF_USES:   
	  return (message ("[uses] %q", traitRefNodeList_unparse (x->content.uses)));
	case INF_EXPORT: 
	  return (message ("[export] %q", exportNode_unparse (x->content.export)));
	case INF_PRIVATE: 
	  return (message ("[private] %q", privateNode_unparse (x->content.private)));
	}

      BADBRANCH;
    }
  else
    {
      return (cstring_makeLiteral ("<interface node undefined>"));
    }

  BADBRANCHRET (cstring_undefined);
}

void interfaceNode_free (/*@null@*/ /*@only@*/ interfaceNode x)
{
  if (x != NULL)
    {
      
      switch (x->kind)
	{
	case INF_IMPORTS: importNodeList_free (x->content.imports); break;
	case INF_USES:    traitRefNodeList_free (x->content.uses); break;
	case INF_EXPORT:  exportNode_free (x->content.export); break;
	case INF_PRIVATE: privateNode_free (x->content.private); break;
	}
      sfree (x);
    }
}

void exportNode_free (/*@null@*/ /*@only@*/ exportNode x)
{
  if (x != NULL)
    {
      switch (x->kind)
	{
	case XPK_CONST: constDeclarationNode_free (x->content.constdeclaration); break;
	case XPK_VAR:   varDeclarationNode_free (x->content.vardeclaration); break;
	case XPK_TYPE: typeNode_free (x->content.type); break;
	case XPK_FCN:  fcnNode_free (x->content.fcn); break;
	case XPK_CLAIM: claimNode_free (x->content.claim); break;
	case XPK_ITER: iterNode_free (x->content.iter); break;
	}

      sfree (x);
    }
}

void privateNode_free (/*@null@*/ /*@only@*/ privateNode x)
{
  if (x != NULL)
    {
      switch (x->kind)
	{
	case PRIV_CONST:
	  constDeclarationNode_free (x->content.constdeclaration); break;
	case PRIV_VAR: 
	  varDeclarationNode_free (x->content.vardeclaration); break;
	case PRIV_TYPE: 
	  typeNode_free (x->content.type); break;
	case PRIV_FUNCTION:
	  fcnNode_free (x->content.fcn); break;
	}

      sfree (x);
    }
}

void constDeclarationNode_free (/*@only@*/ /*@null@*/ constDeclarationNode x)
{
  if (x != NULL)
    {
      lclTypeSpecNode_free (x->type);
      initDeclNodeList_free (x->decls);
      sfree (x);
    }
}

void typeNode_free (/*@only@*/ /*@null@*/ typeNode t)
{
  if (t != NULL)
    {
      switch (t->kind)
	{
	case TK_ABSTRACT: abstractNode_free (t->content.abstract); break;
	case TK_EXPOSED:  exposedNode_free (t->content.exposed); break;
	case TK_UNION: taggedUnionNode_free (t->content.taggedunion); break;
	}

      sfree (t);
    }
}

void claimNode_free (/*@only@*/ /*@null@*/ claimNode x)
{
  if (x != NULL)
    {
      paramNodeList_free (x->params);
      globalList_free (x->globals);
      letDeclNodeList_free (x->lets);
      lclPredicateNode_free (x->require);
      programNode_free (x->body);
      lclPredicateNode_free (x->ensures);
      ltoken_free (x->name);
      sfree (x);
    }
}

void iterNode_free (/*@only@*/ /*@null@*/ iterNode x)
{
  if (x != NULL)
    {
      paramNodeList_free (x->params);
      ltoken_free (x->name);
      sfree (x);
    }
}

void abstractNode_free (/*@only@*/ /*@null@*/ abstractNode x)
{
  if (x != NULL)
    {
      abstBodyNode_free (x->body);
      ltoken_free (x->tok);
      ltoken_free (x->name);
      sfree (x);
    }
}

void exposedNode_free (/*@only@*/ /*@null@*/ exposedNode x)
{
  if (x != NULL)
    {
      lclTypeSpecNode_free (x->type);
      declaratorInvNodeList_free (x->decls);
      ltoken_free (x->tok);
      sfree (x);
    }
}

void taggedUnionNode_free (/*@only@*/ /*@null@*/ taggedUnionNode x)
{
  if (x != NULL)
    {
      stDeclNodeList_free (x->structdecls);
      declaratorNode_free (x->declarator);
      sfree (x);
    }
}

/*@only@*/ /*@null@*/ strOrUnionNode 
  strOrUnionNode_copy (/*@null@*/ strOrUnionNode n)
{
  if (n != NULL)
    {
      strOrUnionNode ret = (strOrUnionNode) dmalloc (sizeof (*ret));

      ret->kind = n->kind;
      ret->tok = ltoken_copy (n->tok);
      ret->opttagid = ltoken_copy (n->opttagid);
      ret->sort = n->sort;
      ret->structdecls = stDeclNodeList_copy (n->structdecls);

      return ret;
    }
  else
    {
      return NULL;
    }
}

void strOrUnionNode_free (/*@null@*/ /*@only@*/ strOrUnionNode n)
{
  if (n != NULL)
    {
      stDeclNodeList_free (n->structdecls);
      ltoken_free (n->tok);
      ltoken_free (n->opttagid);
      sfree (n);
    }
}

void enumSpecNode_free (/*@null@*/ /*@only@*/ enumSpecNode x)
{
  if (x != NULL)
    {
      ltokenList_free (x->enums);
      ltoken_free (x->tok);
      ltoken_free (x->opttagid);
      sfree (x);
    }
}

/*@only@*/ /*@null@*/ enumSpecNode enumSpecNode_copy (/*@null@*/ enumSpecNode x)
{
  if (x != NULL)
    {
      enumSpecNode ret = (enumSpecNode) dmalloc (sizeof (*ret));

      ret->tok = ltoken_copy (x->tok);
      ret->opttagid = ltoken_copy (x->opttagid);
      ret->enums = ltokenList_copy (x->enums);
      ret->sort = x->sort;

      return ret;
    }
  else
    {
      return NULL;
    }
}

void lsymbol_setbool (lsymbol s)
{
  lsymbol_bool = s;
}

lsymbol lsymbol_getbool ()
{
  return lsymbol_bool;
}

lsymbol lsymbol_getBool ()
{
  return lsymbol_Bool;
}

lsymbol lsymbol_getFALSE ()
{
  return lsymbol_FALSE;
}

lsymbol lsymbol_getTRUE ()
{
  return lsymbol_TRUE;
}


