/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef ABSTRACT_H
# define ABSTRACT_H

/*
**  These two are needed in symtable.c
*/

/*@constant int HT_MAXINDEX; @*/
# define HT_MAXINDEX 255 

/* simply use the lower-order bits by masking out the higher order bits */

# include "ltoken.h"
# include "ltokenList.h" 

typedef enum 
{
  TAG_ENUM, TAG_STRUCT, TAG_UNION, TAG_FWDSTRUCT, TAG_FWDUNION
} tagKind;

# include "importNode.h"
# include "importNodeList.h" 

extern void checkBrackets (ltoken p_lb, ltoken p_rb);

# include "sortList.h"
# include "lsymbolList.h"

# include "lsymbolSet.h"
# include "sortSet.h"

/*
** added pointer indirects to all typedefs, except as noted
** evs 94-01-05
*/

# include "pairNode.h"
# include "pairNodeList.h"

# include "declaratorNode.h"
# include "declaratorNodeList.h"

# include "declaratorInvNode.h"
# include "declaratorInvNodeList.h"

# include "typeExpr.h"  /* also defines abstDeclaratorNode */

# include "arrayQualNode.h"

# include "varNode.h"
# include "varNodeList.h"

# include "quantifierNode.h"
# include "quantifierNodeList.h"

# include "storeRefNode.h"
# include "storeRefNodeList.h"

# include "modifyNode.h"

# include "letDeclNode.h"
# include "letDeclNodeList.h"

# include "programNode.h"
# include "programNodeList.h"

# include "lclPredicateNode.h"
# include "exposedNode.h"

typedef enum {
  TK_ABSTRACT, TK_EXPOSED, TK_UNION
  } typeKind ;

# include "CTypesNode.h"

# include "initDeclNode.h"
# include "initDeclNodeList.h"

# include "constDeclarationNode.h"

typedef enum {
  QLF_NONE, QLF_CONST, QLF_VOLATILE
  } qualifierKind;

# include "varDeclarationNode.h"
# include "varDeclarationNodeList.h"

# include "globalList.h"

# include "claimNode.h"

# include "fcnNode.h"
# include "fcnNodeList.h"

# include "iterNode.h"

# include "abstBodyNode.h"
# include "abstractNode.h"

# include "stDeclNode.h"
# include "stDeclNodeList.h"

# include "taggedUnionNode.h"
# include "typeNode.h"

# include "strOrUnionNode.h"
# include "enumSpecNode.h"

# include "lclTypeSpecNode.h"
# include "typeNamePack.h"

# include "typeNameNode.h"
# include "typeNameNodeList.h"              /* this is a list of typeNameNode's */

# include "opFormNode.h"

# include "quantifiedTermNode.h"

typedef enum {
  TRM_LITERAL, TRM_CONST, TRM_VAR, 
  TRM_ZEROARY, TRM_APPLICATION, TRM_QUANTIFIER,
  TRM_UNCHANGEDALL, TRM_UNCHANGEDOTHERS, 
  TRM_SIZEOF
  } termKIND;

# include "sigNode.h"
# include "sigNodeSet.h"

# include "signNode.h"
# include "nameNode.h"

# include "lslOp.h"
# include "lslOpSet.h"

# include "replaceNode.h"
# include "replaceNodeList.h"

# include "renamingNode.h"

# include "traitRefNode.h"
# include "traitRefNodeList.h"

# include "exportNode.h"
# include "privateNode.h"

# include "interfaceNode.h"
# include "interfaceNodeList.h" /* note: interfaceList --> interfaceNodeList */

# include "termNode.h"
# include "termNodeList.h"
# include "stmtNode.h"

/* The following are for parsing LSL signatures */

# include "sortSetList.h"
# include "lslOpList.h"

/* function prototypes for parsing LSL signatures */

extern /*@only@*/ lslOp
  makelslOpNode (/*@only@*/ /*@null@*/ nameNode p_name,
		 /*@dependent@*/ sigNode p_s);

extern /*@only@*/ cstring lslOp_unparse (lslOp p_x);

/*@notfunction@*/
# define MASH(x,y) \
  (/*@+enumint@*/ (((unsigned) ((x)+1) << 1) + (y)) & HT_MAXINDEX /*@=enumint@*/) 

