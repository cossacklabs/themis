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
** functionClause.c
*/

# include "splintMacros.nf"
# include "basic.h"

static /*@only@*/ /*@notnull@*/ /*@special@*/ functionClause  
functionClause_alloc (functionClauseKind kind) /*@defines result->kind@*/
{
  functionClause res = (functionClause) dmalloc (sizeof (*res));

  res->kind = kind;
  return res;
}

extern functionClause functionClause_createGlobals (globalsClause node) /*@*/ 
{ 
  functionClause res = functionClause_alloc (FCK_GLOBALS);
  res->val.globals = node;
  return res;
}

extern functionClause functionClause_createModifies (modifiesClause node) /*@*/ 
{ 
  functionClause res = functionClause_alloc (FCK_MODIFIES);
  res->val.modifies = node;
  return res;
}

extern functionClause functionClause_createState (stateClause node) /*@*/ 
{
  if (stateClause_hasEmptyReferences (node) &&
      (!stateClause_isMetaState (node) ) )
    {
      DPRINTF((message("functionClause_createState:: Returning functionClause_undefined" ) ));
      stateClause_free (node);
      return functionClause_undefined;
    }
  else
    {
      functionClause res = functionClause_alloc (FCK_STATE);
      res->val.state = node;
      return res;
    }
}

extern functionClause functionClause_createEnsures (functionConstraint node) /*@*/ 
{ 
  functionClause res = functionClause_alloc (FCK_ENSURES);
  res->val.constraint = node;
  return res;
}

extern functionClause functionClause_createRequires (functionConstraint node) /*@*/ 
{ 
  functionClause res = functionClause_alloc (FCK_REQUIRES);
  res->val.constraint = node;
  return res;
}

extern functionClause functionClause_createWarn (warnClause node) /*@*/ 
{ 
  functionClause res = functionClause_alloc (FCK_WARN);
  res->val.warn = node;
  return res;
}

/*@only@*/ cstring functionClause_unparse (functionClause p)
{
  if (functionClause_isUndefined (p))
    {
      return cstring_undefined;
    }

  switch (p->kind)
    {
    case FCK_GLOBALS:
      return globalsClause_unparse (p->val.globals);
    case FCK_MODIFIES:
      return modifiesClause_unparse (p->val.modifies);
    case FCK_WARN:
      return warnClause_unparse (p->val.warn);
    case FCK_STATE:
      return stateClause_unparse (p->val.state);
    case FCK_ENSURES:
      return message ("ensures %q", functionConstraint_unparse (p->val.constraint));
    case FCK_REQUIRES:
      return message ("requires %q", functionConstraint_unparse (p->val.constraint));
    case FCK_DEAD:
      return cstring_makeLiteral ("<dead clause>");
    }

  BADBRANCHRET (cstring_undefined);
}

extern bool functionClause_matchKind (functionClause p, functionClauseKind kind) /*@*/
{
  if (functionClause_isDefined (p))
    {
      return (p->kind == kind);
    }
  else 
    {
      return FALSE;
    }
}

extern stateClause functionClause_getState (functionClause node)
{
  llassert (functionClause_isDefined (node));
  llassert (node->kind == FCK_STATE);

  return node->val.state;
}

extern stateClause functionClause_takeState (functionClause fc)
{
  stateClause res;
  llassert (functionClause_isDefined (fc));
  llassert (fc->kind == FCK_STATE);

  res = fc->val.state;
  fc->val.state = NULL;
  fc->kind = FCK_DEAD;
  return res;
}

extern functionConstraint functionClause_getEnsures (functionClause node)
{
  llassert (functionClause_isDefined (node));
  llassert (node->kind == FCK_ENSURES);

  return node->val.constraint;
}

extern functionConstraint functionClause_takeEnsures (functionClause fc)
{
  functionConstraint res;
  llassert (functionClause_isDefined (fc));
  llassert (fc->kind == FCK_ENSURES);

  res = fc->val.constraint;
  fc->val.constraint = NULL;
  fc->kind = FCK_DEAD;
  return res;
}

extern functionConstraint functionClause_getRequires (functionClause node)
{
  llassert (functionClause_isDefined (node));
  llassert (node->kind == FCK_REQUIRES);

  return node->val.constraint;
}

extern functionConstraint functionClause_takeRequires (functionClause fc)
{
  functionConstraint res;
  llassert (functionClause_isDefined (fc));
  llassert (fc->kind == FCK_REQUIRES);

  res = fc->val.constraint;
  fc->val.constraint = NULL;
  fc->kind = FCK_DEAD;
  return res;
}

extern warnClause functionClause_getWarn (functionClause node)
{
  llassert (functionClause_isDefined (node));
  llassert (node->kind == FCK_WARN);

  return node->val.warn;
}

extern warnClause functionClause_takeWarn (functionClause fc)
{
  warnClause res;
  llassert (functionClause_isDefined (fc));
  llassert (fc->kind == FCK_WARN);

  res = fc->val.warn;
  fc->val.warn = warnClause_undefined;
  fc->kind = FCK_DEAD;
  return res;
}

extern modifiesClause functionClause_getModifies (functionClause node)
{
  llassert (functionClause_isDefined (node));
  llassert (node->kind == FCK_MODIFIES);

  return node->val.modifies;
}

extern globalsClause functionClause_getGlobals (functionClause node)
{
  llassert (functionClause_isDefined (node));
  llassert (node->kind == FCK_GLOBALS);

  return node->val.globals;
}

extern void functionClause_free (/*@only@*/ functionClause node) 
{
  if (node != NULL)
    {
      DPRINTF (("free: %s", functionClause_unparse (node)));

      switch (node->kind)
	{
	case FCK_GLOBALS:
	  globalsClause_free (node->val.globals);
	  break;
	case FCK_MODIFIES:
	  modifiesClause_free (node->val.modifies);
	  break;
	case FCK_WARN:
	  warnClause_free (node->val.warn);
	  break;
	case FCK_STATE:
	  stateClause_free (node->val.state);
	  break;
	case FCK_ENSURES:
	  functionConstraint_free (node->val.constraint);
	  break;
	case FCK_REQUIRES:
	  functionConstraint_free (node->val.constraint);
	  break;
	case FCK_DEAD:
	  /* Nothing to release */
	  break;
	}
      
      sfree (node);
    }
}
