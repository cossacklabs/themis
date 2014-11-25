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
** pointers.c
*/

# include "splintMacros.nf"
# include "basic.h"

pointers pointers_create (lltok tok)
{
  return pointers_createMods (tok, qualList_undefined);
}

pointers pointers_createMods (/*@unused@*/ lltok tok, qualList quals)
{
  pointers res = (pointers) dmalloc (sizeof (*res));

  res->quals = quals;
  res->rest = pointers_undefined;

  return res;
}

pointers pointers_createMt (mttok tok)
{
  return pointers_createModsMt (tok, qualList_undefined);
}

pointers pointers_createLt (ltoken tok)
{
  return pointers_createModsLt (tok, qualList_undefined);
}

pointers pointers_createModsMt (/*@unused@*/ mttok tok, qualList quals)
{
  pointers res = (pointers) dmalloc (sizeof (*res));

  res->quals = quals;
  res->rest = pointers_undefined;

  return res;
}

pointers pointers_createModsLt (/*@unused@*/ ltoken tok, qualList quals)
{
  pointers res = (pointers) dmalloc (sizeof (*res));

  res->quals = quals;
  res->rest = pointers_undefined;

  return res;
}

pointers pointers_extend (pointers p1, pointers p2)
{
  llassert (pointers_isDefined (p1));
  llassert (pointers_isUndefined (p1->rest));
  p1->rest = p2;
  return p1;
}

pointers pointers_getRest (pointers p)
{
  llassert (pointers_isDefined (p));
  return p->rest;
}

cstring pointers_unparse (pointers p)
{
  if (pointers_isDefined (p))
    {
      if (qualList_isDefined (p->quals))
	{
	  if (pointers_isDefined (p->rest))
	    {
	      return (message ("* %q %q", qualList_unparse (p->quals), pointers_unparse (p->rest)));
	    }
	  else
	    {
	      return (message ("* %q", qualList_unparse (p->quals)));
	    }
	}
      else
	{
	  if (pointers_isDefined (p->rest))
	    {
	      return (message ("* %q", pointers_unparse (p->rest)));
	    }
	  else
	    {
	      return (cstring_makeLiteral ("*"));
	    }
	}
    }
  else
    {
      return cstring_undefined;
    }
}

int pointers_depth (pointers p)
{
  if (pointers_isUndefined (p))
    {
      return 0;
    }
  else
    {
      return 1 + pointers_depth (p->rest);
    }
}

void pointers_free (/*@only@*/ pointers p)
{
  if (pointers_isDefined (p))
    {
      qualList_free (p->quals);
      pointers_free (p->rest);
      sfree (p);
    }
}