extern void abstract_init (void);
extern void resetImports (cstring p_current) ;

extern interfaceNodeList 
  consInterfaceNode (/*@only@*/ interfaceNode p_n, /*@returned@*/ interfaceNodeList p_ns);

/* evs 8 Sept 1993       changed to importNodeList */
extern /*@only@*/ interfaceNode 
  makeInterfaceNodeImports (/*@only@*/ importNodeList p_x);

extern /*@only@*/ nameNode 
  makeNameNodeForm (/*@only@*/ /*@null@*/ opFormNode p_opform) /*@*/ ;
extern /*@only@*/ nameNode
  makeNameNodeId (/*@only@*/ ltoken p_opid) /*@*/ ;
extern /*@only@*/ interfaceNode 
  makeInterfaceNodeUses (/*@only@*/ traitRefNodeList p_x) /*@*/ ;
extern /*@only@*/ interfaceNode 
  interfaceNode_makeConst (/*@only@*/ constDeclarationNode p_x) /*@*/ ;
extern /*@only@*/ interfaceNode 
  interfaceNode_makeVar (/*@only@*/ varDeclarationNode p_x) /*@*/ ;
extern /*@only@*/ interfaceNode 
  interfaceNode_makeType (/*@only@*/ typeNode p_x) /*@*/ ;
extern /*@only@*/ interfaceNode 
  interfaceNode_makeFcn (/*@only@*/ fcnNode p_x) /*@*/ ;
extern /*@only@*/ interfaceNode 
  interfaceNode_makeClaim (/*@only@*/ claimNode p_x) /*@*/ ;
extern /*@only@*/ interfaceNode interfaceNode_makeIter (/*@only@*/ iterNode p_x) /*@*/ ;
extern /*@only@*/ interfaceNode interfaceNode_makePrivConst(/*@only@*/ constDeclarationNode p_x) /*@*/ ;
extern /*@only@*/ interfaceNode 
  interfaceNode_makePrivVar(/*@only@*/ varDeclarationNode p_x) /*@*/ ;
extern /*@only@*/ interfaceNode 
  interfaceNode_makePrivType(/*@only@*/ typeNode p_x) /*@*/ ;
extern /*@only@*/ interfaceNode 
  interfaceNode_makePrivFcn(/*@only@*/ fcnNode p_x) /*@*/ ;
extern /*@only@*/ typeNode makeAbstractTypeNode (/*@only@*/ abstractNode p_x) /*@*/ ;
extern /*@only@*/ typeNode makeExposedTypeNode (/*@only@*/ exposedNode p_x) /*@*/ ;

extern /*@only@*/ traitRefNode 
  makeTraitRefNode(/*@only@*/ ltokenList p_fl, /*@null@*/ /*@only@*/ renamingNode p_r) /*@*/ ;

extern /*@only@*/ cstring printLeaves2 (ltokenList p_f) /*@*/ ;
extern /*@only@*/ cstring printRawLeaves2 (ltokenList p_f) /*@*/ ;
extern /*@only@*/ cstring sigNode_unparseText (/*@null@*/ sigNode p_n) /*@*/ ;

extern /*@only@*/ renamingNode 
  makeRenamingNode (/*@only@*/ typeNameNodeList p_n, 
		    /*@only@*/ replaceNodeList p_r) /*@*/ ; 
extern /*@only@*/ replaceNode 
  makeReplaceNode (/*@only@*/ ltoken p_t, /*@only@*/ typeNameNode p_tn, bool p_is_ctype, 
		   /*@only@*/ ltoken p_ct, 
		   /*@null@*/ /*@only@*/ nameNode p_nn, 
		   /*@null@*/ /*@only@*/ sigNode p_sn) /*@*/ ;

extern /*@only@*/ sigNode 
  makesigNode (/*@only@*/ ltoken p_t, /*@only@*/ ltokenList p_domain, /*@only@*/ ltoken p_range) /*@*/ ;

extern /*@only@*/ replaceNode 
  makeReplaceNameNode (/*@only@*/ ltoken p_t, /*@only@*/ typeNameNode p_tn, 
		       /*@only@*/ nameNode p_nn) /*@*/ ;

