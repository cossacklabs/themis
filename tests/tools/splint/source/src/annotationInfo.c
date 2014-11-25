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
** annotationInfo.c
*/

# include "splintMacros.nf"
# include "basic.h"

annotationInfo annotationInfo_create (cstring name,
				      metaStateInfo state, mtContextNode context, 
				      int value, fileloc loc)
{
  annotationInfo res = (annotationInfo) dmalloc (sizeof (*res));

  res->name = name;
  res->state = state;
  res->context = context;
  res->value = value;
  res->loc = loc;

  return res;
}

void annotationInfo_free (annotationInfo a)
{
  if (annotationInfo_isDefined (a))
    {
      cstring_free (a->name);
      fileloc_free (a->loc);
      mtContextNode_free (a->context); /* evans 2002-01-03 */
      sfree (a);
    }
}

cstring annotationInfo_getName (annotationInfo a)
{
  llassert (annotationInfo_isDefined (a));
  return a->name;
}

/*@observer@*/ cstring annotationInfo_unparse (annotationInfo a)
{
  return annotationInfo_getName (a);
}

/*@observer@*/ metaStateInfo annotationInfo_getState (annotationInfo a) /*@*/ 
{
  llassert (annotationInfo_isDefined (a));
  return a->state;
}

/*@observer@*/ fileloc annotationInfo_getLoc (annotationInfo a) /*@*/ 
{
  llassert (annotationInfo_isDefined (a));
  return a->loc;
}

int annotationInfo_getValue (annotationInfo a) /*@*/ 
{
  llassert (annotationInfo_isDefined (a));
  return a->value;
}


bool annotationInfo_matchesContext (annotationInfo a, uentry ue)
{
  /*
  ** Returns true iff the annotation context matches the uentry.
  */

  mtContextNode mcontext;

  llassert (annotationInfo_isDefined (a));
  mcontext = a->context;

  if (mtContextNode_matchesEntry (mcontext, ue))
    {
      /* Matches annotation context, must also match meta state context. */
      metaStateInfo minfo = a->state;

      if (mtContextNode_matchesEntry (metaStateInfo_getContext (minfo), ue))
	{
	  return TRUE;
	}
      else
	{
	  return FALSE;
	}
    }
  else
    {
      return FALSE;
    }
}

bool annotationInfo_matchesContextRef (annotationInfo a, sRef sr)
{
  /*
  ** Returns true iff the annotation context matches the uentry.
  */

  mtContextNode mcontext;

  llassert (annotationInfo_isDefined (a));
  mcontext = a->context;

  if (mtContextNode_matchesRef (mcontext, sr))
    {
      /* Matches annotation context, must also match meta state context. */
      metaStateInfo minfo = a->state;

      if (mtContextNode_matchesRef (metaStateInfo_getContext (minfo), sr))
	{
	  return TRUE;
	}
      else
	{
	  return FALSE;
	}
    }
  else
    {
      return FALSE;
    }
}

cstring annotationInfo_dump (annotationInfo a)
{
  llassert (annotationInfo_isDefined (a));
  return a->name;
}

/*@observer@*/ annotationInfo annotationInfo_undump (char **s)
{
  cstring mname = reader_readUntil (s, '.');
  annotationInfo a;
  
  llassert (cstring_isDefined (mname));
  a = context_lookupAnnotation (mname);

  if (annotationInfo_isUndefined (a))
    {
      llfatalerrorLoc
	(message ("Library uses undefined annotation %s.  Must use same -mts flags as when library was created.",
		  mname));
    }
  else
    {
      cstring_free (mname);
      return a;
    }

  BADBRANCHRET (annotationInfo_undefined);
}

void annotationInfo_showContextRefError (annotationInfo a, sRef sr)
{
  mtContextNode mcontext;
  llassert (!annotationInfo_matchesContextRef (a, sr));
  llassert (annotationInfo_isDefined (a));
  mcontext = a->context;

  if (mtContextNode_matchesRef (mcontext, sr))
    {
      /* Matches annotation context, must also match meta state context. */
      metaStateInfo minfo = a->state;

      if (mtContextNode_matchesRef (metaStateInfo_getContext (minfo), sr))
	{
	  BADBRANCH;
	}
      else
	{
	  mtContextNode_showRefError (metaStateInfo_getContext (minfo), sr);
	}
    }
  else
    {
      mtContextNode_showRefError (mcontext, sr);
    }
}
