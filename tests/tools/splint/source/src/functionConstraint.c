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
** functionConstraint.c
*/

# include "splintMacros.nf"
# include "basic.h"

static /*@only@*/ /*@notnull@*/ /*@special@*/ functionConstraint 
functionConstraint_alloc (functionConstraintKind kind) /*@defines result->kind@*/
{
  functionConstraint res = (functionConstraint) dmalloc (sizeof (*res));

  res->kind = kind;
  return res;
}

extern functionConstraint functionConstraint_createBufferConstraint (constraintList buf)
{ 
  functionConstraint res = functionConstraint_alloc (FCT_BUFFER);
  res->constraint.buffer = buf;
  return res;
}

extern functionConstraint functionConstraint_createMetaStateConstraint (metaStateConstraint msc)
{ 
  functionConstraint res = functionConstraint_alloc (FCT_METASTATE);
  res->constraint.metastate = msc;
  return res;
}

extern functionConstraint functionConstraint_conjoin (functionConstraint f1, functionConstraint f2)
{
  functionConstraint res = functionConstraint_alloc (FCT_CONJUNCT);
  res->constraint.conjunct.op1 = f1;
  res->constraint.conjunct.op2 = f2;
  DPRINTF (("Conjoining ==> %s",
	    functionConstraint_unparse (res)));
  return res;
}

/*@only@*/ cstring functionConstraint_unparse (functionConstraint p)
{
  if (functionConstraint_isDefined (p)) 
    {
      switch (p->kind)
	{
	case FCT_BUFFER: 
	  return constraintList_unparse (p->constraint.buffer);
	case FCT_METASTATE:
	  return metaStateConstraint_unparse (p->constraint.metastate);
	case FCT_CONJUNCT:
	  return message ("%q /\\ %q",
			  functionConstraint_unparse (p->constraint.conjunct.op1),
			  functionConstraint_unparse (p->constraint.conjunct.op2));
	  BADDEFAULT;
	}
      BADBRANCH;
    }
  else
    {
      return cstring_makeLiteral ("< empty constraint >");
    }
}

extern constraintList functionConstraint_getBufferConstraints (functionConstraint node)
{
  if (functionConstraint_isDefined (node))
    {
      if (node->kind == FCT_CONJUNCT)
	{
	  /* make sure this is safe*/
	  return constraintList_addListFree (functionConstraint_getBufferConstraints (node->constraint.conjunct.op1),
					     functionConstraint_getBufferConstraints (node->constraint.conjunct.op2));
	}
      else
	{
	  if (node->kind == FCT_BUFFER)
	    {
	      return constraintList_copy (node->constraint.buffer);
	    }
	  else
	    {
	      return constraintList_undefined;
	    }
	}
    }
  else
    {
      return constraintList_undefined;
    }
}

extern metaStateConstraintList functionConstraint_getMetaStateConstraints (functionConstraint node)
{
  if (functionConstraint_isDefined (node))
    {
      if (node->kind == FCT_CONJUNCT)
	{
	  return metaStateConstraintList_append 
	    (functionConstraint_getMetaStateConstraints (node->constraint.conjunct.op1),
	     functionConstraint_getMetaStateConstraints (node->constraint.conjunct.op2));
	}
      else
	{
	  if (node->kind == FCT_METASTATE)
	    {
	      return metaStateConstraintList_single (node->constraint.metastate);
	    }
	  else
	    {
	      return metaStateConstraintList_undefined;
	    }
	}
    }
  else
    {
      return metaStateConstraintList_undefined;
    }
}

extern bool functionConstraint_hasBufferConstraint (functionConstraint node)
{
  if (functionConstraint_isDefined (node))
    {
      return node->kind == FCT_BUFFER
	|| (node->kind == FCT_CONJUNCT 
	    && (functionConstraint_hasBufferConstraint (node->constraint.conjunct.op1) 
		|| functionConstraint_hasBufferConstraint (node->constraint.conjunct.op2)));
    }
  else
    {
      return FALSE;
    }
}

extern bool functionConstraint_hasMetaStateConstraint (functionConstraint node)
{
  if (functionConstraint_isDefined (node))
    {
      return node->kind == FCT_METASTATE
	|| (node->kind == FCT_CONJUNCT 
	    && (functionConstraint_hasMetaStateConstraint (node->constraint.conjunct.op1) 
		|| functionConstraint_hasMetaStateConstraint (node->constraint.conjunct.op2)));
    }
  else
    {
      return FALSE;
    }
}

extern functionConstraint functionConstraint_copy (functionConstraint node) 
{
  if (functionConstraint_isDefined (node))
    {
      switch (node->kind)
	{
	case FCT_BUFFER:
	  return functionConstraint_createBufferConstraint (constraintList_copy (node->constraint.buffer));
	case FCT_METASTATE:
	  return functionConstraint_createMetaStateConstraint (metaStateConstraint_copy (node->constraint.metastate));
	case FCT_CONJUNCT:
	  return functionConstraint_conjoin (functionConstraint_copy (node->constraint.conjunct.op1),
					     functionConstraint_copy (node->constraint.conjunct.op2));
	}
      
      BADBRANCH;
    }
  else
    {
      return functionConstraint_undefined;
    }

  BADBRANCHRET (functionConstraint_undefined);
}

extern void functionConstraint_free (/*@only@*/ functionConstraint node) 
{
  if (functionConstraint_isDefined (node))
    {
      switch (node->kind)
	{
	case FCT_BUFFER:
	  constraintList_free (node->constraint.buffer);
	  break;
	case FCT_METASTATE:
	  metaStateConstraint_free (node->constraint.metastate);
	  break;
	case FCT_CONJUNCT:
	  functionConstraint_free (node->constraint.conjunct.op1);
	  functionConstraint_free (node->constraint.conjunct.op2);
	  break;
        BADDEFAULT;
	}
      
      sfree (node);
    }
}

/*drl modified */
void functionConstraint_addBufferConstraints (functionConstraint node, constraintList clist) 
{
  constraintList temp;

  temp = constraintList_copy (clist);
  
  if (functionConstraint_isDefined (node))
    {
      if (node->kind == FCT_CONJUNCT)
	{
	  functionConstraint_addBufferConstraints (node->constraint.conjunct.op1, constraintList_copy(temp) );
	  functionConstraint_addBufferConstraints (node->constraint.conjunct.op2,temp);
	}
      else
	{
	  if (node->kind == FCT_BUFFER)
	    {
	      node->constraint.buffer = constraintList_addListFree(node->constraint.buffer, temp);
	    }
	  else
	    {
	      constraintList_free (temp);
	      return;
	    }
	}
    }
  else
    {
      constraintList_free (temp);
      return;
    }
}
