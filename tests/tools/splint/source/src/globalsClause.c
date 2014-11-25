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
** globalsClause.c
*/

# include "splintMacros.nf"
# include "basic.h"

extern globalsClause 
globalsClause_create (lltok tok, globSet gl)
{
  globalsClause res = (globalsClause) dmalloc (sizeof (*res));
  res->globs = gl;
  res->loc = fileloc_copy (lltok_getLoc (tok));
  lltok_free (tok); 
  return res; /* releases doesn't seem to work right here... */
}

globSet globalsClause_getGlobs (globalsClause gclause)
{
  return gclause->globs;
}

globSet globalsClause_takeGlobs (globalsClause gclause)
{
  globSet globs = gclause->globs;
  gclause->globs = globSet_undefined;
  return globs;
}

extern void globalsClause_free (globalsClause gclause)
{
  if (gclause == NULL)
    {
      return; /* shouldn't ever need this? */
    }

  globSet_free (gclause->globs);
  sfree (gclause);
}

extern cstring globalsClause_unparse (globalsClause node)
{
  return message ("globals %q", globSet_unparse (node->globs));
}