extern /*@only@*/ opFormNode 
  makeOpFormNode(/*@only@*/ ltoken p_t, opFormKind p_k, 
		 opFormUnion p_u, /*@only@*/ ltoken p_close) /*@*/ ;

extern /*@only@*/ typeNameNode 
  makeTypeNameNode (bool p_isObj, /*@only@*/ lclTypeSpecNode p_t, 
		    /*@only@*/ abstDeclaratorNode p_n) /*@*/ ;
extern /*@only@*/ typeNameNode 
  makeTypeNameNodeOp (/*@only@*/ opFormNode p_n) /*@*/ ;

extern /*@only@*/ lclTypeSpecNode 
  makeLclTypeSpecNodeConj (/*@only@*/ /*@null@*/ lclTypeSpecNode p_a, 
			   /*@only@*/ /*@null@*/ lclTypeSpecNode p_b) /*@*/ ;

extern /*@only@*/ lclTypeSpecNode
  makeLclTypeSpecNodeType(/*@only@*/ /*@null@*/ CTypesNode p_x) /*@*/ ;

extern /*@only@*/ lclTypeSpecNode 
  makeLclTypeSpecNodeSU(/*@only@*/ /*@null@*/ strOrUnionNode p_x) /*@*/ ;

extern /*@only@*/ lclTypeSpecNode 
  makeLclTypeSpecNodeEnum(/*@only@*/ /*@null@*/ enumSpecNode p_x) /*@*/ ;

extern /*@only@*/ lclTypeSpecNode 
  lclTypeSpecNode_addQual (/*@only@*/ lclTypeSpecNode p_n, qual p_q) 
  /*@modifies p_n@*/ ;

extern /*@only@*/ enumSpecNode 
  makeEnumSpecNode (/*@only@*/ ltoken p_t, /*@only@*/ ltoken p_optTagId, /*@owned@*/ ltokenList p_enums);

extern /*@only@*/ enumSpecNode 
  makeEnumSpecNode2 (/*@only@*/ ltoken p_t, /*@only@*/ ltoken p_tagid);

extern /*@only@*/ strOrUnionNode 
  makestrOrUnionNode (/*@only@*/ ltoken p_str, suKind p_k,
			 /*@only@*/ ltoken p_opttagid, /*@only@*/ stDeclNodeList p_x);

extern /*@only@*/ strOrUnionNode 
  makeForwardstrOrUnionNode (/*@only@*/ ltoken p_str, suKind p_k, 
				/*@only@*/ ltoken p_tagid);

extern /*@only@*/ stDeclNode 
  makestDeclNode (/*@only@*/ lclTypeSpecNode p_s, 
		  /*@only@*/ declaratorNodeList p_x);
extern /*@only@*/ constDeclarationNode 
  makeConstDeclarationNode (/*@only@*/ lclTypeSpecNode p_t,
			    /*@only@*/ initDeclNodeList p_decls);
extern /*@only@*/ varDeclarationNode 
  makeVarDeclarationNode (/*@only@*/ lclTypeSpecNode p_t, 
			  /*@only@*/ initDeclNodeList p_x, 
			  bool p_isGlobal, bool p_isPrivate);

extern varDeclarationNode makeFileSystemNode (void);
extern varDeclarationNode makeInternalStateNode (void);

extern /*@only@*/ initDeclNode 
  makeInitDeclNode (/*@only@*/ declaratorNode p_d, /*@null@*/ /*@only@*/ termNode p_x);

extern /*@only@*/ abstractNode 
  makeAbstractNode (/*@only@*/ ltoken p_t, /*@only@*/ ltoken p_name,
		    bool p_isMutable, bool p_isRefCounted,
		    /*@only@*/ abstBodyNode p_a);

extern /*@unused@*/ /*@only@*/ cstring abstBodyNode_unparseExposed (abstBodyNode p_n);

extern /*@only@*/ exposedNode 
  makeExposedNode (/*@only@*/ ltoken p_t, /*@only@*/ lclTypeSpecNode p_s, 
		   /*@only@*/ declaratorInvNodeList p_d);

extern /*@only@*/ declaratorInvNode 
  makeDeclaratorInvNode (/*@only@*/ declaratorNode p_d, 
			 /*@only@*/ abstBodyNode p_b);

