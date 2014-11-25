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
** declaratorNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

declaratorNodeList
  declaratorNodeList_new ()
{
  declaratorNodeList s = (declaratorNodeList) dmalloc (sizeof (*s));

  s->nelements = 0;
  s->nspace = declaratorNodeListBASESIZE;
  s->elements = (declaratorNode *) 
    dmalloc (sizeof (*s->elements) * declaratorNodeListBASESIZE);

  return (s);
}

static void
declaratorNodeList_grow (declaratorNodeList s)
{
  int i;
  declaratorNode *newelements;

  s->nspace = declaratorNodeListBASESIZE + s->nspace;
  newelements = (declaratorNode *) dmalloc (sizeof (*newelements) * (s->nelements + s->nspace));

  if (newelements == (declaratorNode *) 0)
    {
      llfatalerror (cstring_makeLiteral ("declaratorNodeList_grow: out of memory!"));
    }

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }

  sfree (s->elements); 
  s->elements = newelements;
}

declaratorNodeList 
declaratorNodeList_add (declaratorNodeList s, declaratorNode el)
{
  if (s->nspace <= 0)
    declaratorNodeList_grow (s);

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

/*@only@*/ cstring
declaratorNodeList_unparse (declaratorNodeList s)
{
  cstring st = cstring_undefined;
  bool first = TRUE;

  declaratorNodeList_elements (s, current)
  {
    if (first)
      {
	st = declaratorNode_unparse (current);
	first = FALSE;
      }
    else
      {
	st = message ("%q, %q", st, declaratorNode_unparse (current));
      }
  } end_declaratorNodeList_elements;

  return st;
}

declaratorNodeList
declaratorNodeList_copy (declaratorNodeList s)
{
  declaratorNodeList ret = declaratorNodeList_new ();

  declaratorNodeList_elements (s, el)
    {
      ret = declaratorNodeList_add (ret, declaratorNode_copy (el));
    } end_declaratorNodeList_elements ;

  return ret;
}

void
declaratorNodeList_free (declaratorNodeList s)
{
  int i;
  for (i = 0; i < s->nelements; i++)
    {
      declaratorNode_free (s->elements[i]); 
    }

  sfree (s->elements);
  sfree (s);
}
