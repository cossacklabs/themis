/*
** exprData.c
*/

# include <ctype.h> /* for isdigit */
# include "splintMacros.nf"
# include "basic.h"
# include "cgrammar.h"
# include "cgrammar_tokens.h"

# include "exprChecks.h"
# include "exprNodeSList.h"

void exprData_freeShallow (/*@only@*/ exprData data, exprKind kind)
{
  /*@-compdestroy@*/ 

  if (data == exprData_undefined)
    {
      return;
    }

  switch (kind)
    {
    case XPR_INITBLOCK:
    case XPR_CALL:
      exprNode_freeShallow (data->call->fcn);
      exprNodeList_freeShallow (data->call->args);
      sfree (data->call);
      break;
    case XPR_COMMA:
    case XPR_FETCH:
    case XPR_OP:
    case XPR_ASSIGN:
    case XPR_IF:
    case XPR_WHILE:
    case XPR_DOWHILE:
    case XPR_STMTLIST:
    case XPR_SWITCH:
    case XPR_FOR:
      exprNode_freeShallow (data->op->a);
      exprNode_freeShallow (data->op->b);
      sfree (data->op);
      break;

    case XPR_STMT:
    case XPR_PREOP: 
    case XPR_POSTOP:
    case XPR_PARENS: 
      exprNode_freeShallow (data->uop->a);
      sfree (data->uop);
      break;

    case XPR_FTCASE:
    case XPR_CASE:      
    case XPR_RETURN:
    case XPR_WHILEPRED:
    case XPR_BLOCK:
      exprNode_freeShallow (data->single);
      break;
      
    case XPR_CAST:
    case XPR_VAARG:
      exprNode_freeShallow (data->cast->exp);
      /* NO: qtype_free (data->cast->q); */
      sfree (data->cast);      
      break;
      
    case XPR_ITERCALL:
      exprNodeList_freeShallow (data->itercall->args);
      sfree (data->itercall);
      break;

    case XPR_ITER:
      exprNodeList_freeShallow (data->iter->args);
      exprNode_freeShallow (data->iter->body);
      sfree (data->iter);
      break;
      
    case XPR_FORPRED:
    case XPR_COND:
    case XPR_IFELSE:
      exprNode_freeShallow (data->triple->pred);
      exprNode_freeShallow (data->triple->tbranch);
      exprNode_freeShallow (data->triple->fbranch);
      sfree (data->triple);
      break;
      
    case XPR_INIT:
      exprNode_freeShallow (data->init->exp);
      /* NO: idDecl_free (data->init->id); */
      sfree (data->init);
      break;
      
    case XPR_FACCESS:
    case XPR_ARROW:
      exprNode_freeShallow (data->field->rec);
      /* NO: cstring_free (data->field->field); */
      sfree (data->field);
      break;

    case XPR_LABEL:
    case XPR_CONST:
    case XPR_VAR:
      break;

    case XPR_OFFSETOF:
    case XPR_ALIGNOFT:
    case XPR_ALIGNOF:
    case XPR_SIZEOFT:
    case XPR_SIZEOF:
    case XPR_GOTO:
    case XPR_CONTINUE:
    case XPR_BREAK:
    case XPR_NULLRETURN:
    case XPR_TOK:
    case XPR_FTDEFAULT:
    case XPR_DEFAULT:
      break;

    case XPR_STRINGLITERAL:
    case XPR_NUMLIT:
      llcontbuglit ("exprData_freeShallow: not shallow!");
      break;

    case XPR_EMPTY:
      llcontbuglit ("XPR_EMPTY case!");
      break;

    case XPR_BODY:
      llcontbuglit ("XPR_BODY case!");
      break;
    case XPR_NODE:
      llcontbuglit ("XPR_NODE case!");
      break;
    }

  sfree (data);
  /*@=compdestroy@*/
}

