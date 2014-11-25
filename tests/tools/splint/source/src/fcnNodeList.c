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
** fcnNodeList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
*/

# include "splintMacros.nf"
# include "basic.h"

fcnNodeList
  fcnNodeList_new ()
{
  return fcnNodeList_undefined;
}

static /*@notnull@*/ /*@only@*/ fcnNodeList
fcnNodeList_newEmpty (void)
{
  fcnNodeList s = (fcnNodeList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = fcnNodeListBASESIZE;
  s->elements = (fcnNode *) dmalloc (sizeof (*s->elements) * fcnNodeListBASESIZE);

  return (s);
}

static void
fcnNodeList_grow (/*@notnull@*/ fcnNodeList s)
{
  int i;
  fcnNode *newelements;

  s->nspace += fcnNodeListBASESIZE;
  newelements = (fcnNode *) dmalloc (sizeof (*newelements) 
				     * (s->nelements + s->nspace));

  if (newelements == (fcnNode *) 0)
    {
      llfatalerror (cstring_makeLiteral ("fcnNodeList_grow: out of memory!"));
    }

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }

  sfree (s->elements);
  s->elements = newelements;
}

fcnNodeList
fcnNodeList_add (fcnNodeList s, fcnNode el)
{
  if (fcnNodeList_isUndefined (s))
    {
      s = fcnNodeList_newEmpty ();
    }

  if (s->nspace <= 0)
    fcnNodeList_grow (s);

  s->nspace--;
  s->elements[s->nelements] = el;
  s->nelements++;

  return s;
}

/*@only@*/ cstring
fcnNodeList_unparse (fcnNodeList s)
{
  int i;
  cstring st = cstring_undefined;

  if (fcnNodeList_isDefined (s))
    {
      for (i = 0; i < s->nelements; i++)
	{
	  st = message ("%q%q\n", st, fcnNode_unparse (s->elements[i]));
	}
    }

  return st;
}

void
fcnNodeList_free (/*@null@*/ /*@only@*/ fcnNodeList s)
{
  if (s != NULL)
    {
      int i;
      for (i = 0; i < s->nelements; i++)
	{
	  fcnNode_free (s->elements[i]); 
	}
      
      sfree (s->elements);
      sfree (s);
    
    }
}
