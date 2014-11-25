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
** metaStateExpression.c
*/

# include "splintMacros.nf"
# include "basic.h"

metaStateExpression 
metaStateExpression_create (/*@only@*/ metaStateSpecifier spec)
{
  metaStateExpression res = (metaStateExpression) dmalloc (sizeof (*res));
  res->spec = spec;
  res->rest = NULL;
  return res;
}

metaStateExpression 
metaStateExpression_createMerge (/*@only@*/ metaStateSpecifier spec, /*@only@*/ metaStateExpression exp)
{
  metaStateExpression res = (metaStateExpression) dmalloc (sizeof (*res));
  res->spec = spec;
  res->rest = exp;
  return res;
}

cstring metaStateExpression_unparse (metaStateExpression m) 
{
  llassert (m != NULL);

  if (m->rest != NULL)
    {
      return message ("%q | %q", 
		      metaStateSpecifier_unparse (m->spec),
		      metaStateExpression_unparse (m->rest));
      
    }
  else
    {
      return metaStateSpecifier_unparse (m->spec);
    }
}

metaStateExpression metaStateExpression_copy (metaStateExpression m) 
{
  if (m == NULL) return NULL;

  if (m->rest != NULL)
    {
      return metaStateExpression_createMerge (metaStateSpecifier_copy (m->spec),
					      metaStateExpression_copy (m->rest));
      
    }
  else
    {
      return metaStateExpression_create (metaStateSpecifier_copy (m->spec));
    }
}

metaStateSpecifier metaStateExpression_getSpecifier (metaStateExpression m)
{
  llassert (m != NULL);
  return m->spec;
}

bool metaStateExpression_isMerge (metaStateExpression m)
{
  return (metaStateExpression_isDefined (m) 
	  && metaStateExpression_isDefined (m->rest));
}

/*@observer@*/ metaStateExpression metaStateExpression_getRest (metaStateExpression m)
{
  llassert (m != NULL);
  return m->rest;
}

void metaStateExpression_free (/*@only@*/ metaStateExpression m) 
{
  llassert (m != NULL);

  metaStateSpecifier_free (m->spec);

  if (m->rest != NULL) {
    metaStateExpression_free (m->rest);
  }

  sfree (m);
}

