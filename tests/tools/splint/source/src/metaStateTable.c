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
** metaStateTable.c
** Based on genericTable.c
*/

# include "splintMacros.nf"
# include "basic.h"
# include "randomNumbers.h"

extern void metaStateTable_insert (metaStateTable h, 
				   /*@only@*/ cstring key, 
				   /*@only@*/ metaStateInfo metaState)
{
  llassert (metaStateInfo_isDefined (metaState));
  genericTable_insert ((genericTable) h, key, (void *) metaState);
}

cstring metaStateTable_unparse (metaStateTable h)
{
  cstring res = cstring_newEmpty ();

  metaStateTable_elements (h, key, el) {
    res = cstring_concatFree (res, message ("%s: %x\n", key, metaStateInfo_unparse (el)));
  } end_metaStateTable_elements ;

  return res;
}



