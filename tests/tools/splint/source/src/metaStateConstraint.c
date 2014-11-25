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
** metaStateConstraint.c
*/

# include "splintMacros.nf"
# include "basic.h"

metaStateConstraint 
metaStateConstraint_create (/*@only@*/ metaStateSpecifier lspec, /*@only@*/ metaStateExpression rspec)
{
  metaStateConstraint res = (metaStateConstraint) dmalloc (sizeof (*res));
  res->lspec = lspec;
  res->rspec = rspec;
  return res;
}

cstring metaStateConstraint_unparse (metaStateConstraint m) 
{
  return message ("%q = %q", 
		  metaStateSpecifier_unparse (m->lspec),
		  metaStateExpression_unparse (m->rspec));
}

metaStateConstraint metaStateConstraint_copy (metaStateConstraint m)
{
  return metaStateConstraint_create (metaStateSpecifier_copy (m->lspec),
				     metaStateExpression_copy (m->rspec));
}

/*@observer@*/ metaStateSpecifier metaStateConstraint_getSpecifier (metaStateConstraint m)
{
  return m->lspec;
}

/*@observer@*/ metaStateExpression metaStateConstraint_getExpression (metaStateConstraint m) 
{
  return m->rspec;
}

void metaStateConstraint_free (/*@only@*/ metaStateConstraint m) 
{
  metaStateSpecifier_free (m->lspec);
  metaStateExpression_free (m->rspec);
  sfree (m);
}


