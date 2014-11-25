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
** mtMergeItem.c
*/

# include "splintMacros.nf"
# include "basic.h"

extern mtMergeItem mtMergeItem_createValue (mttok value)
{
  mtMergeItem res = (mtMergeItem) dmalloc (sizeof (*res));
  res->kind = MTMK_VALUE;
  llassert (mttok_isIdentifier (value));
  res->value = mttok_getText (value);
  res->loc = mttok_stealLoc (value);
  mttok_free (value);
  return res;
}

extern mtMergeItem mtMergeItem_createStar (mttok value)
{
  mtMergeItem res = (mtMergeItem) dmalloc (sizeof (*res));

  res->kind = MTMK_STAR;
  res->loc = mttok_stealLoc (value);
  res->value = cstring_undefined;
  mttok_free (value);
  DPRINTF (("Made star!"));
  llassert (mtMergeItem_isStar (res));
  return res;
}

extern cstring mtMergeItem_unparse (mtMergeItem node)
{
  switch (node->kind) {
  case MTMK_VALUE: return cstring_copy (node->value);
  case MTMK_STAR: return cstring_makeLiteral ("{*}");
  BADDEFAULT;
  }

  BADBRANCH;
}

extern void mtMergeItem_free (/*@only@*/ mtMergeItem node)
{
  cstring_free (node->value);
  fileloc_free (node->loc);
  sfree (node);
}
