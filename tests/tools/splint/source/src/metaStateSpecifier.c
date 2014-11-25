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
** metaStateSpecifier.c
*/

# include "splintMacros.nf"
# include "basic.h"

metaStateSpecifier 
metaStateSpecifier_create (/*@only@*/ sRef sr, /*@observer@*/ metaStateInfo msinfo)
{
  metaStateSpecifier res = (metaStateSpecifier) dmalloc (sizeof (*res));
  res->sr = sr;
  res->msinfo = msinfo;
  res->elipsis = FALSE;
  return res;
}

metaStateSpecifier 
metaStateSpecifier_createElipsis (/*@observer@*/ metaStateInfo msinfo)
{
  metaStateSpecifier res = (metaStateSpecifier) dmalloc (sizeof (*res));
  res->sr = sRef_undefined;
  res->msinfo = msinfo;
  res->elipsis = TRUE;
  return res;
}

bool
metaStateSpecifier_isElipsis (metaStateSpecifier m)
{
  return m->elipsis;
}

sRef
metaStateSpecifier_getSref (metaStateSpecifier m)
{
  llassert (!metaStateSpecifier_isElipsis (m));
  return m->sr;
}

metaStateInfo
metaStateSpecifier_getMetaStateInfo (metaStateSpecifier m)
{
  return m->msinfo;
}

metaStateSpecifier 
metaStateSpecifier_copy (metaStateSpecifier m)
{
  if (metaStateSpecifier_isElipsis (m))
    {
      return metaStateSpecifier_createElipsis (m->msinfo);
    }
  else
    {
      return metaStateSpecifier_create (sRef_saveCopy (m->sr), m->msinfo);
    }
}

cstring metaStateSpecifier_unparse (metaStateSpecifier m) 
{
  if (m->elipsis)
    {
      return message ("...:%s", 
		      metaStateInfo_getName (m->msinfo));
    }
  else
    {
      return message ("%q:%s", 
		      sRef_unparse (m->sr),
		      metaStateInfo_getName (m->msinfo));
    }
}

void metaStateSpecifier_free (/*@only@*/ metaStateSpecifier m) 
{
  sRef_free (m->sr);
  sfree (m);
}

