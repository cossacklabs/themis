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
** mtDefaultsNode.c
*/

# include "splintMacros.nf"
# include "basic.h"

extern mtDefaultsNode mtDefaultsNode_create (mttok t, mtDefaultsDeclList decls)
{
  mtDefaultsNode res = (mtDefaultsNode) dmalloc (sizeof (*res));

  res->decls = decls;
  res->loc = mttok_stealLoc (t);
  mttok_free (t);

  return res;
}

void mtDefaultsNode_free (mtDefaultsNode node)
{
  mtDefaultsDeclList_free (node->decls);
  fileloc_free (node->loc);
  sfree (node);
}

extern cstring mtDefaultsNode_unparse (mtDefaultsNode node)
{
  return message ("defaults %q", mtDefaultsDeclList_unparse (node->decls));
}