extern /*@only@*/ fcnNode
  fcnNode_fromDeclarator (/*@only@*/ /*@null@*/ lclTypeSpecNode p_t, 
			  /*@only@*/ declaratorNode p_d);

extern /*@only@*/ fcnNode
  makeFcnNode (qual p_specQual,
	       /*@only@*/ /*@null@*/ lclTypeSpecNode p_t, 
	       /*@only@*/ declaratorNode p_d,
	       /*@only@*/ /*@null@*/ globalList p_g, 
	       /*@only@*/ /*@null@*/ varDeclarationNodeList p_privateinits,
	       /*@only@*/ /*@null@*/ letDeclNodeList p_lets,
	       /*@only@*/ /*@null@*/ lclPredicateNode p_checks,
	       /*@only@*/ /*@null@*/ lclPredicateNode p_requires, 
	       /*@only@*/ /*@null@*/ modifyNode p_m,
	       /*@only@*/ /*@null@*/ lclPredicateNode p_ensures, 
	       /*@only@*/ /*@null@*/ lclPredicateNode p_claims);

extern /*@only@*/ iterNode 
  makeIterNode (/*@only@*/ ltoken p_id, /*@only@*/ paramNodeList p_p);

extern /*@only@*/ claimNode 
  makeClaimNode (/*@only@*/ ltoken p_id, 
		 /*@only@*/ paramNodeList p_p, 
		 /*@only@*/ /*@null@*/ globalList p_g, 
		 /*@only@*/ /*@null@*/ letDeclNodeList p_lets, 
		 /*@only@*/ /*@null@*/ lclPredicateNode p_requires, 
		 /*@only@*/ /*@null@*/ programNode p_b, 
		 /*@only@*/ /*@null@*/ lclPredicateNode p_ensures);

extern /*@only@*/ lclPredicateNode 
  makeIntraClaimNode (/*@only@*/ ltoken p_t, /*@only@*/ lclPredicateNode p_n);

extern /*@only@*/ lclPredicateNode 
  makeRequiresNode (/*@only@*/ ltoken p_t, /*@only@*/ lclPredicateNode p_n);

extern /*@only@*/ lclPredicateNode 
  makeChecksNode (/*@only@*/ ltoken p_t, /*@only@*/ lclPredicateNode p_n);

extern /*@only@*/ lclPredicateNode 
  makeEnsuresNode (/*@only@*/ ltoken p_t, /*@only@*/ lclPredicateNode p_n);

extern /*@only@*/ lclPredicateNode 
  makeLclPredicateNode (/*@only@*/ ltoken p_t, /*@only@*/ termNode p_n, 
			lclPredicateKind p_k);

extern /*@only@*/ stmtNode
  makeStmtNode (/*@only@*/ ltoken p_varId, 
		/*@only@*/ ltoken p_fcnId, /*@only@*/ termNodeList p_v);

extern /*@only@*/ programNode 
  makeProgramNodeAction (/*@only@*/ programNodeList p_x, actionKind p_k);

extern /*@only@*/ programNode 
  makeProgramNode (/*@only@*/ stmtNode p_x);

extern /*@only@*/ storeRefNode 
  makeStoreRefNodeTerm (/*@only@*/ termNode p_t);

extern /*@only@*/ storeRefNode 
  makeStoreRefNodeType (/*@only@*/ lclTypeSpecNode p_t, bool p_isObj);

extern /*@only@*/ modifyNode 
  makeModifyNodeSpecial (/*@only@*/ ltoken p_t, bool p_modifiesNothing);

extern storeRefNode makeStoreRefNodeInternal (void);
extern storeRefNode makeStoreRefNodeSystem (void);

extern /*@only@*/ modifyNode 
  makeModifyNodeRef (/*@only@*/ ltoken p_t, /*@only@*/ storeRefNodeList p_y);

extern /*@only@*/ letDeclNode 
  makeLetDeclNode(/*@only@*/ ltoken p_varid, /*@null@*/ /*@only@*/ lclTypeSpecNode p_t, 
		  /*@only@*/ termNode p_term);

extern /*@only@*/ abstBodyNode 
  makeAbstBodyNode (/*@only@*/ ltoken p_t, /*@only@*/ fcnNodeList p_f);

