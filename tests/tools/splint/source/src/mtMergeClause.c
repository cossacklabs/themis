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
** mtMergeClause.c
*/

# include "splintMacros.nf"
# include "basic.h"

extern mtMergeClause 
mtMergeClause_create (mtMergeItem item1, mtMergeItem item2, mtTransferAction action)
{
  mtMergeClause res = (mtMergeClause) dmalloc (sizeof (*res));

  res->item1 = item1;
  res->item2 = item2;
  res->action = action;
  return res;
}

extern void mtMergeClause_free (mtMergeClause merge)
{
  mtMergeItem_free (merge->item1);
  mtMergeItem_free (merge->item2);
  mtTransferAction_free (merge->action);
  sfree (merge);
}

extern cstring mtMergeClause_unparse (mtMergeClause node)
{
  return message ("%q X %q ==> %q",
		  mtMergeItem_unparse (node->item1), 
		  mtMergeItem_unparse (node->item2), 
		  mtTransferAction_unparse (node->action));
}

