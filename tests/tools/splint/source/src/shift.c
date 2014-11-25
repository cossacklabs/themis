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
** shift.c
**
** Larch shift sequence manager
**
**	Tokens are sent by the parser to this module.  Here they are saved
**	for awhile in a stack, and delivered on demand during reduce actions.
**	The coordination between what the parser sends here and what reduce
**	consumes here must be maintained by the programmer.
**
*/

# include "splintMacros.nf"
# include "basic.h"
# include "shift.h"

/*@constant static int SHIFTMAX=200;@*/
# define SHIFTMAX 200

static o_ltoken Shifts[SHIFTMAX];
static int shiftIndex;

static bool Parsing_LSLinit = TRUE;

static bool
LSLGenIsEmptyShiftStack (void)
{
  return (shiftIndex == 0);
}

void LSLGenShift (ltoken tok)
{
  if (shiftIndex < SHIFTMAX)
    {
      Shifts[shiftIndex] = ltoken_copy (tok);
      shiftIndex++;
    }
  else
    {
      llbuglit ("LSLGenShift: needs MoreMem ()");
    }
}

extern void
LSLGenShiftOnly (/*@only@*/ ltoken tok)
{
  if (shiftIndex < SHIFTMAX)
    {
      Shifts[shiftIndex] = tok;
      shiftIndex++;
    }
  else
    {
      llbugexitlit ("LSLGenShift: needs MoreMem ()");
    }
}

extern ltoken
LSLGenTopPopShiftStack (void)
{
  ltoken res;

  if (LSLGenIsEmptyShiftStack ())
    {
      lclfatalbug ("LSLGenTopPopShiftStack: Empty stack");
    }

  res = Shifts[--shiftIndex];
  Shifts[shiftIndex] = ltoken_undefined;
  /*@-retalias@*/ /*@-dependenttrans@*/
  return res;
  /*@=retalias@*/ /*@=dependenttrans@*/
}

void
LSLGenInit (bool LSLParse)
{
  Parsing_LSLinit = LSLParse;
}