extern /*@only@*/ abstBodyNode 
  makeExposedBodyNode (/*@only@*/ ltoken p_t, /*@only@*/ lclPredicateNode p_inv);

extern /*@only@*/ abstBodyNode 
  makeAbstBodyNode2 (/*@only@*/ ltoken p_t, /*@only@*/ ltokenList p_ops); 

extern paramNode markYieldParamNode (/*@returned@*/ paramNode p_p);

extern /*@only@*/ arrayQualNode 
  makeArrayQualNode (/*@only@*/ ltoken p_t, /*@null@*/ /*@only@*/ termNode p_term);

extern /*@only@*/ quantifierNode 
  makeQuantifierNode (/*@only@*/ varNodeList p_v, /*@only@*/ ltoken p_quant);

extern /*@only@*/ varNode 
  makeVarNode (/*@only@*/ ltoken p_varid, bool p_isObj, /*@only@*/ lclTypeSpecNode p_t);

extern /*@only@*/ typeExpr makeTypeExpr (/*@only@*/ ltoken p_t);

extern /*@only@*/ declaratorNode 
  makeDeclaratorNode (/*@only@*/ typeExpr p_t);

extern /*@only@*/ typeExpr 
  makeFunctionNode (/*@null@*/ /*@only@*/ typeExpr p_x, /*@only@*/ paramNodeList p_p);

extern /*@only@*/ typeExpr
  makePointerNode (/*@only@*/ ltoken p_star, /*@null@*/ /*@only@*/ /*@returned@*/ typeExpr p_x);

extern /*@only@*/ typeExpr 
  makeArrayNode (/*@only@*/ /*@returned@*/ /*@null@*/ typeExpr p_x, 
		 /*@only@*/ arrayQualNode p_a);

extern /*@only@*/ paramNode 
  makeParamNode (/*@only@*/ lclTypeSpecNode p_t, /*@only@*/ typeExpr p_d);

extern /*@only@*/ termNode 
  makeIfTermNode (/*@only@*/ ltoken p_ift, /*@only@*/ termNode p_ifn, 
		  /*@only@*/ ltoken p_thent, /*@only@*/ termNode p_thenn, 
		  /*@only@*/ ltoken p_elset, /*@only@*/ termNode p_elsen);

extern /*@only@*/ termNode 
  makeQuantifiedTermNode (/*@only@*/ quantifierNodeList p_qn, 
			  /*@only@*/ ltoken p_open, 
			  /*@only@*/ termNode p_t, /*@only@*/ ltoken p_close);

extern /*@only@*/ termNode 
  makeInfixTermNode (/*@only@*/ termNode p_x, /*@only@*/ ltoken p_op, 
		     /*@only@*/ termNode p_y);

extern /*@only@*/ termNode 
  makePostfixTermNode (/*@returned@*/ /*@only@*/ termNode p_secondary,
		       /*@only@*/ ltokenList p_postfixops);

extern /*@only@*/ termNode 
  makePostfixTermNode2 (/*@only@*/ /*@returned@*/ termNode p_secondary, 
			/*@only@*/ ltoken p_postfixop);

extern /*@only@*/ termNode 
  makePrefixTermNode (/*@only@*/ ltoken p_op, /*@only@*/ termNode p_arg);

extern /*@exposed@*/ termNode 
  CollapseInfixTermNode (/*@returned@*/ termNode p_secondary, termNodeList p_infix);

extern /*@only@*/ termNode 
  makeMatchedNode (/*@only@*/ ltoken p_open, 
		   /*@only@*/ termNodeList p_args, /*@only@*/ ltoken p_close);

extern /*@only@*/ termNode 
  makeSqBracketedNode (/*@only@*/ ltoken p_lbracket, 
		       /*@only@*/ termNodeList p_args, 
		       /*@only@*/ ltoken p_rbracket);

extern /*@only@*/ termNode
  updateSqBracketedNode (/*@null@*/ /*@only@*/ termNode p_left,
			 /*@only@*/ /*@returned@*/ termNode p_t,
			 /*@null@*/ /*@only@*/ termNode p_right);

extern termNode 
  updateMatchedNode (/*@null@*/ /*@only@*/ termNode p_left, /*@returned@*/ termNode p_t, 
		     /*@null@*/ /*@only@*/ termNode p_right);

