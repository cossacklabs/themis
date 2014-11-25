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
** functionClauseList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

functionClauseList
functionClauseList_new ()
{
  return functionClauseList_undefined;
}

static /*@notnull@*/ functionClauseList
functionClauseList_newEmpty (void)
{
  functionClauseList s = (functionClauseList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = functionClauseListBASESIZE; 
  s->elements = (functionClause *) dmalloc (sizeof (*s->elements) * functionClauseListBASESIZE);

  return (s);
}

static void
functionClauseList_grow (/*@notnull@*/ functionClauseList s)
{
  int i;
  functionClause *newelements;
  
  s->nspace += functionClauseListBASESIZE;

  newelements = (functionClause *) dmalloc (sizeof (*newelements) * (s->nelements + s->nspace));

  if (newelements == (functionClause *) 0)
    {
      llfatalerror (cstring_makeLiteral ("functionClauseList_grow: out of memory!"));
    }

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }
  
  sfree (s->elements); 
  s->elements = newelements;
}

functionClauseList functionClauseList_single (/*@keep@*/ functionClause el) 
{
  functionClauseList s = functionClauseList_new ();
  s = functionClauseList_add (s, el);
  return s;
}

functionClauseList functionClauseList_add (functionClauseList s, /*@keep@*/ functionClause el)
{
  if (!functionClauseList_isDefined (s))
    {
      s = functionClauseList_newEmpty ();
    }

  if (s->nspace <= 0)
    {
      functionClauseList_grow (s);
    }
  
  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

functionClauseList functionClauseList_prepend (functionClauseList s, /*@keep@*/ functionClause el)
{
  int i;

  if (!functionClauseList_isDefined (s))
    {
      return functionClauseList_single (el);
    }

  if (s->nspace <= 0)
    {
      functionClauseList_grow (s);
    }
  
  s->nspace--;

  for (i = s->nelements; i > 0; i--) 
    {
      s->elements[i] = s->elements [i - 1];
    }

  s->elements[0] = el;
  s->nelements++;
  
  return s;
}

cstring
functionClauseList_unparse (functionClauseList s)
{
  return functionClauseList_unparseSep (s, cstring_makeLiteralTemp (" "));
}

cstring
functionClauseList_unparseSep (functionClauseList s, cstring sep)
{
   cstring st = cstring_undefined;

   if (functionClauseList_isDefined (s))
     {
       int i;

       for (i = 0; i < s->nelements; i++)
	 {
	   if (i == 0)
	     {
	       st = functionClause_unparse (s->elements[i]);
	     }
	   else
	     st = message ("%q%s%q", st, sep, functionClause_unparse (s->elements[i]));
	 }
     }

   return st;
}

void
functionClauseList_free (functionClauseList s)
{
  if (functionClauseList_isDefined (s))
    {
      int i;

      for (i = 0; i < s->nelements; i++) {
	functionClause_free (s->elements[i]);
      }

      sfree (s->elements);
      sfree (s);
    }
}

functionClauseList 
functionClauseList_setImplicitConstraints (/*@returned@*/ functionClauseList s)
{
  bool addedConstraints;

  constraintList c;
  
  DPRINTF ((message ("functionClauseList_setImplicitConstraints called ") ));
  
  addedConstraints = FALSE;

  c = getImplicitFcnConstraints ();
  
  if (constraintList_isEmpty(c) )
    {
      return s;
    }
  
  functionClauseList_elements(s, el)
    {
      if (functionClause_isRequires(el))
	{
	  functionConstraint con = functionClause_getRequires(el);
	  
	  if (functionConstraint_hasBufferConstraint(con))
	    {
	      if (functionConstraint_isBufferConstraint (con))
		{
		  constraintList implCons = getImplicitFcnConstraints ();
		  
		  DPRINTF ((message ("functionClauseList_ImplicitConstraints adding the implict constraints: %s to %s",
		  		     constraintList_unparse(implCons), constraintList_unparse (con->constraint.buffer))));
		  
		  functionConstraint_addBufferConstraints (con, constraintList_copy (implCons) );

		  addedConstraints = TRUE;
		  
		  DPRINTF ((message ("functionClauseList_ImplicitConstraints the new constraint is %s",
				     functionConstraint_unparse (con))));

		}
	      else
		{
		  llassert (FALSE);
		}
	    }
	}
    }
  
  end_functionClauseList_elements;

  if (!addedConstraints)
    {
      functionConstraint fCon;
      functionClause fClause;
      
      constraintList implCons = getImplicitFcnConstraints ();
      
      fCon = functionConstraint_createBufferConstraint(constraintList_copy (implCons) );
      fClause = functionClause_createRequires(fCon);
      s = functionClauseList_add(s, fClause);

	
    }
  return s;    
}
