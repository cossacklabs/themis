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
** metaStateInfo.c
*/

# include "splintMacros.nf"
# include "basic.h"

/*@notnull@*/ metaStateInfo 
metaStateInfo_create (cstring name, 
		      cstringList valueNames, 
		      mtContextNode context,
		      stateCombinationTable sctable,
		      stateCombinationTable mergetable,
		      fileloc loc) 
{
  metaStateInfo res = (metaStateInfo) dmalloc (sizeof (*res));
  int i;

  res->name = name;
  res->valueNames = valueNames;
  res->context = context;

  res->sctable = sctable;
  res->mergetable = mergetable;
  res->loc = loc;

  for (i = 0; i < MTC_NUMCONTEXTS; i++)
    {
      res->defaultValue[i] = stateValue_error;
    }

  llassert (stateCombinationTable_size (res->sctable) 
	    == cstringList_size (res->valueNames));

  return res;
}

void metaStateInfo_free (/*@only@*/ metaStateInfo msinfo)
{
  if (metaStateInfo_isDefined (msinfo))
    {
      cstring_free (msinfo->name);
      cstringList_free (msinfo->valueNames);
      stateCombinationTable_free (msinfo->sctable);
      stateCombinationTable_free (msinfo->mergetable);
      fileloc_free (msinfo->loc);
      mtContextNode_free (msinfo->context); /* evans 2002-01-03 */
      sfree (msinfo);
    }
}

cstring metaStateInfo_unparse (metaStateInfo info)
{
  llassert (metaStateInfo_isDefined (info));
  return message ("%s: %q\n%q", info->name,
		  cstringList_unparse (info->valueNames),
		  stateCombinationTable_unparse (info->sctable));
}

cstring metaStateInfo_unparseValue (metaStateInfo info, int value)
{
  llassert (metaStateInfo_isDefined (info));
  
  DPRINTF (("unparse value: %s / %d",
	    metaStateInfo_unparse (info), value));
  
  if (value < 0) 
      {
	  llassert (value == stateValue_error);
	  return cstring_makeLiteralTemp ("error");
      }
  
  llassert (value < cstringList_size (info->valueNames));
  return cstringList_get (info->valueNames, value);
}

/*@observer@*/ mtContextNode metaStateInfo_getContext (metaStateInfo info)
{
  llassert (metaStateInfo_isDefined (info));
  return info->context;
}

/*@observer@*/ cstring metaStateInfo_getName (metaStateInfo info) 
{
  llassert (metaStateInfo_isDefined (info));
  return info->name;
}

/*@observer@*/ fileloc metaStateInfo_getLoc (metaStateInfo info) 
{
  llassert (metaStateInfo_isDefined (info));
  return info->loc;
}

extern /*@exposed@*/ stateCombinationTable 
metaStateInfo_getTransferTable (metaStateInfo info) /*@*/
{
  llassert (metaStateInfo_isDefined (info));
  return info->sctable;
}

extern /*@exposed@*/ stateCombinationTable 
metaStateInfo_getMergeTable (metaStateInfo info) /*@*/
{
  llassert (metaStateInfo_isDefined (info));
  return info->mergetable;
}

/*@+enumindex@*/ /* allow context kinds to reference array */
extern int metaStateInfo_getDefaultValueContext (metaStateInfo info, mtContextKind context)
{
  llassert (metaStateInfo_isDefined (info));
  return info->defaultValue [context];
}

extern int metaStateInfo_getDefaultValue (metaStateInfo info, sRef s)
{
  llassert (metaStateInfo_isDefined (info));

  if (sRef_isParam (s)
      && (info->defaultValue [MTC_PARAM] != stateValue_error))
    {
      return info->defaultValue [MTC_PARAM];
    }
  else if (sRef_isResult (s)
	   && (info->defaultValue [MTC_RESULT] != stateValue_error))
    {
      return info->defaultValue [MTC_RESULT];
    }
  else if (sRef_isConst (s)
	   && (info->defaultValue [MTC_LITERAL] != stateValue_error))
    {
      return info->defaultValue [MTC_LITERAL];
    }
  else 
    {
      llassert (mtContextNode_matchesRef (metaStateInfo_getContext (info), s));
      return info->defaultValue [MTC_REFERENCE];
    }
}

extern int metaStateInfo_getDefaultGlobalValue (metaStateInfo info)
{
  llassert (metaStateInfo_isDefined (info));
  return info->defaultValue [MTC_REFERENCE];
}

void metaStateInfo_setDefaultValueContext (metaStateInfo info, mtContextKind context, int val)
{
  llassert (metaStateInfo_isDefined (info));
  /*@-type@*/ llassert (context >= 0 && context < MTC_NUMCONTEXTS); /*@=type@*/
  llassert (metaStateInfo_getDefaultValueContext (info, context) == stateValue_error);
  info->defaultValue [context] = val;
}
/*@=enumindex@*/ 
\
void metaStateInfo_setDefaultRefValue (metaStateInfo info, int val)
{
  metaStateInfo_setDefaultValueContext (info, MTC_REFERENCE, val);
}

void metaStateInfo_setDefaultResultValue (metaStateInfo info, int val)
{
  metaStateInfo_setDefaultValueContext (info, MTC_RESULT, val);
}

void metaStateInfo_setDefaultParamValue (metaStateInfo info, int val)
{
  metaStateInfo_setDefaultValueContext (info, MTC_PARAM, val);
}

int metaStateInfo_getDefaultRefValue (metaStateInfo info)
{
  return metaStateInfo_getDefaultValueContext (info, MTC_REFERENCE);
}

int metaStateInfo_getDefaultResultValue (metaStateInfo info)
{
  return metaStateInfo_getDefaultValueContext (info, MTC_RESULT);
}

int metaStateInfo_getDefaultParamValue (metaStateInfo info)
{
  return metaStateInfo_getDefaultValueContext (info, MTC_PARAM);
}

