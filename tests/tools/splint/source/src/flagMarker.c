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
** flagMarker.c
*/

# include "splintMacros.nf"
# include "basic.h"

flagMarker flagMarker_createLocalSet (flagcode code, ynm set, fileloc loc)
{
  flagMarker c = (flagMarker) dmalloc (sizeof (*c));
  
  c->kind = FMK_LOCALSET;
  c->code = code;
  c->info.set = set;
  c->loc = fileloc_copy (loc); 

  return c;
}

flagMarker flagMarker_createSuppress (flagcode code, fileloc loc)
{
  flagMarker c = (flagMarker) dmalloc (sizeof (*c));
  
  c->kind = FMK_SUPPRESS;
  c->code = code;
  c->loc = fileloc_copy (loc); 

  return c;
}

flagMarker flagMarker_createIgnoreOn (fileloc loc)
{
  flagMarker c = (flagMarker) dmalloc (sizeof (*c));
  
  c->kind = FMK_IGNOREON;
  c->code = INVALID_FLAG;
  c->loc = fileloc_copy (loc); 

  return c;
}

flagMarker flagMarker_createIgnoreCount (int count, fileloc loc)
{
  flagMarker c = (flagMarker) dmalloc (sizeof (*c));
  
  c->kind = FMK_IGNORECOUNT;
  c->code = INVALID_FLAG;
  c->info.nerrors = count;
  c->loc = fileloc_copy (loc); 

  DPRINTF (("Create ignore count: %s", flagMarker_unparse (c)));
  return c;
}

flagMarker flagMarker_createIgnoreOff (fileloc loc)
{
  flagMarker c = (flagMarker) dmalloc (sizeof (*c));
  
  c->kind = FMK_IGNOREOFF;
  c->code = INVALID_FLAG;
  c->loc = fileloc_copy (loc); 

  return c;
}

ynm flagMarker_getSet (flagMarker f)
{
  llassert (f->kind == FMK_LOCALSET);

  return f->info.set;
}

flagcode flagMarker_getCode (flagMarker f)
{
  llassert (f->kind == FMK_LOCALSET|| f->kind == FMK_SUPPRESS);

  return f->code;
}

int flagMarker_getCount (flagMarker f)
{
  llassert (f->kind == FMK_IGNORECOUNT);

  return f->info.nerrors;
}

bool flagMarker_equal (flagMarker f1, flagMarker f2)
{
  if (f1->kind != f2->kind)
    {
      return FALSE;
    }

  if (!fileloc_equal (f1->loc, f2->loc))
    {
      return FALSE;
    }

  switch (f1->kind)
    {
    case FMK_LOCALSET:
      return (f1->info.set == f2->info.set
	      && flagcode_equal (f1->code, f2->code));
    case FMK_IGNORECOUNT:
      return (f1->info.nerrors == f2->info.nerrors);
    case FMK_IGNOREON:
    case FMK_IGNOREOFF:
      return TRUE;
    case FMK_SUPPRESS:
      return (flagcode_equal (f1->code, f2->code));
    }

  BADBRANCHRET (FALSE);
}

cstring flagMarker_unparse (flagMarker c)
{
  switch (c->kind)
    {
    case FMK_LOCALSET:
      return (message ("%q: %s%s", 
		       fileloc_unparse (c->loc), ynm_unparseCode (c->info.set), 
		       flagcode_unparse (c->code)));
    case FMK_IGNORECOUNT:
      return (message ("%q: ignore count %d", 
		       fileloc_unparse (c->loc), c->info.nerrors));
    case FMK_IGNOREON:
      return (message ("%q: ignore on", 
		       fileloc_unparse (c->loc)));
    case FMK_IGNOREOFF:
      return (message ("%q: ignore off", 
		       fileloc_unparse (c->loc)));
    case FMK_SUPPRESS:
      return (message ("%q: suppress %s", 
		       fileloc_unparse (c->loc),
		       flagcode_unparse (c->code)));
    }

  BADBRANCHRET (cstring_undefined);
}
  
void flagMarker_free (/*@only@*/ flagMarker c)
{
  fileloc_free (c->loc); /* evans 2001-03-24: Splint caught this... */
  sfree (c);
}

bool flagMarker_sameFile (flagMarker c, fileloc loc)
{
  return (fileloc_almostSameFile (c->loc, loc));
}

/*
** return true if loc is before c->loc
*/

bool flagMarker_beforeMarker (flagMarker c, fileloc loc)
{
  return  (!fileloc_notAfter (c->loc, loc));
}



