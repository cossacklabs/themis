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
** mtContextNode.c
*/

# include "splintMacros.nf"
# include "basic.h"

static bool mtContextNode_matchesType (mtContextNode, ctype) /*@*/ ;

static /*@observer@*/ cstring mtContextKind_unparse (mtContextKind ck)
{
  switch (ck)
    {
    case MTC_ANY: return cstring_makeLiteralTemp ("any");
    case MTC_PARAM: return cstring_makeLiteralTemp ("parameter"); 
    case MTC_RESULT: return cstring_makeLiteralTemp ("result"); 
    case MTC_REFERENCE: return cstring_makeLiteralTemp ("reference");
    case MTC_CLAUSE: return cstring_makeLiteralTemp ("clause");
    case MTC_LITERAL: return cstring_makeLiteralTemp ("literal");
    case MTC_NULL: return cstring_makeLiteralTemp ("null");
    }

  BADBRANCHRET (cstring_undefined);
}
 
static mtContextNode mtContextNode_create (mtContextKind context, ctype ct) 
{
  mtContextNode res = (mtContextNode) dmalloc (sizeof (*res));
  res->context = context;
  res->type = ct;
  DPRINTF (("Create: %s", mtContextNode_unparse (res)));
  return res;
}

extern mtContextNode mtContextNode_createAny ()
{
  return mtContextNode_create (MTC_ANY, ctype_unknown);
}

extern mtContextNode mtContextNode_createParameter (ctype ct) 
{
  return mtContextNode_create (MTC_PARAM, ct);
}

extern mtContextNode mtContextNode_createResult (ctype ct) 
{
  return mtContextNode_create (MTC_RESULT, ct);
}

extern mtContextNode mtContextNode_createReference (ctype ct) 
{
  return mtContextNode_create (MTC_REFERENCE, ct);
}

extern mtContextNode mtContextNode_createClause (ctype ct) 
{
  return mtContextNode_create (MTC_CLAUSE, ct);
}

extern mtContextNode mtContextNode_createLiteral (ctype ct) 
{
  return mtContextNode_create (MTC_LITERAL, ct);
}

extern mtContextNode mtContextNode_createNull (ctype ct) 
{
  return mtContextNode_create (MTC_NULL, ct);
}

extern void mtContextNode_free (/*@only@*/ mtContextNode node) 
{
  sfree (node);
}

bool mtContextNode_matchesEntry (mtContextNode context, uentry ue)
{
  ctype ct;

  llassert (mtContextNode_isDefined (context));

  DPRINTF (("Matches context: %s / %s",
	    mtContextNode_unparse (context), uentry_unparse (ue)));

  switch (context->context)
    {
    case MTC_ANY: break; /* everything matches */
    case MTC_RESULT:
      if (!uentry_isFunction (ue))
	{
	  return FALSE;
	}
      break;
    case MTC_PARAM: 
      if (!uentry_isAnyParam (ue))
	{
	  DPRINTF (("not param: %s", uentry_unparseFull (ue)));
	  return FALSE;
	}
      break;
    case MTC_LITERAL:
    case MTC_NULL:
      return FALSE;
    case MTC_REFERENCE:
      break;
    case MTC_CLAUSE:
      BADBRANCH;
    }

  if (uentry_isFunction (ue))
    {
      ct = ctype_getReturnType (uentry_getType (ue));
    }
  else
    {
      ct = uentry_getType (ue);
    }

  return mtContextNode_matchesType (context, ct);
}

bool mtContextNode_matchesRef (mtContextNode context, sRef sr)
{
  ctype ct;

  llassert (mtContextNode_isDefined (context));

  DPRINTF (("Matches context: %s / %s",
	    mtContextNode_unparse (context), sRef_unparse (sr)));

  switch (context->context)
    {
    case MTC_ANY: break; /* everything matches */
    case MTC_RESULT:
      DPRINTF (("Result? %s / %s",
		sRef_unparseFull (sr),
		bool_unparse (sRef_isResult (sr))));
      return sRef_isResult (sr);
    case MTC_PARAM: 
      if (!sRef_isParam (sr))
	{
	  return FALSE;
	}
      break;
    case MTC_LITERAL:
      DPRINTF (("Literal: %s", sRef_unparse (sr)));
      if (!sRef_isConst (sr))
	{
	  return FALSE;
	}
      break;
    case MTC_NULL:
    case MTC_REFERENCE:
      break;
    case MTC_CLAUSE:
      BADBRANCH;
    }

  ct = sRef_getType (sr);
  return mtContextNode_matchesType (context, ct);
}

