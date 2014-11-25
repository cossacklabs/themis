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
** mtLoseReference.c
*/

# include "splintMacros.nf"
# include "basic.h"

extern mtLoseReference mtLoseReference_create (mttok fromname, mtTransferAction action)
{
  mtLoseReference res = (mtLoseReference) dmalloc (sizeof (*res));

  llassert (mttok_isIdentifier (fromname));
  res->fromname = mttok_getText (fromname);

  res->action = action;
  res->loc = mttok_stealLoc (fromname);

  if (fileloc_isUndefined (res->loc))
    {
      llbug (message ("Not fileloc: %q", mttok_unparse (fromname)));
    }

  mttok_free (fromname);
  return res;
}

extern void mtLoseReference_free (mtLoseReference transfer)
{
  cstring_free (transfer->fromname);
  fileloc_free (transfer->loc);
  mtTransferAction_free (transfer->action);
  sfree (transfer);
}

extern cstring mtLoseReference_unparse (mtLoseReference node)
{
  return message ("%s ==> %q",
		  node->fromname, 
		  mtTransferAction_unparse (node->action));
}