extern /*@only@*/ termNode 
  makeSimpleTermNode (/*@only@*/ ltoken p_varid);
extern /*@only@*/ termNode 
  makeSelectTermNode (/*@only@*/ termNode p_pri, /*@only@*/ ltoken p_select,
		      /*@dependent@*/ ltoken p_id);
extern /*@only@*/ termNode 
  makeMapTermNode (/*@only@*/ termNode p_pri, /*@only@*/ ltoken p_map, 
		   /*@dependent@*/ ltoken p_id);
extern /*@only@*/ termNode 
  makeLiteralTermNode (/*@only@*/ ltoken p_tok, sort p_s); 

extern /*@only@*/ termNode 
  makeUnchangedTermNode1 (/*@only@*/ ltoken p_op, /*@only@*/ ltoken p_all); 
extern /*@only@*/ termNode 
  makeUnchangedTermNode2 (/*@only@*/ ltoken p_op, /*@only@*/ storeRefNodeList p_x); 
extern /*@only@*/ termNode 
  makeSizeofTermNode(/*@only@*/ ltoken p_op, /*@only@*/ lclTypeSpecNode p_type);
extern /*@only@*/ termNode 
  makeOpCallTermNode (/*@only@*/ ltoken p_op, /*@only@*/ ltoken p_open, 
		      /*@only@*/ termNodeList p_args, /*@only@*/ ltoken p_close);

extern sort sigNode_rangeSort (sigNode p_sig);

extern /*@only@*/ sortList sigNode_domain (sigNode p_sig);

extern bool sameNameNode (/*@null@*/ nameNode p_n1, /*@null@*/ nameNode p_n2);

extern /*@only@*/ CTypesNode 
  makeCTypesNode (/*@null@*/ /*@only@*/ CTypesNode p_ctypes, /*@only@*/ ltoken p_ct);

extern /*@only@*/ CTypesNode 
  makeTypeSpecifier (/*@only@*/ ltoken p_typedefname) ;

extern bool sigNode_equal (sigNode p_n1, sigNode p_n2);

extern sort lclTypeSpecNode2sort(lclTypeSpecNode p_type);

extern sort typeExpr2ptrSort(sort p_base, /*@null@*/ typeExpr p_t);

/* should be tagKind, instead of int */
extern lsymbol checkAndEnterTag(tagKind p_k, /*@only@*/ ltoken p_opttagid);
extern void enteringFcnScope(lclTypeSpecNode p_t, declaratorNode p_d, globalList p_g);
extern void enteringClaimScope (paramNodeList p_params, globalList p_g);

extern /*@observer@*/ ltoken nameNode_errorToken (/*@null@*/ nameNode p_nn);
extern /*@observer@*/ ltoken termNode_errorToken (/*@null@*/ termNode p_n);
extern /*@observer@*/ ltoken lclTypeSpecNode_errorToken (/*@null@*/ lclTypeSpecNode p_t);

extern opFormUnion opFormUnion_createAnyOp (ltoken p_t); 
extern opFormUnion opFormUnion_createMiddle (int p_middle); 
extern void LCLBuiltins (void);
extern /*@only@*/ paramNode paramNode_elipsis (void);
extern termNodeList 
  pushInfixOpPartNode (/*@returned@*/ termNodeList p_x, /*@only@*/ ltoken p_op,
		       /*@only@*/ termNode p_secondary);
extern /*@only@*/ cstring declaratorNode_unparseCode (declaratorNode p_x);

extern /*@only@*/ cstring typeExpr_name (/*@null@*/ typeExpr p_x);

extern void setExposedType (lclTypeSpecNode p_s);
extern void declareForwardType (declaratorNode p_declare);

extern /*@only@*/ declaratorNode declaratorNode_copy (declaratorNode p_x);

extern bool lslOp_equal (lslOp p_x, lslOp p_y);

extern void lsymbol_setbool (lsymbol p_s) /*@modifies internalState@*/ ;
extern lsymbol lsymbol_getbool (void);
extern lsymbol lsymbol_getBool (void);
extern lsymbol lsymbol_getTRUE (void);
extern lsymbol lsymbol_getFALSE (void);


# else
# error "Multiple include"
# endif
