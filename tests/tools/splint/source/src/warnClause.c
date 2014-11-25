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
** warnClause.c
*/

# include "splintMacros.nf"
# include "basic.h"

static warnClause warnClause_createAux (/*@only@*/ fileloc loc, 
					/*@only@*/ flagSpec flag, 
					/*@only@*/ cstring msg)
{
  warnClause res = (warnClause) dmalloc (sizeof (*res));
  
  res->loc = loc;
  res->flag = flag;
  res->msg = msg;
  
  DPRINTF (("Creating warn clause with flag spec: [%p] %s", flag,
	    flagSpec_unparse (flag)));
  return res;
}

extern warnClause warnClause_create (lltok tok, flagSpec flag, cstring msg) 
{
  warnClause res;
  /*
  ** evans 2002-03-11
  ** was
  **   res = warnClause_createAux (lltok_stealLoc (tok), flag, msg);
  ** but this leads to unexplained (yet) crashes.
  ** Reported by Walter Briscoe
  */

  res = warnClause_createAux (fileloc_copy (lltok_getLoc (tok)), flag, msg);
  lltok_free (tok);
  return res;
}

warnClause warnClause_copy (warnClause w)
{
  if (warnClause_isDefined (w))
    {
      return warnClause_createAux (fileloc_copy (w->loc),
				   flagSpec_copy (w->flag),
				   cstring_copy (w->msg)); 
    }
  else
    {
      return warnClause_undefined;
    }
}

extern flagSpec warnClause_getFlag (warnClause w)
{
  llassert (warnClause_isDefined (w));
  return w->flag;
}

extern cstring warnClause_unparse (warnClause w)
{
  if (warnClause_isDefined (w))
    {
      return message ("<%q> %s", flagSpec_unparse (w->flag), w->msg);
    }
  else
    {
      return cstring_undefined;
    }
}

extern bool warnClause_hasMessage (warnClause w)
{
  return warnClause_isDefined (w) && cstring_isDefined (w->msg);
}

extern /*@observer@*/ cstring warnClause_getMessage (warnClause w)
{
  if (warnClause_isDefined (w)) {
    return w->msg;
  } else {
    return cstring_undefined;
  }
}


extern void warnClause_free (warnClause w)
{
  if (warnClause_isDefined (w))
    {
      flagSpec_free (w->flag);
      fileloc_free (w->loc);
      cstring_free (w->msg);
      sfree (w);
    }
}

/*@only@*/ cstring
warnClause_dump (warnClause wc)
{
  cstring st = cstring_undefined;
  llassert (warnClause_isDefined (wc));
  llassert (!cstring_containsChar (warnClause_getMessage (wc), '#'));

  if (warnClause_hasMessage (wc))
    {
      llassert (cstring_firstChar (warnClause_getMessage (wc)) != '.');
      st = message ("%q#%s#", flagSpec_dump (wc->flag), warnClause_getMessage (wc));
    }
  else
    {
      st = message ("%q#.#", flagSpec_dump (wc->flag));
    }

  return st;
}

warnClause
warnClause_undump (char **s)
{
  flagSpec flag;
  cstring msg;

  DPRINTF (("Undump: %s", *s));
  flag = flagSpec_undump (s);
  DPRINTF (("Here: %s", *s));
  reader_checkChar (s, '#');
  DPRINTF (("Here: %s", *s));

  if (reader_optCheckChar (s, '.'))
    {
      msg = cstring_undefined;
    }
  else
    {
      msg = reader_readUntil (s, '#');
    }
  
  DPRINTF (("Here: %s", *s));
  reader_checkChar (s, '#');

  return warnClause_createAux (fileloc_copy (g_currentloc), flag, msg);
}