void exprData_free (/*@only@*/ exprData data, exprKind kind)
{
  if (data == exprData_undefined)
    {
      return;
    }

  switch (kind)
    {
    case XPR_INITBLOCK:
    case XPR_CALL:
      exprNode_free (data->call->fcn);
      exprNodeList_free (data->call->args);
      sfree (data->call);
      break;

    case XPR_LABEL:
    case XPR_CONST:
    case XPR_VAR:
      cstring_free (data->id);
      break;

    case XPR_COMMA:
    case XPR_FETCH:
    case XPR_OP:
    case XPR_ASSIGN:
    case XPR_IF:
    case XPR_WHILE:
    case XPR_DOWHILE:
    case XPR_STMTLIST:
    case XPR_SWITCH:
    case XPR_FOR:
      exprNode_free (data->op->a);
      exprNode_free (data->op->b);
      sfree (data->op);
      break;

    case XPR_STMT:
    case XPR_PREOP: 
    case XPR_POSTOP:
    case XPR_PARENS: 
      exprNode_free (data->uop->a);
      sfree (data->uop);
      break;

    case XPR_OFFSETOF:
      qtype_free (data->offset->q);
      cstringList_free (data->offset->field);
      sfree (data->offset);
      break;

    case XPR_ALIGNOFT:
    case XPR_SIZEOFT:
      qtype_free (data->qt);
      break;

    case XPR_FTCASE:
    case XPR_CASE:      
    case XPR_SIZEOF:
    case XPR_ALIGNOF:
    case XPR_RETURN:
    case XPR_WHILEPRED:
    case XPR_BLOCK:
      exprNode_free (data->single);
      break;
      
    case XPR_CAST:
    case XPR_VAARG:
      exprNode_free (data->cast->exp);
      qtype_free (data->cast->q);
      sfree (data->cast);
      break;
      
    case XPR_ITERCALL:
      exprNodeList_free (data->itercall->args);
      sfree (data->itercall);
      break;

    case XPR_ITER:
      exprNodeList_free (data->iter->args);
      exprNode_free (data->iter->body);
      sfree (data->iter);
      break;
      
    case XPR_FORPRED:
    case XPR_COND:
    case XPR_IFELSE:
      exprNode_free (data->triple->pred);
      exprNode_free (data->triple->tbranch);
      exprNode_free (data->triple->fbranch);
      sfree (data->triple);
      break;
      
    case XPR_GOTO:
    case XPR_STRINGLITERAL:
    case XPR_NUMLIT:
      cstring_free (data->literal);
      break;

    case XPR_CONTINUE:
    case XPR_BREAK:
    case XPR_NULLRETURN:
      break;

    case XPR_FTDEFAULT:
    case XPR_DEFAULT:
    case XPR_TOK:
      break;
      
    case XPR_INIT:
      exprNode_free (data->init->exp);
      idDecl_free (data->init->id);
      sfree (data->init);
      break;
      
    case XPR_FACCESS:
    case XPR_ARROW:
      exprNode_free (data->field->rec);
      cstring_free (data->field->field);
      sfree (data->field);
      break;

    case XPR_EMPTY:
      llcontbuglit ("XPR_EMPTY case!");
      break;
    case XPR_BODY:
      llcontbuglit ("XPR_BODY case!");
      break;
    case XPR_NODE:
      llcontbuglit ("XPR_NODE case!");
      break;
    }

  sfree (data);
}

/*@exposed@*/ exprNode exprData_getInitNode (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->init->exp);
}

/*@exposed@*/ idDecl exprData_getInitId (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->init->id);
}

/*@exposed@*/ exprNode exprData_getOpA (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->op->a);
}

/*@exposed@*/ exprNode exprData_getOpB (exprData data) /*@*/
{
  llassertretval (data != exprData_undefined, exprNode_undefined);
  return (data->op->b);
}

/*@observer@*/ lltok exprData_getOpTok (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->op->op);
}

/*@exposed@*/ exprNode exprData_getPairA (exprData data) /*@*/
{
  llassertretval (data != exprData_undefined, exprNode_undefined);
  return (data->pair->a);
}

/*@exposed@*/ exprNode exprData_getPairB (exprData data) /*@*/
{
  llassertretval (data != exprData_undefined, exprNode_undefined);
  return (data->pair->b);
}

/*@exposed@*/ uentry exprData_getIterSname (exprData data) /*@*/
{
  llassertretval (data != exprData_undefined, uentry_undefined);
  return (data->iter->sname);
}

/*@exposed@*/ exprNodeList exprData_getIterAlist (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->iter->args);
}

/*@exposed@*/ exprNode exprData_getIterBody (exprData data) /*@*/
{
  llassertretval (data != exprData_undefined, exprNode_undefined);
  return (data->iter->body);
}

/*@exposed@*/ uentry exprData_getIterEname (exprData data) /*@*/
{
  llassertretval (data != exprData_undefined, uentry_undefined);
  return (data->iter->ename);
}

/*@exposed@*/ exprNode exprData_getFcn (exprData data) /*@*/
{
  llassertretval (data != exprData_undefined, exprNode_undefined);
  return (data->call->fcn);
}

