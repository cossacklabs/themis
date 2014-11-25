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
** ekind.c
*/

# include "splintMacros.nf"
# include "basic.h"

ekind
ekind_fromInt (int i)
{
  if /*@+enumint@*/ (i < KINVALID || i > KELAST) /*@=enumint@*/
    {
      llcontbug (message ("ekind_fromInt: invalid: %d", i));
      return KINVALID; 
    }

  return (ekind)i;
}

cstring ekind_unparse (ekind k)
{
  switch (k)
    {
    case KENUMCONST:   return cstring_makeLiteralTemp ("ect");
    case KCONST:       return cstring_makeLiteralTemp ("cst");
    case KVAR:         return cstring_makeLiteralTemp ("var");
    case KITER:        return cstring_makeLiteralTemp ("itr");
    case KENDITER:     return cstring_makeLiteralTemp ("etr");
    case KDATATYPE:    return cstring_makeLiteralTemp ("dtp");
    case KFCN:         return cstring_makeLiteralTemp ("fcn");
    case KSTRUCTTAG:   return cstring_makeLiteralTemp ("stg");
    case KUNIONTAG:    return cstring_makeLiteralTemp ("utg");
    case KENUMTAG:     return cstring_makeLiteralTemp ("etg");
    case KELIPSMARKER: return cstring_makeLiteralTemp ("eps");
    case KINVALID:     return cstring_makeLiteralTemp ("[invalid]");
    }

  BADEXIT;
}

cstring ekind_unparseLong (ekind k)
{
  switch (k)
    {
    case KCONST:       return cstring_makeLiteralTemp ("constant");
    case KENUMCONST:   return cstring_makeLiteralTemp ("enum member");
    case KVAR:         return cstring_makeLiteralTemp ("variable");
    case KITER:        return cstring_makeLiteralTemp ("iterator");
    case KENDITER:     return cstring_makeLiteralTemp ("iterator finalizer");
    case KDATATYPE:    return cstring_makeLiteralTemp ("datatype");
    case KFCN:         return cstring_makeLiteralTemp ("function");
    case KSTRUCTTAG:   return cstring_makeLiteralTemp ("struct tag");
    case KUNIONTAG:    return cstring_makeLiteralTemp ("union tag");
    case KENUMTAG:     return cstring_makeLiteralTemp ("enum tag");
    case KELIPSMARKER: return cstring_makeLiteralTemp ("...");
    case KINVALID:     return cstring_makeLiteralTemp ("[invalid]");
    }

  BADEXIT;
}

cstring
ekind_capName (ekind k)
{
  switch (k)
    {
    case KCONST:       return cstring_makeLiteralTemp ("Constant");
    case KENUMCONST:   return cstring_makeLiteralTemp ("Enum member");
    case KVAR:         return cstring_makeLiteralTemp ("Variable");
    case KITER:        return cstring_makeLiteralTemp ("Iterator");
    case KENDITER:     return cstring_makeLiteralTemp ("Iterator finalizer");
    case KDATATYPE:    return cstring_makeLiteralTemp ("Datatype");
    case KFCN:         return cstring_makeLiteralTemp ("Function");
    case KSTRUCTTAG:   return cstring_makeLiteralTemp ("Struct tag");
    case KUNIONTAG:    return cstring_makeLiteralTemp ("Union tag");
    case KENUMTAG:     return cstring_makeLiteralTemp ("Enum tag");
    case KELIPSMARKER: return cstring_makeLiteralTemp ("...");
    case KINVALID:     return cstring_makeLiteralTemp ("[invalid]");
    }

  BADEXIT;
}

