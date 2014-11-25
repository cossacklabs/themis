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
** declaratorInvNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

declaratorInvNodeList
  declaratorInvNodeList_new ()
{
  declaratorInvNodeList s = (declaratorInvNodeList) dmalloc (sizeof (*s));

  s->nelements = 0;

  s->nspace = declaratorInvNodeListBASESIZE;
  s->elements = (declaratorInvNode *) 
    dmalloc (sizeof (*s->elements) * declaratorInvNodeListBASESIZE);

  return (s);
}

static void
declaratorInvNodeList_grow (declaratorInvNodeList s)
{
  int i;
  declaratorInvNode *newelements;

  s->nspace = declaratorInvNodeListBASESIZE + s->nspace;

   newelements = (declaratorInvNode *) 
     dmalloc (sizeof (*newelements) * (s->nelements + s->nspace));

  if (newelements == (declaratorInvNode *) 0)
    {
      llfatalerror (cstring_makeLiteral ("declaratorInvNodeList_grow: out of memory!"));
    }

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }

  sfree (s->elements); 
  s->elements = newelements;
}

declaratorInvNodeList
declaratorInvNodeList_add (declaratorInvNodeList s, declaratorInvNode el)
{
  if (s->nspace <= 0)
    {
      declaratorInvNodeList_grow (s);
    }

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

/*@only@*/ cstring
declaratorInvNodeList_unparse (declaratorInvNodeList s)
{
  int i;
  cstring st = cstring_undefined;

  for (i = 0; i < s->nelements; i++)
    {
      st = message ("%q%q", st, declaratorInvNode_unparse (s->elements[i]));
    }

  return st;
}

void
declaratorInvNodeList_free (declaratorInvNodeList s)
{
  int i;
  for (i = 0; i < s->nelements; i++)
    {
      declaratorInvNode_free (s->elements[i]);
    }
  
  sfree (s->elements); 
  sfree (s);
}
