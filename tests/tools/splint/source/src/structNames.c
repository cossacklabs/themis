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
** structNames.c
**
** Hacks to fit tags into the same namespace.
*/

# include "splintMacros.nf"
# include "basic.h"
# include "structNames.h"

/*@constant char MARKCHAR_STRUCT; @*/
# define MARKCHAR_STRUCT '@'

/*@constant char MARKCHAR_UNION; @*/
# define MARKCHAR_UNION  '$'

/*@constant char MARKCHAR_ENUM; @*/
# define MARKCHAR_ENUM   '&'

/*@constant char MARKCHAR_PARAM; @*/
# define MARKCHAR_PARAM  '%'

/*@observer@*/ cstring plainTagName (cstring s)
{
  llassert (!isFakeTag (s));

  return cstring_suffix (s, 1);
}

/*@only@*/ cstring fixTagName (cstring s)
{
  if (isFakeTag (s))
    {
      switch (cstring_firstChar (s))
	{
	case MARKCHAR_STRUCT: return (cstring_makeLiteral ("struct"));
	case MARKCHAR_UNION:  return (cstring_makeLiteral ("union"));
	case MARKCHAR_ENUM:   return (cstring_makeLiteral ("enum"));
	default:         return (message ("<bad tag name: %s>", s));
	}
    }
  else
    {
      if (cstring_isDefined (s)) {
	switch (cstring_firstChar (s))
	  {
	  case MARKCHAR_STRUCT:
	    return (message ("struct %s", cstring_suffix (s, 1)));
	  case MARKCHAR_UNION: 
	    return (message ("union %s", cstring_suffix (s, 1)));
	  case MARKCHAR_ENUM:   
	    return (message ("enum %s", cstring_suffix (s, 1)));
	    BADDEFAULT;
	  }
      } else {
	return (cstring_makeLiteral ("<missing tag name>"));
      }
    }
}

cstring makeParam (cstring s)
{
  if (cstring_length(s) > 0 && cstring_firstChar (s) == MARKCHAR_PARAM)
    {
      llbug (message ("makeParam: %s\n", s));
    }

  if (cstring_isUndefined (s))
    {
      return cstring_undefined;
    }

  return (cstring_prependChar (MARKCHAR_PARAM, s));  
}

/*@observer@*/ cstring fixParamName (cstring s)
{
  if (cstring_length(s) < 1)
    {
      return cstring_undefined;
    }

  if (cstring_firstChar (s) != MARKCHAR_PARAM)
    {
      llbug (message ("fixParamName (no #): %s", s));
    }

  return (cstring_suffix (s, 1));
}

cstring makeStruct (cstring s)
{
  if (cstring_firstChar (s) == '@')
    {
      llbug (message ("makeStruct: %s\n", s));
    }

  return (cstring_prependChar (MARKCHAR_STRUCT, s));
}

cstring makeUnion (cstring s)
{
  return (cstring_prependChar (MARKCHAR_UNION, s));
}

cstring makeEnum (cstring s)
{
  return (cstring_prependChar (MARKCHAR_ENUM, s));
}

static unsigned int tagno = 1;

void setTagNo (unsigned int n)
{
  if (n > tagno)
    tagno = n;
}

bool isFakeTag (cstring s)
{
  size_t length = cstring_length (s);

  return ((length >= 1 && cstring_firstChar (s) == '!')
	  || (length >= 2 && cstring_getChar (s, 2) == '!'));
}

cstring fakeTag ()
{
  tagno++;

  return (message ("!%u", tagno));
}





