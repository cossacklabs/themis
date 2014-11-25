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
** mtTransferClause.c
*/

# include "splintMacros.nf"
# include "basic.h"

extern mtTransferClause mtTransferClause_create (mttok fromname, mttok toname, mtTransferAction action)
{
  mtTransferClause res = (mtTransferClause) dmalloc (sizeof (*res));

  llassert (mttok_isIdentifier (fromname));
  res->fromname = mttok_getText (fromname);

  llassert (mttok_isIdentifier (toname));
  res->toname = mttok_getText (toname);

  res->action = action;
  res->loc = mttok_stealLoc (fromname);
  if (fileloc_isUndefined (res->loc))
    {
      llbug (message ("Not fileloc: %q", mttok_unparse (fromname)));
    }

  mttok_free (fromname);
  mttok_free (toname);
  return res;
}

extern void mtTransferClause_free (mtTransferClause transfer)
{
  cstring_free (transfer->fromname);
  cstring_free (transfer->toname);
  fileloc_free (transfer->loc);
  mtTransferAction_free (transfer->action);
  sfree (transfer);
}

extern cstring mtTransferClause_unparse (mtTransferClause node)
{
  return message ("%s as %s ==> %q",
		  node->fromname, node->toname, 
		  mtTransferAction_unparse (node->action));
}