/*@exposed@*/ exprNodeList exprData_getArgs (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->call->args);
}

/*@exposed@*/ exprNode exprData_getTriplePred (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->triple->pred);
}

/*@exposed@*/ uentry exprData_getIterCallIter (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->itercall->iter);
}

/*@exposed@*/ exprNodeList
  exprData_getIterCallArgs (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->itercall->args);
}

/*@exposed@*/ exprNode exprData_getTripleInit (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->triple->pred);
}

/*@exposed@*/ exprNode exprData_getTripleTrue (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->triple->tbranch);
}

/*@exposed@*/ exprNode exprData_getTripleTest (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->triple->tbranch);
}

/*@exposed@*/ exprNode exprData_getTripleFalse (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->triple->fbranch);
}

/*@exposed@*/ exprNode exprData_getTripleInc (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->triple->fbranch);
}

/*@exposed@*/ exprNode exprData_getFieldNode (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->field->rec);
}

/*@exposed@*/ cstring exprData_getFieldName (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->field->field);
}

/*@observer@*/ lltok exprData_getUopTok (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->uop->op);
}

/*@exposed@*/ exprNode exprData_getUopNode (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->uop->a);
}

/*@exposed@*/ exprNode exprData_getCastNode (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->cast->exp);
}

/*@observer@*/ lltok exprData_getCastTok (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->cast->tok);
}

/*@exposed@*/ qtype exprData_getCastType (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->cast->q);
}

/*@exposed@*/ cstring exprData_getLiteral (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->literal);
}

/*@exposed@*/ cstring exprData_getId (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return (data->id);
}

/*@observer@*/ lltok exprData_getTok (exprData data) /*@*/
{
  llassert (data != exprData_undefined);
  return data->tok;
}

/*@exposed@*/ qtype exprData_getType (exprData data) /*@*/ 
{
  llassert (data != exprData_undefined);
  return (data->qt);
}

/*@exposed@*/ qtype exprData_getOffsetType (exprData data) /*@*/ 
{
  llassert (data != exprData_undefined);
  return (data->offset->q);
}

/*@exposed@*/ cstringList exprData_getOffsetName (exprData data) /*@*/ 
{
  llassert (data != exprData_undefined);
  return (data->offset->field);
}

/*@exposed@*/ exprNode exprData_getSingle (exprData data) /*@*/
{
  exprNode ret;
  llassert (data != exprData_undefined);

  ret = data->single;
  return (ret);
}


/*@only@*/ exprData 
  exprData_makeOp (/*@keep@*/ exprNode a, /*@keep@*/ exprNode b, /*@keep@*/ lltok op)
{
  exprData ed = (exprData) dmalloc (sizeof (*ed));
  
  ed->op = (exprOp) dmalloc (sizeof (*ed->op));
  ed->op->a = a;
  ed->op->b = b;
  ed->op->op = op;
  
  return ed;
}

/*@only@*/ exprData exprData_makeUop (/*@keep@*/ exprNode a, /*@keep@*/ lltok op)
{
  exprData ed = (exprData) dmalloc (sizeof (*ed));
  
  ed->uop = (exprUop) dmalloc (sizeof (*ed->uop));
  ed->uop->a = a;
  ed->uop->op = op;
  
  return ed;
}

/*@only@*/ exprData exprData_makeSingle (/*@only@*/ exprNode a)
{
  exprData ed = (exprData) dmalloc (sizeof (*ed));
  
  ed->single = a;  
  return ed;
}

/*@only@*/ exprData exprData_makeTok (/*@only@*/ lltok op)
{
  exprData ed = (exprData) dmalloc (sizeof (*ed));
  ed->tok = op;  

  return ed;
}

/*@only@*/ exprData 
exprData_makeIter (/*@exposed@*/ uentry sname, /*@keep@*/ exprNodeList args,
		   /*@keep@*/ exprNode body, /*@exposed@*/ uentry ename)
{
  exprData ed = (exprData) dmalloc (sizeof (*ed));
  
  ed->iter = (exprIter) dmalloc (sizeof (*ed->iter));
  ed->iter->sname = uentry_isVar (sname) ? uentry_copy (sname) : sname; 
  ed->iter->args = args;
  ed->iter->body = body;
  ed->iter->ename = uentry_isVar (ename) ? uentry_copy (ename) : ename; 
  return ed;
}

