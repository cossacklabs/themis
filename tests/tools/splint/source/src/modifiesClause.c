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
** modifiesClause.c
*/

# include "splintMacros.nf"
# include "basic.h"

extern modifiesClause 
modifiesClause_createNoMods (lltok tok)
{
  modifiesClause res = (modifiesClause) dmalloc (sizeof (*res));

  res->isnomods = TRUE;
  res->srs = sRefSet_undefined;

  res->loc = fileloc_copy (lltok_getLoc (tok));
  lltok_free (tok);

  return res;
}

extern modifiesClause 
modifiesClause_create (lltok tok, sRefSet rfs)
{
  modifiesClause res = (modifiesClause) dmalloc (sizeof (*res));

  res->isnomods = FALSE;
  res->srs = rfs;

  res->loc = fileloc_copy (lltok_getLoc (tok));
  lltok_free (tok);

  return res;
}

extern void modifiesClause_free (modifiesClause mclause)
{
  sRefSet_free (mclause->srs);
  fileloc_free (mclause->loc);
  sfree (mclause);
}

extern cstring modifiesClause_unparse (modifiesClause node)
{
  if (node->isnomods)
    {
      return cstring_makeLiteral ("modifies nothing");
    }
  else
    {
      return message ("modifies %q", sRefSet_unparse (node->srs));
    }
}

extern sRefSet modifiesClause_getMods (modifiesClause m)
{
  llassert (!m->isnomods);
  return m->srs;
}

extern sRefSet modifiesClause_takeMods (modifiesClause m)
{
  sRefSet mods;
  llassert (!m->isnomods);
  mods = m->srs;
  m->srs = sRefSet_undefined;
  return mods;
}
