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
** mtDefaultsDecl.c
*/

# include "splintMacros.nf"
# include "basic.h"

extern mtDefaultsDecl 
mtDefaultsDecl_create  (/*@only@*/ mtContextNode context, 
			       /*@only@*/ mttok value)
{
  mtDefaultsDecl res = (mtDefaultsDecl) dmalloc (sizeof (*res));
  
  llassert (mttok_isIdentifier (value));

  res->context = context;
  res->loc = fileloc_copy (mttok_getLoc (value));
  res->value = mttok_getText (value);
  
  mttok_free (value);
  return res;
}

extern void mtDefaultsDecl_free (mtDefaultsDecl dd)
{
  cstring_free (dd->value);
  fileloc_free (dd->loc);
  mtContextNode_free (dd->context);
  sfree (dd);
}

extern cstring mtDefaultsDecl_unparse (mtDefaultsDecl node)
{
  return message ("%s %q",
		  node->value, mtContextNode_unparse (node->context));
}