/*@only@*/ exprData exprData_makeTriple (/*@keep@*/ exprNode pred, 
						/*@keep@*/ exprNode tbranch, 
						/*@keep@*/ exprNode fbranch)
{
  exprData ed = (exprData) dmalloc (sizeof (*ed));
  
  ed->triple = (exprTriple) dmalloc (sizeof (*ed->triple));
  ed->triple->pred = pred;
  ed->triple->tbranch = tbranch;
  ed->triple->fbranch = fbranch;

  return ed;
}

/*@only@*/ exprData exprData_makeCall (/*@keep@*/ exprNode fcn,
					      /*@keep@*/ exprNodeList args)
{
  exprData ed = (exprData) dmalloc (sizeof (*ed));

  ed->call = (exprCall) dmalloc (sizeof (*ed->call));
  ed->call->fcn = fcn;
  ed->call->args = args;

  return ed;
}

/*@only@*/ exprData exprData_makeIterCall (/*@dependent@*/ uentry iter,
						  /*@keep@*/ exprNodeList args)
{
  exprData ed = (exprData) dmalloc (sizeof (*ed));

  ed->itercall = (exprIterCall) dmalloc (sizeof (*ed->itercall));
  ed->itercall->iter = uentry_isVar (iter) ? uentry_copy (iter) : iter; 
  ed->itercall->args = args;

  return ed;
}

/*@only@*/ exprData exprData_makeField (/*@keep@*/ exprNode rec, 
					       /*@keep@*/ cstring field)
{
  exprData ed = (exprData) dmalloc (sizeof (*ed));

  ed->field = (exprField) dmalloc (sizeof (*ed->field));
  ed->field->rec = rec;
  ed->field->field = field;

  return ed;
}

/*@only@*/ exprData exprData_makeOffsetof (/*@only@*/ qtype q,
						  /*@keep@*/ cstringList s)
{
  exprData ed = (exprData) dmalloc (sizeof (*ed));
  ed->offset = (exprOffsetof) dmalloc (sizeof (*ed->offset));

  ed->offset->q = q;
  ed->offset->field = s;
  return ed;
}

/*@only@*/ exprData exprData_makeSizeofType (/*@only@*/ qtype q)
{
  exprData ed = (exprData) dmalloc (sizeof (*ed));

  ed->qt = q;
  return ed;
}

/*@only@*/ exprData 
  exprData_makeCast (/*@keep@*/ lltok tok, /*@keep@*/ exprNode e, /*@only@*/ qtype q)
{
  exprData ed = (exprData) dmalloc (sizeof (*ed));

  ed->cast = (exprCast) dmalloc (sizeof (*ed->cast));
  ed->cast->tok = tok;
  ed->cast->exp = e;
  ed->cast->q = q;

  return ed;
}

/*@only@*/ exprData 
  exprData_makeInit (/*@keep@*/ idDecl t, /*@keep@*/ exprNode e)
{
  exprData ed = (exprData) dmalloc (sizeof (*ed));

  ed->init = (exprInit) dmalloc (sizeof (*ed->init));
  ed->init->exp = e;
  ed->init->id = t;

  return ed;
}

/*@only@*/ exprData exprData_makeCond (/*@keep@*/ exprNode pred, 
					      /*@keep@*/ exprNode ifclause, 
					      /*@keep@*/ exprNode elseclause)
{
  return exprData_makeTriple (pred, ifclause, elseclause);
}

/*@only@*/ exprData exprData_makeFor (/*@keep@*/ exprNode init, 
				      /*@keep@*/ exprNode pred, 
				      /*@keep@*/ exprNode inc)
{
  return exprData_makeTriple (init, pred, inc);
}


/*@only@*/ exprData exprData_makeLiteral (/*@only@*/ cstring s)
{
  exprData ed = (exprData) dmalloc (sizeof (*ed));
  
  ed->literal = s;
  return ed;
}

/*@only@*/ exprData exprData_makeId (/*@temp@*/ uentry id)
{
  exprData ed = (exprData) dmalloc (sizeof (*ed));
  ed->id = cstring_copy (uentry_rawName (id));
  return ed;
}

/*@only@*/ exprData exprData_makePair (/*@keep@*/ exprNode a, /*@keep@*/ exprNode b)
{
  exprData ed = (exprData) dmalloc (sizeof (*ed));

  ed->pair = (exprPair) dmalloc (sizeof (*ed->pair));
  ed->pair->a = a;
  ed->pair->b = b;

  return ed;
}
