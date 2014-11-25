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
** clause.c
*/

# include "splintMacros.nf"
# include "basic.h"

cstring
clause_unparse (clause cl)
{
  switch (cl)
    {
    case TRUECLAUSE:   return (cstring_makeLiteralTemp ("true"));
    case FALSECLAUSE:  return (cstring_makeLiteralTemp ("false"));
    case ANDCLAUSE:    return (cstring_makeLiteralTemp ("and"));
    case ORCLAUSE:     return (cstring_makeLiteralTemp ("or"));
    case DOWHILECLAUSE:  return (cstring_makeLiteralTemp ("do ... while"));
    case WHILECLAUSE:  return (cstring_makeLiteralTemp ("while"));
    case ITERCLAUSE:   return (cstring_makeLiteralTemp ("iter"));
    case FORCLAUSE:    return (cstring_makeLiteralTemp ("for"));
    case CASECLAUSE:   return (cstring_makeLiteralTemp ("case"));
    case NOCLAUSE:     return (cstring_makeLiteralTemp ("none"));
    case SWITCHCLAUSE: return (cstring_makeLiteralTemp ("switch"));
    case CONDCLAUSE:   return (cstring_makeLiteralTemp ("cond"));
    case TRUEEXITCLAUSE: return (cstring_makeLiteralTemp ("trueexit"));
    case FALSEEXITCLAUSE: return (cstring_makeLiteralTemp ("falseexit"));
    }

  BADEXIT;
}

cstring
clause_nameTaken (clause cl)
{
  switch (cl)
    {
    case TRUECLAUSE:   return (cstring_makeLiteralTemp ("in true branch"));
    case FALSECLAUSE:  return (cstring_makeLiteralTemp ("in true branch"));
    case ANDCLAUSE:    return (cstring_makeLiteralTemp ("in first and clause"));
    case ORCLAUSE:     return (cstring_makeLiteralTemp ("in first or clause"));
    case DOWHILECLAUSE:  return (cstring_makeLiteralTemp ("in do ... while body"));
    case WHILECLAUSE:  return (cstring_makeLiteralTemp ("in while body"));
    case ITERCLAUSE:   return (cstring_makeLiteralTemp ("in iter body"));
    case FORCLAUSE:    return (cstring_makeLiteralTemp ("in for body"));
    case CASECLAUSE:   return (cstring_makeLiteralTemp ("in one case"));
    case NOCLAUSE:     return (cstring_makeLiteralTemp ("in some clause"));
    case SWITCHCLAUSE: return (cstring_makeLiteralTemp ("in one possible execution"));
    case CONDCLAUSE:   return (cstring_makeLiteralTemp ("in true condition"));
    case TRUEEXITCLAUSE: return (cstring_makeLiteralTemp ("in trueexit"));
    case FALSEEXITCLAUSE: return (cstring_makeLiteralTemp ("in falseexit"));
    }

  BADBRANCHRET (cstring_undefined);
}

cstring
clause_nameAlternate (clause cl)
{
  switch (cl)
    {
    case TRUECLAUSE:   return (cstring_makeLiteralTemp ("in continuation"));  
    case FALSECLAUSE:  return (cstring_makeLiteralTemp ("in false branch"));
    case ANDCLAUSE:    return (cstring_makeLiteralTemp ("in second and clause"));
    case ORCLAUSE:     return (cstring_makeLiteralTemp ("in second or clause"));
    case DOWHILECLAUSE:  return (cstring_makeLiteralTemp ("if loop is not taken"));
    case WHILECLAUSE:  return (cstring_makeLiteralTemp ("if loop is not taken"));
    case ITERCLAUSE:   return (cstring_makeLiteralTemp ("if iter body does not execute"));
    case FORCLAUSE:    return (cstring_makeLiteralTemp ("if for loop body does not execute"));
    case CASECLAUSE:   return (cstring_makeLiteralTemp ("in other case"));
    case NOCLAUSE:
    case SWITCHCLAUSE: return (cstring_makeLiteralTemp ("in other possible execution"));
    case CONDCLAUSE:   return (cstring_makeLiteralTemp ("in false condition"));
    case TRUEEXITCLAUSE: return (cstring_makeLiteralTemp ("in trueexit"));
    case FALSEEXITCLAUSE: return (cstring_makeLiteralTemp ("in falseexit"));
    }

  BADBRANCHRET (cstring_undefined);
}

cstring clause_nameFlip (clause cl, bool flip)
{
  if (flip)
    {
      return clause_nameAlternate (cl);
    }
  else
    {
      return clause_nameTaken (cl);
    }
}

bool clause_isBreakable (clause cl)
{
  return (cl == SWITCHCLAUSE
	  || cl == WHILECLAUSE 
	  || cl == DOWHILECLAUSE
	  || cl == FORCLAUSE
	  || cl == ITERCLAUSE);
}

bool clause_isLoop (clause cl)
{
  return (cl == WHILECLAUSE 
	  || cl == FORCLAUSE
	  || cl == ITERCLAUSE
	  || cl == DOWHILECLAUSE);
}

bool clause_isConditional (clause cl)
{
  return (   cl == TRUECLAUSE
	  || cl == FALSECLAUSE
	  || cl == WHILECLAUSE
	  || cl == FORCLAUSE
	  || cl == SWITCHCLAUSE
	  || cl == ITERCLAUSE);
}

bool clause_isSwitch (clause cl)
{
  return (cl == SWITCHCLAUSE);
}

bool clause_isCase (clause cl)
{
  return (cl == CASECLAUSE);
}

bool clause_isNone (clause cl)
{
  return (cl == NOCLAUSE);
} 
