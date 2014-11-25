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
** mtTransferAction.c
*/

# include "splintMacros.nf"
# include "basic.h"

extern mtTransferAction mtTransferAction_createValue (mttok value)
{
  mtTransferAction res = (mtTransferAction) dmalloc (sizeof (*res));

  res->kind = MTAK_VALUE;
  llassert (mttok_isIdentifier (value));
  res->value = mttok_getText (value);
  res->message = cstring_undefined;
  res->loc = mttok_stealLoc (value);
  mttok_free (value);
  return res;
}

extern mtTransferAction mtTransferAction_createError (mttok tok)
{
  mtTransferAction res = (mtTransferAction) dmalloc (sizeof (*res));

  res->kind = MTAK_ERROR;
  res->value = cstring_undefined;
  res->message = cstring_undefined;
  res->loc = mttok_stealLoc (tok);
  mttok_free (tok);
  return res;
}

extern mtTransferAction mtTransferAction_createErrorMessage (mttok message)
{
  mtTransferAction res = (mtTransferAction) dmalloc (sizeof (*res));

  res->kind = MTAK_ERROR;
  res->message = mttok_getText (message);
  res->value = cstring_undefined;
  res->loc = mttok_stealLoc (message);
  mttok_free (message);
  return res;
}

extern cstring mtTransferAction_getMessage (mtTransferAction node)
{
  llassert (node->kind == MTAK_ERROR);
  return node->message;
}

extern cstring mtTransferAction_unparse (mtTransferAction node)
{
  switch (node->kind) {
  case MTAK_VALUE: return cstring_copy (node->value);
  case MTAK_ERROR: 
    if (cstring_isDefined (node->message)) {
      return message ("error \"%s\"", node->message);
    } else {
      return cstring_makeLiteral ("<error>");
    }
  BADDEFAULT;
  }

  BADBRANCH;
}

extern void mtTransferAction_free (/*@only@*/ mtTransferAction node)
{
  cstring_free (node->value);
  cstring_free (node->message);
  fileloc_free (node->loc);
  sfree (node);
}
