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
** constraintExprData.c
*/

# include "splintMacros.nf"
# include "basic.h"
# include "cgrammar.h"
# include "cgrammar_tokens.h"

# include "exprChecks.h"
# include "exprNodeSList.h"

static /*@out@*/ constraintExprData constraintExprData_alloc (void)
{
  constraintExprData ret;

  ret = dmalloc (sizeof (*ret) );

  return ret;
}

static /*@out@*/ constraintExprData constraintExprData_allocBinaryOp (void)
{
  constraintExprData ret;

  ret = constraintExprData_alloc();

  ret->binaryOp.expr1 = constraintExpr_undefined;
  ret->binaryOp.expr2 = constraintExpr_undefined;

  return ret;
}

static /*@out@*/ constraintExprData constraintExprData_allocUnaryOp (void)
{
  constraintExprData ret;

  ret = constraintExprData_alloc();

  ret->unaryOp.expr = constraintExpr_undefined;

  return ret;
}

static /*@out@*/ constraintExprData constraintExprData_allocTerm (void)
{
  constraintExprData ret;

  ret = constraintExprData_alloc();

  ret->term = NULL;
  return ret;
}


void constraintExprData_freeBinaryExpr (/*@only@*/constraintExprData data)
{
  constraintExpr_free (data->binaryOp.expr1);
  constraintExpr_free (data->binaryOp.expr2);
  free(data);  
}

constraintExprData constraintExprData_copyBinaryExpr(/*@observer@*/ constraintExprData data)
{
  constraintExprData ret;
  llassert(constraintExprData_isDefined(data));

  ret = constraintExprData_allocBinaryOp();
  
  ret->binaryOp.binaryOp = data->binaryOp.binaryOp;
  ret->binaryOp.expr1 = constraintExpr_copy (data->binaryOp.expr1);
  ret->binaryOp.expr2 = constraintExpr_copy (data->binaryOp.expr2);
  return ret;
}

void constraintExprData_freeUnaryExpr (/*@only@*/constraintExprData data)
{
  constraintExpr_free (data->unaryOp.expr);
  free(data);  
}

constraintExprData constraintExprData_copyUnaryExpr(/*@observer@*/ constraintExprData data)
{
 constraintExprData ret;
  llassert(constraintExprData_isDefined(data));

  ret = constraintExprData_allocUnaryOp();
  
  ret->unaryOp.unaryOp = data->unaryOp.unaryOp;
  ret->unaryOp.expr = constraintExpr_copy (data->unaryOp.expr);
  
  return ret;
}

void constraintExprData_freeTerm (/*@only@*/constraintExprData data)
{
  constraintTerm_free (data->term);
  free(data);  
}


constraintExprData constraintExprData_copyTerm (/*@observer@*/ constraintExprData data)
{
 constraintExprData ret;
  llassert(constraintExprData_isDefined(data));

  ret = constraintExprData_allocTerm();

  ret->term = constraintTerm_copy (data->term);
  return ret;
}

constraintExprData 
constraintExprData_termSetTerm (/*@returned@*/ constraintExprData data,  
				/*@only@*/ constraintTerm term)
{
  llassert (constraintExprData_isDefined (data));

  if (constraintTerm_isDefined (data->term))
    {
      constraintTerm_free (data->term);
    }

  data->term = term;
  return data;
}

/*@exposed@*/ constraintTerm 
constraintExprData_termGetTerm (/*@observer@*/ constraintExprData data)
{
  llassert (constraintExprData_isDefined (data));
  return data->term;
}


constraintExprUnaryOpKind 
constraintExprData_unaryExprGetOp (/*@observer@*/ constraintExprData data)
{
  llassert (constraintExprData_isDefined (data));
  return (data->unaryOp.unaryOp);
}


constraintExpr
constraintExprData_unaryExprGetExpr (/*@observer@*/ constraintExprData data)
{
  llassert (constraintExprData_isDefined (data));
  return (data->unaryOp.expr);
}

constraintExprData 
constraintExprData_unaryExprSetOp (/*@returned@*/ constraintExprData data,
				   constraintExprUnaryOpKind op)
{
  llassert (constraintExprData_isDefined (data));
  data->unaryOp.unaryOp = op;
  return data;
}

constraintExprData  
constraintExprData_unaryExprSetExpr (constraintExprData data,
				     /*@only@*/ constraintExpr expr)
{
  llassert (constraintExprData_isDefined (data));
  
  if (constraintExpr_isDefined (data->unaryOp.expr))
    {
      constraintExpr_free (data->unaryOp.expr);
    }
  
  data->unaryOp.expr = expr;
  return data;
}

/*
** binaryExpr stuff
*/

constraintExprBinaryOpKind constraintExprData_binaryExprGetOp (constraintExprData data)
{
  llassert (constraintExprData_isDefined (data));
  return (data->binaryOp.binaryOp);
}

constraintExpr constraintExprData_binaryExprGetExpr1 (constraintExprData data)
{
  llassert (constraintExprData_isDefined (data));
  return (data->binaryOp.expr1);
}

constraintExpr constraintExprData_binaryExprGetExpr2 (constraintExprData data)
{
  llassert (constraintExprData_isDefined(data));
  return (data->binaryOp.expr2);
}

constraintExprData 
constraintExprData_binaryExprSetExpr1 (/*@returned@*/ constraintExprData data,
				       /*@only@*/ constraintExpr expr)
{
  llassert (constraintExprData_isDefined (data));

  if (constraintExpr_isDefined (data->binaryOp.expr1))
    constraintExpr_free(data->binaryOp.expr1);
  
  data->binaryOp.expr1 = expr;
  return data;
}

constraintExprData  
constraintExprData_binaryExprSetExpr2 (constraintExprData data, constraintExpr expr) 
{
  llassert (constraintExprData_isDefined (data));

  if (constraintExpr_isDefined (data->binaryOp.expr2))
    constraintExpr_free (data->binaryOp.expr2);
  
  data->binaryOp.expr2 = expr;
  return data;
}

constraintExprData  
constraintExprData_binaryExprSetOp (constraintExprData data, 
				    constraintExprBinaryOpKind op)
{
  llassert (constraintExprData_isDefined (data));
  data->binaryOp.binaryOp = op;
  return data;
}