bool mtContextNode_matchesRefStrict (mtContextNode context, sRef s)
{
  if (mtContextNode_isDefined (context)
      && mtContextNode_matchesRef (context, s))
    {
      if (ctype_isKnown (context->type) 
	  && (ctype_isUnknown (sRef_getType (s))
	      || ctype_isVoidPointer (sRef_getType (s))))
	{
	  return FALSE;
	}
      else
	{
	  return TRUE;
	}
    }
  
  return FALSE;
}

bool mtContextNode_matchesType (mtContextNode context, ctype ct)
{
  DPRINTF (("Context type..."));
  llassert (mtContextNode_isDefined (context));
  
  if (!ctype_match (context->type, ct))
    {
      DPRINTF (("Type mismatch: %s / %s",
		ctype_unparse (context->type),
		ctype_unparse (ct)));
      return FALSE;
    }
  else
    {
      /* evans 2001-08-21 - don't match if only one type is unknown */
      if (ctype_isUnknown (ct) && !ctype_isUnknown (context->type))
	{
	  return FALSE;
	}

      DPRINTF (("Type okay: %s / %s",
		ctype_unparse (context->type),
		ctype_unparse (ct)));
    }
  
  return TRUE;
}

cstring mtContextNode_unparse (mtContextNode node)
{
  llassert (mtContextNode_isDefined (node));

  if (ctype_isKnown (node->type))
    {
      return message ("%s %s", mtContextKind_unparse (node->context),
		      ctype_unparse (node->type));
    }
  else
    {
      return message ("%s", mtContextKind_unparse (node->context));
    }
}

bool mtContextNode_isClause (mtContextNode n)
{
  llassert (mtContextNode_isDefined (n));
  return (n->context == MTC_CLAUSE);
}

bool mtContextNode_isParameter (mtContextNode n)
{
  llassert (mtContextNode_isDefined (n));
  return (n->context == MTC_PARAM);
}

bool mtContextNode_isReference (mtContextNode n)
{
  llassert (mtContextNode_isDefined (n));
  return (n->context == MTC_REFERENCE);
}

bool mtContextNode_isResult (mtContextNode n)
{
  llassert (mtContextNode_isDefined (n));
  return (n->context == MTC_RESULT);
}

bool mtContextNode_isLiteral (mtContextNode n)
{
  llassert (mtContextNode_isDefined (n));
  return (n->context == MTC_LITERAL);
}

bool mtContextNode_isNull (mtContextNode n)
{
  llassert (mtContextNode_isDefined (n));
  return (n->context == MTC_NULL);
}

void mtContextNode_showRefError (mtContextNode context, sRef sr)
{
  ctype ct;

  llassert (mtContextNode_isDefined (context));
  llassert (!mtContextNode_matchesRef (context, sr));

  DPRINTF (("Matches context: %s / %s",
	    mtContextNode_unparse (context), sRef_unparse (sr)));

  switch (context->context)
    {
    case MTC_ANY: break; /* everything matches */
    case MTC_RESULT:
      if (!sRef_isResult (sr))
	{
	  llgenindentmsgnoloc 
	    (message ("Context is result, doesn't match %q", sRef_unparse (sr)));
	  return;
	}
      break;
    case MTC_PARAM: 
      if (!sRef_isResult (sr))
	{
	  llgenindentmsgnoloc 
	    (message ("Context is parameter, doesn't match %q", sRef_unparse (sr)));
	  return;
	}
      break;
    case MTC_LITERAL:
      DPRINTF (("Literal: %s", sRef_unparse (sr)));
      if (!sRef_isConst (sr))
	{
	  llgenindentmsgnoloc
	    (message ("Context is literal, doesn't match %q", sRef_unparse (sr)));
	  return;
	}
      break;
    case MTC_NULL:
    case MTC_REFERENCE:
      break;
    case MTC_CLAUSE:
      BADBRANCH;
    }

  ct = sRef_getType (sr);
  
  if (!mtContextNode_matchesType (context, ct))
    {
      llgenindentmsgnoloc
	(message ("Context type is %s, doesn't match type %s", 
		  ctype_unparse (context->type),
		  ctype_unparse (ct)));
    }
  else
    {
      BADBRANCH;
    }
}



