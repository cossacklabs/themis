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
** mtAnnotationDecl.c
*/

# include "splintMacros.nf"
# include "basic.h"

extern mtAnnotationDecl mtAnnotationDecl_create (mttok id, mtContextNode c, mttok value) 
{
  mtAnnotationDecl res = (mtAnnotationDecl) dmalloc (sizeof (*res));

  llassert (mttok_isIdentifier (id));
  res->name = mttok_getText (id);
  res->context = c;
  res->loc = mttok_stealLoc (id);

  llassert (mttok_isIdentifier (value));
  res->value = mttok_getText (value);

  mttok_free (id);
  mttok_free (value);
  return res;
}

mtContextNode mtAnnotationDecl_stealContext (mtAnnotationDecl annot)
{
  mtContextNode mtc = annot->context;
  annot->context = mtContextNode_undefined;
  return mtc;
}

extern cstring mtAnnotationDecl_unparse (mtAnnotationDecl node)
{
  return message ("%s %q ==> %s",
		  node->name, 
		  mtContextNode_unparse (node->context),
		  node->value);
}
